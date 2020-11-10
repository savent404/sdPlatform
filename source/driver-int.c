#include <driver.h>
#include <internal/driver.h>

int driver_init(driver_id_t id, int argc, char **argv)
{
    // driver's id is not registered, so be it
    return EDENY;
}

int driver_deinit(driver_id_t id)
{
    driver_t dri = dri_by_id(id);
    return dri_deinit(dri);
}

int driver_bind(driver_id_t id, device_id_t dev_id)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_bind(dri, dev);
}

int driver_unbind(driver_id_t id, device_id_t dev_id)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);
    
    return dri_unbind(dri, dev);
}

int driver_open(driver_id_t id, device_id_t dev_id, int flags)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_open(dri, dev, flags);
}

int driver_close(driver_id_t id, device_id_t dev_id)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_close(dri, dev);
}

int driver_write(driver_id_t id, device_id_t dev_id, const void *in, size_t size)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_write(dri, dev, in, size);
}

int driver_read(driver_id_t id, device_id_t dev_id, void  *out, size_t size)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_read(dri, dev, out, size);
}

int driver_transfer(driver_id_t id, device_id_t dev_id, const void *in, size_t in_size, void *out, size_t out_size)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_transfer(dri, dev, in, in_size, out, out_size);
}

int driver_ioctl(driver_id_t id, device_id_t dev_id, uint32_t cmd, void *in_out, size_t* size)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_ioctl(dri, dev, cmd, in_out, size);
}

int driver_select(driver_id_t id, device_id_t dev_id, uint32_t flags, size_t timeout)
{
    driver_t dri = dri_by_id(id);
    device_t dev = dev_by_id(dev_id);

    return dri_select(dri, dev, flags, timeout);
}
