#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#define TEST_ASSERT(assert)                             \
do {                                                                    \
        printf("ASSERT: " #assert " ... ");     \
        if (assert) {                                           \
                printf("PASS\n");                               \
        } else  {                                                       \
                printf("FAIL\n");                               \
                exit(1);                                                \
        }                                                                       \
} while(0)

/* Destory */
void test_destroy(void)
{
	int data[] = {3, 4}, retval;
        queue_t q;

        fprintf(stderr, "*** TEST destroy ***\n");

        q = queue_create();
        queue_enqueue(q, &data[0]);
        queue_enqueue(q, &data[1]);

        retval = queue_destroy(q);
        TEST_ASSERT(retval == -1);

        queue_delete(q, &data[0]);
        queue_delete(q, &data[1]);
        retval = queue_destroy(q);
        TEST_ASSERT(retval == 0);
}	

/* Enqueue and Dequeue */
void test_queue(void)
{
        int data[] = {3, 4, 5, 6}, *ptr, retval;
        queue_t q;

        fprintf(stderr, "*** TEST enqueue ***\n");

        q = queue_create();
        queue_enqueue(q, &data[0]);
        queue_enqueue(q, &data[1]);
        queue_enqueue(q, &data[2]);
        queue_enqueue(q, &data[3]);

        queue_dequeue(q, (void**) &ptr);
        TEST_ASSERT(ptr ==  &data[0]);
        queue_dequeue(q, (void**) &ptr);
        TEST_ASSERT(ptr ==  &data[1]);
        queue_dequeue(q, (void**) &ptr);
        TEST_ASSERT(ptr == &data[2]);
        queue_dequeue(q, (void**) &ptr);
        TEST_ASSERT(ptr == &data[3]);

        retval = queue_dequeue(q, (void**)&ptr);
        TEST_ASSERT(retval == -1);
}

/* Delete */
void test_delete(void)
{
	int data[] = {1, 2, 3, 4, 5, 6, 7}, *ptr, retval;
        queue_t q;

        fprintf(stderr, "*** TEST delete ***\n");

        q = queue_create();
        queue_enqueue(q, &data[0]);
        queue_enqueue(q, &data[1]);
        queue_enqueue(q, &data[2]);
        queue_enqueue(q, &data[3]);
        queue_enqueue(q, &data[4]);
        queue_enqueue(q, &data[5]);
        queue_enqueue(q, &data[6]);

        /* Delete non-head and non-tail item in queue
         * q = {1, 2, 3, 4, 5, 6, 7}
         */
        queue_delete(q, &data[1]);
        queue_dequeue(q, (void **) &ptr);
        TEST_ASSERT(*ptr == 1);
        queue_dequeue(q, (void **) &ptr);
        TEST_ASSERT(*ptr == 3);

        /* Delete oldest item, 4, in queue
         * q = {4, 5, 6, 7}
         */
        queue_delete(q, &data[3]);
        queue_dequeue(q, (void **) &ptr);
        TEST_ASSERT(*ptr == 5);

        /* Delete newest item, 7, in queue
        * q = {6, 7}
        */
        queue_delete(q, &data[6]);
        queue_dequeue(q, (void **) &ptr);
        TEST_ASSERT(*ptr == 6);
        retval = queue_dequeue(q, (void**) &ptr);
        TEST_ASSERT(retval == -1);
}

static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

/* Iterate */
void test_iterate(void)
{
	queue_t q;
        int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
        size_t i;

        fprintf(stderr, "*** TEST iterate ***\n");

        /* Initialize the queue and enqueue items */
        q = queue_create();
        for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
                queue_enqueue(q, &data[i]);
        }

        /* Increment every item of the queue, delete item '42' */
        queue_iterate(q, (queue_func_t)iterator_inc);
        TEST_ASSERT(queue_length(q) == 9);
        TEST_ASSERT(data[0] == 2);
        TEST_ASSERT(data[1] == 3);
        TEST_ASSERT(data[2] == 4);
        TEST_ASSERT(data[3] == 5);
        TEST_ASSERT(data[4] == 6);
        TEST_ASSERT(data[5] == 42);
        TEST_ASSERT(data[6] == 7);
        TEST_ASSERT(data[7] == 8);
        TEST_ASSERT(data[8] == 9);
        TEST_ASSERT(data[9] == 10);	
}

/* Length */
void test_length(void)
{
	int data[] = {3, 4, 5, 6}, *ptr;
        queue_t q;

        fprintf(stderr, "*** TEST length ***\n");

        q = queue_create();
        queue_enqueue(q, &data[0]);
        queue_enqueue(q, &data[1]);
        queue_enqueue(q, &data[2]);
        queue_enqueue(q, &data[3]);

        TEST_ASSERT(queue_length(q) == 4);
        queue_dequeue(q, (void**)&ptr);
        TEST_ASSERT(queue_length(q) == 3);
        queue_dequeue(q, (void**)&ptr);
        TEST_ASSERT(queue_length(q) == 2);
        queue_dequeue(q, (void**)&ptr);
        TEST_ASSERT(queue_length(q) == 1);
        queue_dequeue(q, (void**)&ptr);
        TEST_ASSERT(queue_length(q) == 0);

}

int main(void)
{
        test_queue();
        test_delete();
        test_destroy();
        test_length();
        test_iterate();
	
        return 0;
}
