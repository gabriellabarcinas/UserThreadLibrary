#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"
#include "context.c"

/* THREAD STATES */
#define T_RUN 0
#define T_READY 1
#define T_BLOCK 2
#define T_EXIT 3

/* THREAD STATE MANAGEMENT */
struct uthread_tcb *running_thread; // currently running thread
queue_t ready_q, blocked_q, exited_q;

typedef struct uthread_tcb {
        /* Phase 2 */
        int state;
        uthread_ctx_t context;
        void *stack;
} uthread_tcb;

struct uthread_tcb *uthread_current(void)
{
        /* Phase 2/3 */
        return running_thread;
}

void uthread_yield(void)
{
        /* Phase 2 */
        preempt_disable();

        uthread_tcb *next_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));

        /* Dequeue oldest thread in ready queue and set it to be the newly running thread */
        queue_dequeue(ready_q, (void **) &next_thread);
        running_thread->state = T_READY;
        queue_enqueue(ready_q, running_thread);

        /* Temporarily store pointer to last running thread */
        uthread_tcb *prev_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));
        prev_thread = running_thread;

        /* Change running thread */
        next_thread->state = T_RUN;
        running_thread = next_thread;
        
        preempt_enable();
        
        uthread_ctx_switch(& prev_thread->context, & running_thread->context);
}

void uthread_exit(void)
{
        /* Phase 2 */
        preempt_disable();
        uthread_tcb *next_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));
        uthread_tcb *prev_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));

        /* Dequeue oldest thread in ready queue */
        queue_dequeue(ready_q, (void **) &next_thread);

        /* Store pointer to last running thread and set to exit state */
        prev_thread = running_thread;
        prev_thread->state = T_EXIT;
        queue_enqueue(exited_q, prev_thread);

        /* Set new running thread */
        next_thread->state = T_RUN;
        running_thread = next_thread;
        
        preempt_enable();

        uthread_ctx_switch(& prev_thread->context, & running_thread->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
        /* Phase 2 */
        preempt_disable();
        int ctx_retval;

        /* Allocate space for new thread and its members */
        uthread_tcb *new_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));
        new_thread->stack = uthread_ctx_alloc_stack();

        if (new_thread == NULL || new_thread->stack == NULL) {
                return -1;
        }

        ctx_retval = uthread_ctx_init(& new_thread->context, new_thread->stack, func, arg);
        if (ctx_retval == -1) {
                return -1;
        }

        new_thread->state = T_READY;
        queue_enqueue(ready_q, new_thread);

        preempt_enable();
        
        return 0;
}

void uthread_destroy(void)
{
        preempt_disable();
        
        uthread_tcb *thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));

        while (queue_length(exited_q) > 0) {
                queue_dequeue(exited_q, (void **) &thread);
                if(thread->stack) {
                        uthread_ctx_destroy_stack(thread->stack);
                }
                free(thread);
        }
        
        preempt_enable();
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
        /* Phase 2 */
        
        /* Start preemption while uthread library is initializing */
        preempt_start(preempt);
        
        /* Create state queues */
        ready_q = queue_create();
        blocked_q = queue_create();
        exited_q = queue_create();

        /* 1. REGISTER IDLE THREAD */
        uthread_tcb *idle_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));
        idle_thread->state = T_RUN;

        /* Allocate memory for the running thread global variable */
        running_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));

        /* Set running thread to idle thread */
        running_thread = idle_thread;

        /* 2. CREATE INITIAL THREAD */
        uthread_create(func, arg);
        
        /* Enable preemption */
        preempt_enable();

        /* 3. EXECUTE INFINITE LOOP WHILE READY QUEUE IS NOT EMPTY */
        while (1) {
                /* Destroy threads and their associated TCB's in exited queue */
                if (queue_length(exited_q) > 0) {
                        uthread_destroy();
                }
                /* Stop idle loop and return if no more threads to execute
                 * otherwise, yield to next available thread if preempt is false 
                 * */
                if (queue_length(ready_q) == 0) {
                        return 0;
                } else {
                        if (!preempt) {
                                uthread_yield();
                        }
                }
        }
        
        preempt_disable();
        
        /* Free memory allocated for queues */
        queue_destroy(ready_q);
        queue_destroy(blocked_q);
        queue_destroy(exited_q);
        
        if (preempt) {
                preempt_stop();
        }

        return 0;
}

void uthread_block(void)
{
        /* Phase 3 */
        /* Disable preemption when entering critical section */
        preempt_disable();

        uthread_tcb *next_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));

        /* Dequeue oldest thread in ready queue and set it to be the newly running thread */
        queue_dequeue(ready_q, (void **) &next_thread);
        /* Change the state of the currently running thread to blocked */
        running_thread->state = T_BLOCK;
        queue_enqueue(blocked_q, running_thread);

        /* Temporarily store pointer to blocked thread */
        uthread_tcb *blocked_thread = (uthread_tcb *) malloc(sizeof(uthread_tcb));
        blocked_thread = running_thread;

        /* Change running thread */
        next_thread->state = T_RUN;
        running_thread = next_thread;

        uthread_ctx_switch(& blocked_thread->context, & running_thread->context);

        preempt_enable();
}


void uthread_unblock(struct uthread_tcb *uthread)
{
        /* Phase 3 */
        /* Disable preemption when entering critical section */
        preempt_disable();

        /* Change uthread state to ready */
        uthread->state = T_READY;
        queue_delete(blocked_q, uthread);

        /* Enqueue uthread back into the ready queue */
        queue_enqueue(ready_q, uthread);
        preempt_enable();
}
