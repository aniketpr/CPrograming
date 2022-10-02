#include <stdio.h>
#include <stdlib.h>

struct Node{
    int data;
    struct Node *next;
};

void linkedListTraversal(struct Node *ptr){
    while(ptr != NULL){
        printf("Element: %d\n", ptr->data);
        ptr = ptr->next;
    }
}

struct Node * insertAtFirst(struct Node *head, int data){
    struct Node * ptr = (struct Node *) malloc(sizeof(struct Node));
    ptr->data = data;
    ptr->next = head;
    return ptr;
}

struct Node * insertAtEnd(struct Node *head, int data){
    struct Node * ptr = (struct Node *) malloc(sizeof(struct Node));
    ptr->data = data;
    struct Node * p = head;
    while(p->next!=NULL){
        p = p->next;
    }
    p->next = ptr;
    ptr->next = NULL;
    return head;
}

//insert in between
struct Node * insertAtIndex(struct Node *head, int data, int index)
{
    struct Node * ptr = (struct Node *) malloc(sizeof(struct Node));
    struct Node * p = head;
    int i = 0;
    while(i!=index-1){
        p = p->next;
        i++;
    }
    ptr->data = data;
    ptr->next = p->next;
    p->next = ptr;
    return head;
}

struct Node * insertAfterNode(struct Node * head, struct Node * previoudNode, int data){
    struct Node * ptr = (struct Node *) malloc(sizeof(struct Node));
    ptr->data = data;

    ptr->next = previoudNode->next;
    previoudNode->next = ptr;

    return head;
}

int main(){
    struct Node *head;
    struct Node *second;
    struct Node *third;

    head = (struct Node *)malloc(sizeof(struct Node));
    second = (struct Node *)malloc(sizeof(struct Node));
    third = (struct Node *)malloc(sizeof(struct Node));

    head->data = 7;
    head->next = second;

    second->data = 11;
    second->next = third;

    third->data = 41;
    third->next = NULL;

    linkedListTraversal(head);

    // printf("After Insertion A First Place\n");
    // head = insertAtFirst(head,12);
    // linkedListTraversal(head);

    // printf("After Insertion In Between \n");
    // head = insertAtIndex(head, 56, 1);
    // linkedListTraversal(head);
    
    // printf("Insert At End \n");
    // head = insertAtEnd(head,99);
    // linkedListTraversal(head);

    printf("Insert After insertion \n");
    head = insertAfterNode(head,third, 100);
    linkedListTraversal(head);

    return 0;

}