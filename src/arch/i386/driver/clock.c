/*
 * =====================================================================================
 *
 *       Filename:  clock.c
 *
 *    Description:  定时中断函数
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 14时58分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <debug.h>
#include <common.h>
#include <sched.h>
#include <sched.h>
#include <arch.h>

#include "clock.h"

void init_clock(void)
{
        // 注册时间相关的处理函数
        register_interrupt_handler(IRQ0, clock_callback);

        uint32_t divisor = TIMER_FREQ / FREQUENCY;

        outb(TIMER_MODE, TIMER_SEL0 | TIMER_RATEGEN | TIMER_16BIT);

        // 拆分低字节和高字节
        uint8_t low = (uint8_t)(divisor & 0xFF);
        uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);
        
        // 分别写入低字节和高字节
        outb(IO_TIMER, low);
        outb(IO_TIMER, hign);
}

