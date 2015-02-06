/*
 * =====================================================================================
 *
 *       Filename:  vfs.c
 *
 *    Description:  虚拟文件系统
 *
 *        Version:  1.0
 *        Created:  2015年02月06日 10时15分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <mm/mm.h>
#include <lib/string.h>
#include "vfs.h"

// 根文件系统iode节点
static inode_t *rootfs_node = NULL;

fs_t *__alloc_fs(fs_type_t type)
{
        fs_t *fs;
        if ((fs = kmalloc(sizeof(fs_t))) != NULL) {
                fs->fs_type = type;
        }
        return fs;
}

// 初始化可用设备列表
static void __vfs_devlist_init(void)
{

}

// VFS初始化
void vfs_init(void)
{
        __vfs_devlist_init();
}

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
