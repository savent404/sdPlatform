#include <internal/devmgr.h>
#include <internal/driver.h>
#include <internal/device.h>
#include <devmgr.h>

const char*
parse_device_to_json(device_t dev)
{
    return "{}";
}

const char*
parse_driver_to_json(driver_t dri)
{
    return "{}";
}

device_t
parse_device_from_json(const char* p)
{
    return NULL;
}

driver_t
parse_driver_from_json(const char* p)
{
    return NULL;
}

device_id_t
register_device(device_t dev)
{
    const char* str = parse_device_to_json(dev);
    return devmgr_register_device(str);
}

driver_id_t
register_driver(driver_t dri)
{
    const char* str = parse_driver_to_json(dri);
    return devmgr_register_device(str);
}

bool
unregister_device(device_t dev)
{
    return devmgr_unregister_device(dev_id(dev));
}

bool
unregister_driver(driver_t dri)
{
    return devmgr_unregister_driver(dri->d_self);
}

device_t
query_device(device_id_t dev_id)
{
    const char* str = devmgr_query_device(dev_id);
    device_t dev = parse_device_from_json(str);
    return dev;
}

driver_t
query_driver(driver_id_t dri_id)
{
    const char* str = devmgr_query_driver(dri_id);
    driver_t dri = parse_driver_from_json(str);
    return dri;
}

int
update_device(device_t dev)
{
    const char* str = parse_device_to_json(dev);
    return devmgr_update_device(dev_id(dev), str);
}

int
update_driver(driver_t dri)
{
    const char* str = parse_driver_to_json(dri);
    return devmgr_update_driver(dri->d_self, str);
}
