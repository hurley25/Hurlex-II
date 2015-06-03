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

// 全局的文件系统指针
struct filesystem *file_systems;

// 初始化VFS目录树
static void init_mount_tree(struct vfsmount *mount);

// vfs 初始化
void vfs_init(void)
{
        cprintk(rc_black, rc_light_brown, "Init Virtual File System ...\n\n");

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
        add_filesystem(&fs_ramfs);

        INIT_LIST_HEAD(&(fs_ramfs.fs_supers));

        // 获取根文件系统的超级块结构
        struct super_block *sb = alloc_super_block();
        bzero(sb, sizeof(struct super_block));
        
        // 将 super_block 添加到文件系统控制信息下
        list_add(&(sb->s_list), &(fs_ramfs.fs_supers));

        // 为根文件系统初始化超级块
        fs_ramfs.read_super(sb);

        // 初始化根结点 inode
        struct inode *inode = alloc_inode();
        bzero(inode, sizeof(struct inode));
        inode->i_sb = sb;

        // 初始化根结点 dentry
        struct dentry *dentry = alloc_dentry();
        atomic_set(&(dentry->d_count), 0);
        INIT_LIST_HEAD(&(dentry->d_subdirs));
        INIT_LIST_HEAD(&(dentry->d_child));

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
        assert(sb != NULL, "free_super_block error, sb is NULL!");
        kfree(sb);
}

void free_super_block_ops(struct super_ops *sb_ops)
{
        assert(sb_ops != NULL, "free_super_block_ops error, sb_ops is NULL!");
        kfree(sb_ops);
}

void free_inode(struct inode *inode)
{
        assert(inode != NULL, "free_inode error, inode is NULL!");
        kfree(inode);
}

void free_inode_ops(struct inode_ops *inode_ops)
{
        assert(inode_ops != NULL, "free_inode_ops error, inode_ops is NULL!");
        kfree(inode_ops);
}

void free_dentry(struct dentry *dentry)
{
        assert(dentry != NULL, "free_dentry error, dentry is NULL!");
        kfree(dentry);
}

void free_dentry_ops(struct dentry_ops *dentry_ops)
{
        assert(dentry_ops != NULL, "free_dentry_ops error, detry_ops is NULL!");
        kfree(dentry_ops);
}

void free_file(struct file *file)
{
        assert(file != NULL, "free_file error, file is NULL!");
        kfree(file);
}

void free_file_ops(struct file_ops *file_ops)
{
        assert(file_ops != NULL, "free_file_ops error, file_ops is NULL!");
        kfree(file_ops);
}

// 打开文件
int vfs_open(const char *filename, uint32_t openflag)
{
        if (!filename || !openflag) {
                return 0;
        }

        return 0;
}

// 关闭文件
int vfs_close(int fd)
{
        return fd;
}

// 读取文件
int vfs_read(int fd, char *buff, size_t size)
{
        if (!buff || !size) {
                return 0;
        }

        return fd;
}

// 写入文件
int vfs_write(int fd, const char *buff, size_t size)
{
        if (!buff || !size) {
                return 0;
        }

        return fd;
}

