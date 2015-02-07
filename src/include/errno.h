/*
 * =====================================================================================
 *
 *       Filename:  errno.h
 *
 *    Description:  错误编号
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 15时22分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_ERRNO_H_
#define INCLUDE_ERRNO_H_

#include <types.h>

#define E_UNSPECIFIED       1   // Unspecified or unknown problem
#define E_BAD_PROC          2   // Process doesn't exist or otherwise
#define E_INVAL             3   // Invalid parameter
#define E_NO_MEM            4   // Request failed due to memory shortage
#define E_NO_FREE_PROC      5   // Attempt to create a new process beyond
#define E_FAULT             6   // Memory fault

#define MAXERROR            6

// 获得错误号对应的字符串
const char *strerr(int errno);

#endif  // INCLUDE_ERRNO_H_
