#include <internal/drivers/i2c-bus.h>
#include <internal/dops.h>

static int _init(int, char**);
static int _deinit(void);
static int _bind(device_t);
static int _unbind(device_t, int);
static int _open(device_t, int);
static int _close(device_t);
static int _write(device_t, const void *, size_t);
static int _read(device_t, void*, size_t);
static int _transfer(device_t, uint32_t, void*, size_t*);
static int _ioctl(device_t, uint32_t, void*, size_t*);
static int _select(device_t, uint32_t, size_t);

static struct driver_ops _fops = {
    .init = _init,
    .deinit = _deinit,
    .bind = _bind,
    .unbind = _unbind,
    .open = _open,
    .close = _close,
    .write = _write,
    .read = _read,
    .transfer = _transfer,
    .ioctl = _ioctl,
    .select = _select,
};

static struct i2c_bus_api _api = {

};

static struct driver _driver = {
    .d_env = NULL,
    .d_data = NULL,
    .d_self = 0,
    .d_ops = &_fops,
    .d_intapi = &_api,
};

static int _init(int, char**);
static int _deinit(void);
static int _bind(device_t);
static int _unbind(device_t, int);
static int _open(device_t, int);
static int _close(device_t);
static int _write(device_t, const void *, size_t);
static int _read(device_t, void*, size_t);
static int _transfer(device_t, uint32_t, void*, size_t*);
static int _ioctl(device_t, uint32_t, void*, size_t*);
static int _select(device_t, uint32_t, size_t);

