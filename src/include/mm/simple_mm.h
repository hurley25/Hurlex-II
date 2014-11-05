/*
 * =====================================================================================
 *
 *       Filename:  simple_mm.h
 *
 *    Description:  简单的物理内存管理子系统
 *
 *        Version:  1.0
 *        Created:  2014年11月05日 11时36分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_MM_SIMPLE_MM_H_
#define INCLUDE_MM_SIMPLE_MM_H_

// 简单内存管理系统
extern struct pmm_manager simple_mm;

// 内存管理结构
struct smm_struct {
        uint32_t pmm_stack[PAGE_MAX_SIZE+1];    // 物理内存页面管理的栈
        volatile uint32_t pmm_stack_top;        // 物理内存管理的栈指针
        uint32_t smm_start_addr;                // 物理内存管理的起始地址
        uint32_t smm_end_addr;                  // 物理内存管理的结束地址
        uint32_t phy_page_count;                // 物理内存页的总数量
        volatile uint32_t phy_page_now_count;   // 物理内存页的当前数量
};

#endif  // INCLUDE_MM_SIMPLE_MM_H_
