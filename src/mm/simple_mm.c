/*
 * =====================================================================================
 *
 *       Filename:  simple_mm.c
 *
 *    Description:  简单的内存管理
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
#include <mm/simple_mm.h>
#include <debug.h>

// 每个独立的内存管理算法必须实现的6个接口函数
static void simple_page_init(page_t *pages, uint32_t n);
static page_t *simple_alloc_pages(uint32_t n);
static void simple_free_pages(page_t *page, uint32_t n);
static void simple_show_memory_info(void);
static void simple_show_management_info(void);
static void simple_test_mm(void);

// 管理结构
struct pmm_manager simple_mm = {
                "Simple_Memory_Managentment",
                &simple_page_init,
                &simple_alloc_pages,
                &simple_free_pages,
                &simple_show_memory_info,
                &simple_show_management_info,
                &simple_test_mm
};

// 物理内存页面管理的最大值(512MB)
#define PAGE_MAX_SIZE (0x20000000/0x1000)

// 内存管理结构
struct smm_struct {
        page_t *pmm_stack[PAGE_MAX_SIZE];        // 物理内存页面管理的栈
        volatile uint32_t pmm_stack_top;        // 物理内存管理的栈指针
        uint32_t smm_start_addr;                // 物理内存管理的起始地址
        uint32_t smm_end_addr;                  // 物理内存管理的结束地址
        uint32_t phy_page_count;                // 物理内存页的总数量
        volatile uint32_t phy_page_now_count;   // 物理内存页的当前数量
};

static struct smm_struct smm_info;

static void simple_page_init(page_t *pages, uint32_t n)
{
        smm_info.phy_page_count = n;
        smm_info.smm_start_addr = pages[0].addr;
        smm_info.smm_end_addr = pages[n-1].addr;
        
        for (uint32_t i = 0; i < n; i++) {
                free_pages(&pages[i], 1);
        }
}

static page_t *simple_alloc_pages(uint32_t n)
{
        assert(n == 1, "simple mm must alloc 1");
        assert(smm_info.pmm_stack_top != 0, "out of memory");

        page_t *page = smm_info.pmm_stack[smm_info.pmm_stack_top--];
        smm_info.phy_page_now_count--;

        return page;
}

static void simple_free_pages(page_t *page, uint32_t n)
{
        assert(n == 1, "simple mm must free 1");
        assert(smm_info.pmm_stack_top != PAGE_MAX_SIZE, "out of pmm_stack stack");
        
         smm_info.pmm_stack[++smm_info.pmm_stack_top] = page;
         smm_info.phy_page_now_count++;
}

static void simple_show_memory_info(void)
{
        printk("memory management start address: %08X\n", smm_info.smm_start_addr);
        printk("memory management end   address: %08X\n\n", smm_info.smm_end_addr);

        printk("Physical Memory Pages Total: %d Pages = %d KB\n",
                        smm_info.phy_page_count, smm_info.phy_page_count * 4);
}

static void simple_show_management_info(void)
{
        printk("Physical Memory Pages Used: %d Pages = %d KB\n\n", 
                        smm_info.phy_page_count - smm_info.phy_page_now_count,
                        (smm_info.phy_page_count - smm_info.phy_page_now_count) * 4);
}

static void simple_test_mm(void)
{
        printk("\n%s Test Now:\n\n", simple_mm.name);

        page_t *page1 = simple_mm.alloc_pages(1);
        printk("Alloc Page In: %08X\n", page1->addr);

        page_t *page2 = simple_mm.alloc_pages(1);
        printk("Alloc Page In: %08X\n", page2->addr);
                
        simple_mm.show_management_info();

        printk("Free Page In: %08X\n", page1->addr);
        simple_mm.free_pages(page1, 1);
        simple_mm.show_management_info();

        printk("Free Page In: %08X\n", page2->addr);
        simple_mm.free_pages(page2, 1);
        simple_mm.show_management_info();
}

