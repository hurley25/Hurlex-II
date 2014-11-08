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
        uint32_t phy_page_count;       // 物理内存页的总数量
        uint32_t phy_page_now_count;   // 物理内存页的当前数量
};

static struct ff_mm_struct ff_mm_info;

static void ff_show_memory_info(void);
static void ff_show_management_info(void);
static void ff_test_mm(void);

static void ff_page_init(page_t *pages, uint32_t n)
{
        ff_mm_info.phy_page_count = n;
        
        printk("pages_t");
        
        //ff_mm_info.mm_start_addr = ;
        //ff_mm_info.mm_end_addr = ;
        
        for (uint32_t i = 0; i < n; i++) {
                
        }
        
        ff_show_memory_info();
        ff_show_management_info();
        ff_test_mm();
}

static uint32_t ff_alloc_pages(uint32_t n)
{

        ff_mm_info.phy_page_now_count -= n;

        return 0;
}

static void ff_free_pages(uint32_t base, uint32_t n)
{
         ff_mm_info.phy_page_now_count += n;
}

static uint32_t ff_free_pages_count(void)
{
        return ff_mm_info.phy_page_now_count;
}


static void ff_show_memory_info(void)
{
        printk("Physical Memory Pages Total: %d Pages = %d KB\n\n",
                        ff_mm_info.phy_page_count, ff_mm_info.phy_page_count * 4);
}

static void ff_show_management_info(void)
{
        printk("Physical Memory Pages Used: %d Pages = %d KB\n\n", 
                        ff_mm_info.phy_page_count - ff_mm_info.phy_page_now_count,
                        (ff_mm_info.phy_page_count - ff_mm_info.phy_page_now_count) * 4);
}

static void ff_test_mm(void)
{
}

