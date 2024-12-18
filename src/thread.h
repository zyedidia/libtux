#pragma once

#include <assert.h>
#include <pthread.h>

static inline pthread_mutex_t*
lock(pthread_mutex_t* l)
{
    int r = pthread_mutex_lock(l);
    assert(r == 0);
    return l;
}

static inline void
unlock(pthread_mutex_t** l)
{
    int r = pthread_mutex_unlock(*l);
    assert(r == 0);
}

#define LOCK_WITH_DEFER(x, y) pthread_mutex_t* y __attribute__((cleanup(unlock))) = lock(x); (void) y;
