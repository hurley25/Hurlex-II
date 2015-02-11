/*
 * =====================================================================================
 *
 *       Filename:  fs.h
 *
 *    Description:  文件系统相关
 *
 *        Version:  1.0
 *        Created:  2015年02月08日 10时07分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include <vfs.h>

// 进程最大打开的文件个数
#define MAX_OPEN_FILE   64

// 进程PCB里描述文件系统的结构
struct file_struct {
        spinlock_t fs_lock;                     // 同步修改保护锁
        struct vfsmount *vfsmount;              // 文件系统根结构
        struct file file_array[MAX_OPEN_FILE];  // 进程打开的文件描述
};

// 文件系统初始化
void fs_init(void);

#endif  // INCLUDE_FS_H_
