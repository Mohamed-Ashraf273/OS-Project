#ifndef LL_H
#define LL_H

// Node structure
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Linked list structure
typedef struct {
    Node *head;
    Node *tail;
} LinkedList;

// Function prototypes
void initializeList(LinkedList *list);
void addToHead(LinkedList *list, int data);
void addToTail(LinkedList *list, int data);
Node* searchNode(LinkedList *list, int data);
int removeFromHead(LinkedList *list);
int removeFromTail(LinkedList *list);
void removeNode(LinkedList *list, Node *target);
void displayList(LinkedList *list);
int isEmpty(LinkedList* list);
void sortAscending(LinkedList *list);

#endif /* LL_H */
