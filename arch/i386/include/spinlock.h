/*
 * =====================================================================================
 *
 *       Filename:  spinlock.h
 *
 *    Description:  自旋锁的实现
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 11时16分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */


#ifndef INCLUDE_SPINLOCK_H_
#define INCLUDE_SPINLOCK_H_

#include <types.h>

typedef
struct spinlock_t {
        volatile uint32_t lock;
} spinlock_t;

// 1 表示 spinlock 可用
#define SPIN_LOCK_UNLOCKED (spinlock_t) { 1 }

// 初始化自旋锁
#define spin_lock_init(x) do { *(x) = SPIN_LOCK_UNLOCKED; } while(0)

// spinlock 加锁
static inline void spinlock_lock(spinlock_t *lock)
{
        __asm__ volatile ("\n1:\t"
                        "lock; decb %0\n\t"                     // decb 将 lock->lock 减 1，lock 表示要锁住地址总线
                        "js 2f\n\t" 
                        ".section .text.lock, \"ax\"\n"
                        "2:\t" 
                        "cmpb $0, %0\n\t" 
                        "rep; nop\n\t" 
                        "jle 2b\n\t" 
                        "jmp 1b\n\t" 
                        ".previous"
                        :"=m" (lock->lock) : : "memory"); 
}

// spinlock 解锁
static inline void spinlock_unlock(spinlock_t *lock)
{
        __asm__ volatile ("movb $1, %0" 
                        :"=m" (lock->lock) : : "memory");
}

#endif // INCLUDE_SPINLOCK_H_
