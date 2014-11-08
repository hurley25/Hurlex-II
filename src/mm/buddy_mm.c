/*
 * =====================================================================================
 *
 *       Filename:  buddy_mm.c
 *
 *    Description:  伙伴算法的内存管理
 *
 *        Version:  1.0
 *        Created:  2014年11月06日 11时47分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <mm/pmm.h>
#include <mm/buddy_mm.h>
#include <debug.h>

// 每个独立的内存管理算法必须实现的4个接口函数
static void buddy_init(page_t *pages, uint32_t n);
static uint32_t buddy_alloc_pages(uint32_t n);
static void buddy_free_pages(uint32_t base, uint32_t n);
static uint32_t buddy_free_pages_count(void);

// 管理结构
struct pmm_manager buddy_mm = {
                "Simple_Memory_Managentment",
                &buddy_init,
                &buddy_alloc_pages,
                &buddy_free_pages,
                &buddy_free_pages_count
};

// 内存管理结构
struct buddy_mm_struct {
        uint32_t buddy_mm_start_addr;                // 物理内存管理的起始地址
        uint32_t buddy_mm_end_addr;                  // 物理内存管理的结束地址
        uint32_t phy_page_count;                     // 物理内存页的总数量
        volatile uint32_t phy_page_now_count;        // 物理内存页的当前数量
};

static struct buddy_mm_struct buddy_mm_info;

static void buddy_show_memory_info(void);
static void buddy_show_management_info(void);
static void buddy_test_mm(void);

static void buddy_init(page_t *pages, uint32_t n)
{
//        smm_info.smm_start_addr = mstart;
//        smm_info.smm_end_addr = mend;
//
//        uint32_t page_addr = mstart;
//        uint32_t end_addr = mend;
//        while (page_addr < end_addr && page_addr < PMM_MAX_SIZE) {
//                buddy_free_pages(page_addr, 1);
//                page_addr += PMM_PAGE_SIZE;
//                smm_info.phy_page_count++;
//        }
//
//        smm_info.phy_page_now_count = smm_info.phy_page_count;

        buddy_show_memory_info();
        buddy_show_management_info();
        buddy_test_mm();
}

static uint32_t buddy_alloc_pages(uint32_t n)
{
        return 0;
}

static void buddy_free_pages(uint32_t base, uint32_t n)
{
}

static uint32_t buddy_free_pages_count(void)
{
        return 0;
}

static void buddy_show_memory_info(void)
{
//        printk("memory management start address: %08X\n", smm_info.smm_start_addr);
//        printk("memory management end   address: %08X\n\n", smm_info.smm_end_addr);
//
//        printk("Physical Memory Pages Total: %d Pages = %d KB\n",
//                        smm_info.phy_page_count, smm_info.phy_page_count * 4);
}

static void buddy_show_management_info(void)
{
//        printk("Physical Memory Pages Used: %d Pages = %d KB\n\n", 
//                        smm_info.phy_page_count - smm_info.phy_page_now_count,
//                        (smm_info.phy_page_count - smm_info.phy_page_now_count) * 4);
}

static void buddy_test_mm(void)
{
//        printk("\n%s Test Now:\n\n", buddy_mm.name);
//
//        uint32_t page1 = buddy_mm.alloc_pages(1);
//        printk("Alloc Page In: %08X\n", page1);
//
//        uint32_t page2 = buddy_mm.alloc_pages(1);
//        printk("Alloc Page In: %08X\n", page2);
//                
//        buddy_mm.show_management_info();
//
//        printk("Free Page In: %08X\n", page1);
//        buddy_mm.free_pages(page1, 1);
//        buddy_mm.show_management_info();
//
//        printk("Free Page In: %08X\n", page2);
//        buddy_mm.free_pages(page2, 1);
//        buddy_mm.show_management_info();
}

