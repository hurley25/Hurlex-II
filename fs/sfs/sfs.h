/*
 * =====================================================================================
 *
 *       Filename:  sfs.h
 *
 *    Description:  simple file system
 *
 *        Version:  1.0
 *        Created:  2015年06月02日 10时09分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef FS_SFS_SFS_H
#define FS_SFS_SFS_H

#include <types.h>

struct sfs_super_block {
        uint16_t s_type;                // 文件系统类型
        uint32_t s_inode_count;         // inode数量
        uint32_t s_block_count;         // block数量
        uint32_t s_block_size;          // block大小
        uint32_t s_max_file;            // 文件最大尺寸
        uint32_t s_root_inode;          // 根目录 inode 号

        uint32_t in_secs;               // inode 所占扇区数
        uint32_t bl_secs;               // block 所占扇区数
        uint32_t im_start;              // inode map 起始
        uint32_t im_secs;               // inode map 所占扇区数
        uint32_t bm_start;              // block map 起始地址
        uint32_t bm_secs;               // block map 所占扇区数
        uint32_t in_start;              // inode 起始位置
        uint32_t bl_start;              // block 起始位置
};

#define P_BLOCK_MAP_SIZE     32            // 一级 block 数量
#define PP_BLOCK_MAP_SIZE    32            // 二级 block 数量

struct sfs_inode {
        uint32_t i_type;                // inode 类型
        uint32_t i_ino;                 // 索引节点号
        time_t i_atime;                 // 文件最后一次访问时间
        time_t i_mtime;                 // 文件最后一次修改时间
        time_t i_ctime;                 // 文件首次创建时间
        uint32_t i_size;                // 文件字节数
        uint32_t i_blocks;              // 文件使用block数
        uint32_t i_bytes;               // 文件最后一个block的字节数

        uint32_t p_block_map[P_BLOCK_MAP_SIZE];          // 一级 block 指向
        uint32_t pp_block_map[PP_BLOCK_MAP_SIZE];        // 二级 block 指向
};

// 最长文件名
#define SFS_MAX_FILE_NAME   123

// 目录类型inode存储的目录数据块 sizeof = 128
struct sfs_dirent {
        char d_name[SFS_MAX_FILE_NAME+1];   // 目录项名称
        uint32_t inode;                     // 对应的 inode 号
};

#endif  // FS_SFS_SFS_H
