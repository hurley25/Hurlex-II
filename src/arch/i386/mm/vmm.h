/*
 * =====================================================================================
 *
 *       Filename:  vmm.h
 *
 *    Description:  虚拟内存管理相关
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 11时41分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */
 
#ifndef INCLUDE_MM_VMM_H
#define INCLUDE_MM_VMM_H

#include <arch.h>
#include <types.h>
#include <lib/list.h>

/* *
 * Virtual memory map:
 *
 *
 *     4G ------------------> +---------------------------------+
 *                            |                                 |
 *                            |         Empty Memory (*)        |
 *                            |                                 |
 *                            +---------------------------------+ 0xF8000000
 *                            |                                 |
 *                            |                                 |
 *                            |    Remapped Physical Memory     | RW/-- KMEMSIZE
 *                            |                                 |
 *                            |                                 |
 *                            +---------------------------------+ 
 *                            |             Kernel              |
 *     KERNBASE ------------> +---------------------------------+ 0xC0000000
 *                            |        Invalid Memory (*)       | --/--
 *     USERTOP -------------> +---------------------------------+ 0xB0000000
 *                            |           User stack            |
 *                            +---------------------------------+
 *                            :                                 :
 *                            |         ~~~~~~~~~~~~~~~~        |
 *                            :                                 :
 *                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                            |       User Program & Heap       |
 *     UTEXT ---------------> +---------------------------------+ 0x00800000
 *                            |        Invalid Memory (*)       | --/--
 *      USERBASE -----------> +---------------------------------+ 0x00200000
 *                            |        Invalid Memory (*)       | --/--
 *     0 -------------------> +---------------------------------+ 0x00000000
 * (*) Note: The kernel ensures that "Invalid Memory" is *never* mapped.
 *     "Empty Memory" is normally unmapped, but user programs may map pages
 *     there if desired.
 *
 * */

// A linear address 'la' has a three-part structure as follows:
//
// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |     Index      |                     |
// +----------------+----------------+---------------------+
//   \PGD_INDEX(la)/ \ PTE_INDEX(la) /  \OFFSET_INDEX(la)/

// 虚拟分页大小(4KB)
#define PAGE_SIZE 	  (0x1000)

// 页掩码，用于 4KB 对齐
#define PAGE_MASK         (0xFFFFF000)

// 内核起始虚拟地址
#define KERNBASE          (0xC0000000)

// 内核管理内存的大小
#define KMEMSIZE          (0x38000000)

// 内核页表大小
#define KVPAGE_SIZE       (0x400000)

// 内核管理的物理内存的顶端地址
#define KERNTOP           (KERNBASE + KMEMSIZE)

// 内核的偏移地址
#define PAGE_OFFSET 	  KERNBASE

// 每个页表可以映射的内存数
#define PAGE_MAP_SIZE 	  (0x400000)

// 映射 KMEM_SIZE 的内存所需要的页数
#define PTE_COUNT         (KMEMSIZE/PAGE_MAP_SIZE)

// 获取一个地址的页目录项
#define PGD_INDEX(x)      (((x) >> 22) & 0x3FF)

// 获取一个地址的页表项
#define PTE_INDEX(x)      (((x) >> 12) & 0x3FF)

// 获取一个地址的页內偏移
#define OFFSET_INDEX(x)   ((x) & 0xFFF)

// P--位0是存在标识，为 1 则内存页在内存中
#define PAGE_PRESENT 	(0x1)

// R/W--位1是读/写标识，如果等于 1，表示页面可以被读、写或执行。
#define PAGE_WRITE 	(0x2)

// U/S--位2是用户/超级用户标识，为 1 则任何特权级上的程序都可以访问该页面。
#define PAGE_USER 	(0x4)

// 页表成员数
#define PGD_SIZE (PAGE_SIZE/sizeof(pte_t))

// 页表成员数
#define PTE_SIZE (PAGE_SIZE/sizeof(uint32_t))

// 任务内存信息
struct mm_struct {
        pgd_t *pgdir;
        int vma_count;
        struct list_head vma_list;
};

// 任务虚拟内存区间
struct vma_struct {
        struct mm_struct *mm;
        uint32_t vm_start;
        uint32_t vm_end;
        uint32_t vm_flags;
        struct list_head list;
};

#define le_to_vma(le) list_entry(le, struct vma_struct, list)

#define VM_READ       (1u << 0)
#define VM_WRITE      (1u << 1)
#define VM_EXEC       (1u << 2)

// 物理地址转换内核虚拟地址
static inline void *pa_to_ka(void *pa)
{
        return (void *)((uint32_t)pa + KERNBASE);
}

// 内核虚拟地址转换物理地址
static inline void *ka_to_pa(void *ka)
{
        return (void *)((uint32_t)ka - KERNBASE);
}

// 内核页目录区域
extern pgd_t pgd_kern[];

// 虚拟内存管理初始化
void vmm_init(void);

// 使用 flags 指出的页权限，把物理地址 pa 映射到虚拟地址 va
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

// 取消虚拟地址 va 的物理映射
void unmap(pgd_t *pgd_now, uint32_t va);

// 如果虚拟地址 va 映射到物理地址则返回 1
// 同时如果 pa 不是空指针则把物理地址写入 pa 参数
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

// 页错误中断的函数处理
void do_page_fault(pt_regs_t *regs);

#endif 	// INCLUDE_MM_VMM_H
