/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  杂项函数
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 10时48分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "types.h"

// 端口写一个字节
inline void outb(uint16_t port, uint8_t value);

// 端口读一个字节
inline uint8_t inb(uint16_t port);

// 端口读一个字
inline uint16_t inw(uint16_t port);

// 开启中断
inline void enable_intr(void);

// 关闭中断
inline void disable_intr(void);

// 执行CPU空操作
inline void cpu_hlt(void);

#endif  // INCLUDE_COMMON_H_
