/*
 * =====================================================================================
 *
 *       Filename:  sched.h
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
#include <task/task.h>

// clock 中断回调函数
void clock_callback(pt_regs_t *regs);

// 任务调度
void schedule(void);

// 唤醒任务
void wakeup_task(struct task_struct *task);

#endif  // INCLUDE_SCHEDULING_H_
