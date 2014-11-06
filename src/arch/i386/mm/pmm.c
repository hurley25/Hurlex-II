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

#include <mboot.h>
#include <types.h>
#include <debug.h>
#include <sync.h>
#include <mm/mm.h>

// 选择物理内存管理算法
static struct pmm_manager *pmm_manager = &simple_mm;

void init_pmm(void)
{
	show_kernel_memory_map();
	
        uint32_t mstart, mend;
        get_ram_info(&mstart, &mend);
        
        page_init(mstart, mend);
        show_memory_info();
        show_management_info();
        test_mm();
}

void get_ram_info(uint32_t *mstart, uint32_t *mend)
{
        mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
        mmap_entry_t *mmap_end_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;

        mmap_entry_t *map_entry;
        for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {
                if (map_entry->type == MULTIBOOT_TYPE_RAM && map_entry->base_addr_low == RAM_KERNEL_START) {
                        *mstart = map_entry->base_addr_low + (uint32_t)(kern_end - kern_start);
                        *mend = map_entry->base_addr_low + map_entry->length_low;
                        break;
                }
        }
}

void page_init(uint32_t mstart, uint32_t mend)
{
        pmm_manager->page_init(mstart, mend);
}

uint32_t alloc_pages(uint32_t n)
{
        uint32_t page;
        uint32_t eflag;
        
        local_intr_store(eflag);
        page = pmm_manager->alloc_pages(n);
        local_intr_restore(eflag);

        return page;
}

void free_pages(uint32_t page, uint32_t n)
{
        uint32_t eflag;
        
        local_intr_store(eflag);
        pmm_manager->free_pages(page, n);
        local_intr_restore(eflag);
}

void show_memory_info(void)
{
        pmm_manager->show_memory_info();
}

void show_management_info(void)
{
        pmm_manager->show_management_info();
}

void test_mm(void)
{
        pmm_manager->test_mm();
}

