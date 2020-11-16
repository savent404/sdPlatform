#include <internal/devmgr.h>
#include <internal/driver.h>
#include <string.h>

driver_t
dri_by_id(driver_id_t id)
{
    return 0;
}

driver_id_t
dri_id(driver_t dri)
{
    return dri ? dri->d_self : 0;
}

driver_data_t
dri_get_data(driver_t dri)
{
    return dri ? dri->d_data : NULL;
}

void
dri_set_data(driver_t dri, driver_data_t data)
{
    if (!dri)
        return;
    dri->d_data = data;
}

bool
dri_is_registered(driver_t dri)
{
    return dri_id(dri) == 0;
}

bool
dri_match_device(driver_t dri, device_t dev)
{
    const char *now = NULL, *next = NULL, *compat = NULL;
    if (!dri || !dri->d_env || !dri->d_env->compat || !dev || !dev->d_env ||
        !dev->d_env->compat)
        return false;

    compat = dev->d_env->compat;
    next = dri->d_env->compat - 1;
    do {
        now = next + 1;
        next = strchr(now, '|');
        int letter = next ? next - now : strlen(now);
        if (!strncmp(now, compat, letter))
            return true;
    } while (next);
    return false;
}

int
dri_init(driver_t dri, int argc, char** argv)
{
    int res = dri ? dops_init(dri->d_ops, argc, argv) : EINVALIDE;
    if (!res) {
        driver_id_t id = register_driver(dri);
        dri->d_self = id;
    }
    return res;
}

int
dri_deinit(driver_t dri)
{
    int res = dri ? dops_deinit(dri->d_ops) : EINVALIDE;
    if (!res) {
        unregister_driver(dri);
        dri->d_self = 0;
    }
    return res;
}

int
dri_bind(driver_t dri, device_t dev)
{
    if (!dri_match_device(dri, dev) && dev_is_binded(dev))
        return EDENY;
    int res = dri ? dops_bind(dri->d_ops, dev) : EINVALIDE;
    if (!res) {
        dev->d_driver = dri_id(dri);
        dev->_d_driver = dri;
    }
    return res;
}

int
dri_unbind(driver_t dri, device_t dev)
{
    int res = dri ? dops_unbind(dri->d_ops, dev) : EINVALIDE;
    if (!res) {
        dri->d_self = 0;
    }
    return 0;
}

int
dri_open(driver_t dri, device_t dev, int flags)
{
    return dri ? dops_open(dri->d_ops, dev, flags) : EINVALIDE;
}

int
dri_close(driver_t dri, device_t dev)
{
    return dri ? dops_close(dri->d_ops, dev) : EINVALIDE;
}

int
dri_write(driver_t dri, device_t dev, const void* in, size_t size)
{
    return dri ? dops_write(dri->d_ops, dev, in, size) : EINVALIDE;
}

int
dri_read(driver_t dri, device_t dev, void* out, size_t size)
{
    return dri ? dops_read(dri->d_ops, dev, out, size) : EINVALIDE;
}

int
dri_transfer(driver_t dri,
             device_t dev,
             const void* in,
             size_t in_size,
             void* out,
             size_t out_size)
{
    return dri ? dops_transfer(dri->d_ops, dev, in, in_size, out, out_size)
               : EINVALIDE;
}

int
dri_ioctl(driver_t dri, device_t dev, uint32_t cmd, void* in_out, size_t* size)
{
    return dri ? dops_ioctl(dri->d_ops, dev, cmd, in_out, size) : EINVALIDE;
}

int
dri_select(driver_t dri, device_t dev, uint32_t flags, size_t timeout)
{
    return dri ? dops_select(dri->d_ops, dev, flags, timeout) : EINVALIDE;
}
