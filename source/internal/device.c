#include <cJSON.h>
#include <internal/btree.h>
#include <internal/device.h>

bool
dev_is_binded(device_t dev)
{
    return dev && dev->d_data;
}

bool
dev_is_registered(device_t dev)
{
    return dev->d_self == 0;
}

device_t
dev_by_id(device_id_t devid)
{
    return NULL;
}

device_id_t
dev_id(device_t dev)
{
    return dev && dev->d_self ? dev->d_self : 0;
}

cJSON*
dev_env_json(device_t dev)
{
    if (!dev || !dev->d_env || !dev->d_env->_json_obj)
        return NULL;
    return dev->d_env->_json_obj;
}

const char*
dev_env_string(device_t dev)
{
    if (!dev || !dev->d_env || !dev->d_env->_json_str)
        return NULL;
    return dev->d_env->_json_str;
}

bool
dev_env_parse(device_t dev)
{
    cJSON* c = NULL;
    if (!dev || !dev->d_env || !dev->d_env->_json_str)
        return false;
    c = cJSON_Parse(dev->d_env->_json_str);
    if (c == NULL)
        return false;
    if (dev_env_json(dev)) {
        cJSON* obj = dev_env_json(dev);
        dev->d_env->_json_obj = NULL;
        cJSON_Delete(obj);
    }
    dev->d_env->_json_obj = c;
    return true;
}

bool
dev_env_dump(device_t dev)
{
    if (!dev || !dev->d_env || !dev->d_env->_json_obj)
        return false;
    const char* c = cJSON_Print(dev->d_env->_json_obj);
    if (c == NULL)
        return false;
    if (dev_env_string(dev)) {
        const void* str = dev_env_string(dev);
        dev->d_env->_json_str;
        cJSON_free((void*)str);
    }
    dev->d_env->_json_str = c;
    return c != NULL;
}

driver_id_t
dev_get_driver_id(device_t dev)
{
    return dev->d_driver;
}

driver_id_t
dev_set_driver_id(device_t dev, driver_id_t dri_id)
{
    if (!dev)
        return 0;
    dev->d_driver = dri_id;
}

void*
dev_get_driver(device_t dev)
{
    return dev ? dev->_d_driver : NULL;
}

void
dev_set_driver(device_t dev, void* dri)
{
    if (!dev)
        return;
    dev->_d_driver = dri;
}

void*
dev_get_data(device_t dev)
{
    return dev ? dev->d_data : NULL;
}

void
dev_set_data(device_t dev, device_data_t data)
{
    if (!dev)
        return;
    dev->d_data = data;
}

