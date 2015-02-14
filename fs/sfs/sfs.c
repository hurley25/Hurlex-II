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

#include <debug.h>
#include <lib/string.h>
#include <lib/list.h>
#include <atomic.h>
#include <mbr.h>
#include <fs.h>

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

// sfs 文件系统初始化
void sfs_init(void)
{
        printk_color(rc_black, rc_cyan, "\nInit SFS Filesystem ...\n");

        if (add_filesystem(&fs_sfs) == -1) {
                printk("Add SFS Filesystem Error!\n");
        }
        for (int i = 0; i < PARTITION_COUNT; ++i) {
                if (mbr_info.part[i].partition_type == SFS_T) {
                        printk_color(rc_black, rc_cyan, "Found SFS Filesystem Partition:  ");
                        printk_color(rc_black, rc_cyan, "Start: %u Count: %u\n",
                                        mbr_info.part[i].start_sector, mbr_info.part[i].nr_sectors);  
                }
        }
}

