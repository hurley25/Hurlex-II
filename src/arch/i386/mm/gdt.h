/*
 * =====================================================================================
 *
 *       Filename:  gdt.h
 *
 *    Description:  全局描述符表支持
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 14时11分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef GDT_H_
#define GDT_H_

#include <types.h>

// 各个内存段所在全局描述符表下标
#define SEG_NULL    0
#define SEG_KTEXT   1
#define SEG_KDATA   2
#define SEG_UTEXT   3
#define SEG_UDATA   4
#define SEG_TSS     5

#define GD_KTEXT    ((SEG_KTEXT) << 3)      // 内核代码段
#define GD_KDATA    ((SEG_KDATA) << 3)      // 内核数据段
#define GD_UTEXT    ((SEG_UTEXT) << 3)      // 用户代码段
#define GD_UDATA    ((SEG_UDATA) << 3)      // 用户数据段
#define GD_TSS      ((SEG_TSS) << 3)        // 任务段

// 段描述符 DPL
#define DPL_KERNEL  (0)
#define DPL_USER    (3)

// 各个段的全局描述符表的选择子
#define KERNEL_CS   ((GD_KTEXT) | DPL_KERNEL)
#define KERNEL_DS   ((GD_KDATA) | DPL_KERNEL)
#define USER_CS     ((GD_UTEXT) | DPL_USER)
#define USER_DS     ((GD_UDATA) | DPL_USER)

// 全局描述符类型
typedef
struct gdt_entry_t {
        uint16_t limit_low;     // 段界限   15～0
        uint16_t base_low;      // 段基地址 15～0
        uint8_t  base_middle;   // 段基地址 23～16
        uint8_t  access;        // 段存在位、描述符特权级、描述符类型、描述符子类别
        uint8_t  granularity;   // 其他标志、段界限 19～16
        uint8_t  base_high;     // 段基地址 31～24
} __attribute__((packed)) gdt_entry_t;

// GDTR
typedef
struct gdt_ptr_t {
        uint16_t limit;         // 全局描述符表限长
        uint32_t base;          // 全局描述符表 32位 基地址
} __attribute__((packed)) gdt_ptr_t;

// TSS 描述符
typedef
struct tss_entry_t {
        uint32_t ts_link;         // old ts selector
        uint32_t ts_esp0;         // stack pointers and segment selectors
        uint16_t ts_ss0;          // after an increase in privilege level
        uint16_t ts_padding1;
        uint32_t ts_esp1;
        uint16_t ts_ss1;
        uint16_t ts_padding2;
        uint32_t ts_esp2;
        uint16_t ts_ss2;
        uint16_t ts_padding3;
        uint32_t ts_cr3;          // page directory base
        uint32_t ts_eip;          // saved state from last task switch
        uint32_t ts_eflags;
        uint32_t ts_eax;          // more saved state (registers)
        uint32_t ts_ecx;
        uint32_t ts_edx;
        uint32_t ts_ebx;
        uint32_t ts_esp;
        uint32_t ts_ebp;
        uint32_t ts_esi;
        uint32_t ts_edi;
        uint16_t ts_es;           // even more saved state (segment selectors)
        uint16_t ts_padding4;
        uint16_t ts_cs;
        uint16_t ts_padding5;
        uint16_t ts_ss;
        uint16_t ts_padding6;
        uint16_t ts_ds;
        uint16_t ts_padding7;
        uint16_t ts_fs;
        uint16_t ts_padding8;
        uint16_t ts_gs;
        uint16_t ts_padding9;
        uint16_t ts_ldt;
        uint16_t ts_padding10;
        uint16_t ts_t;            // trap on task switch
        uint16_t ts_iomb;         // i/o map base address
} __attribute__((packed)) tss_entry_t;

// 初始化全局描述符表
void init_gdt(void);

// GDT 加载到 GDTR 的函数
extern void gdt_flush();

// TSS 刷新[汇编实现]
extern void tss_flush();

#endif  // GDT_H_
