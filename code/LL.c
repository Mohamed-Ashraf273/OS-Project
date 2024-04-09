#include <stdio.h>
#include <stdlib.h>
#include"LL.h"
// Function to initialize the linked list
void initializeList(LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
}

// Function to add a node to the head of the list
void addToHead(LinkedList *list, int data) {
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
void addToTail(LinkedList *list, int data) {
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

// Function to search for a given node
Node* searchNode(LinkedList *list, int data) {
    Node *current = list->head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Function to remove a node from the head of the list
int removeFromHead(LinkedList *list) {
    if (list->head == NULL) {
        printf("List is empty.\n");
        return -1;
    }
    int data = list->head->data;
    Node *temp = list->head;
    list->head = list->head->next;
    free(temp);
    if (list->head == NULL) {
        list->tail = NULL;
    }
    return data;
}

// Function to remove a node from the tail of the list
int removeFromTail(LinkedList *list) {
    if (list->tail == NULL) {
        printf("List is empty.\n");
        return -1;
    }
    int data;
    if (list->head == list->tail) {
        // Only one node in the list
        data = list->head->data;
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        Node *current = list->head;
        while (current->next != list->tail) {
            current = current->next;
        }
        data = list->tail->data;
        free(list->tail);
        list->tail = current;
        list->tail->next = NULL;
    }
    return data;
}

// Function to remove a given node from the list
void removeNode(LinkedList *list, Node *target) {
    if (list->head == target) {
        removeFromHead(list);
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
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}
