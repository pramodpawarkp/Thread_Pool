#include <stdio.h>
#include <stdlib.h>
#include "../include/thread_pool.h"

static void* worker_thread(void* arg)
{
    thread_pool_t* pool = (thread_pool_t*)arg;
    task_t task;

    while (1) {
        if (task_queue_dequeue(&pool->queue, &task, &pool->shutdown) != 0) {
            break;  // shutdown and queue empty
        }

        task.function(task.arg);
    }

    return NULL;
}

int thread_pool_init(thread_pool_t* pool, int num_threads, int queue_capacity)
{
    pool->thread_count = num_threads;
    pool->shutdown = 0;  
    pool->threads = malloc(sizeof(pthread_t)*pool->thread_count);
    if(!pool->threads)
    {
        return -1;
    }

    if(task_queue_init(&pool->queue,queue_capacity)!=0)
    {
        return -1;
    }

    for(int i=0;i<num_threads;i++)
    {
        if(pthread_create(&pool->threads[i],NULL,worker_thread,pool)!=0)
        {
            return -1;
        }
    }
    return 0;
}

int thread_pool_submit(thread_pool_t* pool, void (*function)(void*), void* arg)
{
    task_t task;
    task.arg = arg;
    task.function = function;

    return task_queue_enque(&pool->queue,task,&pool->shutdown);
}

void thread_pool_shutdown(thread_pool_t* pool)
{
    pthread_mutex_lock(&pool->queue.mutex);

    pool->shutdown = 1;

    pthread_cond_broadcast(&pool->queue.not_empty);
    pthread_cond_broadcast(&pool->queue.not_full);

    pthread_mutex_unlock(&pool->queue.mutex);

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    task_queue_destroy(&pool->queue);
    free(pool->threads);
}