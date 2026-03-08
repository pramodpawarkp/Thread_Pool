#include "future.h"
#include <stdlib.h>

future_t* future_create()
{
    future_t* f = malloc(sizeof(future_t));
    f->result = NULL;
    f->ready = 0;

    pthread_mutex_init(&f->mutex, NULL);
    pthread_cond_init(&f->cond, NULL);

    return f;
}

void future_set_result(future_t* f, void* result)
{
    pthread_mutex_lock(&f->mutex);

    f->result = result;
    f->ready = 1;

    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}

void* future_get_result(future_t* f)
{
    pthread_mutex_lock(&f->mutex);

    while (!f->ready)
        pthread_cond_wait(&f->cond, &f->mutex);

    void* result = f->result;

    pthread_mutex_unlock(&f->mutex);
    return result;
}

void future_destroy(future_t* f)
{
    pthread_mutex_destroy(&f->mutex);
    pthread_cond_destroy(&f->cond);
    free(f);
}