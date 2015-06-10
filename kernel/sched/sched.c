/*
 * =====================================================================================
 *
 *       Filename:  sched.c
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
#include <sync.h>
#include <lib/list.h>

#include "sched.h"

void clock_callback(__UNUSED__ pt_regs_t *regs)
{
        schedule();
}

void schedule(void)
{
        struct task_struct *task_next = NULL;
        struct list_head *le = NULL, *last = NULL;

        bool intr_flag = false;
        local_intr_store(intr_flag);
        {
                current->need_resched = false;

                // 如果当前是内核调度线程在执行，就从任务链起始开始，否则从当前任务开始
                le = last = (current == glb_idle_task) ? &task_list : &(current->list);
                for (le = le->next; le != last; le = le->next) {
                        if (le != &task_list) {
                                task_next = le_to_task(le);
                                if (task_next->state == TASK_RUNNABLE) {
                                        break;
                                }
                        }
                }

                // 未找到可运行的任务
                if (!task_next || task_next->state != TASK_RUNNABLE) {
                        task_next = glb_idle_task;
                }

                // 时间片递增
                task_next->runs_time++;

                if (task_next != current) {
                        task_run(task_next);
                }
        }
        local_intr_restore(intr_flag);
}

void wakeup_task(struct task_struct *task)
{
        if (task->state != TASK_ZOMBIE) {
                task->state = TASK_RUNNABLE;
        }
}

