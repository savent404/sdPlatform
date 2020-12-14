// Copyright 2020 jrlc
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 用户API
 * @defgroup user_api
 * @{
 */

/**
 * @brief 设备绑定驱动
 * 
 * @param device_id 
 * @param driver_id 
 * @return int 
 */
int dev_bind(int device_id, int driver_id);
/**
 * @brief 设备解绑驱动
 * 
 * @param device_id 
 * @return int 
 */
int dev_unbind(int device_id);

/**
 * @brief 打开设备
 * 
 * @param device_id 
 * @param flags see enum fflag
 * @return int 
 */
int dev_open(int device_id, int flags);
/**
 * @brief 关闭设备
 * 
 * @param device_id 
 * @return int 
 */
int dev_close(int device_id);

/**
 * @brief 读写设备
 * 
 * @param device_id 
 * @param[in] in 输入数据的指针
 * @param in_len 输入数据的大小(Bytes)
 * @param[out] out 输出数据的指针
 * @param out_len  输出数据的大小(Bytes)
 * @return int 
 */
int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len);
/**
 * @brief 写设备
 * 
 * @param device_id 
 * @param[in] in 输入数据的指针
 * @param len 输出数据大小(Bytes)
 * @return int >0 实际写入大小
 */
int dev_write(int device_id, const void *in, size_t len);
/**
 * @brief 读设备
 * 
 * @param device_id 
 * @param[out] out 输出数据的指针
 * @param len 输出数据大小(Bytes)
 * @note 确保buffer足够接受 len 长度的数据
 * @return int >0 实际读出大小
 */
int dev_read(int device_id, void *out, size_t len);

/**
 * @brief 设备控制接口
 * 
 * @param device_id 
 * @param cmds 控制字
 * @param in_out 数据指针，即用于输入也用于输出，根据控制字定义其方向
 * @param in_len 输入数据大小(Bytes)
 * @param out_max_len in_out最大能接受的字节数
 * @return int >0 实际输出的字节大小
 */
int dev_ioctl(int device_id, int cmds, void *in_out, size_t in_len, size_t out_max_len);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
