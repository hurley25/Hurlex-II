/*
 * =====================================================================================
 *
 *       Filename:  pmm.h
 *
 *    Description:  物理内存管理
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 13时03分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_MM_PMM_H
#define INCLUDE_MM_PMM_H

#include <types.h>
#include <atomic.h>

// 默认栈的大小
#define STACK_SIZE 8192

// 物理内存页框大小 
#define PMM_PAGE_SIZE 0x1000

// 页掩码 按照 0x1000(4096) 对齐地址
#define PMM_PAGE_MASK 0xFFFFF000

// 内核在物理内存起始位置
#define RAM_KERNEL_START 0x100000

// 内核代码在内存中的起始和结束位置，在链接脚本中定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];

// BIOS int 0x15 AX = 0xE820 常量
#define E820MAX             20      // 最大的表项数目
#define E820_ARM            1       // 可用 RAM
#define E820_ARR            2       // 保留区域

typedef
struct e820map_t {
        uint32_t count;
        struct {
                uint32_t addr_low;
                uint32_t addr_high;
                uint32_t length_low;
                uint32_t length_high;
                uint32_t type;
        } __attribute__((packed)) map[E820MAX];
} e820map_t;

// 内存页类型
typedef
enum mem_zone_t {
        ZONE_DMA = 0,
        ZONE_NORMAL = 1,
        ZONE_HIGHMEM = 2
} mem_zone_t;

#define ZONE_NORMAL_ADDR     0x1000000   // 16 MB
#define ZONE_HIGHMEM_ADDR    0x38000000  // 896 MB

// 物理页结构
typedef
struct page_t {
        atomic_t ref;               // 物理页被引用的次数
        uint32_t flag;              // 当前页状态
	uint32_t count;             // 当前页后续连续页的数量
	struct list_head list;      // 链接下一个连续页
} page_t;

// page_t 的 flag 参数的操作宏
#define PG_RESERVED    0       // 1 << 0 表示页当前不可用
#define PG_COUNT       1       // 1 << 1 表示 count 字段有效

#define set_page_reserved_flag(page)       set_bit(PG_RESERVED, &((page)->flag))
#define clear_page_reserved_flag(page)     clear_bit(PG_RESERVED, &((page)->flag))
#define is_page_reserved(page)             test_bit(PG_RESERVED, &((page)->flag))
#define set_page_count_flag(page)          set_bit(PG_COUNT, &((page)->flag))
#define clear_page_count_flag(page)        clear_bit(PG_COUNT, &((page)->flag))
#define is_page_count(page)                test_bit(PG_COUNT, &((page)->flag))

static inline int32_t page_ref(page_t *page)
{
        return atomic_read(&page->ref);
}

static inline void set_page_ref(page_t *page, int32_t val)
{
        atomic_set(&page->ref, val);
}

static inline void page_ref_inc(page_t *page)
{
        atomic_inc(&page->ref);
}

static inline void page_ref_dec(page_t *page)
{
        atomic_dec(&page->ref);
}

// 由物理地址计算出该地址所处页的管理结构的指针
page_t *addr_to_page(uint32_t addr);

// 由页的管理结构的指针计算出页所在物理地址
uint32_t page_to_addr(page_t *page);

// 内存管理子系统管理对象
struct pmm_manager {
        const char *name;                                // 管理算法的名称
        void (*page_init)(page_t *pages, uint32_t n);    // 初始化
        uint32_t (*alloc_pages)(uint32_t n);             // 申请物理内存页(n为字节数)
        void (*free_pages)(uint32_t addr, uint32_t n);   // 释放内存页
        uint32_t (*free_pages_count)(void);              // 返回当前可用内存页
};

// 初始化物理内存管理
void init_pmm(void);

// 内存管理算法初始化
void page_init(page_t *pages, uint32_t n);

// 申请内存页
uint32_t alloc_pages(uint32_t n);

// 释放内存页
void free_pages(uint32_t addr, uint32_t n);

// 当前可用内存页
uint32_t free_pages_count(void);

#endif  // INCLUDE_MM_PMM_H
