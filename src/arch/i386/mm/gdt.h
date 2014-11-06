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

// 全局描述符表长度
#define GDT_LENGTH 6

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

// 初始化全局描述符表
void init_gdt(void);

// GDT 加载到 GDTR 的函数
extern void gdt_flush();

// TSS 刷新[汇编实现]
extern void tss_flush();

#endif  // GDT_H_
