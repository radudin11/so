#include "so_scheduler.h"
#include "scheduler.h"
#include "thread.h"

extern Scheduler* scheduler;

DECL_PREFIX int so_init(unsigned int time_quantum, unsigned int io) {
	if (io > SO_MAX_NUM_EVENTS || time_quantum <=0 || scheduler->is_initialized == 1)
		return -1;
    
    scheduler = create_scheduler(time_quantum, io);

	return 0;
}

DECL_PREFIX tid_t so_fork(so_handler *func, unsigned int priority) {
    if (priority > SO_MAX_PRIO || func == NULL)
        return -1;

    // allocate memory
    Thread *thread = create_thread(0, func, priority, scheduler->quantum);
    if (thread == NULL)
        return -1;

    // try to create a thread in the system
    if (pthread_create(&thread->tid, NULL, (void *) thread_start, thread)) {
        perror("pthread_create");
        free_thread(thread);
        return -1;
    }

    // add it to the queue
    scheduler_add_thread(thread);

    // call the scheduler 
    scheduler_call();

    return thread->tid;
}

DECL_PREFIX void so_exec(void) {
    if (scheduler->running_thread == NULL)
        return;
    
    if (--(scheduler->running_thread->time) == 0)
        scheduler_call();
}

DECL_PREFIX void so_end(void)
{
    // end before init
	if (scheduler == NULL || scheduler->is_initialized == 0)
		return;

	// wait for all threads to finish
    for (int i = 0; i < scheduler->thread_count; i++) {
        pthread_join(scheduler->threads[i]->tid, NULL);
    }

    // free
    free_scheduler();
}

DECL_PREFIX int so_wait(unsigned int io) {
    // if (io >= scheduler->io_num)
    //     return -1;

    // // set the thread to waiting
    // scheduler->running_thread->state = THREAD_STATE_WAITING;
    // scheduler->running_thread->io = io;

    // // call the scheduler
    // scheduler_call();

    return 0;
}

DECL_PREFIX int so_signal(unsigned int io) {
    // if (io >= scheduler->io_num)
    //     return -1;

    // // find the thread that is waiting for the io
    // for (int i = 0; i < scheduler->thread_count; i++) {
    //     if (scheduler->threads[i]->io == io && scheduler->threads[i]->state == THREAD_STATE_WAITING) {
    //         scheduler->threads[i]->state = THREAD_STATE_READY;
    //         enqueue(scheduler->ready_queue[scheduler->threads[i]->priority], scheduler->threads[i]);
    //         break;
    //     }
    // }

    // // call the scheduler
    // scheduler_call();

    return 0;
}


