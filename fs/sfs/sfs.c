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

#include <fs/sfs/sfs.h>
#include <debug.h>
#include <lib/string.h>
#include <lib/list.h>
#include <atomic.h>
#include <mbr.h>
#include <fs.h>

#define SUPER_BLOCK_OFFSET   4   // super_block 偏移
#define INODEMAP_OFFSET     12   // inode_map 偏移

// block 默认大小
#define BLOCK_SIZE        2048

// 最大文件大小
#define MAX_FILE_SIZE     (P_BLOCK_MAP_SIZE * BLOCK_SIZE + \
                         PP_BLOCK_MAP_SIZE * (BLOCK_SIZE / sizeof(uint32_t)) * BLOCK_SIZE)

// SFS 文件系统的 super_block
static struct super_block *sfs_sb;

// SFS 文件系统的 inode_map
static uint8_t *sfs_inode_map;

// SFS 文件系统的 block_map
static uint8_t *sfs_block_map;

// SFS 文件系统的 inode cache
static uint8_t **inode_cache;

// SFS 文件系统的 block cache
static uint8_t **block_cache;

// 初始化文件系统
static void sfs_mkfs(uint32_t start_sec, uint32_t end_sec, uint32_t block_size);

// 读取super_block
static struct super_block *sfs_read_super(struct super_block *sb);

// 回写 super_block
static void sfs_write_super(struct sfs_super_block *sb);

// 读取 inode_map
static int sfs_read_inode_map(void);

// 回写 inode_map
static void sfs_write_inode_map(void);

// 读取 block_map
static int sfs_read_block_map(void);

// 回写 block_map
static void sfs_write_block_map(void);

// 申请 inode 号
static uint32_t sfs_alloc_inode(void);

// 释放 inode 号
static void sfs_free_inode(uint32_t inode_no);

// 申请 block 号
static uint32_t sfs_alloc_block(void);

// 释放 block 号
static void sfs_free_block(uint32_t block_no);

// 读取 inode
static struct sfs_inode *sfs_read_inode(uint32_t inode_no);

// 获取 inode 数据指针(从 inode_cache 获取，可能阻塞于磁盘IO)
static uint8_t *sfs_read_inode(uint32_t inode_no);

// 获取 blcok 指针(从 block_cache 获取，可能阻塞于磁盘IO)
static uint8_t *sfs_read_block(uint32_t block_no);

// sfs 定义
struct filesystem fs_sfs = {
        .name = "sfs",
        .type = SFS_T,
        .read_super = sfs_read_super
};

// sfs 文件系统初始化
void sfs_init(void)
{
        cprintk(rc_black, rc_cyan, "\nInit SFS Filesystem ...\n");

        if (add_filesystem(&fs_sfs) == -1) {
                printk("Add SFS Filesystem Error!\n");
        }

        uint32_t start_sec = 0;
        uint32_t end_sec = 0;
        for (int i = 0; i < PARTITION_COUNT; ++i) {
                if (mbr_info.part[i].partition_type == SFS_T) {
                        cprintk(rc_black, rc_cyan, "Found SFS Filesystem Partition:  ");
                        cprintk(rc_black, rc_cyan, "Start: %u Count: %u\n\n",
                                        mbr_info.part[i].start_sector, mbr_info.part[i].nr_sectors);
                        start_sec = mbr_info.part[i].start_sector;
                        end_sec = mbr_info.part[i].nr_sectors;
                }
        }
        if (start_sec > 0 && end_sec > 0 && end_sec > start_sec) {
                sfs_mkfs(start_sec, end_sec, BLOCK_SIZE);
        }
}

// 初始化文件系统
static void sfs_mkfs(uint32_t start_sec, uint32_t end_sec, uint32_t block_size)
{
        uint32_t sb_start = start_sec + SUPER_BLOCK_OFFSET;          // super_block 起始扇区
        uint32_t all_sec = end_sec - start_sec - INODEMAP_OFFSET;    // 可用扇区数

        // inode : block = 1 : 9
        uint32_t in_secs = all_sec / 10;                   // inode 所占扇区数
        uint32_t bl_secs = all_sec / 10 * 9;               // block 所占扇区数
        uint32_t in_count = in_secs;                       // inode 数量
        uint32_t bl_count = bl_secs / (block_size / 512);  // block 数量
        uint32_t im_start = start_sec + INODEMAP_OFFSET;   // inode map 起始
        uint32_t im_secs  = in_secs / 512 + 1;             // inode map 所占扇区数
        uint32_t bm_start = im_start + im_secs;            // block map 起始地址
        uint32_t bm_secs  = bl_secs / block_size + 1;      // block map 所占扇区数
        uint32_t in_start = bm_start + bm_secs;            // inode 起始位置
        uint32_t bl_start = in_start + in_secs;            // block 起始位置

        printk("super_block start: %u\n", sb_start);
        printk("all          secs: %u\n\n", all_sec);
        printk("inode_map   start: %u\n", im_start);
        printk("inode_map    secs: %u\n\n", im_secs);
        printk("block_map   start: %u\n", bm_start);
        printk("block_map    secs: %u\n\n", bm_secs);
        printk("inode       start: %u\n", in_start);
        printk("inode        secs: %u\n", in_secs);
        printk("inode       count: %u\n\n", in_count);
        printk("block       start: %u\n", bl_start);
        printk("block        secs: %u\n", bl_secs);
        printk("block       count: %u\n\n", bl_count);
        printk("max file size: %u B ~= %d MB\n\n", MAX_FILE_SIZE, MAX_FILE_SIZE / 1024 / 1024);

        struct sfs_super_block sfs_sb;
        bzero(&sfs_sb, sizeof(sfs_sb));

        sfs_sb.s_type = SFS_T;
        sfs_sb.s_inode_count = in_count;
        sfs_sb.s_block_count = bl_count;
        sfs_sb.s_block_size = block_size;
        sfs_sb.s_max_file = MAX_FILE_SIZE;
        sfs_sb.s_root_inode = 0;

        sfs_sb.in_secs = in_secs;
        sfs_sb.bl_secs = bl_secs;
        sfs_sb.im_start = im_start;
        sfs_sb.im_secs = im_secs;
        sfs_sb.bm_start = bm_start;
        sfs_sb.bm_secs = bm_secs;
        sfs_sb.in_start = in_start;
        sfs_sb.bl_start = bl_start;
}

// 读取super_block
static struct super_block *sfs_read_super(struct super_block *sb)
{
        sfs_sb = NULL;

        return NULL;
}

// 回写 super_block
static void sfs_write_super(struct sfs_super_block *sb)
{
}

// 读取 inode_map
static int sfs_read_inode_map(void)
{
        sfs_inode_map = NULL;

        return 0;
}

// 回写 inode_map
static void sfs_write_inode_map(void)
{
}

// 读取 block_map
static int sfs_read_block_map(void)
{
        sfs_block_map = NULL;
        return 0;
}

// 回写 block_map
static void sfs_write_block_map(void)
{
}

// 申请 inode 号
static uint32_t sfs_alloc_inode(void)
{
        return 0;
}

// 释放 inode 号
static void sfs_free_inode(uint32_t inode_no)
{
}

// 申请 block 号
static uint32_t sfs_alloc_block(void)
{
        return 0;
}

// 释放 block 号
static void sfs_free_block(uint32_t block_no)
{
}

// 获取 inode 数据指针(从 inode_cache 获取，可能阻塞于磁盘IO)
static uint8_t *sfs_read_inode(uint32_t inode_no)
{
        return NULL;
}


// 获取 blcok 指针(从 block_cache 获取，可能阻塞)
static uint8_t *sfs_read_block(uint32_t block_no)
{
        return NULL;
}

