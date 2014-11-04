/*
 * =====================================================================================
 *
 *       Filename:  common.c
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

#include "common.h"

// 端口写一个字节
inline void outb(uint16_t port, uint8_t value)
{
        asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

// 端口读一个字节
inline uint8_t inb(uint16_t port)
{
        uint8_t ret;

        asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));

        return ret;
}

// 端口读一个字
inline uint16_t inw(uint16_t port)
{
        uint16_t ret;

        asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));

        return ret;
}

// 开启中断
inline void enable_intr(void)
{
        asm volatile ("sti");
}

// 关闭中断
inline void disable_intr(void)
{
        asm volatile ("cli" ::: "memory");
}

// 执行CPU空操作
inline void cpu_hlt(void)
{
        asm volatile ("hlt");
}

