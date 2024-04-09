#ifndef LL_H
#define LL_H

// Node structure
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

// Linked list structure
typedef struct {
    Node *head;
    Node *tail;
} LinkedList;

// Function prototypes
void initializeList(LinkedList *list);
void addToHead(LinkedList *list, void *data);
void addToTail(LinkedList *list, void *data);
void removeNode(LinkedList *list, Node *target);
void displayList(LinkedList *list);
void sortAscending(LinkedList *list);
#endif /* LL_H */
