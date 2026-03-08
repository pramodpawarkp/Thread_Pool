#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>  // Added for srand/time
#include "include/thread_pool.h"

void* square(void* arg)
{
    int value = *(int*)arg;
    int* result = malloc(sizeof(int));
    printf("Value: %d started (thread %lu)\n", value, (unsigned long)pthread_self());
    *result = value * value;
    sleep(1);  // simulate work
    printf("Result: %d finished (thread %lu)\n", *result, (unsigned long)pthread_self());

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

    srand(time(NULL));  // Seed random number generator

    int value_arr[10];
    future_t* futures[10];  // Array to store futures
    int priorities[10];     // Array to store priorities for verification

    for (int i = 1; i <= 10; i++) {
        value_arr[i - 1] = i;
        int priority = rand() % 10 + 1;  // Generate random priority 1-10
        priorities[i - 1] = priority;    // Store for later check
        printf("Submitting task %d with priority %d\n", i, priority);

        future_t* future = thread_pool_submit(&pool, square, &value_arr[i - 1], priority);
        if (future == NULL) {
            printf("Task %d rejected (shutdown or full)\n", i);
            priorities[i - 1] = -1;  // Mark as rejected
            continue;
        }
        futures[i - 1] = future;
    }

    // Collect and print results later (after all submissions)
    printf("\nCollecting results:\n");
    for (int i = 0; i < 10; i++) {
        if (priorities[i] == -1) {
            printf("Task %d was rejected\n", i + 1);
            continue;
        }
        int* result = (int*)future_get_result(futures[i]);
        printf("Task %d (priority %d): Result = %d\n", i + 1, priorities[i], *result);
        free(result);
        future_destroy(futures[i]);
    }

    sleep(1);  // Allow remaining tasks to complete

    printf("\nInitiating shutdown...\n");
    thread_pool_shutdown(&pool);

    printf("Thread pool shutdown complete\n");
    return 0;
}