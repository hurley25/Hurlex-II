/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  调试函数
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 11时45分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_DEBUG_H_
#define INCLUDE_DEBUG_H_

#include "console.h"
#include "vargs.h"
#include "elf.h"

#define assert(x, info)                                         \
        do {                                                    \
                if (!(x)) {                                     \
                        panic(info);                            \
                }                                               \
        } while (0)

// 编译期间静态检测
#define static_assert(x)                                        \
        switch (x) { case 0: case (x): ; }

// 初始化 Debug 信息
void init_debug(void);

// 打印当前的函数调用栈信息
void panic(const char *msg);

// 打印当前的段存器值
void print_cur_status(void);

// 内核的打印函数
void printk(const char *format, ...);

// 内核的打印函数 带颜色
void printk_color(real_color_t back, real_color_t fore, const char *format, ...);

#endif  // INCLUDE_DEBUG_H_
