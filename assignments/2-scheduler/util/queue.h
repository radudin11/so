#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifndef _QUEUE_
#define _QUEUE_

typedef struct Node {
  void* data;
  struct Node* next;
}Node;

Node* create_node(void* data);
void free_node(Node* node);


typedef struct Queue {
  struct Node* head;
  struct Node* tail;
  int size;
} Queue;

Queue* create_queue();
void destroy_queue(Queue* queue);
void enqueue(Queue* queue, void* data);
void* dequeue(Queue* queue);

#endif
