#ifndef THREAD_POOL
#define THREAD_POOL
#include <pthread.h>
#include "../include/task_queue.h"

typedef struct
{
    int thread_count;
    pthread_t* threads;
    
    task_queue_t queue;
    int shutdown;
} thread_pool_t;

int thread_pool_init(thread_pool_t* pool, int num_threads, int queue_capacity);
future_t* thread_pool_submit(thread_pool_t* pool, void* (*function)(void*), void* arg);
void thread_pool_shutdown(thread_pool_t* pool);
#endif
