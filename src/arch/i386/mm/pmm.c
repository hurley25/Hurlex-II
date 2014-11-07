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
#include <lib/string.h>

// 物理内存管理算法
static const struct pmm_manager *pmm_manager;

// 物理页帧数组指针
page_t *phy_pages;

// 物理页帧数组长度
uint32_t phy_pages_count;

// 获取可用内存的起始和结束地址
static void get_ram_info(e820map_t *e820map);

// 物理内存页初始化
static void phy_pages_init(void);

void init_pmm(void)
{
	show_kernel_memory_map();
	
        e820map_t e820map;
        bzero(&e820map, sizeof(e820map));
        
        get_ram_info(&e820map);
        phy_pages_init();
        page_init();
        
        show_memory_info();
        show_management_info();
        test_mm();
}

static void get_ram_info(e820map_t *e820map)
{
        mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
        mmap_entry_t *mmap_end_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;

        mmap_entry_t *map_entry;
        for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {
                if (map_entry->type == MULTIBOOT_TYPE_RAM && map_entry->base_addr_low >= RAM_KERNEL_START) {
                        e820map->map[e820map->count].addr_low = map_entry->base_addr_low;
                        e820map->map[e820map->count].addr_high = map_entry->base_addr_high;
                        e820map->map[e820map->count].length_low = map_entry->length_low;
                        e820map->map[e820map->count].length_high = map_entry->length_high;
                        e820map->map[e820map->count].type = E820_ARM;
                        e820map->count++;
                }
        }
}

static void phy_pages_init(void)
{
}

void page_init(void)
{
        pmm_manager = &simple_mm;
        pmm_manager->page_init();
}

page_t *alloc_pages(uint32_t n)
{
        page_t *page;
        uint32_t eflag;
        
        local_intr_store(eflag);
        page = pmm_manager->alloc_pages(n);
        local_intr_restore(eflag);

        return page;
}

void free_pages(page_t *page, uint32_t n)
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

