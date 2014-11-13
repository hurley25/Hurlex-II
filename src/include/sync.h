/*
 * =====================================================================================
 *
 *       Filename:  sync.h
 *
 *    Description:  sync
 *
 *        Version:  1.0
 *        Created:  2014年11月06日 16时56分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
*/

#ifndef INCLUDE_SYNC_H_
#define INCLUDE_SYNC_H_

#include <common.h>
#include <types.h>
#include <arch.h>

static inline bool __intr_store(void)
{
        if (read_eflags() & FL_IF) {
                disable_intr();
                return true;
        }

        return false;
}

static inline void __intr_restore(bool flag)
{
        if (flag) {
                enable_intr();
        }
}

#define local_intr_store(x)      do { x = __intr_store(); } while (0)
#define local_intr_restore(x)   __intr_restore(x);

#endif  // INCLUDE_SYNC_H_

