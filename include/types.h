/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  一些类型的定义
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 10时51分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#ifndef NULL
        #define NULL 0
#endif

#ifndef TRUE
        #define TRUE  1
        #define FALSE 0
#endif

#define __UNUSED__ __attribute__((unused))

typedef
enum bool {
        false = 0,
        true = 1
} bool;

typedef unsigned long long uint64_t;
typedef          long long int64_t;
typedef unsigned int       uint32_t;
typedef          int       int32_t;
typedef unsigned short     uint16_t;
typedef          short     int16_t;
typedef unsigned char      uint8_t;
typedef          char      int8_t;
typedef unsigned int       size_t;
typedef          int       ssize_t;

// 时间类型
typedef uint32_t time_t;

// 进程 PID
typedef int32_t pid_t;

// 页目录数据类型
typedef uint32_t pgd_t;

// 页表数据类型
typedef uint32_t pte_t;

// 原子类型
typedef
struct {
	volatile int counter;
} atomic_t;

// 内核链表类型
struct list_head {
        struct list_head *next, *prev;
};

#endif  // INCLUDE_TYPES_H_
