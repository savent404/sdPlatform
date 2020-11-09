#pragma once

#include <internal/type.h>

typedef int mutex_t;

static inline mutex_t mutex_new(void) { return 0; }
static inline mutex_t mutex_del(mutex_t mutex) { return 0; }
static inline int mutex_lock(mutex_t mutex) { return 0; }
static inline int mutex_unlock(mutex_t mutex) { return 0; }
static inline int mutex_trylock(mutex_t mutex) { return 0; }
static inline bool mutex_is_valid(mutex_t mutex) { return true; }
