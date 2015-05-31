/*
 * =====================================================================================
 *
 *       Filename:  ff_mm.c
 *
 *    Description:  First-Fit 算法内存管理
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

#include <atomic.h>
#include <debug.h>
#include <mm/pmm.h>
#include <mm/ff_mm.h>
#include <lib/list.h>

// 简化 list_entry 使用
#define le_to_page(le) list_entry(le, page_t, list)

// 每个独立的内存管理算法必须实现的4个接口函数
static void ff_page_init(page_t *pages, uint32_t n);
static uint32_t ff_alloc_pages(uint32_t n);
static void ff_free_pages(uint32_t addr, uint32_t n);
static uint32_t ff_free_pages_count(void);

// 管理结构
struct pmm_manager ff_mm_manager = {
                "First_Fit_Memory_Managentment",
                &ff_page_init,
                &ff_alloc_pages,
                &ff_free_pages,
                &ff_free_pages_count
};

// 内存管理结构
struct ff_mm_struct {
        struct list_head free_list;    // 空闲内存块链
        uint32_t mm_addr_start;        // 物理内存页管理起始地址
        uint32_t mm_addr_end;          // 物理内存页管理结束地址
        atomic_t phy_page_count;       // 物理内存页的总数量
        atomic_t phy_page_now_count;   // 物理内存页的当前数量
};

static struct ff_mm_struct ff_mm_info;

static void ff_show_memory_info(void);
static void ff_show_management_info(void);
static void ff_test_mm(void) __UNUSED__;

static void ff_page_init(page_t *pages, uint32_t n)
{
        atomic_set(&ff_mm_info.phy_page_count, n);
        atomic_set(&ff_mm_info.phy_page_now_count, n);
        
        ff_mm_info.mm_addr_start = page_to_addr(&pages[0]);
        ff_mm_info.mm_addr_end = page_to_addr(&pages[n-1]) + PMM_PAGE_SIZE;

        INIT_LIST_HEAD(&ff_mm_info.free_list);
        for (page_t *p = pages; p < pages + n; ++p) {
                set_page_reserved_flag(p);
                list_add_before(&p->list, &ff_mm_info.free_list);
        }

        pages[0].ncount = n;
        set_page_ncount_flag(&pages[0]);
        
        ff_show_memory_info();
        //ff_test_mm();
}

static uint32_t ff_alloc_pages(uint32_t n)
{
        if (n <= 0 || n > (uint32_t)atomic_read(&ff_mm_info.phy_page_now_count)) {
                return 0;
        }
        
        struct list_head *le, *len;

        le = &ff_mm_info.free_list;
        while ((le = le->next) != &ff_mm_info.free_list) {
                page_t *p = le_to_page(le);

                // 当前链之后的空闲内存页数满足需求
                if (is_page_ncount(p) && p->ncount >= n) {
                        for (uint32_t i = 0; i < n; ++i) {
                                len = le->next;
                                page_t *pp = le_to_page(le);
                                clear_page_reserved_flag(pp);
                                list_del(le);
                                le = len;
                        }
                }

                // 切割当前链后重新计算剩余值
                if (p->ncount > n) {
                        set_page_ncount_flag(le_to_page(le));
                        (le_to_page(le))->ncount = p->ncount - n;
                }

                p->ncount = 0;
                clear_page_ncount_flag(p);

                atomic_sub(&ff_mm_info.phy_page_now_count, n);

                return page_to_addr(p);
        }

        // 找不到满足请求的连续的物理内存块
        return 0;
}

static void ff_free_pages(uint32_t addr, uint32_t n)
{
        if (n <= 0) {
                return;
        }

        page_t *base = addr_to_page(addr);
        assert(!is_page_reserved(base), "ff_free_pages error!");

        set_page_ref(base, 0);
        set_page_ncount_flag(base);
        base->ncount = n;

        // 找到插入点
        page_t *p;
        struct list_head *le = &ff_mm_info.free_list;
        while ((le = le->next) != &ff_mm_info.free_list) {
                p = le_to_page(le);
                if (p > base) {
                        break;
                }
        }
        // 插入所有释放的连续节点到 le 节点之前
        // 即使 le 此时回到头节点也依旧正确
        for (p = base; p < base + n; ++p) {
                list_add_before(&p->list, le);
        }

        // 如果 base 与之后的内存连续，则合并
        p = le_to_page(le);
        if (base + n == p) {
                base->ncount += p->ncount;
                clear_page_ncount_flag(p);
                p->ncount = 0;
        }

        // 如果 base 与之前的内存连续，则合并
        le = base->list.prev;
        p = le_to_page(le);
        if (le != &ff_mm_info.free_list && p == base - 1) {
                while (le != &ff_mm_info.free_list) {
                        if (is_page_ncount(p)) {
                                p->ncount += base->ncount;
                                clear_page_ncount_flag(base);
                                base->ncount = 0;
                        }
                        le = le->prev;
                        p = le_to_page(le);
                }
        }

        atomic_add(&ff_mm_info.phy_page_now_count, n);
}

static uint32_t ff_free_pages_count(void)
{
        return atomic_read(&ff_mm_info.phy_page_now_count);
}


static void ff_show_memory_info(void)
{
        printk("Physical Memory Pages Start: %08X  End: %08X\n\n", ff_mm_info.mm_addr_start, ff_mm_info.mm_addr_end);
        printk("Physical Memory Pages Total: %d Pages = %d KB\n\n",
                        atomic_read(&ff_mm_info.phy_page_count), atomic_read(&ff_mm_info.phy_page_count) * 4);
}

static void ff_show_management_info(void)
{
        printk("Physical Memory Pages Used: %d Pages = %d KB\n\n", 
                        atomic_read(&ff_mm_info.phy_page_count) - atomic_read(&ff_mm_info.phy_page_now_count),
                        (atomic_read(&ff_mm_info.phy_page_count) - atomic_read(&ff_mm_info.phy_page_now_count)) * 4);
}

static void ff_test_mm(void)
{
        printk("\n%s Test Now:\n\n", ff_mm_manager.name);

        uint32_t page1 = ff_alloc_pages(1);
        printk("Alloc Page 1 In: %08X\n", page1);
        ff_show_management_info();

        uint32_t page2 = ff_alloc_pages(2);
        printk("Alloc Page 2 In: %08X\n", page2);
        ff_show_management_info();

        printk("Free Page 1 In: %08X\n", page1);
        ff_free_pages(page1, 1);
        ff_show_management_info();

        printk("Free Page 2 In: %08X\n", page2);
        ff_free_pages(page2, 2);
        ff_show_management_info();

        uint32_t page3 = ff_alloc_pages(10);
        printk("Alloc Page 10 In: %08X\n", page3);
        ff_show_management_info();

        printk("Free Page 10 In: %08X\n", page3);
        ff_free_pages(page3, 10);
        ff_show_management_info();
}

