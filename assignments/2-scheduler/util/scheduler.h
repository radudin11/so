#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "thread.h"
#include "queue.h"
#include <semaphore.h>
#include <stdlib.h>

#define MAX_NUM_THREADS 256

typedef struct Scheduler {
    char is_initialized;

	int quantum;
    int io_num;

    Thread* running_thread;
    Thread** threads;
    Queue** ready_queue;
    

    int thread_count;
} Scheduler;

Scheduler* create_scheduler(unsigned int quantum, unsigned int io_num);
void free_scheduler();

void scheduler_add_thread(Thread* thread);

void scheduler_call();
Thread* scheduler_get_next_thread();
int greater_prio_thread();

#endif