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

// 全局的文件系统指针
fs_t *file_systems = NULL; 

// vfs 初始化
void vfs_init(void)
{

}

// 添加文件系统
int register_filesystem(fs_t *fs)
{
        fs_t *p = file_systems;
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

