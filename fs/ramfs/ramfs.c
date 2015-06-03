/*
 * =====================================================================================
 *
 *       Filename:  ramfs.c
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

#include <fs.h>

// 读取super_block
static struct super_block *ramfs_read_super(struct super_block *sb);

// ramfs 定义
struct filesystem fs_ramfs = {
        .name = "ramfs",
        .type = RAMFS_T,
        .read_super = ramfs_read_super
};

// 申请 inode
static struct inode *ramfs_alloc_inode(struct super_block *sb);

// 释放inode
static void ramfs_destroy_inode(struct inode *inode);

// 设备写回超级块
static void ramfs_write_super(struct super_block *sb);

// 同步文件系统修改
static int ramfs_sync_fs(struct super_block *sb);

// super_block_ops 定义
static struct super_block_ops ramfs_sb_ops = {
        .alloc_inode = ramfs_alloc_inode,
        .destroy_inode = ramfs_destroy_inode,
        .write_super = ramfs_write_super,
        .sync_fs = ramfs_sync_fs,
};

// 读取super_block
static struct super_block *ramfs_read_super(struct super_block *sb)
{
        sb->s_type = RAMFS_T;
        sb->s_ops = &ramfs_sb_ops;

        return sb;
}

// 申请 inode
static struct inode *ramfs_alloc_inode(__UNUSED__ struct super_block *sb)
{
        return alloc_inode();
}

// 释放inode
static void ramfs_destroy_inode(struct inode *inode)
{
        free_inode(inode);
}

// 设备写回超级块
static void ramfs_write_super(__UNUSED__ struct super_block *sb)
{
        // do nothing ...
}

// 同步文件系统修改
static int ramfs_sync_fs(__UNUSED__ struct super_block *sb)
{
        // do nothing ...
        return 0;
}

