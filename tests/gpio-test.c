#include <internal/devmgr.h>
#include <internal/driver.h>
#include <internal/mutex.h>
#include <stdio.h>
#include <stdlib.h>

struct self_gpio_env
{
    int level;
    int pin;
    int port;
    int mode;
    int is_opened;
    mutex_t lock;
};

struct self_driver_data
{
    mutex_t lock;
};

static int
self_init(int, char**);
static int
self_deinit();
static int self_bind(device_t);
static int self_unbind(device_t);
static int
self_open(device_t, int flags);
static int self_close(device_t);
static int
self_read(device_t, void*, size_t);
static int
self_write(device_t, const void*, size_t);
static int
self_ioctl(device_t, uint32_t cmd, void* in_out, size_t* size);
static int self_select(device_t, uint32_t, size_t);

static struct driver_ops self_gpio_dirver_ops = {
    .init = self_init,
    .deinit = self_deinit,
    .bind = self_bind,
    .unbind = self_unbind,
    .open = self_open,
    .close = self_close,
    .read = self_read,
    .write = self_write,
    .ioctl = self_ioctl,
    // .select = self_select,
};

__init_data static struct driver_env self_gpio_env = {
    .name = "self test gpio",
    .compat = "x86,self-gpio|arm,self-gpio|arm64,self-gpio",
    ._json = NULL,
};

static struct driver self_gpio_driver = {
    .d_ops = &self_gpio_dirver_ops,
    .d_env = &self_gpio_env,
    .d_self = 0,
};

static int init_res = -1;

static int
lock_driver()
{
    struct self_driver_data* data =
      (struct self_driver_data*)(self_gpio_driver.d_data);
    mutex_t lock = data->lock;
    if (!mutex_is_valid(lock))
        return EINVALIDE;
    return mutex_lock(lock);
}

static int
unlock_driver(void)
{
    struct self_driver_data* data =
      (struct self_driver_data*)(self_gpio_driver.d_data);
    mutex_t lock = data->lock;
    if (!mutex_is_valid(lock))
        return EINVALIDE;
    return mutex_unlock(lock);
}

__init_code static int
self_init(int argc, char** argv)
{
    printf("init %s\n", self_gpio_env.name);
    driver_id_t dri_id = register_driver(&self_gpio_driver);
    struct self_driver_data* data =
      (struct self_driver_data*)malloc(sizeof(*data));
    data->lock = mutex_new();
    self_gpio_driver.d_data = data;
    self_gpio_driver.d_self = dri_id;
    init_res = dri_id != 0 ? 0 : EINVALIDE && mutex_is_valid(data->lock);
    if (!init_res) {
        update_driver(&self_gpio_driver);
    } else {
        mutex_del(data->lock);
        free(data);
    }
    return init_res;
}

static int
self_deinit()
{
    struct self_driver_data* data =
      (struct self_driver_data*)(self_gpio_driver.d_data);
    mutex_t lock = data->lock;
    mutex_lock(lock);
    unregister_driver(&self_gpio_driver);
    self_gpio_driver.d_self = 0;
    free(self_gpio_driver.d_data);
    mutex_unlock(lock);
    mutex_del(lock);
    return 0;
}

static int
self_bind(device_t dev)
{
    device_env_t env = dev->d_env;
    if (!env)
        return EINVALIDE;
    if (!dev)
        return EINVALIDE;
    cJSON* root = dev_env_json(dev);

    if (!cJSON_HasObjectItem(root, "config"))
        return EINVALIDE;

    cJSON* config = cJSON_GetObjectItem(root, "config");
    if (!cJSON_HasObjectItem(config, "gpio-pin") ||
        !cJSON_HasObjectItem(config, "gpio-port") ||
        !cJSON_HasObjectItem(config, "gpio-mode"))
        return EINVALIDE;

    cJSON* pin = cJSON_GetObjectItem(config, "gpio-pin");
    cJSON* port = cJSON_GetObjectItem(config, "gpio-port");
    cJSON* mode = cJSON_GetObjectItem(config, "gpio-mode");
    if (!cJSON_IsNumber(pin) || !cJSON_IsNumber(port) || !cJSON_IsNumber(mode))
        return EINVALIDE;

    lock_driver();
    struct self_gpio_env* self_data =
      (struct self_gpio_env*)malloc(sizeof(*self_data));
    self_data->pin = cJSON_GetNumberValue(pin);
    self_data->port = cJSON_GetNumberValue(port);
    self_data->mode = cJSON_GetNumberValue(mode);
    self_data->is_opened = false;
    self_data->lock = mutex_new();
    self_data->level = 0; // dfeault as low level
    dev->d_data = self_data;
    unlock_driver();

    return 0;
}

static int
self_unbind(device_t dev)
{
    struct self_gpio_env* data = (struct self_gpio_env*)dev->d_data;
    mutex_t lock = data->lock;

    if (!data)
        return EINVALIDE;

    if (data->is_opened) {
        return EBUSY;
    }

    mutex_lock(lock);
    dev->d_data = NULL;
    dev->d_driver = 0;
    free(data);
    mutex_unlock(lock);
    mutex_del(lock);

    return 0;
}

static int
self_open(device_t dev, int flags)
{
    struct self_gpio_env* data = (struct self_gpio_env*)dev->d_data;
    if (!data)
        return EINVALIDE;
    if (data->is_opened)
        return EBUSY;

    mutex_lock(data->lock);
    // ignore flags
    data->is_opened = true;
    printf("device opening...\n");
    mutex_unlock(data->lock);
    return 0;
}

static int
self_close(device_t dev)
{
    struct self_gpio_env* data = (struct self_gpio_env*)dev->d_data;
    if (!data)
        return EINVALIDE;
    if (!data->is_opened)
        return 0;

    mutex_lock(data->lock);
    data->is_opened = false;
    printf("device closing...\n");
    mutex_unlock(data->lock);
    return 0;
}

static int
self_read(device_t dev, void* out, size_t size)
{
    struct self_gpio_env* data = (struct self_gpio_env*)dev->d_data;
    if (!data || !data->is_opened || size != 1 || !out)
        return EINVALIDE;

    mutex_lock(data->lock);
    char* c = (char*)out;
    *c = data->level;
    mutex_unlock(data->lock);
    return (int)size;
}

static int
self_write(device_t dev, const void* in, size_t size)
{
    struct self_gpio_env* data = (struct self_gpio_env*)dev->d_data;
    if (!data || !data->is_opened || size != 1 || !in)
        return EINVALIDE;

    mutex_lock(data->lock);
    const char* c = (const char*)in;
    data->level = *c ? 1 : 0;
    mutex_unlock(data->lock);
    return (int)size;
}

static int
self_ioctl(device_t dev, uint32_t cmd, void* in_out, size_t* size)
{
    struct self_gpio_env* data = (struct self_gpio_env*)dev->d_data;
    if (!data || !data->is_opened)
        return EINVALIDE;
    switch (cmd) {
        // read mode
        case 0x100:
            if (*size != sizeof(data->mode))
                return EINVALIDE;
            mutex_lock(data->lock);
            int* c = (int*)in_out;
            *c = data->mode;
            mutex_unlock(data->lock);
            break;
        default:
            return EINVALIDE;
    }
    return 0;
}

#include <assert.h>
static struct device_env test_device_env = {
    .name = "test-gpio-a-0",
    .compat = "x86,self-gpio",
    ._json_str = "{\
  \"config\": { \
    \"gpio-pin\": 0,\
    \"gpio-port\": 0,\
    \"gpio-mode\": 0\
}}",
};

static struct device test_device = {
    .d_env = &test_device_env,
    .d_data = NULL,
    .d_driver = 0,
    .d_self = 0x8970000,
    .d_parent = 0,
    .d_child = 0,
    .d_prev = 0,
    .d_next = 0,
};

int
main(void)
{
    // called self_init already
    dev_env_parse(&test_device);
    // asume test_device regiesterd already
    driver_t dri = &self_gpio_driver;
    device_t dev = &test_device;

    assert(dri_bind(dri, dev) == 0);
    assert(dri_open(dri, dev, F_READ | F_WRITE) == 0);
    char val;
    assert(dri_read(dri, dev, &val, 1) == 1);
    assert(val == 0);
    val = 1;
    assert(dri_write(dri, dev, &val, 1) == 1);
    assert(dri_read(dri, dev, &val, 1) == 1);
    assert(val == 1);
    assert(dri_close(dri, dev) == 0);
    assert(dri_unbind(dri, dev) == 0);
    assert(dri_deinit(dri) == 0);

    return init_res;
}
