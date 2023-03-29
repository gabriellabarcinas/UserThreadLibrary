/* Preemption test */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <uthread.h>

void thread3(void *arg)
{
        (void)arg;
        printf("In thread3\n");
        exit(EXIT_SUCCESS);
}
               
void thread2(void *arg)
{
        (void)arg;
        uthread_create(thread3, NULL);
        printf("In thread2\n");
        
        while(1) {}
}

void thread1(void *arg)
{
        (void)arg;
        uthread_create(thread2, NULL);
        printf("In thread1\n");

        while (1) {}
}

int main(void)
{
        uthread_run(true, thread1, NULL);
        return 0;
}
