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
#include <mm/mm.h>
#include <lib/string.h>

#include "vfs.h"

// 全局的文件系统指针
struct filesystem *file_systems = NULL; 

// 初始化VFS目录树
static void init_mount_tree(void);

// vfs 初始化
void vfs_init(void)
{
        printk_color(rc_black, rc_red, "Init VFS ...\n\n");
        
        init_mount_tree();
}

// 初始化VFS目录树
static void init_mount_tree(void)
{

}

// 添加文件系统
int register_filesystem(struct filesystem *fs)
{
        struct filesystem *p = file_systems;
        while (p) {
                if (strcmp(p->fs_name, fs->fs_name) == 0) {
                        return -1;
                }
                p = p->next;
        }

        fs->next = file_systems;
        file_systems = fs;

        return 0;
}

