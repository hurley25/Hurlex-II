/*
 * =====================================================================================
 *
 *       Filename:  mbr.h
 *
 *    Description:  MBR 信息定义
 *
 *        Version:  1.0
 *        Created:  2015年02月04日 10时40分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_MBR_H_
#define INCLUDE_MBR_H_

#include <types.h>
#include <block_dev.h>

#define SECTION_SIZE      512 	  // 扇区大小
#define MBR_CODE_LENGTH   446     // MBR代码长度
#define PARTITION_SIZE    16      // 分区表项大小
#define PARTITION_COUNT   4       // 分区表项目个数

typedef
struct partition_info_t {
        uint8_t active_flag;       // 活动分区标记(0x0表示非活动,0x80表示活动)
        uint8_t start_chs[3];      // 起始磁头号+扇区号+柱面号。磁头号1字节;扇区号2字节低6位;柱面号2字节高2位+3字节
        uint8_t partition_type;    // 分区类型
        uint8_t end_chs[3];        // 结束磁头号,含义同起始磁头号
        uint32_t start_sector;     // 逻辑起始扇区号
        uint32_t nr_sectors;       // 所占用扇区数
} __attribute__((packed)) partition_info_t;

typedef
struct mbr_info_t
{
        uint8_t  mbr_code[MBR_CODE_LENGTH];         // 主引导代码
        partition_info_t part[PARTITION_COUNT];     // 4 个主分区表信息
        uint16_t magic_num;                         // 魔数 0xAA55
} __attribute__((packed)) mbr_info_t;

// MBR信息
extern mbr_info_t mbr_info;

// 读取分区信息
int read_mbr_info(block_dev_t *bdev);

// 输出分区信息
void show_partition_info(void);

#endif  // INCLUDE_MBR_H_
