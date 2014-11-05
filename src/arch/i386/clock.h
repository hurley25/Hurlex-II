/*
 * =====================================================================================
 *
 *       Filename:  clock.h
 *
 *    Description:  定时中断相关
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 15时01分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include <types.h>

#define FREQUENCY       100

// 定时中断由 8253/8254 芯片从 IRQ0 提供
#define IO_TIMER        0x040               // 8253 Timer #1

// 输入频率为 1193180，frequency 即每秒中断次数
#define TIMER_FREQ      1193180

// Intel 8253/8254 PIT芯片 I/O端口地址范围是40h~43h
#define TIMER_MODE      (IO_TIMER + 3)         // timer mode port

#define TIMER_SEL0      0x00                    // select counter 0
#define TIMER_RATEGEN   0x04                    // mode 2
#define TIMER_CLK       0x06                    // mode 3
#define TIMER_16BIT     0x30                    // r/w counter 16 bits, LSB first

void init_clock(void);

#endif  // INCLUDE_TIMER_H_
