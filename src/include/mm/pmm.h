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

#include <types.h>

#ifndef INCLUDE_MM_PMM_H
#define INCLUDE_MM_PMM_H

// 默认栈的大小
#define STACK_SIZE 8192

// 支持的最大物理内存(512MB)
#define PMM_MAX_SIZE 0x20000000

// 物理内存页框大小 
#define PMM_PAGE_SIZE 0x1000

// 最多支持的物理页面个数
#define PAGE_MAX_SIZE (PMM_MAX_SIZE / PMM_PAGE_SIZE)

// 页掩码 按照 4096 对齐地址
#define PHY_PAGE_MASK 0xFFFFF000

// 内核在物理内存起始位置
#define RAM_KERNEL_START 0x100000

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

// 输出 BIOS 提供的物理内存布局
void show_memory_map(void);

// 输出内核当前占据的内存地址
void show_kernel_memory_map(void);

// 内存管理子系统管理对象
struct pmm_manager {
        const char *name;                                 // 管理算法的名称
        void (*init)(uint32_t mstart, uint32_t mend);     // 初始化
        uint32_t (*alloc_pages)(uint32_t n);              // 申请物理内存页(n为字节数)
        void (*free_pages)(uint32_t base, uint32_t n);    // 释放内存页
        void (*show_memory_info)(void);                   // 输出内存信息
        void (*show_management_info)(void);               // 输出当前管理信息
        void (*test_mm)(void);                            // 测试当前管理子系统
};

// 全局的物理内存管理接口
extern struct pmm_manager *mm;

// 初始化物理内存管理
void init_pmm(void);

#endif  // INCLUDE_MM_PMM_H
