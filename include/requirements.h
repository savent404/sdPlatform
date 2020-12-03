// Copyright 2020 jrlc
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 向devmgr申请创建设备
 * 
 * @param json_str json 字符串
 * @param id       预申请id(非0) 若需要设备管理器分配则填0
 * @return int 0  创建失败
 *         int !0 创建成功，为设备的id值
 */
int devmgr_create_device(const char *json_str, int id);
/**
 * @brief 向devmgr申请更新设备信息
 * 
 * @param device_id 设备id值
 * @param json_str  json 字符串
 * @return int 0  更新成功
 *             !0 更新失败
 */
int devmgr_update_device(int device_id, const char *json_str);
/**
 * @brief 向devmgr申请删除设备
 * 
 * @param device_id 设备id值
 * @return int 0  更新成功
 *             !0 更新失败
 */
int devmgr_remove_device(int device_id);
/**
 * @brief 向devmgr申请查询设备信息
 * 
 * @param driver_id 设备id值
 * @return const char* NULL  查询是被
 *                     !NULL 返回json字符串
 */
const char *devmgr_query_driver(int driver_id);

/**
 * @brief 向devmgr申请创建驱动
 * 
 * @param json_str json 字符串
 * @return int 0  创建失败
 *         int !0 创建成功，为驱动的id值
 */
int devmgr_create_driver(const char *json_str);
/**
 * @brief 向devmgr申请更新驱动信息
 * 
 * @param device_id 驱动id值
 * @param json_str  json 字符串
 * @return int 0  更新成功
 *             !0 更新失败
 */
int devmgr_update_driver(int driver_id, const char *json_str);
/**
 * @brief 向devmgr申请删除驱动
 * 
 * @param device_id 驱动id值
 * @return int 0  更新成功
 *             !0 更新失败
 */
int devmgr_remove_driver(int driver_id);
/**
 * @brief 向devmgr申请查询驱动信息
 * 
 * @param driver_id 驱动id值
 * @return const char* NULL  查询是被
 *                     !NULL 返回json字符串
 */
const char *devmgr_query_device(int device_id);

#ifdef __cplusplus
}
#endif
