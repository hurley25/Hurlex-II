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
#include <sync.h>
#include <lib/list.h>

#include "sched.h"

void clock_callback(__attribute__((unused))pt_regs_t *regs)
{
        static uint32_t tick = 0;
        printk_color(rc_black, rc_red, "Tick Count: %d\n", tick++);
}

void schedule(void)
{
        struct task_struct *task_next = NULL;
        struct list_head *le = NULL, *last = NULL;

        bool intr_flag = false;
        local_intr_store(intr_flag);
        {
                current->need_resched = 0;
                last = (current == glb_idle_task) ? &task_list : &(current->list);
                le = last;
                do {
                        if ((le = le->next) != &task_list) {
                                task_next = le_to_task(le);
                                if (task_next->state == TASK_RUNNABLE) {
                                        break;
                                }
                        }
                } while (le != last);

                if (!task_next || task_next->state != TASK_RUNNABLE) {
                        task_next = glb_idle_task;
                }
                task_next->runs_time++;
                if (task_next != current) {
                        task_run(task_next);
                }
        }
        local_intr_restore(intr_flag);
}

void wakeup_task(struct task_struct *task)
{
        if (task->state != TASK_ZOMBIE && task->state != TASK_RUNNABLE) {
                task->state = TASK_RUNNABLE;
        }
}

