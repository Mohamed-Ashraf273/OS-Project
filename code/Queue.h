// Node structure for the queue
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Queue structure
typedef struct {
    Node *front;
    Node *rear;
} Queue;

// Function prototypes
void initializeQueue(Queue *queue);
int isEmpty(Queue *queue);
void enqueue(Queue *queue, int data);
int dequeue(Queue *queue);
void display(Queue *queue);

