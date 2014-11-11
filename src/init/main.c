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
#include <lib/string.h>
#include <mm/mm.h>
#include <arch.h>

// 内核初始化函数
void kern_init(void)
{
        init_debug();
        init_arch();

        init_console();
        printk_color(rc_black, rc_green, "Hello, Hurlex II kernel!\n\n");
        
        init_mm();

        printk_color(rc_black, rc_green, "Init Virtual Memory Succeed!\n\n");

        //init_clock();
        //enable_intr();

        while (1) {
                cpu_hlt();
        }
}

