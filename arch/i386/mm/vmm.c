/*
 * =====================================================================================
 *
 *       Filename:  vmm.c
 *
 *    Description:  虚拟内存管理
 *
 *        Version:  1.0
 *        Created:  2014年11月05日 09时55分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <arch.h>
#include <common.h>
#include <debug.h>
#include <mm/mm.h>
#include <lib/string.h>

// 内核页目录
pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

// 内核页表起始
static pte_t *pte_addr = (pte_t *)((uint32_t)kern_end + KERNBASE);

void vmm_init(void)
{
        // 注册页错误中断的处理函数 
        register_interrupt_handler(INT_PAGE_FAULT, &do_page_fault);
        
        // 页表数组指针
        pte_t (*pte_kern)[PTE_SIZE] = (pte_t (*)[PTE_SIZE])pte_addr;

        // 构造页目录(MMU需要的是物理地址，此处需要减去偏移)
        uint32_t pgd_idx = PGD_INDEX(PAGE_OFFSET);
        for (uint32_t i = pgd_idx; i < pgd_idx + PTE_COUNT; ++i) {
                pgd_kern[i] = ((uint32_t)ka_to_pa(pte_kern[i])) | PAGE_PRESENT | PAGE_WRITE;
        }

        // 构造页表映射，内核 0xC0000000～0xF8000000 映射到 物理 0x00000000～0x38000000 (物理内存前896MB)
        uint32_t *pte_start = (uint32_t *)(pte_addr + PTE_SIZE * pgd_idx);
        for (uint32_t i = 0; i < PTE_SIZE * PTE_COUNT; ++i) {
                pte_start[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
        }

        switch_pgd((uint32_t)ka_to_pa(pgd_kern));
}

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{       
        uint32_t pgd_idx = PGD_INDEX(va);
        uint32_t pte_idx = PTE_INDEX(va); 
        
        pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
        if (!pte) {
                pte = (pte_t *)alloc_pages(1);
                pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;
                pte = (pte_t *)pa_to_ka(pte);
        } else {
                pte = (pte_t *)pa_to_ka(pte);
        }

        pte[pte_idx] = (pa & PAGE_MASK) | flags;

        tlb_reload_page(va);
}

void unmap(pgd_t *pgd_now, uint32_t va)
{
        uint32_t pgd_idx = PGD_INDEX(va);
        uint32_t pte_idx = PTE_INDEX(va);

        pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);

        if (!pte) {
                return;
        }

        // 转换到内核线性地址
        pte = (pte_t *)(pa_to_ka(pte));

        pte[pte_idx] = 0;
        
        tlb_reload_page(va);
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
        uint32_t pgd_idx = PGD_INDEX(va);
        uint32_t pte_idx = PTE_INDEX(va);

        pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
        if (!pte) {
              return 0;
        }
        
        // 转换到内核线性地址
        pte = (pte_t *)(pa_to_ka(pte));

        // 如果地址有效而且指针不为NULL，则返回地址
        if (pte[pte_idx] != 0 && pa) {
                 *pa = pte[pte_idx] & PAGE_MASK;
                return 1;
        }

        return 0;
}

