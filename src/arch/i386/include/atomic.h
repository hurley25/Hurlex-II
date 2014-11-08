/*
 * =====================================================================================
 *
 *       Filename:  atomic.h
 *
 *    Description:  一些原子操作
 *
 *        Version:  1.0
 *        Created:  2014年11月08日 20时51分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef ATOMIC_H_
#define ATOMIC_H_

#include <types.h>

#define LOCK_PREFIX "lock;"

#define ATOMIC_INIT(i)  { (i) }

static inline void set_bit(int32_t nr, volatile void *addr)
{
        __asm__ volatile ("btsl %1, %0" 
                        :"=m" (*(volatile long *)addr) 
                        : "Ir" (nr));
}

static inline void clear_bit(int32_t nr, volatile void *addr)
{
        __asm__ volatile ("btrl %1, %0" 
                        :"=m" (*(volatile long *)addr) 
                        : "Ir" (nr));
}

static inline void change_bit(int32_t nr, volatile void *addr)
{
        __asm__ volatile ("btcl %1, %0" 
                        :"=m" (*(volatile long *)addr) 
                        : "Ir" (nr));
}

static inline bool test_bit(int32_t nr, volatile void *addr)
{
        int32_t oldbit;
        
        __asm__ volatile ("btl %2, %1; sbbl %0,%0" 
                        : "=r" (oldbit) 
                        : "m" (*(volatile long *)addr), "Ir" (nr));
        
        return (oldbit != 0);
}

static inline int32_t atomic_read(const atomic_t *v)
{
        return v->counter;
}

static inline void atomic_set(atomic_t *v, int32_t i)
{
        v->counter = i;
}

static inline void atomic_add(atomic_t *v, int32_t i)
{
        __asm__ volatile(LOCK_PREFIX "addl %1,%0"
                     : "+m" (v->counter)
                     : "ir" (i));
}

static inline void atomic_sub(atomic_t *v, int32_t i)
{
        __asm__ volatile(LOCK_PREFIX "subl %1,%0"
                     : "+m" (v->counter)
                     : "ir" (i));
}

static inline int32_t atomic_sub_and_test(atomic_t *v, int32_t i)
{
        unsigned char c;

        __asm__ volatile(LOCK_PREFIX "subl %2,%0; sete %1"
                     : "+m" (v->counter), "=qm" (c)
                     : "ir" (i) : "memory");
        return c;
}

static inline void atomic_inc(atomic_t *v)
{
        __asm__ volatile(LOCK_PREFIX "incl %0"
                     : "+m" (v->counter));
}

static inline int32_t atomic_inc_and_test(atomic_t *v)
{
        unsigned char c;

        __asm__ volatile(LOCK_PREFIX "incl %0; sete %1"
                     : "+m" (v->counter), "=qm" (c)
                     : : "memory");
        return (c != 0);
}

static inline void atomic_dec(atomic_t *v)
{
        __asm__ volatile(LOCK_PREFIX "decl %0"
                     : "+m" (v->counter));
}

static inline int32_t atomic_dec_and_test(atomic_t *v)
{
        unsigned char c;

        __asm__ volatile(LOCK_PREFIX "decl %0; sete %1"
                     : "+m" (v->counter), "=qm" (c)
                     : : "memory");
        return (c != 0);
}

#endif  // ATOMIC_H_
