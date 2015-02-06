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

struct inode_t;

#define FS_NAME_MAX     32      // 文件系统名称最大值

typedef
enum fs_type_t {
        fs_sfs = 0xAA
} fs_type_t;

// 抽象的文件系统定义
typedef
struct fs_t {
        uint8_t fs_name[FS_NAME_MAX];               // 文件系统名称
        fs_type_t fs_type;                          // 文件系统类型
        inode_t *(*fs_get_root)(struct fs_t *fs);   // 获取文件系统根结点inode结构
        int (*fs_mount)(struct fs_t *fs);           // 挂载文件系统
        int (*fs_unmount)(struct fs_t *fs);         // 卸载文件系统
        int (*fs_sync)(struct fs_t *fs);            // 刷新文件系统修改到设备
} fs_t;


#define fsop_get_root(fs)       ((fs)->fs_get_root(fs))
#define fsop_mount(fs)          ((fs)->fs_mount(fs))
#define fsop_unmount(fs)        ((fs)->fs_unmount(fs))
#define fsop_sync(fs)           ((fs)->fs_sync(fs))

// VFS初始化
void vfs_init(void);

// 添加文件系统
int vfs_add_fs(const char *devname, fs_t *fs);

// 添加设备
int vfs_add_dev(const char *devname, inode_t *devnode, bool mountable);

// 挂载文件系统
// int vfs_mount(const char *devname, int (*mountfunc)(struct device *dev, fs_t **fs_store));

// 卸载文件系统
int vfs_unmount(const char *devname);

// 获取设备根节点inode
int vfs_get_root(const char *devname, inode_t **root_inode);


// 打开文件
int vfs_open(char *path, uint32_t open_flags, inode_t **inode_store);

// 关闭文件
int vfs_close(inode_t *inode);

// 创建目录
int vfs_mkdir(char *path);

// 文件重命名
int vfs_rename(char *old_path, char *new_path);

#endif  // INCLUDE_VFS_H_
