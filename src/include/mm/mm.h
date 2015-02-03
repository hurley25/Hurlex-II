/*
 * =====================================================================================
 *
 *       Filename:  mm.h
 *
 *    Description:  内存管理的头文件
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 13时31分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_MM_MM_H
#define INCLUDE_MM_MM_H

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/slob.h>

// 内存管理子系统初始化
void mm_init(void);

#endif  // INCLUDE_MM_MM_H
