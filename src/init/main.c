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

#include <common.h>
#include <console.h>
#include <debug.h>
#include <arch.h>
#include <mm/mm.h>
#include <lib/string.h>

// 内核初始化函数
void kern_init(void)
{
        init_debug();
        init_arch();

        init_console();
        printk_color(rc_black, rc_green, "Hello, Hurlex II kernel!\n\n");
        
        init_mm();

        init_clock();
        init_task();
        
        //enable_intr();

        printk_color(rc_black, rc_green, "\nIt's %s thread  pid = %d\n\n", current->name, current->pid);

        cpu_idle();

        printk_color(rc_black, rc_green, "\nIt's %s thread  pid = %d\n\n", current->name, current->pid);

        while (true) {
                cpu_hlt();
        }
}

