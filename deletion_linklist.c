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

struct Node * deleteFirstNode(struct Node * head){
    struct Node * ptr = head;
    head = head->next;
    free(ptr);
    return head;
}

// delete given node
struct Node * deleteAtIndex(struct Node * head, int index){
    struct Node *p = head;
    struct Node *q = head->next;
    for(int i=0; i<index-1; i++){
        p = p->next;
        q = q->next;
    }
    p->next = q->next;
    free(q);
    return head;
}

// delete last node
struct Node * deleteAtLastNode(struct Node * head){
    struct Node *p = head;
    struct Node *q = head->next;
    while(q->next != null){
        p = p->next;
        q = q->next;
    }
    p->next = NULL;
    free(q);
    return head;
}

struct Node * deleteNodeAtGivenValue(struct Node * head, int value){
    struct Node *p = head;
    struct Node *q =  head->next;
    while(q->data!=value && q->next != NULL){
        p = p->next;
        q = q->next;
    }
    if(q->data == value){
        p->next = q->next;
        free(q);
    }
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

    // printf("\nAfter deletion of first Node\n");
    // head = deleteFirstNode(head);
    // linkedListTraversal(head);

    // printf("\nThis is example of After deletion given Node from link list\n");
    // head = deleteAtIndex(head, 1);
    // linkedListTraversal(head);

    printf("\nThis is example of deletion of last node\n");
    head = deleteAtLastNode(head);
    linkedListTraversal(head);

    printf("\nThis is example of deletion of Node at given value\n");
    head = deleteNodeAtGivenValue(head, 2);
    linkedListTraversal(head);
    return 0;

}