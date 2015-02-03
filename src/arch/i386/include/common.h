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

/* Eflags register */
#define FL_CF           0x00000001  // Carry Flag
#define FL_PF           0x00000004  // Parity Flag
#define FL_AF           0x00000010  // Auxiliary carry Flag
#define FL_ZF           0x00000040  // Zero Flag
#define FL_SF           0x00000080  // Sign Flag
#define FL_TF           0x00000100  // Trap Flag
#define FL_IF           0x00000200  // Interrupt Flag
#define FL_DF           0x00000400  // Direction Flag
#define FL_OF           0x00000800  // Overflow Flag
#define FL_IOPL_MASK    0x00003000  // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000  // IOPL == 0
#define FL_IOPL_1       0x00001000  // IOPL == 1
#define FL_IOPL_2       0x00002000  // IOPL == 2
#define FL_IOPL_3       0x00003000  // IOPL == 3
#define FL_NT           0x00004000  // Nested Task
#define FL_RF           0x00010000  // Resume Flag
#define FL_VM           0x00020000  // Virtual 8086 mode
#define FL_AC           0x00040000  // Alignment Check
#define FL_VIF          0x00080000  // Virtual Interrupt Flag
#define FL_VIP          0x00100000  // Virtual Interrupt Pending
#define FL_ID           0x00200000  // ID flag

#define __barrier__() __asm__ volatile ("" ::: "memory")

// 端口写一个字节
static inline void outb(uint16_t port, uint8_t value)
{
        __asm__ volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

// 端口读一个字节
static inline uint8_t inb(uint16_t port)
{
        uint8_t ret;

        __asm__ volatile("inb %1, %0" : "=a" (ret) : "dN" (port));

        return ret;
}

// 端口读一个字
static inline uint16_t inw(uint16_t port)
{
        uint16_t ret;

        __asm__ volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));

        return ret;
}

static inline void insl(uint32_t port, void *addr, int cnt)
{
        __asm__ volatile (
                        "cld;"
                        "repne; insl;"
                        : "=D" (addr), "=c" (cnt)
                        : "d" (port), "0" (addr), "1" (cnt)
                        : "memory", "cc");
}

static inline void outsl(uint32_t port, const void *addr, int cnt)
{
        __asm__ volatile (
                        "cld;"
                        "repne; outsl;"
                        : "=S" (addr), "=c" (cnt)
                        : "d" (port), "0" (addr), "1" (cnt)
                        : "memory", "cc");
}

// 开启中断
static inline void enable_intr(void)
{
        __asm__ volatile ("sti");
}

// 关闭中断
static inline void disable_intr(void)
{
        __asm__ volatile ("cli" ::: "memory");
}

// 执行CPU空操作
static inline void cpu_hlt(void)
{
        __asm__ volatile ("hlt");
}

// 读取 EFLAGS
static inline uint32_t read_eflags(void)
{
        uint32_t eflags;

        __asm__ volatile ("pushfl; popl %0" : "=r" (eflags));

        return eflags;
}

// 写入EFALGS
static inline void write_eflags(uint32_t eflags)
{
        __asm__ volatile ("pushl %0; popfl" :: "r" (eflags));
}

// 修改当前页表
static inline void switch_pgd(uint32_t pd)
{
        __asm__ volatile ("mov %0, %%cr3" : : "r" (pd));
}

// 通知 CPU 更新页表缓存
static inline void tlb_reload_page(uint32_t va)
{
        __asm__ volatile ("invlpg (%0)" : : "a" (va));
}

// 修改栈地址
static inline void load_esp(uint32_t esp)
{
        __asm__ volatile ("mov %0, %%esp" : : "r" (esp));
}

#endif  // INCLUDE_COMMON_H_
