#include "scheduler.h"
#include "so_scheduler.h"

Scheduler* scheduler;

Scheduler* create_scheduler(unsigned int quantum, unsigned int io_num) {
    Scheduler* new_scheduler = (Scheduler*)malloc(sizeof(Scheduler));
    new_scheduler->quantum = quantum;
    new_scheduler->io_num = io_num;
    new_scheduler->is_initialized = 1;

    new_scheduler->threads = malloc(sizeof(Thread*) * MAX_NUM_THREADS);
    if (new_scheduler->threads == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    new_scheduler->running_thread = NULL;

    new_scheduler->ready_queue = malloc(sizeof(Queue*) * SO_MAX_PRIO);
    if (new_scheduler->ready_queue == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < SO_MAX_PRIO; i++) {
        new_scheduler->ready_queue[i] = create_queue();
    }
    return new_scheduler;
}

void free_scheduler() {
    for (int i = 0; i < SO_MAX_PRIO; i++) {
        destroy_queue(scheduler->ready_queue[i]);
    }
    free(scheduler->ready_queue);

    for (int i = 0; i < scheduler->thread_count; i++) {
        free_thread(scheduler->threads[i]);
    }
    free(scheduler);
}

void scheduler_add_thread(Thread* thread) {
    scheduler->threads[scheduler->thread_count++] = thread;

    thread->state = THREAD_STATE_READY;
    enqueue(scheduler->ready_queue[thread->priority], thread);
}

void scheduler_call() {
    // No running thread
    if (scheduler->running_thread == NULL) {
        scheduler->running_thread = scheduler_get_next_thread();

        if (scheduler->running_thread == NULL) {
            return;
        }

        scheduler->running_thread->state = THREAD_STATE_RUNNING;

        sem_post(&scheduler->running_thread->sem);

        return;
    }

    
    // running_thread thread is terminated or waiting
    if (scheduler->running_thread->state == THREAD_STATE_TERMINATED || scheduler->running_thread->state == THREAD_STATE_WAITING) {
        scheduler->running_thread = scheduler_get_next_thread();

        if (scheduler->running_thread == NULL) {
            return;
        }

        scheduler->running_thread->state = THREAD_STATE_RUNNING;

        sem_post(&scheduler->running_thread->sem);

        return;
    }

    // There is already a running thread

    if (scheduler->running_thread->time == 0 || greater_prio_thread()) {
        // if the running thread has no more time or there is a thread with higher priority
        scheduler->running_thread->state = THREAD_STATE_READY;
        scheduler->running_thread->time = scheduler->quantum;

        enqueue(scheduler->ready_queue[scheduler->running_thread->priority], scheduler->running_thread);
        scheduler->running_thread = scheduler_get_next_thread();

        if (scheduler->running_thread == NULL) {
            return;
        }

        scheduler->running_thread->state = THREAD_STATE_RUNNING;

        sem_post(&scheduler->running_thread->sem);

        return;
    }
}

Thread* scheduler_get_next_thread() {
    for (int i = SO_MAX_PRIO - 1; i >= 0; i--) {
        if (scheduler->ready_queue[i]->size > 0) {
            return (Thread *) dequeue(scheduler->ready_queue[i]);
        }
    }
    return NULL;
}

int greater_prio_thread() {
    for (unsigned int i = SO_MAX_PRIO - 1; i > scheduler->running_thread->priority; i--) {
        if (scheduler->ready_queue[i]->size > 0) {
            return 1;
        }
    }
    return 0;
}