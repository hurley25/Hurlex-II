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
        return sb;
}

