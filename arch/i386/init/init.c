/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  内核初始化
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 10时03分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <common.h>
#include <debug.h>
#include <init.h>
#include <mm/mm.h>

// 开启分页机制之后的 Multiboot 数据指针
multiboot_t *glb_mboot_ptr;

// 开启分页机制之后的内核栈
uint8_t kern_stack[STACK_SIZE]  __attribute__ ((aligned(STACK_SIZE)));

// 内核栈的栈顶
uint32_t kern_stack_top = (uint32_t)kern_stack + STACK_SIZE;

// 内核使用的临时页表和页目录
// 该地址必须是页对齐的地址，内存 0-640KB 肯定是空闲的
__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pte_t *pte_low  = (pte_t *)0x2000;
__attribute__((section(".init.data"))) pte_t *pte_hign = (pte_t *)0x3000;

// 映射临时页表
__attribute__((section(".init.text"))) void mmap_tmp_page(void);

// 启用分页
__attribute__((section(".init.text"))) void enable_paging(void);

// 内核入口函数
__attribute__((section(".init.text"))) void kern_entry(void)
{
        // 映射临时页表
        mmap_tmp_page();

        // 启用分页
        enable_paging();

        // 切换临时内核栈到分页后的新栈
        __asm__ volatile ("mov %0, %%esp\n\t"
                        "xor %%ebp, %%ebp" : : "r" (kern_stack_top));

        // 更新全局 multiboot_t 指针指向
        glb_mboot_ptr = (multiboot_t *)((uint32_t)mboot_ptr_tmp + PAGE_OFFSET);

        // 调用内核初始化函数
        kern_init();

        // 之前的函数调用链自栈切换后断开，无法再返回之前的调用点
}

// 映射临时页表
__attribute__((section(".init.text"))) void mmap_tmp_page(void)
{
        pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;

        for (int i = 0; i < 4; ++i) {
                uint32_t pgd_idx = PGD_INDEX(PAGE_OFFSET + PAGE_MAP_SIZE * i);
                pgd_tmp[pgd_idx] = ((uint32_t)pte_hign + PAGE_SIZE * i) | PAGE_PRESENT | PAGE_WRITE;
        }

        // 映射内核虚拟地址 4MB 到物理地址的前 4MB
        // 因为 .init.text 段的代码在物理地址前 4MB 处(肯定不会超出这个范围)，
        // 开启分页后若此处不映射，代码执行立即会出错，离开 .init.text 段后的代码执行，
        // 不再需要映射物理前 4MB 的内存
        for (int i = 0; i < 1024; i++) {
                pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
        }

        // 映射 0x00000000-0x01000000 的物理地址到虚拟地址 0xC0000000-0xC1000000
        for (int i = 0; i < 1024 * 4; i++) {
                pte_hign[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
        }
        
        // 设置临时页表
        __asm__ volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));
}

// 启用分页
__attribute__((section(".init.text"))) void enable_paging(void)
{
        uint32_t cr0;
        
        __asm__ volatile ("mov %%cr0, %0" : "=r" (cr0));
        // 最高位 PG 位置 1，分页开启
        cr0 |= (1u << 31);
        __asm__ volatile ("mov %0, %%cr0" : : "r" (cr0));
}

