#include "thread.h"
#include "scheduler.h"

Thread* create_thread(tid_t tid, so_handler* handler, unsigned int priority, int time) {
    Thread* thread = (Thread*)malloc(sizeof(Thread));
    if (thread == NULL) {
        perror("malloc");
        return NULL;
    }

    thread->tid = tid;
    thread->handler = handler;
    thread->priority = priority;
    thread->time = time;
    thread->state = THREAD_STATE_NEW;

    if (sem_init(&thread->sem, 0, 0)) {
        perror("sem_init");
        free_thread(thread);
        return NULL;
    }

    return thread;
}

void free_thread(Thread* thread) {
    free(thread);
}

void thread_start(void* args) {
    Thread* thread = (Thread*)args;

    sem_wait(&thread->sem);

    // ?
    thread->handler(thread->priority);

    thread->state = THREAD_STATE_TERMINATED;

    scheduler_call();
}