/*
 * =====================================================================================
 *
 *       Filename:  pmm.h
 *
 *    Description:  物理内存管理
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 13时03分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include <multiboot.h>

// 线程栈的大小
#define STACK_SIZE 8192

// 支持的最大物理内存(512MB)
#define PMM_MAX_SIZE 0x20000000

// 物理内存页框大小 
#define PMM_PAGE_SIZE 0x1000

// 最多支持的物理页面个数
#define PAGE_MAX_SIZE (PMM_MAX_SIZE / PMM_PAGE_SIZE)

// 页掩码 按照 4096 对齐地址
#define PHY_PAGE_MASK 0xFFFFF000

// 内核代码段在内存中的起始和结束位置，在链接脚本中定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];
extern uint8_t kern_init_text_start[];
extern uint8_t kern_init_text_end[];
extern uint8_t kern_init_data_start[];
extern uint8_t kern_init_data_end[];
extern uint8_t kern_text_start[];
extern uint8_t kern_text_end[];
extern uint8_t kern_data_start[];
extern uint8_t kern_data_end[];

// 内核栈的栈顶
extern uint32_t kern_stack_top;

// 动态分配物理内存页的总数
extern uint32_t phy_page_count;

// 输出 BIOS 提供的物理内存布局
void show_memory_map(void);

// 输出内核当前占据的内存地址
void show_kernel_memory_map(void);

// 初始化物理内存管理
void init_pmm(void);

// 返回一个内存页的物理地址
uint32_t pmm_alloc_page(void);

// 释放申请的内存
void pmm_free_page(uint32_t p);

#endif  // INCLUDE_PMM_H
