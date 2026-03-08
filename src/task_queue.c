#include <stdio.h>
#include <stdlib.h>
#include "../include/task_queue.h"

static void heapify_up(task_queue_t* queue, int index);
static void heapify_down(task_queue_t* queue, int index);
static void swap(task_t* a, task_t* b)
{
    task_t temp = *a;
    *a = *b;
    *b = temp;
}

int task_queue_init(task_queue_t* queue, int capacity)
{
    if (!queue || capacity <= 0) return -1;  // Add validation
    queue->tasks = malloc(sizeof(task_t) * capacity);
    if (!queue->tasks) return -1;
    queue->capacity = capacity;
    queue->count = 0;

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    return 0;
}

int task_queue_destroy(task_queue_t* queue)
{
    if (!queue) return -1;  // Add validation
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    free(queue->tasks);
    return 0;
}

bool is_empty(task_queue_t* queue)
{
    if (!queue) return true;  // Add validation
    return queue->count == 0;
}

bool is_full(task_queue_t* queue)
{
    if (!queue) return false;  // Add validation
    return queue->capacity == queue->count;
}

static void heapify_up(task_queue_t* queue, int childIndex)
{
    while(childIndex>0)
    {
        int parentIndex = (childIndex-1)/2;
        if (queue->tasks[parentIndex].priority >= queue->tasks[childIndex].priority)
            break;

        swap(&queue->tasks[parentIndex], &queue->tasks[childIndex]);
        childIndex = parentIndex;
    }
}

static void heapify_down(task_queue_t* queue, int parentIndex)
{
    while (1)
    {
        int leftChildIndex = 2 * parentIndex + 1;
        int rightChildIndex = 2 * parentIndex + 2;
        int largest = parentIndex;

        if (leftChildIndex < queue->count &&
            queue->tasks[leftChildIndex].priority > queue->tasks[largest].priority)
            largest = leftChildIndex;

        if (rightChildIndex < queue->count &&
            queue->tasks[rightChildIndex].priority > queue->tasks[largest].priority)
            largest = rightChildIndex;

        if (largest == parentIndex)
            break;

        swap(&queue->tasks[parentIndex], &queue->tasks[largest]);
        parentIndex = largest;
    }
}

int task_queue_enque(task_queue_t* queue, task_t task, int* shutdown)
{
    if (!queue || !shutdown) return -1;  // Add validation
    pthread_mutex_lock(&queue->mutex);
    while (is_full(queue) && !(*shutdown)) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }
    if (*shutdown) {
        pthread_mutex_unlock(&queue->mutex);
        return -1;  // Fixed: Return immediately on shutdown
    }
    queue->tasks[queue->count] = task;
    heapify_up(queue,queue->count);
    queue->count++;
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

int task_queue_dequeue(task_queue_t* queue, task_t* task, int* shutdown)
{
    if (!queue || !task || !shutdown) return -1;  // Add validation
    pthread_mutex_lock(&queue->mutex);
    while (is_empty(queue) && !(*shutdown)) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);  // Fixed: Wait on not_empty
    }
    if (*shutdown && is_empty(queue)) {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }
    *task = queue->tasks[0];
    queue->count--;
    queue->tasks[0] = queue->tasks[queue->count];
    heapify_down(queue,0);
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}
