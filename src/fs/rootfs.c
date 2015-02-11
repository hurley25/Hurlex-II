/*
 * =====================================================================================
 *
 *       Filename:  rootfs.c
 *
 *    Description:  根文件系统
 *
 *        Version:  1.0
 *        Created:  2015年02月10日 12时06分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <lib/string.h>
#include <lib/list.h>
#include <atomic.h>
#include <vfs.h>

// 读取super_block
static struct super_block *ramfs_read_super(struct super_block *sb);

// rootfs 定义
struct filesystem fs_rootfs = {
        .name = "rootfs",
        .type = ROOTFS_T,
        .read_super = ramfs_read_super
};

// 申请 inode
static struct inode *rootfs_alloc_inode(struct super_block *sb);

// 释放inode
static void rootfs_destroy_inode(struct inode *inode);

// 设备写回超级块
static void rootfs_write_super(struct super_block *sb);

// 同步文件系统修改
static int rootfs_sync_fs(struct super_block *sb);

// 删除inode及其对应文件系统数据
static void rootfs_delete_inode(struct inode *inode);

// super_block_ops 定义
static struct super_block_ops rootfs_sb_ops = {
        .alloc_inode = rootfs_alloc_inode,
        .destroy_inode = rootfs_destroy_inode,
        .write_super = rootfs_write_super,
        .sync_fs = rootfs_sync_fs,
        .delete_inode = rootfs_delete_inode
};

// 读取super_block
static struct super_block *ramfs_read_super(struct super_block *sb)
{
        // 初始化根结点 inode
        struct inode *inode = alloc_inode();
        bzero(inode, sizeof(struct inode));
        inode->i_sb = sb;

        // 初始化根结点 dentry
        struct dentry *dentry = alloc_dentry();
        atomic_set(&(dentry->d_count), 0);
        INIT_LIST_HEAD(&(dentry->d_brother));
        INIT_LIST_HEAD(&(dentry->d_subdirs));
        dentry->d_status = 1;
        dentry->d_parent = NULL;
        dentry->d_sb = sb;
        dentry->d_inode = inode;
        dentry->is_mounted = 0;
        strcpy(dentry->d_name, "/");
        
        sb->s_type = ROOTFS_T;
        sb->s_root = dentry;
        sb->s_ops = &rootfs_sb_ops;

        return sb;
}

// 申请 inode
static struct inode *rootfs_alloc_inode(__UNUSED struct super_block *sb)
{
        return alloc_inode();
}

// 释放inode
static void rootfs_destroy_inode(struct inode *inode)
{
        free_inode(inode);
}

// 设备写回超级块
static void rootfs_write_super(__UNUSED struct super_block *sb)
{
        // do nothing ...
}

// 同步文件系统修改
static int rootfs_sync_fs(__UNUSED struct super_block *sb)
{
        // do nothing ...
        return 0;
}

// 删除inode及其对应文件系统数据
static void rootfs_delete_inode(struct inode *inode)
{
        free_inode(inode);
}

