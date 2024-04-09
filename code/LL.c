#include <stdio.h>
#include <stdlib.h>
#include "LL.h"

// Function to initialize the linked list
void initializeList(LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
}

// Function to add a node to the head of the list
void addToHead(LinkedList *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;
    if (list->tail == NULL) {
        list->tail = newNode; // Update tail if list was empty
    }
}

// Function to add a node to the tail of the list
void addToTail(LinkedList *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;
    if (list->tail == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

// Function to remove a given node from the list
void removeNode(LinkedList *list, Node *target) {
    if (list->head == target) {
        list->head = list->head->next;
        free(target);
        if (list->head == NULL) {
            list->tail = NULL;
        }
        return;
    }
    Node *current = list->head;
    while (current != NULL && current->next != target) {
        current = current->next;
    }
    if (current == NULL) {
        printf("Node not found in the list.\n");
        return;
    }
    current->next = target->next;
    if (target == list->tail) {
        list->tail = current;
    }
    free(target);
}

// Function to display the elements of the list
void displayList(LinkedList *list) {
    Node *current = list->head;
    printf("List: ");
    while (current != NULL) {
        // Assume data is an integer for demonstration
        printf("%d ", *((int *)current->data));
        current = current->next;
    }
    printf("\n");
}
