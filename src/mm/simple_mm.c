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
static void init(uint32_t mstart, uint32_t mend);
static uint32_t alloc_pages(uint32_t n);
static void free_pages(uint32_t page, uint32_t n);
static void show_memory_info(void);
static void show_management_info(void);
static void test_mm(void);

// 管理结构
struct pmm_manager simple_mm = {
                "Simple_Memory_Managentment",
                &init,
                &alloc_pages,
                &free_pages,
                &show_memory_info,
                &show_management_info,
                &test_mm
};

static struct smm_struct smm_info;

static void init(uint32_t mstart, uint32_t mend)
{
        smm_info.smm_start_addr = mstart;
        smm_info.smm_end_addr = mend;

        uint32_t page_addr = mstart;
        uint32_t end_addr = mend;
        while (page_addr < end_addr && page_addr < PMM_MAX_SIZE) {
                free_pages(page_addr, 1);
                page_addr += PMM_PAGE_SIZE;
                smm_info.phy_page_count++;
        }

        smm_info.phy_page_now_count = smm_info.phy_page_count;
}

static uint32_t alloc_pages(uint32_t n)
{
        assert(smm_info.pmm_stack_top != 0, "out of memory");

        uint32_t page = smm_info.pmm_stack[smm_info.pmm_stack_top--];
        smm_info.phy_page_now_count--;

        return page;
}

static void free_pages(uint32_t page, uint32_t n)
{
        assert(smm_info.pmm_stack_top != PAGE_MAX_SIZE, "out of pmm_stack stack");

        smm_info.pmm_stack[++smm_info.pmm_stack_top] = page;
        smm_info.phy_page_now_count++;
}

static void show_memory_info(void)
{
        printk("memory management start address: %08X\n", smm_info.smm_start_addr);
        printk("memory management end   address: %08X\n\n", smm_info.smm_end_addr);

        printk("Physical Memory Pages Total: %d Pages = %d KB\n",
                        smm_info.phy_page_count, smm_info.phy_page_count * 4);
}

static void show_management_info(void)
{
        printk("Physical Memory Pages Used: %d Pages = %d KB\n\n", 
                        smm_info.phy_page_count - smm_info.phy_page_now_count,
                        (smm_info.phy_page_count - smm_info.phy_page_now_count) * 4);
}

static void test_mm(void)
{
        printk("\n%s Test Now:\n\n", simple_mm.name);

        uint32_t page1 = simple_mm.alloc_pages(1);
        printk("Alloc Page In: %08X\n", page1);

        uint32_t page2 = simple_mm.alloc_pages(1);
        printk("Alloc Page In: %08X\n", page2);
                
        simple_mm.show_management_info();

        printk("Free Page In: %08X\n", page1);
        simple_mm.free_pages(page1, 1);
        simple_mm.show_management_info();

        printk("Free Page In: %08X\n", page2);
        simple_mm.free_pages(page2, 1);
        simple_mm.show_management_info();
}
        
