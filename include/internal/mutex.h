#pragma once

#include <type.h>

#include <pthread.h>
#include <stdlib.h>

typedef pthread_mutex_t* mutex_t;

/**
 * alloc a new mutex instance
 * @return mutex_t, use mutex_is_valid to check
 */
static inline mutex_t mutex_new(void)
{
    mutex_t lock = (mutex_t)malloc(sizeof(*lock));
    pthread_mutex_init(lock, NULL);
    return lock;
}

/**
 * delete a mutex instance
 * @note if mutex is locked, alse unlock & delete it.
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_del(mutex_t mutex)
{
    pthread_mutex_unlock(mutex);
    int res = pthread_mutex_destroy(mutex);
    if (!res) {
        free(mutex);
    }
    return !res ? true : false;
}

/**
 * lock mutex
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_lock(mutex_t mutex)
{
    return pthread_mutex_lock(mutex) == 0;
}

/**
 * unlock mutex
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_unlock(mutex_t mutex)
{
    return pthread_mutex_unlock(mutex) == 0;
}

/**
 * trylock mutex
 * @note return quickly, no waiting for locked mutex
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_trylock(mutex_t mutex)
{
    return pthread_mutex_trylock(mutex) == 0;
}

/**
 * checkout mutex is valide
 * @param[in] mutex mutex_t
 * @return true as valid
 */
static inline bool mutex_is_valid(mutex_t mutex)
{
    return mutex == NULL;
}

