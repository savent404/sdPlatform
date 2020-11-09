#pragma once

#include <internal/type.h>

typedef int mutex_t;

/**
 * alloc a new mutex instance
 * @return mutex_t, NULL or 0 means failed
 */
static inline mutex_t mutex_new(void) { return 0; }

/**
 * delete a mutex instance
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_del(mutex_t mutex) { return 0; }

/**
 * lock mutex
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_lock(mutex_t mutex) { return 0; }

/**
 * unlock mutex
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_unlock(mutex_t mutex) { return 0; }

/**
 * trylock mutex
 * @note return quickly, no waiting for locked mutex
 * @param[in] mutex mutex_t
 * @return true as success
 */
static inline bool mutex_trylock(mutex_t mutex) { return 0; }

/**
 * checkout mutex is valide
 * @param[in] mutex mutex_t
 * @return true as valid
 */
static inline bool mutex_is_valid(mutex_t mutex) { return true; }
