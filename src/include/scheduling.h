/*
 * =====================================================================================
 *
 *       Filename:  scheduling.h
 *
 *    Description:  调度相关
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 15时07分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_SCHEDULING_H_
#define INCLUDE_SCHEDULING_H_

#include "idt.h"

// 时钟中断函数
void timer_callback(pt_regs *regs);

#endif  // INCLUDE_SCHEDULING_H_
