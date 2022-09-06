#include <bfgn_errm.h>
#include <bfgn_head.h>
#include <fullpathfn.h>
#include <bfgn_gen_dtls.h>
#include <bfrt_gen_dtls.h>
#include <errlibg.h> //Jiten

#include <bfoi_iter.h>
#define EXTERN extern  
#include <ddcr_functions.h>
#include <bfcs_head.h>
#undef EXTERN

#define BufferSize (pdl2afp_req_len + 3)
#define ZIP_CODE_SIZE 9

int g_noOfRequests=0;
int g_iFirst;
static const int RecordLength = (pdl2afp_req_len + 2);
char currentZipCode[ZIP_CODE_SIZE]="";
char nextZipCode[ZIP_CODE_SIZE]="";

static long long counter_pdf = 0;
/*EMail Request File and Interface Rewuset file*/
FILE *pFEmail_rep = NULL;
FILE *pFIntEmail = NULL;


/**********************************************************************\
* Name: ddcr_processRequests
*
* Purpose: 
* 
*
* Description: 
***********************************************************************/
FUNCSTS_C_T ddcr_processRequests(const char* requestsFileName)
{
      Trace("ddcr_processRequests");

      char buffer[BufferSize];
      char *pEmailFileName;
      char *pEmail_Interface_FileName;
      char mv_cmd[1250];
   char o_linux_pcBillName[1024];
    int noOfRequests;
      FUNCSTS_C_T status = SUCCESS;
    if(getenv("CR_NO_REQUESTS")){
         noOfRequests = atoi(getenv("CR_NO_REQUESTS"));
      }
   else
   {
      noOfRequests=1;
      printf("\n Environment varibale CR_NO_REQUESTS not defined ..");
   }
   FILE* requestsFile = fopen(requestsFileName, "r");
   
      if (requestsFile == NULL)
            ERR_StdReportAndReturn_1(BFGN_IO_FILE_OPEN, requestsFileName);
   
      while (fgets(buffer, RecordLength, requestsFile) != NULL)
      {
            pdl2afp_req_t* pdl2afp_req = (pdl2afp_req_t*) malloc(sizeof(pdl2afp_req_t));
            status = ddcr_populateInput(buffer, pdl2afp_req);
      
            if (status != SUCCESS)
            {
               fclose(requestsFile);
               ERR_StdReportAndReturn_2(BFGN_FUNC_FAILED_ERR, "ddcr_populateInput", status);
            }
      
      if( ((memcmp(g_format_type,"POST",4))==0) && ((memcmp(getenv("PRINTER_TYPE_LOCAL"),"AFP",3))==0) && ((memcmp(getenv("FORMAT_TYPE_LOCAL"),"POST",4))==0)) 
      /* POST AFP */
      {
               addBillRequest(pdl2afp_req);
      }
      }
   
   fclose(requestsFile);
   
   pdl2afp_req_t* billRequests = getAllRequests(&noOfRequests);
   g_noOfRequests=noOfRequests;

   int index=0;
   for (index = 0; index < noOfRequests; index++)
   {
      //FULLPATHFN_C_D(pcPDLFileName);
      char pcPDLFileName[PDF_FILE_NAME_MAX_LEN];
      memset(pcPDLFileName, ' ', sizeof(pcPDLFileName));
      g_currentBillRequest = &(billRequests[index]);

      if((index+1) < noOfRequests)
   g_nextBillRequest = &(billRequests[index+1]);

      printf("\nProcessing request for "); PRINT_BF1BILLID_C("BILL_ID", billRequests[index].pdl2bill.bill_id); printf("\n");
      printf("------------------------------\n");
      fflush(NULL);

      reset_ban_flag=1; /*Resetting for every bill */


      memset(currentZipCode,'\0',sizeof(currentZipCode));
      memset(nextZipCode,'\0',sizeof(nextZipCode));

     memset(g_email_add,'\0',sizeof(g_email_add));
//      strncpy(g_email_add,g_currentBillRequest->addressLine2,100);

    /*  if((index+1) < noOfRequests)
      {
   strncpy(nextZipCode,g_nextBillRequest->addressLine2,8);
      } */

      status = ddcr_getRetFile(&(billRequests[index].pdl2bill), pcPDLFileName);
      if (status != SUCCESS) ERR_StdReportAndReturn_2(BFGN_FUNC_FAILED_ERR, "ddcr_getRetFile", status);
      //printf("RET FILE NAME <%s> \n", pcPDLFileName);
      fflush(NULL);
        /* Start Changed for Little ENdian and Big Endian */
                if (EndianSwitchNeeded(pcPDLFileName)==0)
                {
                        printf("\n\n\nProcessing Unix PDL\n\n\n");
                        memset(o_linux_pcBillName,'\0',sizeof(o_linux_pcBillName));
                        sprintf(o_linux_pcBillName, "%s.esw", pcPDLFileName);
                        char cmd_mv[2048];
                        sprintf( cmd_mv,"/bin/mv -f %s %s.esw",pcPDLFileName,pcPDLFileName);
                        system( cmd_mv );
                        printf("*** Calling SwitchEndian***");
                        status = SwitchEndianInFile(pcPDLFileName,o_linux_pcBillName);
                        if ( status != SUCCESS )
                                ERR_StdCheckReportAndReturn_0( BFGN_TRACE_ERROR)
                                        printf("*** Switch Endian converted successfully****");
                }
        /* End Changed for Little ENdian and Big Endian */
      memset(g_bfcs_csmreq.ArchFilePath, '\0', ARCH_FILE_PATH_LENGTH);
      memset(g_bfcs_csmreq.ArchFileName, '\0', ARCH_FILE_NAME_LENGTH);
      sprintf(g_bfcs_csmreq.ArchFileName, "%s", pcPDLFileName);

      if(g_currentBillRequest->pdl2bill.full_opt_ind == 'Y')
         g_bfcs_csmreq.PrintOnlyPrtInd = 0;
      else
         g_bfcs_csmreq.PrintOnlyPrtInd = 1;
      g_bfcs_csmreq.FullBillReq = 1;
      g_bfcs_csmreq.PrtPgNum = 0;
      g_bfcs_csmreq.OptPgNum = 0;
      g_bfcs_csmreq.ReqCopies = 1;

      g_iFirst = 1;

      status = Bfcs_PDF_Generation();
      if (status != SUCCESS) ERR_StdReportAndReturn_2(BFGN_FUNC_FAILED_ERR, "Bfcs_PDF_Generation", status);

      status = remove(pcPDLFileName);
      if (!IS_C_FUNCSTS_SUCCESS(status))
         ERR_StdReportAndReturn_1(BFGN_IO_FILE_REMOVE_WAR, pcPDLFileName);
   }
   
/* if (noOfRequests != 0)
   {
      // End activities
      status = Bfpr_EndMain();
      if (status != SUCCESS)  ERR_Report_2(BFGN_FUNC_FAILED_ERR, "Bfpr_EndMain", status);
   }
*/

   return status;
}

/**********************************************************************\
* Name: ddcr_populateInput
*
* Purpose: 
*     
*
* Description: 
***********************************************************************/
FUNCSTS_C_T ddcr_populateInput(const char* buffer, pdl2afp_req_t* pdl2afp_req)
{
      Trace("ddcr_populateInput");

      int t_processed_len = 0;
      char ddnumber9[10], cycle_run_month[3], cycle_run_year[5], bill_seq_no[4], cycle_code[5];
   
      memset(cycle_run_month, 0, sizeof(cycle_run_month));
      memset(cycle_run_year, 0, sizeof(cycle_run_year));
      memset(bill_seq_no, 0, sizeof(bill_seq_no));
      memset(cycle_code, 0, sizeof(cycle_code));
   
      TB_INITIALIZE_PDL2BILL_REQ_T_C(pdl2afp_req->pdl2bill);
   
      GI_StringCollapseN(pdl2afp_req->pdl2bill.event_id, buffer + t_processed_len, BF1BILLID_C_L);
      t_processed_len += BF1BILLID_C_L;
   
      GI_StringCollapseN(pdl2afp_req->pdl2bill.bill_id, buffer + t_processed_len, BF1BILLID_C_L);
      t_processed_len += BF1BILLID_C_L;
   
      pdl2afp_req->pdl2bill.full_opt_ind = (buffer + t_processed_len)[0];
      t_processed_len += 1;
   
      GI_StringCollapseN(bill_seq_no, buffer+t_processed_len, BF1BLSEQNO_C_SZ);
      BF1BLSEQNO_MOVE_C_TO_C(pdl2afp_req->pdl2bill.bill_seq_no,bill_seq_no);
      t_processed_len += BF1BLSEQNO_C_SZ;
   
      GI_StringCollapseN(pdl2afp_req->pdl2bill.production_date,buffer+t_processed_len, DATETIMEX_C_L);
      t_processed_len += DATETIMEX_C_L;
   
      pdl2afp_req->pdl2bill.production_type = (buffer+t_processed_len)[0];
      t_processed_len += 1;
      
      GI_StringCollapseN(pdl2afp_req->pdl2bill.pdl_file_path, buffer+t_processed_len, FULLPATH_C_L);
      t_processed_len += FULLPATH_C_L;
   
      GI_StringCollapseN(pdl2afp_req->pdl2bill.pdl_file_name, buffer+t_processed_len, FILENAME_C_L);
      t_processed_len += FILENAME_C_L;
   
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,BF1BUFSIZE_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.archive_bill_offset_bytes = atol(ddnumber9);
      t_processed_len += BF1BUFSIZE_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,BF1BUFSIZE_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.original_bill_size_bytes = atol(ddnumber9);
      t_processed_len += BF1BUFSIZE_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,BF1BUFSIZE_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.bill_size_bytes_ldg = atol(ddnumber9);
      t_processed_len += BF1BUFSIZE_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,BF1BUFSIZE_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.bill_size_bytes_prt = atol(ddnumber9);
      t_processed_len += BF1BUFSIZE_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,BF1BUFSIZE_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.bill_size_bytes_opt = atol(ddnumber9);
      t_processed_len += BF1BUFSIZE_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,BF1BUFSIZE_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.archive_size_bytes = atol(ddnumber9);
      t_processed_len += BF1BUFSIZE_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,COUNTER_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.num_prt_pages = atol(ddnumber9);
      t_processed_len += COUNTER_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,COUNTER_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.num_opt_pages = atol(ddnumber9);
      t_processed_len += COUNTER_C_ASCII_SZ;
   
      memset(ddnumber9, 0, sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,COUNTER_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.num_of_copies = atol(ddnumber9);
      t_processed_len += COUNTER_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,COUNTER_C_ASCII_SZ);
      //pdl2afp_req->pdl2bill.bill_rec_cnt_ldg = atol(ddnumber9); -- Jiten
      pdl2afp_req->pdl2bill.bill_rec_cnt_ldg = atol(ddnumber9);
      t_processed_len += COUNTER_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,COUNTER_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.bill_rec_cnt_prt = atol(ddnumber9);
      t_processed_len += COUNTER_C_ASCII_SZ;
   
      memset(ddnumber9,0,sizeof(ddnumber9));
      GI_StringCollapseN(ddnumber9, buffer+t_processed_len,COUNTER_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.bill_rec_cnt_opt = atol(ddnumber9);
      t_processed_len += COUNTER_C_ASCII_SZ;
   
      GI_StringCollapseN(cycle_run_year, buffer+t_processed_len,YEAR_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.cycle_run_year = atol(cycle_run_year);
      t_processed_len += YEAR_C_ASCII_SZ;
   
      GI_StringCollapseN(cycle_run_month, buffer+t_processed_len,MONTH_C_ASCII_SZ); 
      pdl2afp_req->pdl2bill.cycle_run_month = atol(cycle_run_month); 
      t_processed_len += MONTH_C_ASCII_SZ;
   
      GI_StringCollapseN(cycle_code, buffer+t_processed_len,BF1BLCYCLCD_C_ASCII_SZ);
      pdl2afp_req->pdl2bill.cycle_code = atol(cycle_code);
      t_processed_len += BF1BLCYCLCD_C_ASCII_SZ;

   /* Defect#2931 */
      memset(pdl2afp_req->sort_key_rp, '\0', sizeof(pdl2afp_req->sort_key_rp));
      GI_StringCollapseN(pdl2afp_req->sort_key_rp, buffer+t_processed_len, SORT_KEY_LENGTH);
      t_processed_len += SORT_KEY_LENGTH;
   /* End of Defect#2931 */

      memset(pdl2afp_req->addressElement1, '\0', sizeof(pdl2afp_req->addressElement1));
      GI_StringCollapseN(pdl2afp_req->addressElement1, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement2, '\0', sizeof(pdl2afp_req->addressElement2));
      GI_StringCollapseN(pdl2afp_req->addressElement2, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;
   
      memset(pdl2afp_req->addressElement3, '\0', sizeof(pdl2afp_req->addressElement3));
      GI_StringCollapseN(pdl2afp_req->addressElement3, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement4, '\0', sizeof(pdl2afp_req->addressElement4));
      GI_StringCollapseN(pdl2afp_req->addressElement4, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement5, '\0', sizeof(pdl2afp_req->addressElement5));
      GI_StringCollapseN(pdl2afp_req->addressElement5, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;
   
      memset(pdl2afp_req->addressElement6, '\0', sizeof(pdl2afp_req->addressElement6));
      GI_StringCollapseN(pdl2afp_req->addressElement6, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement7, '\0', sizeof(pdl2afp_req->addressElement7));
      GI_StringCollapseN(pdl2afp_req->addressElement7, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement8, '\0', sizeof(pdl2afp_req->addressElement8));
      GI_StringCollapseN(pdl2afp_req->addressElement8, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement9, '\0', sizeof(pdl2afp_req->addressElement9));
      GI_StringCollapseN(pdl2afp_req->addressElement9, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement10, '\0', sizeof(pdl2afp_req->addressElement10));
      GI_StringCollapseN(pdl2afp_req->addressElement10, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement11, '\0', sizeof(pdl2afp_req->addressElement11));
      GI_StringCollapseN(pdl2afp_req->addressElement11, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement12, '\0', sizeof(pdl2afp_req->addressElement12));
      GI_StringCollapseN(pdl2afp_req->addressElement12, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement13, '\0', sizeof(pdl2afp_req->addressElement13));
      GI_StringCollapseN(pdl2afp_req->addressElement13, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement14, '\0', sizeof(pdl2afp_req->addressElement14));
      GI_StringCollapseN(pdl2afp_req->addressElement14, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement15, '\0', sizeof(pdl2afp_req->addressElement15));
      GI_StringCollapseN(pdl2afp_req->addressElement15, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement16, '\0', sizeof(pdl2afp_req->addressElement16));
      GI_StringCollapseN(pdl2afp_req->addressElement16, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement17, '\0', sizeof(pdl2afp_req->addressElement17));
      GI_StringCollapseN(pdl2afp_req->addressElement17, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement18, '\0', sizeof(pdl2afp_req->addressElement18));
      GI_StringCollapseN(pdl2afp_req->addressElement18, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement19, '\0', sizeof(pdl2afp_req->addressElement19));
      GI_StringCollapseN(pdl2afp_req->addressElement19, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement20, '\0', sizeof(pdl2afp_req->addressElement20));
      GI_StringCollapseN(pdl2afp_req->addressElement20, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement21, '\0', sizeof(pdl2afp_req->addressElement21));
      GI_StringCollapseN(pdl2afp_req->addressElement21, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;

      memset(pdl2afp_req->addressElement22, '\0', sizeof(pdl2afp_req->addressElement22));
      GI_StringCollapseN(pdl2afp_req->addressElement22, buffer+t_processed_len, ADDRESS_LINE_LENGTH);
      t_processed_len += ADDRESS_LINE_LENGTH;
      
   if (debug_mode) TB_PRINT_PDL2BILL_REQ_C(pdl2afp_req->pdl2bill);
      /*CR38 Start Here for Storing the Adress line information */
      memset(g_format_type, '\0', sizeof(g_format_type));
      sprintf(g_format_type,"%s", "POST");
      memset(g_email_add, '\0', sizeof(g_format_type));
   //   sprintf(g_email_add,"%s",pdl2afp_req->addressLine2);
   
      /*CR38 End Here for Storing the Adress line information */
      if(t_processed_len != (pdl2afp_req_len-44))
      {
            printf("ddcr_populateInput: Invalid Record Length %d (expected %d)!\n", t_processed_len, pdl2afp_req_len);
            return FAILURE;
      }
   
      return SUCCESS;
}


/**********************************************************************\
* Name: ddrp_DeCompressBillGzip
*
* Purpose: DeCompressing data compressed by GZip from archive file
*     corresponding to the requested Bill.
*
* Description: Uses Gunzip to de-compress the archived bill.
***********************************************************************/
FUNCSTS_C_T ddcr_DeCompressBillGzip(const pdl2bill_req_t* i_pcBillReq, char* archiveFileName, char *io_pcPDLFileName, char IfGzip)
{
   FUNCSTS_C_D(status);
   FILE *s_pfDeComprBill;
   FILE *s_pfArchive;
   
   Trace("ddrp_DeCompressBillGzip");

   /*----------------------------------------------------*/
   /* Get the details of the compression tools directory */
   /*----------------------------------------------------*/

   FULLPATH_C_D(s_pcToolPath);
   status = Bfgn_GetEnvValue("ARS_TOOLS_DIR", s_pcToolPath, FULLPATH_C_L);
   if (!IS_C_FUNCSTS_SUCCESS(status ))
         ERR_StdReportAndReturn_1(BFGN_ENV_ERR, "ARS_TOOLS_DIR");

   /*-----------------------------------------------*/
   /* Open Archive file name in Read Mode */
   /*-----------------------------------------------*/

   status = Bfgn_OpenCloseFile(archiveFileName, &s_pfArchive, Bfgn_OPEN_R);
   if (!IS_C_FUNCSTS_SUCCESS( status ))
      ERR_StdReportAndReturn_1(BFGN_IO_FILE_OPEN, archiveFileName);

   /*---------------------------------------------------------------------------*/
   /* Set the pointer to the start of the bill in the archived file.            */
   /* Open the Pipe to read from the archive file char by char and write it into*/
   /* the extract PDL file char by char.                                        */
   /*---------------------------------------------------------------------------*/

   status = fseek(s_pfArchive, i_pcBillReq->archive_bill_offset_bytes, SEEK_SET);
   if (!IS_C_FUNCSTS_SUCCESS(status))
   {
      printf("\nError in fseek in gunzip\n");
      fflush(stdout);
      return (FAILURE);
   }

   if(IfGzip == Bfgn_ON)
   {
      char s_pcCommand[FULLPATH_C_L];
      sprintf(s_pcCommand,"%s/gunzip -f > %s", s_pcToolPath, io_pcPDLFileName );
      //printf("\nCommand to gunzip is %s plus %s\n", s_pcToolPath, io_pcPDLFileName );
      //fflush(stdout);
      s_pfDeComprBill = popen(s_pcCommand, FILE_OPEN_W);
      if (s_pfDeComprBill == NULL)
      {
        printf("\nError in popen in gunzip\n");
        fflush(stdout);
        return (FAILURE);
      }
   }
   else
   {
      status = Bfgn_OpenCloseFile(io_pcPDLFileName, &s_pfDeComprBill, Bfgn_OPEN_W);
      if (!IS_C_FUNCSTS_SUCCESS( status ))
         ERR_StdReportAndReturn_1( BFGN_IO_FILE_OPEN,io_pcPDLFileName);
   }

   int iIndex = 0;
   for (iIndex = 0 ; iIndex < i_pcBillReq->archive_size_bytes ; iIndex++ )
   {
      char pcChar = getc(s_pfArchive);
      if ( ferror(s_pfArchive))
      {
         printf("\ngetc error in gunzip");
         fflush(stdout);
         return (FAILURE);
      }
      fputc(pcChar, s_pfDeComprBill);
      if (ferror(s_pfDeComprBill))
      {
         printf("\nfputc error in gunzip");
         fflush(stdout);
         return (FAILURE);
      }

   }

   if(IfGzip == Bfgn_ON)
   {
      status = pclose(s_pfDeComprBill);
      if ((status == -1) || (status == 127))
      {
         printf("pclose is closing the file\n");
         printf("\n error in pclose in gunzip\n");
         fflush(stdout);
         return (FAILURE);
      }
   }
   else
   {
      status = Bfgn_OpenCloseFile(io_pcPDLFileName, &s_pfDeComprBill, Bfgn_CLOSE);
      if (!IS_C_FUNCSTS_SUCCESS(status))
         ERR_StdReportAndReturn_1(BFGN_IO_FILE_CLOSE, io_pcPDLFileName);
   }

   status = Bfgn_OpenCloseFile(archiveFileName, &s_pfArchive, Bfgn_CLOSE);
   if (!IS_C_FUNCSTS_SUCCESS(status))
        ERR_StdReportAndReturn_1(BFGN_IO_FILE_CLOSE, archiveFileName);

   return SUCCESS;
}

/**********************************************************************\
* Name: ddcr_getRetFile
*
* Purpose: 
* 
*
* Description: 
***********************************************************************/
FUNCSTS_C_T ddcr_getRetFile(pdl2bill_req_t* i_pcBillReq, char * io_pcPDLFileName)
{
   FUNCSTS_C_D(status);
   char s_pcArchiveNM[FULLPATH_C_L];
   
   FULLPATH_C_D(pcBillPath);
   char file_path[FULLPATH_C_L + 1];
   char file_name[FILENAME_C_L + 1];
   char pcBillId[BF1BILLID_C_L + 1];
   char tempBillId[23] = "";
   char tempBillId1[23] = "";
   char *part1 = 0, *part2 = 0;

   Trace("ddcr_getRetFile");

   memset(pcBillId, '\0', sizeof(pcBillId));
   memcpy(pcBillId, i_pcBillReq->bill_id, BF1BILLID_C_L);

   /* ------------------------------------*/
   /* Build De-Compressed bill file name */
   /*------------------------------------*/

   status = Bfgn_GetEnvValue("ABP_DD_WORK", pcBillPath, FULLPATH_C_L);
   if (!IS_C_FUNCSTS_SUCCESS(status ))
         ERR_StdReportAndReturn_1(BFGN_ENV_ERR,"ABP_DD_WORK");

   /* sprintf((char *)io_pcPDLFileName, "%s/bfrt.%s.ext.ret", pcBillPath, pcBillId); */
   /* Jiten - Changed to handle space in the file name */ 
   memset(tempBillId,0,23);
   memcpy(tempBillId,pcBillId,22);
   GI_StringCollapse(tempBillId,tempBillId);
   part1 = strtok(tempBillId," ");
   part2 = strtok(NULL," ");
   /* memset(tempBillId,0,23); */
   if(part1 != NULL)   
   strcpy(tempBillId1,part1);
   if(part2!=NULL)   
      strcat(tempBillId1,part2);
   sprintf((char *)io_pcPDLFileName, "%s/bfrt.%s.ext.ret", pcBillPath, tempBillId1); 
   //printf("RET FILE NAME <%s>\n",io_pcPDLFileName);
   /* Jiten - Change End */

   /*-----------------------------------------------*/
   /* Build Archive file name                       */
   /*-----------------------------------------------*/
   memset(file_path, '\0', sizeof(file_path));
   memset(s_pcArchiveNM, '\0', FULLPATH_C_L);
   
   memccpy(file_path, i_pcBillReq->pdl_file_path, ' ', FULLPATH_C_L);
   sprintf(i_pcBillReq->pdl_file_path, "%s/", file_path);
   sprintf(s_pcArchiveNM, "%s", i_pcBillReq->pdl_file_path);
   
   memset(file_name, '\0', sizeof(file_name));
   memccpy(file_name, i_pcBillReq->pdl_file_name, ' ', FILENAME_C_L);
   sprintf(i_pcBillReq->pdl_file_name, "%s", file_name);
   
   if (s_pcArchiveNM[strlen(s_pcArchiveNM) - 1] == ' ')
   {
      sprintf(s_pcArchiveNM+strlen(s_pcArchiveNM) - 1,"%s", i_pcBillReq->pdl_file_name);
      
      if (s_pcArchiveNM[strlen(s_pcArchiveNM) - 1]==' ')
         s_pcArchiveNM[strlen(s_pcArchiveNM) - 1]='\0';
      else
         s_pcArchiveNM[strlen(s_pcArchiveNM)]='\0';
   }
   else
   {
      sprintf(s_pcArchiveNM+strlen(s_pcArchiveNM), "%s", i_pcBillReq->pdl_file_name);
      
      if (s_pcArchiveNM[strlen(s_pcArchiveNM) - 1] == ' ')
         s_pcArchiveNM[strlen(s_pcArchiveNM) - 1] = '\0';
      else
         s_pcArchiveNM[strlen(s_pcArchiveNM)] = '\0';
   }
   s_pcArchiveNM[strlen(s_pcArchiveNM)] = '\0';



   /*---------------------------------------------------------------------------*/
   /* Call the compression utility gunzip to decompress the archived bill which */
   /* has been compressed using gzip utility.                                   */
   /*---------------------------------------------------------------------------*/
   printf("Opening archive file %s \n", s_pcArchiveNM);
   /* May 9, 2018: Defect#2285 */
   struct stat s_sFileStat;
   memset(&s_sFileStat, '\0', sizeof(s_sFileStat));
   status = stat(s_pcArchiveNM, &s_sFileStat);
   if (!IS_C_FUNCSTS_SUCCESS(status))
   {
      printf("\n\n\t\tERROR:   Archive file does not exist.\n"); 
      ERR_StdReportAndReturn_1(BFGN_IO_FILE_OPEN, s_pcArchiveNM);
   }
   /* End of Defect#2285 */

   if (i_pcBillReq->archive_size_bytes != i_pcBillReq->original_bill_size_bytes)
      status = ddcr_DeCompressBillGzip(i_pcBillReq, s_pcArchiveNM, io_pcPDLFileName, Bfgn_ON);
   else
      status = ddcr_DeCompressBillGzip(i_pcBillReq, s_pcArchiveNM, io_pcPDLFileName, Bfgn_OFF);

      
   if (!IS_C_FUNCSTS_SUCCESS(status))
      ERR_StdReportAndReturn_2(BFGN_FUNC_FAILED_ERR, "ddrp_DeCompressBillGzip", status);


   /*---------------------------------------------------------------------------*/
   /* Check the status to see if the extract file is created or not.            */
   /*---------------------------------------------------------------------------*/
   /* May 9, 2018: Defect#2285 
   struct stat s_sFileStat; */
   memset(&s_sFileStat, '\0', sizeof(s_sFileStat));
   status = stat(io_pcPDLFileName, &s_sFileStat);
   if (!IS_C_FUNCSTS_SUCCESS(status))
      ERR_StdReportAndReturn_1(BFGN_IO_FILE_OPEN, io_pcPDLFileName);

   if ((i_pcBillReq->original_bill_size_bytes != MAX_ORIG_BILL_SIZE) &&
       (s_sFileStat.st_size != i_pcBillReq->original_bill_size_bytes))
   {
      printf("Extract File Size <%ld> does not match Original Bill size <%d>", s_sFileStat.st_size, i_pcBillReq->original_bill_size_bytes);
      printf("Skipping bill id <%s>", pcBillId);
      fflush(stdout);

      status = remove(io_pcPDLFileName);
      if (!IS_C_FUNCSTS_SUCCESS(status))
        ERR_StdReportAndReturn_1(BFGN_IO_FILE_REMOVE_WAR,io_pcPDLFileName);
   }

   return SUCCESS;

}

//Passing total requests in .crpRequestsFile
int get_total_requests()
{
return g_noOfRequests;
}
int ddcr_pdf_bill()
{

 /* needed to read VR records */
 short shVarNameLength;
 short shVarLength;
 int CYCLE_CODE;

 /* names of VR variables */
 char flsVarValue[120];


 char banum[]="BANUMBER";
 char docseq[]="DOCSEQNUM";
 char cust_first_name[]="CUSTOMER_FIRST_NAME";
 char cust_bill_date[]="CUSTOMER_BILL_DATE";
 char cust_address_type[]="CUSTOMER_ADDRESS_TYPE";
 char cust_email_id[]="CUSTOMER_EMAIL_ID";

 char t_banum[13];
 char t_docseq[13];
 char t_cust_first_name[100];
 char t_cust_bill_date[10];
 char t_cust_address_type[20];
 char t_cust_email_id[100];
 char t_pdf_file_name[200];
 char result_date[100];

 /* used for processing string variables */
 int  counter;
 int status = Bfgn_RC_SUCCESS;

 /* Varibale to store current date & time */
 char curDateTime[15]='\0';

  /* variables used to move temp PDF to final PDF name */
  char    file_path[512];
  char    mv_cmd[1250];
  char output_file_name_final[PDF_FILE_NAME_MAX_LEN];
  char output_file_path[512];


    
      /******************/
      /* Read BA_NUMBER */
      /******************/
      memset(t_banum, '\0', sizeof(t_banum));
      shVarNameLength = strlen(banum);
      status = Bfoi_GetVar(shVarNameLength, banum, &shVarLength, flsVarValue);
      if(status != Bfgn_RC_SUCCESS) return status;
      for( counter=0; counter < shVarLength; counter++)
      {
         if ((flsVarValue[counter] < '0') || (flsVarValue[counter] > '9'))
         {
            flsVarValue[counter] = 0;
            counter = shVarLength;//to exit the for loop
         }
      }
      memcpy(t_banum, flsVarValue, strlen(flsVarValue));

      printf("\n PRABHAKAR The BA_NUMBER IS [%s]",t_banum);


      /**********************/
      /* Read CUSTOMER_FIRST_NAME      */
      /**********************/
      memset(t_cust_first_name, '\0', sizeof(t_cust_first_name));
      shVarNameLength = strlen(cust_first_name);
      status = Bfoi_GetVar(shVarNameLength, cust_first_name, &shVarLength, flsVarValue);
      if(status != Bfgn_RC_SUCCESS)
      {
         printf("\n *** EMAIL FUNCTIONALITY ERROR in fetching VR CUSTOMER_FIRST_NAME***");
         return status;
      }
      memcpy(t_cust_first_name, flsVarValue, shVarLength);
//    bfus_trim(t_cust_first_name, strlen(t_cust_first_name), 2);

      printf("\n PRABHAKAR The CUSTOMER_FIRST_NAME IS [%s]",t_cust_first_name);


      /**********************/
      /* Read CUSTOMER_BILL_DATE      */
      /**********************/
      memset(t_cust_bill_date, '\0', sizeof(t_cust_bill_date));
      shVarNameLength = strlen(cust_bill_date);
      status = Bfoi_GetVar(shVarNameLength, cust_bill_date, &shVarLength, flsVarValue);
      if(status != Bfgn_RC_SUCCESS)
      {
         printf("\n *** EMAIL FUNCTIONALITY ERROR in fetching VR CUSTOMER_BILL_DATE***");
         return status;
      }
      memcpy(t_cust_bill_date, flsVarValue, shVarLength);
//    bfus_trim(t_cust_bill_date, strlen(t_cust_bill_date), 2);

      printf("\n PRABHAKAR The CUSTOMER_BILL_DATE IS [%s]",t_cust_bill_date);

   // Creating final PDF file name
    getCurrentDateTime_rep(curDateTime);
   memset(output_file_name_final,'\0',sizeof(output_file_name_final));
   sprintf(output_file_name_final, "bf.REG.%s_%s_%d.pdf", t_banum, curDateTime,counter_pdf);
   memset(output_file_path, '\0',sizeof(output_file_path));
   sprintf(output_file_path, "%s", getenv("ABP_DD_ROOT_OUTPUT"));//for fileplacin

    sprintf(mv_cmd,"mv -f %s %s/%s", oPdfFileName, output_file_path, output_file_name_final);
   printf("\n mv_cmd = [%s]\n", mv_cmd);
   system(mv_cmd);
   counter_pdf++;

   memset(t_pdf_file_name, '\0', sizeof(t_pdf_file_name));
   sprintf(t_pdf_file_name, "%s", output_file_name_final);

   CYCLE_CODE=atoi(getenv("RUN_TIME_DAY"));

   memset(result_date,'\0',sizeof(result_date));
   memcpy(result_date,Reprint_Date_Conversion(t_cust_bill_date),sizeof(result_date));


   printf("\n PRA the CYCLE_CODE [%d]", CYCLE_CODE);

   fprintf(pFEmail_rep, "%s,%s,%s,%s,%s,%s\n", t_banum, t_cust_first_name, t_cust_bill_date, g_email_add, t_pdf_file_name,result_date);
   fprintf(pFIntEmail, "%d|%s|%s|%s|%s|%s\n",CYCLE_CODE,t_banum, t_cust_first_name, t_cust_bill_date, g_email_add, t_pdf_file_name);


 return SUCCESS;
}

int ddcr_pdf_file_name()
{
 int status;
 char tmpName[PDF_FILE_NAME_MAX_LEN];
 static long long counter = 0;

     Trace( "ddcr_pdf_file_name" );
    counter++;
    memset(tmpName,'\0',sizeof(tmpName));
    sprintf(tmpName, "%s_%d.pdf", getenv("PDL_FILE_NAME_ODI_INF"), counter);
    strcpy(oPdfFileName,tmpName);

    return SUCCESS;
}

// Function to get the current Date & time in the MMDDYYYYHHmmss format
void getCurrentDateTime_rep( char *curDateTime)
{
   struct tm *timeinfo;
   time_t time_of_day;

   Trace("getCurrentDateTime_rep");

   time ( &time_of_day );
   timeinfo = localtime ( &time_of_day );
   mktime ( timeinfo );

   sprintf(curDateTime,"%02d%02d%04d%02d%02d%02d",timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_year+1900,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

}

char *Reprint_Date_Conversion(char *date_str)
{
   char FullMonthLower[12][12]={
                     "January",
                     "February",
                     "March",
                     "April",
                     "May",
                     "June",
                     "July",
                     "August",
                     "September",
                     "October",
                     "November",
                     "December"
                     };

char date[2];
char month[2];
char year[4];
char result[50];
char month1[3];
int mon_final=0;

   memset(date,'\0',sizeof(date));
   memset(month,'\0',sizeof(month));
   memset(year,'\0',sizeof(year));
   memset(result,'\0',sizeof(result));

   memcpy(year,date_str,4);
   memcpy(month,date_str+4,2);
   memcpy(date,date_str+6,2);

   month1[0]=month[0];
   month1[1]=month[1];
    month1[2]='\0';

    mon_final=atoi(month1);

   if(mon_final > 12)
   {
       printf("\n The month Greater than 12 ");
       return ;
   }

   sprintf(result,"%2.2s %s %4.4s",date,FullMonthLower[mon_final-1],year);

return result;
}


long Each_Page_Of_Bill()
{
  int status=Bfgn_RC_SUCCESS;
  short PAGE_NO_LAY_ID = 72;
  short TSA_LOGO_LAY_ID = 73;
  char BUFFER_VALUE[50];
  char tsalogo[8]="";
  char Print_Type[3]="";


    memset(BUFFER_VALUE, '\0', sizeof(BUFFER_VALUE));
       sprintf(BUFFER_VALUE,"Page %d of %d ",g_sBillIter.iSheetCount,g_sBillIter.iCurrBillTotPages);

   if(g_sBillIter.iSheetCount <= g_sBillIter.iCurrBillTotPages) 
   //Avoiding the Blank Page no Other Wise It will come Current page will geater like Page 4 of 3
   {
    status = Bfoi_SpecInsert(&PAGE_NO_LAY_ID, "PAGE_NO", 0, 1,BUFFER_VALUE,strlen(BUFFER_VALUE));
    if(status != SUCCESS)
    {
      printf("\n SPEC INSERT Failing in the PAGE_NO SLAY FOR PDF");
      //   ERR_StdReportAndReturn_2( BFGN_FUNC_FAILED_ERR,"Bfoi_SpecInsert",status);
    }
        }
   /* Print the Telkom logo on all Pages */  
       memset(Print_Type,'\0',strlen(Print_Type));
       sprintf(Print_Type,"%-3.3s", getenv("PRINTER_TYPE_LOCAL"));

       if(memcmp(Print_Type,"PDF",3)==0)
       {
          memset(tsalogo, '\0', sizeof(tsalogo));
             sprintf(tsalogo,"telkomsa");

          status = Bfoi_SpecInsert(&TSA_LOGO_LAY_ID, "TSA_LOGO", 0, 1, tsalogo, -1);
          if(status != SUCCESS)
          {
             printf("\n SPEC INSERT Failing  For First Page Logo into PDF");
             //ERR_StdReportAndReturn_2( BFGN_FUNC_FAILED_ERR,"Bfoi_SpecInsert",status);
          }
       }

  return status;
}
