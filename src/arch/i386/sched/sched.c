/*
 * =====================================================================================
 *
 *       Filename:  scheduling.c
 *
 *    Description:  调度程序
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 15时06分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <debug.h>

#include "sched.h"

void clock_callback(__attribute__((unused))pt_regs *regs)
{
        static uint32_t tick = 0;
        printk_color(rc_black, rc_red, "Tick Count: %d\n", tick++);
}

