/*
 * =====================================================================================
 *
 *       Filename:  sfs.c
 *
 *    Description:  simple file system
 *
 *        Version:  1.0
 *        Created:  2015年02月10日 14时09分46秒
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
static struct super_block *sfs_read_super(struct super_block *sb);

// sfs 定义
struct filesystem fs_sfs = {
        .name = "sfs",
        .type = SFS_T,
        .read_super = sfs_read_super
};

// 读取super_block
static struct super_block *sfs_read_super(struct super_block *sb)
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
        
        // 设置超级块对应的根目录
        sb->s_root = dentry;

        return sb;
}

