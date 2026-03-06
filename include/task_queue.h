#ifndef TASK_QUEUE
#define TASK_QUEUE

#include <pthread.h>
#include <stdbool.h>

typedef struct 
{
    void (*function)(void*);
    void* arg;
} task_t;

typedef struct
{
    task_t* tasks;
    int capacity;
    int count;
    int front;
    int rear;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} task_queue_t;

int task_queue_init(task_queue_t* queue, int capacity);
int task_queue_destroy(task_queue_t* queue);
bool is_empty(task_queue_t* queue);
bool is_full(task_queue_t* queue);
int task_queue_enque(task_queue_t* queue, task_t task, int* shutdown);
int task_queue_dequeue(task_queue_t* queue, task_t* task, int* shutdown);

#endif
