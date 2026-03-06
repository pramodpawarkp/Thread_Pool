#include <stdio.h>
#include <stdlib.h>
#include "../include/task_queue.h"

int task_queue_init(task_queue_t* queue, int capacity)
{
    queue->tasks = malloc(sizeof(task_t)*capacity);
    if(!queue->tasks)
    {
        return -1;
    }
    queue->capacity = capacity;
    queue->count = 0;
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    return 0;
}

int task_queue_destroy(task_queue_t* queue)
{
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    free(queue->tasks);
    return 0;
}

bool is_empty(task_queue_t* queue)
{
    return queue->count == 0;
}

bool is_full(task_queue_t* queue)
{
    return queue->capacity == queue->count;
}

int task_queue_enque(task_queue_t* queue, task_t task, int* shutdown)
{
    pthread_mutex_lock(&queue->mutex);
    while(is_full(queue) && !(*shutdown))
    {
        pthread_cond_wait(&queue->not_full,&queue->mutex);
    }

    if(*shutdown)
    {
        pthread_mutex_unlock(&queue->mutex);
    }

    queue->tasks[queue->front] = task;
    queue->front = (queue->front+1) % queue->capacity;
    queue->count ++;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

int task_queue_dequeue(task_queue_t* queue, task_t* task, int* shutdown)
{
    pthread_mutex_lock(&queue->mutex);
    while(is_empty(queue) && !(*shutdown))
    {
        pthread_cond_wait(&queue->not_full,&queue->mutex);
    }

    if(*shutdown  && is_empty(queue))
    {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }

    *task = queue->tasks[queue->rear];
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->count--;
 
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);

    return 0;
}
 