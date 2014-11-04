/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  内核初始化
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 10时05分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include "common.h"
#include "console.h"
#include "string.h"
#include "debug.h"

// 内核初始化函数
void kern_init(void)
{
        init_debug();
//      init_gdt();
//      init_idt();

        console_clear();
        printk_color(rc_black, rc_green, "Hello, OS kernel!\n\n");
//      init_timer(200);

//      printk("kernel in memory start: 0x%08X\n", kern_start);
//      printk("kernel in memory end:   0x%08X\n", kern_end);
//      printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
//      show_memory_map();
//      init_heap();
//      printk_color(rc_black, rc_red, "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);
//      test_heap();
//      init_sched();

//      init_syscall();
//      load_kern_esp((uint32_t)current->stack + STACK_SIZE);

        while (1) {
                cpu_hlt();
        }
}

