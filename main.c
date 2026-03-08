#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>    // added
#include "include/thread_pool.h"

void* square(void* arg)
{
    int value = *(int*)arg;
    int* result = malloc(sizeof(int));
    printf("Value: %d started (thread %lu)\n", value, (unsigned long)pthread_self()); // cast added
    *result = value*value;
    sleep(1);  // simulate work
    printf("Result: %d finished (thread %lu)\n", *result , (unsigned long)pthread_self()); // cast added

    return result;
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

    int value_arr[10];

    for (int i = 1; i <= 10; i++) {
        value_arr[i-1] = i;
        printf("Submitting task %d\n", i);

        future_t* future = thread_pool_submit(&pool, square, &value_arr[i-1]);
        if(future==NULL)
        {
           printf("Task %d rejected (shutdown or full)\n", i); 
        }

        int* result = future_get_result(future);

        printf("Result = %d\n", *result);

        free(result);
        future_destroy(future);
    }

    sleep(1);  // allow some tasks to complete

    printf("\nInitiating shutdown...\n");
    thread_pool_shutdown(&pool);

    printf("Thread pool shutdown complete\n");
    return 0;
}