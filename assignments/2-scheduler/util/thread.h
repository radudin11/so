#include "so_scheduler.h"
#include <semaphore.h>

#ifndef _THREAD_
#define _THREAD_

#define THREAD_STATE_NEW 0
#define THREAD_STATE_READY 1
#define THREAD_STATE_RUNNING 2
#define THREAD_STATE_WAITING 3
#define THREAD_STATE_TERMINATED 4


typedef struct Thread {
    tid_t tid;
    sem_t sem;
    so_handler* handler;
    unsigned int priority;
    int time;
    int state;
} Thread;

Thread* create_thread(tid_t tid, so_handler* handler, unsigned int priority, int time);
void free_thread(Thread* thread);
void thread_start(void* args);

#endif