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

#ifndef INCLUDE_MM_PMM_H
#define INCLUDE_MM_PMM_H

#include <types.h>

// 默认栈的大小
#define STACK_SIZE 8192

// 物理内存页框大小 
#define PMM_PAGE_SIZE 0x1000

// 页掩码 按照 0x1000(4096) 对齐地址
#define PHY_PAGE_MASK 0xFFFFF000

// 内核在物理内存起始位置
#define RAM_KERNEL_START 0x100000

// 内核代码在内存中的起始和结束位置，在链接脚本中定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];

// 内存管理子系统管理对象
struct pmm_manager {
        const char *name;                                 // 管理算法的名称
        void (*page_init)(uint32_t mstart, uint32_t mend);     // 初始化
        uint32_t (*alloc_pages)(uint32_t n);              // 申请物理内存页(n为字节数)
        void (*free_pages)(uint32_t base, uint32_t n);    // 释放内存页
        void (*show_memory_info)(void);                   // 输出内存信息
        void (*show_management_info)(void);               // 输出当前管理信息
        void (*test_mm)(void);                            // 测试当前管理子系统
};

// 获取可用内存的起始和结束地址
void get_ram_info(uint32_t *mstart, uint32_t *mend);

// 初始化物理内存管理
void init_pmm(void);

// 内存管理算法初始化
void page_init(uint32_t mstart, uint32_t mend);

// 申请内存页
uint32_t alloc_pages(uint32_t n);

// 释放内存页
void free_pages(uint32_t page, uint32_t n);

// 输出内存信息
void show_memory_info(void);

// 输出当前管理信息
void show_management_info(void);

// 测试当前管理子系统
void test_mm(void);

#endif  // INCLUDE_MM_PMM_H
