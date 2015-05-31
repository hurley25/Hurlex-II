/*
 * =====================================================================================
 *
 *       Filename:  task.h
 *
 *    Description:  任务相关定义
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 10时18分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_TASK_H_
#define INCLUDE_TASK_H_

#include <types.h>
#include <arch.h>
#include <common.h>
#include <mm/mm.h>
#include <lib/list.h>
#include <fs.h>

// fork flag
#define CLONE_VM            0x00000100  // set if VM shared between processes
#define CLONE_THREAD        0x00000200  // thread group

// 任务状态字段
typedef
enum task_state {
        TASK_UNINIT = 0,       // 任务未初始化
        TASK_SLEEPING = 1,     // 任务休眠中
        TASK_RUNNABLE = 2,     // 任务可执行(可能正在执行)
        TASK_ZOMBIE = 3,       // 任务已结束(等待回收)
} task_state_t;

// 按照 x86 规范保存被调用者需要保存的寄存器即可
struct context {
        uint32_t eip;
        uint32_t esp;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
        uint32_t esi;
        uint32_t edi;
        uint32_t ebp;
};

#define TASK_NAME_MAX  (20)
#define MAX_TASK       (4096)
#define MAX_PID        (MAX_TASK*2)

// 任务描述 PCB
struct task_struct {

        task_state_t state;             // 任务状态

        void *stack;                    // 任务的内核栈指针
        
        pid_t pid;                      // 任务的PID
        char name[TASK_NAME_MAX+1];     // 任务名称

        uint32_t runs_time;             // 当前任务运行时间
        volatile bool need_resched;     // 是否需要被重新调度
        
        struct task_struct *parent;     // 父进程指针
        
        struct mm_struct *mm;           // 任务的内存信息
        struct pt_regs_t *pt_regs;      // 任务中断保存的寄存器信息
        struct context context;         // 任务切换上下文信息
        uint32_t flags;                 // 任务的一些标识
        uint32_t exit_code;             // 任务的退出代码

        struct file_struct files;       // 文件系统相关信息
        
        struct list_head list;          // 任务链表
};

#define le_to_task(le) list_entry(le, struct task_struct, list)

extern struct list_head task_list;

// idle 任务指针
extern struct task_struct *glb_idle_task;

// init 任务指针
extern struct task_struct *glb_init_task;

/*  
 * 因为pt_regs结构最后的部分实际上是CPU自动压栈，内核访问的。
 * 即中断产生后，CPU会自动压入这些寄存器，ss和sp仅在特权级发生变化时压入
 * （比如从用户态ring0到ring3，会压入用户态的ss和sp），
 * 如果是内核态发生中断，CPU不会压入ss和sp，这种情况，再访问ss和sp字段就越界了，
 * 所以预留了8字节。
 */
#define task_to_stack(task) ((void *)((uint32_t)task + STACK_SIZE - 8))

#define current get_current()

// 获得当前执行的任务指针
struct task_struct *get_current(void);

// 任务调度初始化
void task_init(void);

// 运行一个任务
void task_run(struct task_struct *task);

// 创建一个内核线程
int kernel_thread(int (*func)(void *), void *args, uint32_t clone_flags);

// 通过 PID 查找任务
struct task_struct *find_task(pid_t pid);

// 设置任务名称
void set_proc_name(struct task_struct *task, char *name);

void cpu_idle(void);

pid_t do_fork(uint32_t clone_flags, struct pt_regs_t *pt_regs);

void do_exit(int errno);

#endif  // INCLUDE_TASK_H_
