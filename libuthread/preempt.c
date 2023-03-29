#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define MICROSEC 1000000

static struct sigaction sa, prev_sa;
static struct itimerval it, prev_it;
static sigset_t ss, prev_ss;

void preempt_disable(void)
{
        /* TODO Phase 4 */
        sigprocmask(SIG_BLOCK, &ss, NULL);
}

void preempt_enable(void)
{
        /* TODO Phase 4 */
        sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void sig_handler(int signum) {
        if(signum == SIGVTALRM) {
                uthread_yield();
        }
}

void preempt_start(bool preempt)
{
        /* TODO Phase 4 */
        if (preempt) {
                /* 1. Set up signal handler that recieves alarm signals */
                sa.sa_handler = sig_handler;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags = 0;
                sigaction(SIGVTALRM, &sa, &prev_sa);

                /* Set up block and unblocking signals */
                sigemptyset(&ss);
                sigaddset(&ss, SIGVTALRM);
                sigprocmask(SIG_SETMASK, NULL, &prev_ss);

                /* 2. Configure a timer to fire alarm 
                 * it_interval: interval for periodic timer 
                 * it_value: time until next expiration
                 * tv_sec: seconds 
                 * tv_usec: microseconds 
                 * 1000000 microseconds in a second */
                it.it_interval.tv_sec = 0;
                it.it_interval.tv_usec = MICROSEC / HZ;
                it.it_value.tv_sec = 0;
                it.it_value.tv_usec = MICROSEC / HZ;

                if (setitimer(ITIMER_VIRTUAL, &it, &prev_it) == -1) {
                        perror("setitimer");
                        exit(1);
                }
        }
}

void preempt_stop(void)
{
        /* TODO Phase 4 */
        setitimer(SIGVTALRM, &prev_it, NULL);
        sigaction(SIGVTALRM, &prev_sa, NULL);
        sigprocmask(SIG_SETMASK, &prev_ss, NULL);
}