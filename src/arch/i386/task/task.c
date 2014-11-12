/*
 * =====================================================================================
 *
 *       Filename:  task.c
 *
 *    Description:  人物相关的定义
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 12时24分42秒
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
#include <mm/mm.h>
#include <lib/string.h>

#include "task.h"

struct list_head task_list;

struct task_struct *glb_idle_task;
struct task_struct *glb_init_task;

// 任务总数
static int nr_task = 0;

struct task_struct *get_current(void)
{
        register uint32_t esp __asm__ ("esp");;

        return (struct task_struct *)(esp & (~(STACK_SIZE-1)));
}

static int init_main(void *arg)
{
        printk("this initproc, pid = %d, name = \"%s\"\n", current->pid, current->name);
        printk("To U: \"%s\".\n", (const char *)arg);
        printk("To U: \"en.., Bye, Bye. :)\"\n");

        return 0;
}

// 任务调度初始化
void init_task(void)
{
        INIT_LIST_HEAD(&task_list);
        struct task_struct *idle_task = (struct task_struct *)kern_stack;

        bzero(idle_task, sizeof(struct task_struct));

        idle_task->state = TASK_RUNNABLE;
        idle_task->stack = (void *)kern_stack_top;
        idle_task->pid = 0;
        idle_task->need_resched = true;
        set_proc_name(idle_task, "idle");

        nr_task++;
        list_add(&idle_task->list, &task_list);

        glb_idle_task = idle_task;

        pid_t pid = kernel_thread(init_main, "Hello New Thread!", 0);
        assert(pid >= 0, "init_task error!");

        glb_init_task = find_task(pid);
        set_proc_name(glb_init_task, "init");

        assert(glb_idle_task != NULL && glb_idle_task->pid == 0, "init_task error");
        assert(glb_init_task != NULL && glb_init_task->pid == 1, "init_task error");
}

// 运行一个任务
void task_run(struct task_struct *task)
{
        if (task != current) {
                struct task_struct *prev = current;
                struct task_struct *next = task;
                bool intr_flag = false;
                local_intr_store(intr_flag);
                {
                        if (!task && task->mm && task->mm->pgdir) {
                                // load cr3
                        }
                        switch_to(&prev->context, &next->context);
                }
                local_intr_restore(intr_flag);
        }
}

// 通过 PID 查找任务(可以考虑建立hash表存储)
struct task_struct *find_task(pid_t pid)
{
        if (pid > 0 && pid < MAX_PID) {
                struct list_head *le;
                list_for_each(le, &task_list) {
                        struct task_struct *task = le_to_task(le);
                        if (task->pid == pid) {
                                return task;
                        }
                }
        }

        return NULL;
}

void set_proc_name(struct task_struct *task, char *name)
{
        bzero(task->name, sizeof(task->name));
        strncpy(task->name, name, TASK_NAME_MAX);
}

static struct task_struct *alloc_task_struct(void)
{
        void *addr = (void *)alloc_pages(STACK_SIZE/PAGE_SIZE);
        assert(addr != 0, "alloc_task_struct error!");

        struct task_struct *task = pa_to_ka(addr);
        bzero(task, sizeof(struct task_struct));

        task->state = TASK_UNINIT;
        task->stack = task_to_stack(task);
        task->pid = -1;

        return task;
}

static int copy_mm(uint32_t clone_flags, struct task_struct *task)
{
        if (!clone_flags && !task) {
                return -1;
        }

        return 0;
}

static void copy_thread(struct task_struct *task, struct pt_regs_t *pt_regs)
{
        task->pt_regs = (struct pt_regs_t *)((uint32_t)task->stack - sizeof(struct pt_regs_t));
        *(task->pt_regs) = *pt_regs;
        task->pt_regs->eax = 0;
        task->pt_regs->esp = (uint32_t)task->stack;
        task->pt_regs->eflags |= FL_IF;

        // TODO
        task->context.eip = task->pt_regs->eip;
        task->context.ebx = task->pt_regs->ebx;
        task->context.edx = task->pt_regs->edx;
        
        task->context.esp = (uint32_t)task->pt_regs;
}

pid_t do_fork(uint32_t clone_flags, struct pt_regs_t *pt_regs)
{
        if (nr_task >= MAX_TASK) {
                return -E_NO_FREE_PROC;
        }

        struct task_struct *task = alloc_task_struct();
        if (!task) {
                return -E_NO_MEM;
        }

        if (copy_mm(clone_flags, task) != 0) {
                free_pages((uint32_t)ka_to_pa(task), STACK_SIZE/PAGE_SIZE);
                return -E_NO_MEM;
        }

        copy_thread(task, pt_regs);

        bool intr_flag = false;
        local_intr_store(intr_flag);
        {
                //task->pid = alloc_pid();
                task->pid = 1;
                list_add(&task->list, &task_list);
                nr_task ++;
        }
        local_intr_restore(intr_flag);

        wakeup_task(task);

        return task->pid;
}

int kernel_thread(int (*func)(void *), void *args, uint32_t clone_flags)
{
        pt_regs_t pt_regs;
        bzero(&pt_regs, sizeof(pt_regs_t));

        pt_regs.cs = KERNEL_CS;
        pt_regs.ds = KERNEL_DS;
        pt_regs.ss = KERNEL_DS;
        pt_regs.ebx = (uint32_t)func;
        pt_regs.edx = (uint32_t)args;
        pt_regs.eip = (uint32_t)kern_thread_entry;

        return do_fork(clone_flags | CLONE_VM, &pt_regs);
}

int do_exit(int errno)
{
        printk("do_exit errno is: \n", errno);
        panic("de_exit");

        return 0;
}

void cpu_idle(void)
{
        while (true) {
                if (current->need_resched) {
                        schedule();
                }
        }
}
 
