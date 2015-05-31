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

#include <arch.h>
#include <vargs.h>
#include <elf.h>
#include <errno.h>

// 内核代码段在内存中的起始和结束位置，在链接脚本中定义
extern uint8_t kern_init_text_start[];
extern uint8_t kern_init_text_end[];
extern uint8_t kern_init_data_start[];
extern uint8_t kern_init_data_end[];
extern uint8_t kern_text_start[];
extern uint8_t kern_text_end[];
extern uint8_t kern_data_start[];
extern uint8_t kern_data_end[];

#define assert(x, info)                                         \
        do {                                                    \
                if (!(x)) {                                     \
                        panic(info);                            \
                }                                               \
        } while (0)

// 初始化 Debug 信息
void debug_init(void);

// 输出 BIOS 提供的物理内存布局
void show_memory_map(void);

// 输出内核当前占据的内存地址
void show_kernel_memory_map(void);

// 打印当前的函数调用栈信息
void panic(const char *msg);

// 打印当前的段存器值
void print_cur_status(void);

// 内核的打印函数
void printk(const char *format, ...);

// 内核的打印函数(带颜色)
void cprintk(real_color_t back, real_color_t fore, const char *format, ...);

#endif  // INCLUDE_DEBUG_H_
