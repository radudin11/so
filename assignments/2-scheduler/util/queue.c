#include "./queue.h"

Node* create_node(void* data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    return node;
}

void free_node(Node* node) {
    free(node);
}

Queue* create_queue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void destroy_queue(Queue* queue) {
    Node* node = queue->head;
    while (node != NULL) {
        Node* next = node->next;
        free_node(node);
        node = next;
    }
    free(queue);
}

void enqueue(Queue* queue, void* data) {
    Node* node = create_node(data);
    if (queue->head == NULL) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->size++;
}


void* dequeue(Queue* queue) {
    if (queue->head == NULL) {
        return NULL;
    }
    Node* node = queue->head;
    queue->head = queue->head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    void* data = node->data;
    free_node(node);

    queue->size--;
    return data;
}