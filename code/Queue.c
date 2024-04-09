#include <stdio.h>
#include <stdlib.h>
#include "Queue.h"

// Function to initialize the queue
void initializeQueue(Queue *queue) {
    queue->front = NULL;
    queue->rear = NULL;
}

// Function to check if the queue is empty
int isEmpty(Queue *queue) {
    return (queue->front == NULL);
}

// Function to enqueue an element into the queue
void enqueue(Queue *queue, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (isEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Function to dequeue an element from the queue
void* dequeue(Queue *queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return NULL;
    }
    void *data = queue->front->data;
    Node *temp = queue->front;
    queue->front = queue->front->next;
    free(temp);
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    return data;
}

// Function to display the elements of the queue
void display(Queue *queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return;
    }
    Node *current = queue->front;
    printf("Queue: ");
    while (current != NULL) {
        printf("%p ", current->data);
        current = current->next;
    }
    printf("\n");
}
