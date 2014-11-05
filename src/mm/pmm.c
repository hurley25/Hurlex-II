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
#include <types.h>
#include <debug.h>
#include <mm/mm.h>

// 选择物理内存管理算法
struct pmm_manager *mm = &simple_mm;

static void get_ram_info(uint32_t *mstart, uint32_t *mend);

void init_pmm(void)
{
        uint32_t mstart, mend;
        get_ram_info(&mstart, &mend);
        
        mm->init(mstart, mend);
        mm->show_memory_info();
        mm->show_management_info();
        mm->test_mm();
}

static void get_ram_info(uint32_t *mstart, uint32_t *mend)
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

