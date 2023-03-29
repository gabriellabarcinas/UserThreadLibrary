# PHASE 1: Queue API

## Design Choices
Phase 1 asks us to "implement a simple FIFO queue", so for our implementation we
decided to create a queue via doubly linked list. Each queue contains a count 
of the number of items it contains, and a pointer to the first and last item of
the queue. Each node contains a pointer to its data, the next node in the 
queue, and the previous node in the queue which are helpful for queue 
management and manipulation. 

## Implementation

### queue_create
Allocates memory for a queue and initializes queue struct members. 

### queue_destroy
De-allocates the memory pointed by queue pointer using free. 

### queue_enqueue
Allocates memory for a new node to be added to a queue. Assigns data to the new 
node and adjusts queue properties accordingly. Specifically, increments the 
count of the queue and reassigns the head node to be the new node.

### queue_dequeue
De-allocates memory pointed by the head node in the queue and adjusts queue 
properties accordingly. Specifically, decrements the count of the queue and 
reassigns the head node to its next node.  

### queue_delete
Deletes a specific node in the queue whose data points to a specific memory 
location. Adjusts queue properties according to whether the node was at the 
head, tail, or middle of the queue. 

### queue_iterate
Iterates through all node items of the queue and applies a specific function 
to each node in the queue.

### queue_length
Returns the length of the queue. 

## Testing
To test our queue API, we generated unit tests to asses whether the 
functionality of each of the above functions performed appropriately. For each 
of the test cases, we created a mock data set that would be added, unadded, 
and/or manipulated somehow in the queue. 

### test_queue
To test enqueue and dequeue, we simply added 4 data values via their memory 
addresses and dequeued them one by one to evaluate whether or not the pointers 
to data values were the same as the memory addresses of the data values in the 
correct order.

### test_destroy
To test the destroy function, we just ensured that calling the function while 
the queue still contained elements returned the value of -1 and after deleting 
all elements in the queue, the return value should be 0. 

### test_delete
To test deletion, we assessed the pointers of data values after a deletion 
operation occurred given 3 specific cases. The cases tested deletion of a 
non-head and non-tail item, a head item, and a tail item.

### test_iterate
To test the iteration operation, we assessed whether the pointers to data items 
in the queue evaluated to their appropriate integer values after performing an 
iteration that incremented each of their values and deleting a specific data 
value. 

### test_length
This test just evaluated whether the length of a queue was correct after 
enqueuing and dequeuing.
 
# PHASE 2: Uthread API

## Design Choices
Phase 2 asks us to implement a thread library, which is responsible for creating
and starting new threads, terminating threads, or manipulating threads in 
various ways. For our implementation of the uthread API, we chose to use 
three structs, three state queues: ready, blocked, exited, four states 
represented by macros: run, ready, block, exit, and a stack to implement our 
library.

## Implementation

### uthread_yield
Yields currently running thread to next thread by dequeuing the oldest thread in 
ready queue and setting it to be the newly running thread, changing the state of 
the current running thread to ready, enqueuing the current running thread to the
ready queue, changing the state of the next thread to run, and context switching.

### uthread_exit
Frees the currently running thread and yields to next thread. Follows a similar 
implementation as uthread_yield, except the state of current running thread is 
changed to exit and the thread is enqueued into the exit queue.

### uthread_create
Creates new threads by allocating memory for a new thread, initializing it, 
changing the state to ready, then enqueuing the new thread into the ready queue. 

### uthread_destroy
Frees threads and their associated TCB's in the exited queue.

### uthread_run
Runs the multi-threading library by registering the idle thread, creating the 
initial thread, and executing an infinite loop until the ready queue is empty.

# PHASE 3: Semaphore API

## Design Choices
For our implementation of the semaphore API, we designed a semaphore 
struct with an internal count member and a wait queue. The internal count keeps 
track of the number of available resources and the wait queue stores the threads 
that become blocked when a resource that is requested is not available.

## Implementation

### sem_create
Allocates memory for a semaphore and initializes internal struct member count.

### sem_destroy
De-allocates the memory pointed by the semaphore pointer using free. 

### sem_down
Takes a resource from the semaphore if it is available. If available, the 
resource is considered "consumed" and count is decremented. If not, the 
currently running thread is enqueued into the semaphore wait queue and 
uthread_block is called.

### sem_up
Releases a resource to the semaphore by incrementing semaphore count. If the 
semaphore wait queue is not empty at the time of releasing the resource, the 
first thread is dequeued from the semaphore wait queue and it becomes unblocked 
via uthread_unblock. 

### uthread_block
Follows the same process as uthread_yield except it dequeues the current 
running thread from the ready queue then changes it's state
to blocked. The blocked thread is then enqueued into the blocked queue and the 
state of the next thread is changed to run.

### uthread_unblock
The state of uthread is changed to ready and uthread is removed from the blocked 
queue via queue_delete(). Uthread is then enqueued into the ready queue.

# PHASE 4: Preemption
For our implementation of preemption, we forcefully yield a thread after a 
allotting a certain amount of CPU time. To do so, we install a signal handler 
that receives alarm signals of type SIGVTALRM and utilize timer interrupts to 
determine when to change running threads. Global variables keep track of current
signal action, previous signal action, current timer configuration, previous 
timer configuration, current signal 

## Design Choices
Throughout our uthread API, we implement preempt_disable in critical sections 
where we are allocating memory, de-allocating memory, and performing queue 
operations. This is because malloc is not re-entrant and queue operations are 
constantly adjusting pointers which can be incorrectly manipulated if an 
interrupt were to occur. Otherwise, preemption should be enabled.

## Implementation

### preempt_disable
Blocks signals of type SIGVTALRM.

### preempt_enable
Unlocks signals of type SIGVTALRM.

### sig_handler
Function handler for the timer interrupt. Immediately yields the currently 
running thread by calling uthread_yield.

### preempt_start
If preempt is true, begins preemption by setting up signal handler, blocking, 
and unblocking signals. Additionally, configures a timer to fire an alarm 100 
times per second using setitimer.

### preempt_stop
Restores the previous signal action, timer configuration, and signal set.

## Testing
To test our preemption implementation, we test the creation of 3 threads without
calling uthread_yield explicitly. Thread 1 which is initially passed in 
uthread_run creates thread 2 and thread 2 creates thread 3. Threads 1 and 2 
remain in an infinite while loop, so neither thread should terminate unless 
thread 3 is scheduled and exits. This test should demonstrate that 
uthread_yield is called only when there is a timer interrupt which is handled 
in sig_handler which then calls uthread_yield. 
