/*
 * =====================================================================================
 *
 *       Filename:  pmm.c
 *
 *    Description:  物理内存管理
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 13时13分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <multiboot.h>
#include <common.h>
#include <debug.h>
#include <mm/mm.h>

// 物理内存页面管理的栈
static uint32_t pmm_stack[PAGE_MAX_SIZE+1];

// 物理内存管理的栈指针
static uint32_t pmm_stack_top;

// 物理内存页的数量
uint32_t phy_page_count;

void show_memory_map(void)
{
        uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
        uint32_t mmap_length = glb_mboot_ptr->mmap_length;

        printk("Memory map:\n\n");

        mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
        for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) {
                printk("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
                        (uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
                        (uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
                        (uint32_t)mmap->type);
        }

        printk("\n");
}

void show_kernel_memory_map(void)
{
        printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);

        /*
	printk("\nkernel segment in memory:\n");
	printk("  .init.text    0x%08X ~ 0x%08X \n", kern_init_text_start, kern_init_text_end);
	printk("  .init.data    0x%08X ~ 0x%08X \n", kern_init_data_start, kern_init_data_end);
	printk("  .text         0x%08X ~ 0x%08X \n", kern_text_start, kern_text_end);
	printk("  .data         0x%08X ~ 0x%08X \n", kern_data_start, kern_data_end);
        */
	printk("\nkernel in memory used: %d KB = %d Pages\n\n",
                        (kern_end - kern_start) / 1024, (kern_end - kern_start) / 1024 / 4);

}

void init_pmm(void)
{
        mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
        mmap_entry_t *mmap_end_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;

        mmap_entry_t *map_entry;
        for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {

                // 如果是可用内存 ( 按照协议，1 表示可用内存，其它数字指保留区域 )
                if (map_entry->type == 1 && map_entry->base_addr_low == 0x100000) {
                        uint32_t page_addr = map_entry->base_addr_low + (uint32_t)(kern_end - kern_start);
                        uint32_t end_addr = map_entry->base_addr_low + map_entry->length_low;
                        printk("memory management start address: %08X\n", page_addr);
                        while (page_addr < end_addr && page_addr <= PMM_MAX_SIZE) {
                                pmm_free_page(page_addr);
                                page_addr += PMM_PAGE_SIZE;
                                phy_page_count++;
                        }
                        printk("memory management end address:   %08X\n\n", page_addr - PMM_PAGE_SIZE);
                }
        }
        printk("Physical Memory Pages Total: %d Pages = %d MB\n", phy_page_count, phy_page_count * 4 / 1024);
}

uint32_t pmm_alloc_page(void)
{
        assert(pmm_stack_top != 0, "out of memory");

        uint32_t page = pmm_stack[pmm_stack_top--];

        return page;
}

void pmm_free_page(uint32_t p)
{
        assert(pmm_stack_top != PAGE_MAX_SIZE, "out of pmm_stack stack");

        pmm_stack[++pmm_stack_top] = p;
}

