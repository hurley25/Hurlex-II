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

#include <debug.h>
#include <atomic.h>
#include <arch.h>
#include <mm/mm.h>
#include <lib/string.h>
#include <lib/list.h>
#include <fs.h>
#include <vfs.h>

// 全局的文件系统指针
struct filesystem *file_systems; 

// 初始化VFS目录树
static void init_mount_tree(struct vfsmount *mount);

// vfs 初始化
void vfs_init(void)
{
        printk_color(rc_black, rc_red, "Init VFS ...\n\n");

        struct vfsmount *mount;
        mount = kmalloc(sizeof(struct vfsmount));
        assert(mount != NULL, "vfs_init alloc vfsmount failed!");

        // 初始化VFS目录树
        init_mount_tree(mount);

        current->files.vfsmount = mount;
}

// 初始化VFS目录树
static void init_mount_tree(struct vfsmount *mount)
{
        // 添加根文件系统
        add_filesystem(&fs_rootfs);

        INIT_LIST_HEAD(&(fs_rootfs.fs_supers));

        // 获取根文件系统的超级块结构
        struct super_block *sb = alloc_super_block();
        bzero(sb, sizeof(struct super_block));
        
        // 将 super_block 添加到文件系统控制信息下
        list_add(&(sb->s_list), &(fs_rootfs.fs_supers));

        // 为根文件系统初始化超级块
        fs_rootfs.read_super(sb);

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
        
        // 链接根节点 dentry
        sb->s_root = dentry;

        mount->mnt_devname = "RAM";
        mount->mnt_sb = sb;
        mount->mnt_root = dentry;
        mount->mnt_mountpoint = dentry;
        mount->mnt_parent = NULL;
}

// 添加文件系统
int add_filesystem(struct filesystem *fs)
{
        struct filesystem *p = file_systems;
        while (p) {
                if (strcmp(p->name, fs->name) == 0) {
                        return -1;
                }
                p = p->next;
        }

        fs->next = file_systems;
        file_systems = fs;

        return 0;
}

struct super_block *alloc_super_block(void)
{
        struct super_block *sb;

        sb = kmalloc(sizeof(struct super_block));
        assert(sb != NULL, "alloc super_block failed!");

        return sb;
}

struct super_block_ops *alloc_super_block_ops(void)
{
        struct super_block_ops *sb_ops;

        sb_ops = kmalloc(sizeof(struct super_block_ops));
        assert(sb_ops != NULL, "alloc super_block_ops failed!");

        return sb_ops;
}

struct inode *alloc_inode(void)
{
        struct inode *inode;

        inode = kmalloc(sizeof(struct inode));
        assert(inode != NULL, "alloc inode failed!");

        return inode;
}

struct inode_ops *alloc_inode_ops(void)
{
        struct inode_ops *inode_ops;

        inode_ops = kmalloc(sizeof(struct inode_ops));
        assert(inode_ops != NULL, "alloc inode_ops failed!");

        return inode_ops;
}

struct dentry *alloc_dentry(void)
{
        struct dentry *dentry;

        dentry = kmalloc(sizeof(struct dentry));
        assert(dentry != NULL, "alloc dentry failed!");

        return dentry;
}

struct dentry_ops *alloc_dentry_ops(void)
{
        struct dentry_ops *dentry_ops;

        dentry_ops = kmalloc(sizeof(struct dentry_ops));
        assert(dentry_ops != NULL, "alloc dentry_ops failed!");

        return dentry_ops;
}

struct file *alloc_file(void)
{
        struct file *file;

        file = kmalloc(sizeof(struct file));
        assert(file != NULL, "alloc file failed!");

        return file;
}

struct file_ops *alloc_file_ops(void)
{
        struct file_ops *file_ops;

        file_ops = kmalloc(sizeof(struct file_ops));
        assert(file_ops != NULL, "alloc file_ops failed!");

        return file_ops;
}

void free_super_block(struct super_block *sb)
{
        kfree(sb);
}

void free_super_block_ops(struct super_ops *sb_ops)
{
        kfree(sb_ops);
}

void free_inode(struct inode *inode)
{
        kfree(inode);
}

void free_inode_ops(struct inode_ops *inode_ops)
{
        kfree(inode_ops);
}

void free_dentry(struct dentry *dentry)
{
        kfree(dentry);
}

void free_dentry_ops(struct dentry_ops *dentry_ops)
{
        kfree(dentry_ops);
}

void free_file(struct file *file)
{
        kfree(file);
}

void free_file_ops(struct file_ops *file_ops)
{
        kfree(file_ops);
}

