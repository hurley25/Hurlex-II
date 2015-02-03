/*
 * =====================================================================================
 *
 *       Filename:  ide.h
 *
 *    Description:  IDE 设备驱动
 *
 *        Version:  1.0
 *        Created:  2015年02月03日 14时40分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_IDE_H_
#define INCLUDE_IDE_H_

#include <types.h>

// 初始化IDE设备
void ide_init(void);

// 检测指定IDE设备是否可用
bool ide_device_valid(uint16_t ideno);

// 获取指定IDE设备尺寸
uint32_t ide_device_size(uint16_t ideno);

// 读取指定IDE设备若干扇区
int32_t ide_read_secs(uint16_t ideno, uint32_t secno, void *dst, uint32_t nsecs);

// 写入指定IDE设备若干扇区
int32_t ide_write_secs(uint16_t ideno, uint32_t secno, const void *src, uint32_t nsecs);

#endif  // INCLUDE_IDE_H_
