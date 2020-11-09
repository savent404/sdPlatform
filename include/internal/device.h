#pragma once

#include <internal/btree.h>
#include <internal/toolchain.h>
#include <internal/type.h>
#include <cJSON.h>


// treat this as a json object
typedef struct device_env
{
    const char *name;
    const char *compat;
    const char *_json_str;
    void *_json_obj;
} * device_env_t;

typedef void* device_data_t;

typedef struct device
{
    device_env_t d_env;   // device init info
    device_data_t d_data; // device runtime info
    driver_id_t d_driver; // opinter to driver

    device_id_t d_self;
    device_id_t d_parent;
    device_id_t d_child;
    device_id_t d_prev;
    device_id_t d_next;
} * device_t;

/**
 * @brief checkout if device is binded
 * @param[in] dev device_t
 * @return true as binded
 * @note if device is not binded, dev->d_data must be NULL
 */
static inline bool
dev_is_binded(device_t dev)
{
    return dev && dev->d_data;
}

/**
 * @brief checkout if device id registered
 * @param[in] dev device_t
 * @return true as registered
 * @note if device is not registered, dev->d_self must be 0
 */
static inline bool
dev_is_registered(device_t dev)
{
    return dev->d_self == 0;
}

/**
 * @brief get device structure from device_id
 * @param[in] dev_id device_id_t
 * @return device_t
 * @note TODO(savent): implement a method to get device struct
 */
static inline device_t
dev_by_id(device_id_t devid)
{
    return NULL;
}

/**
 * @brief get device's id
 * @return device_it_t
 */
static inline device_id_t
dev_id(device_t dev)
{
    return dev && dev->d_self ? dev->d_self : 0;
}

/**
 * @brief return cJSON obj from d_env
 * @param[in] dev
 * @return cJSON*, NULL means failed
 */
static inline cJSON*
dev_env_json(device_t dev)
{
    if (!dev || !dev->d_env || !dev->d_env->_json_obj)
        return NULL;
    return dev->d_env->_json_obj;
}

/**
 * @brief return json string from d_env
 * @param[in] dev
 * @return string, NULL means failed
 */
static inline const char *
dev_env_string(device_t dev)
{
    if (!dev || !dev->d_env || !dev->d_env->_json_str)
    return NULL;
    return dev->d_env->_json_str;
}

/**
 * @brief parse dev env's json string to cJSON obj
 * @param[in] dev device_t
 * @note output see 'dev->d_env->_json_obj' and method 'dev_env_json'
 * @return true as success
 */
static inline bool
dev_env_parse(device_t dev)
{
    cJSON *c = NULL;
    if (!dev || !dev->d_env || !dev->d_env->_json_str)
        return false;
    c = cJSON_Parse(dev->d_env->_json_str);
    if (c == NULL)
        return false;
    if (dev_env_json(dev)) {
        cJSON *obj = dev_env_json(dev);
        dev->d_env->_json_obj = NULL;
        cJSON_Delete(obj);
    }
    dev->d_env->_json_obj = c;
    return true;
}

/**
 * @brief dump dev env's cJSON obj to json string
 * @param[in] dev device_t
 * @note output see 'dev->d_env->_json_str' and method 'dev_env_string'
 * @return true as success
 */
static inline bool
dev_env_dump(device_t dev)
{
    if (!dev || !dev->d_env || !dev->d_env->_json_obj)
        return false;
    const char *c = cJSON_Print(dev->d_env->_json_obj);
    if (c == NULL)
        return false;
    if (dev_env_string(dev)) {
        const void *str = dev_env_string(dev);
        dev->d_env->_json_str;
        cJSON_free((void*)str);
    }
    dev->d_env->_json_str = c;
    return c != NULL;
}


