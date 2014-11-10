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
#include <mm/mm.h>
#include <lib/string.h>

// 内核页目录
pgd_t pgd_kern[PGD_SIZE];

void init_vmm(void)
{
        // 注册页错误中断的处理函数 
        register_interrupt_handler(INT_PAGE_FAULT, &do_pgfault);

        //switch_pgd((uint32_t)pgd_kern - PAGE_OFFSET);
}

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{       
        uint32_t pgd_idx = PGD_INDEX(va);
        uint32_t pte_idx = PTE_INDEX(va); 
        
        pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
        if (!pte) {
                pte = (pte_t *)alloc_pages(1);
                pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;
                pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
        } else {
                pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
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
        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

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
        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

        // 如果地址有效而且指针不为NULL，则返回地址
        if (pte[pte_idx] != 0 && pa) {
                 *pa = pte[pte_idx] & PAGE_MASK;
                return 1;
        }

        return 0;
}

