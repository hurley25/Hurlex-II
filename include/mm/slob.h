/*
 * =====================================================================================
 *
 *       Filename:  slob.h
 *
 *    Description:  slob 内存分配器
 *
 *        Version:  1.0
 *        Created:  2014年11月11日 15时39分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_MM_SLOB_H_
#define INCLUDE_MM_SLOB_H_

#include <types.h>

// slob 分配器初始化
void slob_init(void);

// 任意大小的内存分配函数
void *kmalloc(uint32_t size);

// 任意大小的内存释放函数
void kfree(void *addr);

#endif  // INCLUDE_MM_SLOB_H_
