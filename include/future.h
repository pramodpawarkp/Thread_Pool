#ifndef FUTURE_H
#define FUTURE_H

#include <pthread.h>

typedef struct {
    void* result;
    int ready;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} future_t;

future_t* future_create();
void future_set_result(future_t* f, void* result);
void* future_get_result(future_t* f);
void future_destroy(future_t* f);

#endif