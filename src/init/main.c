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
#include "mm.h"

// 内核初始化函数
void kern_init(void)
{
        init_debug();
//      init_gdt();
//      init_idt();

        console_clear();
        printk_color(rc_black, rc_green, "Hello, Hurlex II kernel!\n\n");
//      init_timer(200);

      	show_memory_map();
      	show_kernel_memory_map();
        init_pmm();

//      init_heap();
//      init_sched();

        while (1) {
                cpu_hlt();
        }
}

