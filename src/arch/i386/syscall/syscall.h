/*
 * =====================================================================================
 *
 *       Filename:  syscall.h
 *
 *    Description:  系统调用相关
 *
 *        Version:  1.0
 *        Created:  2014年11月16日 12时17分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_SYSCALL_SYSCALL_H_
#define INCLUDE_SYSCALL_SYSCALL_H_

// 系统调用号
#define SYS_test      0
#define SYS_exit      1
#define SYS_fork      2
#define SYS_read      3
#define SYS_write     4
#define SYS_open      5
#define SYS_close     6
#define SYS_getpid    7

#define SYSCALL_MAX   7

// 系统调用函数
void syscall_handler(pt_regs_t *regs);

#endif  // INCLUDE_SYSCALL_SYSCALL_H_
