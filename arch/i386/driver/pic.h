/*
 * =====================================================================================
 *
 *       Filename:  pic.h
 *
 *    Description:  PIC 相关
 *
 *        Version:  1.0
 *        Created:  2014年11月06日 09时51分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_PIC_H_
#define INCLUDE_PIC_H_

#include <arch.h>

// 设置8259A芯片
void init_interrupt_chip(void);

// 重设 8259A 芯片
void clear_interrupt_chip(uint32_t intr_no);

#endif  // INCLUDE_PIC_H_
