/*
 * =====================================================================================
 *
 *       Filename:  ff_mm.c
 *
 *    Description:  First-Fit 算法内存管理
 *
 *        Version:  1.0
 *        Created:  2014年11月05日 10时47分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <mm/pmm.h>
#include <mm/ff_mm.h>
#include <atomic.h>
#include <debug.h>

// 每个独立的内存管理算法必须实现的4个接口函数
static void ff_page_init(page_t *pages, uint32_t n);
static uint32_t ff_alloc_pages(uint32_t n);
static void ff_free_pages(uint32_t base, uint32_t n);
static uint32_t ff_free_pages_count(void);

// 管理结构
struct pmm_manager ff_mm = {
                "ff_Memory_Managentment",
                &ff_page_init,
                &ff_alloc_pages,
                &ff_free_pages,
                &ff_free_pages_count
};

// 内存管理结构
struct ff_mm_struct {
        uint32_t mm_addr_start;       // 物理内存页管理起始地址
        uint32_t mm_addr_end;         // 物理内存页管理结束地址
        atomic_t phy_page_count;       // 物理内存页的总数量
        atomic_t phy_page_now_count;   // 物理内存页的当前数量
};

static struct ff_mm_struct ff_mm_info;

static void ff_show_memory_info(void);
static void ff_show_management_info(void);
static void ff_test_mm(void);

static void ff_page_init(page_t *pages, uint32_t n)
{
        atomic_set(&ff_mm_info.phy_page_count, n);
        atomic_set(&ff_mm_info.phy_page_now_count, n);
        
        ff_mm_info.mm_addr_start = page_to_addr(&pages[0]);
        ff_mm_info.mm_addr_end = page_to_addr(&pages[n-1]) + PMM_PAGE_SIZE;
        
        ff_show_memory_info();
        ff_show_management_info();
        ff_test_mm();
}

static uint32_t ff_alloc_pages(uint32_t n)
{

        atomic_sub(&ff_mm_info.phy_page_now_count, n);

        return 0;
}

static void ff_free_pages(uint32_t base, uint32_t n)
{
        atomic_add(&ff_mm_info.phy_page_now_count, n);
}

static uint32_t ff_free_pages_count(void)
{
        return atomic_read(&ff_mm_info.phy_page_now_count);
}


static void ff_show_memory_info(void)
{
        printk("Physical Memory Pages Start: %08X  End: %08X\n\n", ff_mm_info.mm_addr_start, ff_mm_info.mm_addr_end);
        printk("Physical Memory Pages Total: %d Pages = %d KB\n\n",
                        atomic_read(&ff_mm_info.phy_page_count), atomic_read(&ff_mm_info.phy_page_count) * 4);
}

static void ff_show_management_info(void)
{
        printk("Physical Memory Pages Used: %d Pages = %d KB\n\n", 
                        atomic_read(&ff_mm_info.phy_page_count) - atomic_read(&ff_mm_info.phy_page_now_count),
                        (atomic_read(&ff_mm_info.phy_page_count) - atomic_read(&ff_mm_info.phy_page_now_count)) * 4);
}

static void ff_test_mm(void)
{
}

