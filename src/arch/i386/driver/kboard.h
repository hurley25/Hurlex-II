/*
 * =====================================================================================
 *
 *       Filename:  kboard.h
 *
 *    Description:  键盘驱动
 *
 *        Version:  1.0
 *        Created:  2015年02月05日 13时35分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_KBOARD_H
#define INCLUDE_KBOARD_H

#include <types.h>
#include <intr/intr.h>

// 初始化键盘驱动
void keyboard_init(void);

// 键盘中断的中断处理函数
void keyboard_handler(pt_regs_t *regs);

// 从键盘读取一个字符，如果没有可用字符返回0
uint8_t keyboard_getchar(void);

#endif  // INCLUDE_KBOARD_H_

