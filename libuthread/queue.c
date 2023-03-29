#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"

typedef struct node* node_t;
struct node {
        void* data;
        node_t next;
        node_t prev;
};

struct queue {
        /* Phase 1 */
        int count;
        node_t head;
        node_t tail;
};

queue_t queue_create(void)
{
        /* Phase 1 */
        queue_t queue = (queue_t)malloc(sizeof(struct queue));

        queue->count = 0;
        queue->head = NULL;
        queue->tail = NULL;

        return queue;
}

int queue_destroy(queue_t queue)
{
        /* Phase 1 */
        if (queue == NULL || queue->count != 0) {
                return -1;
        } else {
                free(queue);
                queue = NULL; // prevent dangling pointer to queue
                return 0;
        }
}

int queue_enqueue(queue_t queue, void *data)
{
        /* Phase 1 */
        node_t new_node = (node_t)malloc(sizeof(struct node));

        if (queue == NULL || data == NULL || new_node == NULL) {
                return -1;
        }

        new_node->data = data;
        new_node->next = NULL;
        new_node->prev = NULL;

        if (queue->count == 0) {
                queue->head = new_node;
                queue->tail = new_node;
        } else {
                new_node->prev = queue->tail;
                queue->tail->next = new_node;
                queue->tail = new_node;
        }
        queue->count++;

        return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
        /* Phase 1 */
        if (queue == NULL || data == NULL || queue->count == 0) {
                return -1;
        }

        node_t node = queue->head;
        *data = queue->head->data; // Assign data of head node to data

        if (queue->head == queue->tail) {
                queue->head = NULL;
                queue->tail = NULL;
        } else {
                queue->head = queue->head->next; // Change head node in queue to the next item in queue
                queue->head->prev = NULL;
        }
        queue->count--;
        free(node); // Free allocated memory

        return 0;
}

int queue_delete(queue_t queue, void *data)
{
        /* Phase 1 */
        if (queue == NULL || data == NULL) {
                return -1;
        }

        node_t node = queue->head;
        while(node != NULL) {
                if (node->data == data) {
                        if (queue->count == 0) { // If there is only one item in queue
                                queue->head = NULL;
                                queue->tail = NULL;
                        } else if (node == queue->head) { // If there is > 1 items in queue and deleting head item
                                queue->head = node->next;
                        } else if (node == queue->tail){ // If there is > 1 item in queue and removing tail item
                                queue->tail = queue->tail->prev;
                                queue->tail->next = NULL;
                        } else { // If there is > 1 items in queue and removing non head and non tail item
                                node->prev->next = node->next;
                                node->next->prev = node->prev;
                        }
                        free(node);
                        node = NULL; // prevent dangling pointers
                        queue->count--;
                        return 0;
                }
                node = node->next;
        }

        return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
        /* Phase 1 */
        if (queue == NULL || func == NULL) {
                return -1;
        }

        node_t node = queue->head;
        while (node != NULL) {
                node_t next = node->next; // keep track of next node in case deleted
                func(queue, node->data);
                node = next;
        }
        return 0;
}

int queue_length(queue_t queue)
{
        /* Phase 1 */
        if (queue == NULL) {
                return -1;
        }

        return queue->count;
}
