
// Node structure for the queue
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

// Queue structure
typedef struct Queue{
    Node *front;
    Node *rear;
} Queue;

// Function prototypes
void initializeQueue(Queue *queue);
int isEmpty(Queue *queue);
void enqueue(Queue *queue, void *data);
void* dequeue(Queue *queue);
void display(Queue *queue);

