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

#include "scheduling.h"
#include "debug.h"

void timer_callback(pt_regs *regs)
{
        static uint32_t tick = 0;
        static uint32_t count = 0;
        if (++tick > 100) {
                printk_color(rc_black, rc_red, "Timer Count: %d\n", ++count);
                tick = 0;
        }
}

