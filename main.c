#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>    // added
#include "include/thread_pool.h"

void sample_task(void* arg)
{
    int id = *(int*)arg;
    printf("Task %d started (thread %lu)\n", id, (unsigned long)pthread_self()); // cast added

    usleep(100000);  // simulate work

    printf("Task %d finished (thread %lu)\n", id, (unsigned long)pthread_self()); // cast added
}

int main()
{
    thread_pool_t pool;

    int num_threads = 3;
    int queue_capacity = 5;

    if (thread_pool_init(&pool, num_threads, queue_capacity) != 0) {
        fprintf(stderr, "Failed to initialize thread pool\n");
        return 1;
    }

    int task_ids[1000];

    for (int i = 0; i < 1000; i++) {
        task_ids[i] = i;
        printf("Submitting task %d\n", i);

        if (thread_pool_submit(&pool, sample_task, &task_ids[i]) != 0) {
            printf("Task %d rejected (shutdown or full)\n", i);
        }
    }

    usleep(10000);  // allow some tasks to complete

    printf("\nInitiating shutdown...\n");
    thread_pool_shutdown(&pool);

    printf("Thread pool shutdown complete\n");
    return 0;
}