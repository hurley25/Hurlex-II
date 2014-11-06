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

#include <arch.h>

// clock 中断回调函数
void clock_callback(pt_regs *regs);

#endif  // INCLUDE_SCHEDULING_H_
