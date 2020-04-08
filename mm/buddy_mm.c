/*
 * =====================================================================================
 *
 *       Filename:  buddy_mm.c
 *
 *    Description:  伙伴算法算法内存管理
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
#include <mm/buddy_mm.h>
#include <lib/list.h>

// 简化 list_entry 使用
#define le_to_page(le) list_entry(le, page_t, list)

// 每个独立的内存管理算法必须实现的4个接口函数
static void buddy_page_init(page_t *pages, uint32_t n);
static uint32_t buddy_alloc_pages(uint32_t n);
static void buddy_free_pages(uint32_t addr, uint32_t n);
static uint32_t buddy_free_pages_count(void);

// 管理结构
struct pmm_manager buddy_mm_manager = {
                "Buddy_Memory_Managentment",
                &buddy_page_init,
                &buddy_alloc_pages,
                &buddy_free_pages,
                &buddy_free_pages_count
};

// 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
// 2^0 ~ 2^10
#define MAX_ORDER  10

// 内存管理结构
struct buddy_mm_struct {
        struct list_head free_list[MAX_ORDER + 1];      // 空闲内存块链
        atomic_t nr_free[MAX_ORDER + 1];                // 对应链上内存块数量
        page_t *pages_base;                             // 物理页管理结构起始地址
        uint32_t mm_addr_start;                         // 物理内存页管理起始地址
        uint32_t mm_addr_end;                           // 物理内存页管理结束地址
        atomic_t phy_page_count;                        // 物理内存页的总数量
        atomic_t phy_page_now_count;                    // 物理内存页的当前数量
};

static struct buddy_mm_struct buddy_mm_info;

// x from 0 ~ MAX_ORDER
#define free_list(x) (buddy_mm_info.free_list[x])
#define nr_free(x) (buddy_mm_info.nr_free[x])

static void buddy_show_memory_info(void);
static void buddy_show_management_info(void) __UNUSED__;
static void buddy_test_mm(void) __UNUSED__;

static void buddy_page_init(page_t *pages, uint32_t n)
{
        atomic_set(&buddy_mm_info.phy_page_count, n);
        atomic_set(&buddy_mm_info.phy_page_now_count, n);
        
        buddy_mm_info.pages_base = pages;
        buddy_mm_info.mm_addr_start = page_to_addr(&pages[0]);
        buddy_mm_info.mm_addr_end = page_to_addr(&pages[n-1]) + PMM_PAGE_SIZE;

        for (int i = 0; i <= MAX_ORDER; ++i) {
                INIT_LIST_HEAD(&free_list(i));
                atomic_set(&nr_free(i), 0);
        }

        uint32_t order = MAX_ORDER;
        uint32_t order_size = (1u << order);

        page_t *p = pages;
        while (n != 0) {
                while (n >= order_size) {
                        set_page_order_flag(p);
                        p->order = order;
                        list_add(&p->list, &free_list(order));
                        n -= order_size;
                        p += order_size;
                        atomic_inc(&nr_free(order));
                }
                order--;
                order_size >>= 1;
        }

        buddy_show_memory_info();
        buddy_show_management_info();
        buddy_test_mm();
}

static uint32_t get_order(uint32_t size)
{
        uint32_t order = 0, order_size = 1;

        while (order <= MAX_ORDER) {
                if (order_size >= size) {
                        return order;
                }
                order++;
                order_size <<= 1;
        }
        panic("getorder failed.");
        return 0;
}

static page_t *buddy_alloc_pages_sub(uint32_t order)
{
        assert(order <= MAX_ORDER, "buddy_alloc_pages_sub cannot alloc over 1024 pages");

        for (uint32_t cur_order = order; cur_order <= MAX_ORDER; ++cur_order) {
                if (!list_empty(&free_list(cur_order))) {
                        struct list_head *le = (&(free_list(cur_order)))->next;
                        page_t *page = le_to_page(le);
                        atomic_dec(&nr_free(cur_order));
                        list_del(le);
                        uint32_t size = 1u << cur_order;
                        while (cur_order > order) {
                                cur_order--;
                                size >>= 1;
                                page_t *buddy = page + size;
                                buddy->order = cur_order;
                                set_page_order_flag(buddy);
                                atomic_inc(&nr_free(cur_order));
                                list_add(&buddy->list, &free_list(cur_order));
                        }
                        clear_page_order_flag(page);

                        return page;
                }
        }

        return 0;
}

static uint32_t buddy_alloc_pages(uint32_t n)
{
        if (n <= 0 || n > (uint32_t)atomic_read(&buddy_mm_info.phy_page_now_count)) {
                return 0;
        }

        uint32_t order = get_order(n);
        uint32_t order_size = (1u << order);
        
        page_t *page = buddy_alloc_pages_sub(order);
        
        if (!page) {
                return 0;
        }

        if (n != order_size) {
                buddy_free_pages(page_to_addr(page + n), order_size -n); 
        }

        atomic_sub(&buddy_mm_info.phy_page_now_count, n);

        return page_to_addr(page);
}

static inline uint32_t page_to_idx(page_t *page)
{
        return (page - buddy_mm_info.pages_base);
}

static inline page_t *idx_to_page(uint32_t idx)
{
        return (buddy_mm_info.pages_base + idx);
}

static bool page_is_buddy(page_t *page, uint32_t order)
{
        if (page_to_idx(page) < (uint32_t)atomic_read(&buddy_mm_info.phy_page_count)) {
                return (is_page_order(page) && page->order == order);
        }

        return false;
}

static void buddy_free_pages_sub(page_t *base, uint32_t order)
{
        uint32_t buddy_idx, page_idx = page_to_idx(base);
        
        for (page_t *p = base; p != base + (1u << order); ++p) {
                set_page_ref(p, 0);
        }

        while (order < MAX_ORDER) {
                buddy_idx = page_idx ^ (1u << order);
                page_t *buddy = idx_to_page(buddy_idx);
                if (!page_is_buddy(buddy, order)) {
                        break;
                }
                atomic_dec(&nr_free(order));
                list_del(&buddy->list);
                clear_page_order_flag(buddy);
                page_idx &= buddy_idx;
                order++;
        }
        page_t *page = idx_to_page(page_idx);
        page->order = order;
        set_page_order_flag(page);
        atomic_inc(&nr_free(order));
        list_add(&page->list, &free_list(order));
}

static void buddy_free_pages(uint32_t addr, uint32_t n)
{
        if (n <= 0) {
                return;
        }

        page_t *base = addr_to_page(addr);

        uint32_t order = 0, order_size = 1;
        while (n >= order_size) {
                if ((page_to_idx(base) & order_size) != 0) {
                        buddy_free_pages_sub(base, order);
                        base += order_size;
                        n -= order_size;
                }
                order++;
                order_size <<= 1;
        }
        while (n != 0) {
                while (n < order_size) {
                        order--;
                        order_size >>= 1;
                }
                buddy_free_pages_sub(base, order);
                base += order_size;
                n -= order_size;
        }

        atomic_add(&buddy_mm_info.phy_page_now_count, n);
}

static uint32_t buddy_free_pages_count(void)
{
        return atomic_read(&buddy_mm_info.phy_page_now_count);
}


static void buddy_show_memory_info(void)
{
        printk("Physical Memory Pages Start: %08X  End: %08X\n\n",
                        buddy_mm_info.mm_addr_start, buddy_mm_info.mm_addr_end);
        printk("Physical Memory Pages Total: %d Pages = %d KB\n\n",
                        atomic_read(&buddy_mm_info.phy_page_count),
                        atomic_read(&buddy_mm_info.phy_page_count) * 4);
}

static void buddy_show_management_info(void)
{
        cprintk(rc_black, rc_green,"Buddy Memory Info:\n\n");
        uint32_t count = 0;
        for (uint32_t order = 0; order < MAX_ORDER + 1; ++order) {
                count += atomic_read(&nr_free(order)) * (1u << order);
                cprintk(rc_black, rc_green, "    order %2u (size: %4u * 4K) count: %3d\n",
                                order, 1u << order, atomic_read(&nr_free(order)));
        }
        cprintk(rc_black, rc_green, "\n        Buddy All Pages Count: %u\n\n", count);

        printk("Physical Memory Pages Used: %d Pages = %d KB\n\n", 
                        atomic_read(&buddy_mm_info.phy_page_count) - atomic_read(&buddy_mm_info.phy_page_now_count),
                        (atomic_read(&buddy_mm_info.phy_page_count) - atomic_read(&buddy_mm_info.phy_page_now_count)) * 4);
}

static void buddy_test_mm(void)
{
        cprintk(rc_black, rc_green, "\n%s Test Now:\n\n", buddy_mm_manager.name);

        cprintk(rc_black, rc_red, "before test:\n\n order 0~10 counts: ");
        for (uint32_t order = 0; order < MAX_ORDER + 1; ++order) {
                cprintk(rc_black, rc_red, "%3u ", atomic_read(&nr_free(order)));
        }
        printk("\n\n");

        uint32_t page[10];

        page[1] = buddy_alloc_pages(1);
        page[2] = buddy_alloc_pages(2);

        buddy_free_pages(page[1], 1);
        buddy_free_pages(page[2], 2);

        page[3] = buddy_alloc_pages(8);
        page[4] = buddy_alloc_pages(16);
        page[5] = buddy_alloc_pages(18);
        page[6] = buddy_alloc_pages(27);
        page[7] = buddy_alloc_pages(32);
        page[8] = buddy_alloc_pages(129);
        page[9] = buddy_alloc_pages(547);

        buddy_free_pages(page[3], 8);
        buddy_free_pages(page[4], 16);
        buddy_free_pages(page[5], 18);
        buddy_free_pages(page[6], 27);
        buddy_free_pages(page[7], 32);
        buddy_free_pages(page[8], 129);
        buddy_free_pages(page[9], 547);
        
        cprintk(rc_black, rc_red, "after test:\n\n order 0~10 counts: ");
        for (uint32_t order = 0; order < MAX_ORDER + 1; ++order) {
                cprintk(rc_black, rc_red, "%3u ", atomic_read(&nr_free(order)));
        }
        printk("\n\n");
}

