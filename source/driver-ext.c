#include <driver.h>

int driver_init(driver_id_t id, int argc, char **argv)
{
    return EDENY;
}

int driver_deinit(driver_id_t id)
{
    return EDENY;
}

int driver_bind(driver_id_t id, device_id_t dev_id)
{
    return EDENY;
}

int driver_unbind(driver_id_t id, device_id_t dev_id)
{
    return EDENY;
}

int driver_open(driver_id_t id, device_id_t dev_id, int flags)
{
    return EDENY;
}

int driver_close(driver_id_t id, device_id_t dev_id)
{
    return EDENY;
}

int driver_write(driver_id_t id, device_id_t dev_id, const void *in, size_t size)
{
    return EDENY;
}

int driver_read(driver_id_t id, device_id_t dev_id, void  *out, size_t size)
{
    return EDENY;
}

int driver_transfer(driver_id_t id, device_id_t dev_id, const void *in, size_t in_size, void *out, size_t out_size)
{
    return EDENY;
}

int driver_ioctl(driver_id_t id, device_id_t dev_id, uint32_t cmd, void *in_out, size_t* size)
{
    return EDENY;
}

int driver_select(driver_id_t id, device_id_t dev_id, uint32_t flags, size_t timeout)
{
    return EDENY;
}
