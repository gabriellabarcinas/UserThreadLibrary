#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
    /* Phase 3 */
    size_t count;
    queue_t wait_q;
};

sem_t sem_create(size_t count)
{
    /* Phase 3 */
    /* Disable preemption when entering critical section */
    preempt_disable();
    sem_t sem = (sem_t) malloc(sizeof(struct semaphore));

    /* Return NULL if sem is NULL */
    if (sem == NULL){
        return NULL;
    }
    preempt_enable();
    sem->wait_q = queue_create();
    sem->count = count;

    return sem;
}

int sem_destroy(sem_t sem)
{
    /* Phase 3 */
    /* Disable preemption when entering critical section */
    preempt_disable();
    
    /* Return -1 if sem is NULL or if other threads are still being blocked on sem */
    if (sem == NULL || queue_destroy(sem->wait_q) == -1){
        return -1;
    }
    preempt_enable();
    free(sem);

    return 0;
}

int sem_down(sem_t sem)
{
    /* Phase 3 */
    /* Return -1 if sem is NULL */
    if (sem == NULL){
        return -1;
    }
    /* When the requested resource is not available, the current 
     * running thread is added into the wait queue and blocked
     */
    while (sem->count == 0){
        struct uthread_tcb *running_thread = uthread_current();
        queue_enqueue(sem->wait_q, running_thread);
        uthread_block();
    }
    /* Take resource */
    if (sem->count > 0){
    	sem->count--;
    }
    
    return 0;
}

int sem_up(sem_t sem)
{
    /* Phase 3 */
    /* Return -1 if sem is NULL */
    if (sem == NULL){
        return -1;
    }
    /* If the wait queue is not empty at the time of release,
     * the first thread in the wait queue is unblocked 
     */
    if (queue_length(sem->wait_q) > 0){
        struct uthread_tcb *head;
        if (queue_dequeue(sem->wait_q, (void**) &head) == -1){
            return -1;
        }
        uthread_unblock(head);
    }
    /* Release resource */
    sem->count++;

    return 0;
}

