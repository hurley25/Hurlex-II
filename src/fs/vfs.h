/*
 * =====================================================================================
 *
 *       Filename:  vfs.h
 *
 *    Description:  虚拟文件系统
 *
 *        Version:  1.0
 *        Created:  2015年02月06日 09时56分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_VFS_H_
#define INCLUDE_VFS_H_

#include <types.h>

#include "inode.h"

struct super_block_t;

// 抽象的文件系统定义
typedef
struct fs_t {
        const char *fs_name;                            // 文件系统名称
        uint8_t  fs_type;                               // 文件系统类型
        //super_block_t *(*read_super)(super_block_t *);  // 读取super_block
        struct fs_t *next;                              // 下一个文件系统
} fs_t;

// 全局的文件系统指针
extern fs_t *file_systems; 

// vfs 初始化
void vfs_init(void);

// 添加文件系统
int register_filesystem(fs_t *fs);

#endif  // INCLUDE_VFS_H_
