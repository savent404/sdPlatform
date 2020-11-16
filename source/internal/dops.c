#include <internal/dops.h>

int dops_init(driver_ops_t ops, int argc, char** argv)
{
    return ops && ops->init ? ops->init(argc, argv) : EINVALIDE;
}

int dops_deinit(driver_ops_t ops)
{
    return ops && ops->deinit ? ops->deinit() : EINVALIDE;
}

int dops_bind(driver_ops_t ops, device_t dev)
{
    return ops && ops->bind ? ops->bind(dev) : EINVALIDE;
}

int dops_unbind(driver_ops_t ops, device_t dev)
{
    return ops && ops->unbind ? ops->unbind(dev) : EINVALIDE;
}

int dops_open(driver_ops_t ops, device_t dev, int flags)
{
    return ops && ops->open ? ops->open(dev, flags) : EINVALIDE;
}

int dops_close(driver_ops_t ops, device_t dev)
{
    return ops && ops->close ? ops->close(dev) : EINVALIDE;
}

int dops_write(driver_ops_t ops,
                                device_t dev,
                                const void* in,
                                size_t size)
{
    return ops && ops->write ? ops->write(dev, in, size) : EINVALIDE;
}

int dops_read(driver_ops_t ops,
                            device_t dev,
                            void* out,
                            size_t size)
{
    return ops && ops->read ? ops->read(dev, out, size) : EINVALIDE;
}

int dops_transfer(driver_ops_t ops,
                                device_t dev,
                                const void* in,
                                size_t in_size,
                                void* out,
                                size_t out_size)
{
    return ops && ops->transfer
                ? ops->transfer(dev, in, in_size, out, out_size)
                : EINVALIDE;
}

int dops_ioctl(driver_ops_t ops,
                                device_t dev,
                                uint32_t cmd,
                                void* in_out,
                                size_t* size)
{
    return ops && ops->ioctl ? ops->ioctl(dev, cmd, in_out, size)
                                : EINVALIDE;
}

int dops_select(driver_ops_t ops,
                                device_t dev,
                                uint32_t flags,
                                size_t timeout)
{
    return ops && ops->select ? ops->select(dev, flags, timeout)
                                : EINVALIDE;
}
