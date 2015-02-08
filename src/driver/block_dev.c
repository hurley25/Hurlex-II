/*
 * =====================================================================================
 *
 *       Filename:  block_dev.c
 *
 *    Description:  块设备相关
 *
 *        Version:  1.0
 *        Created:  2015年02月08日 10时12分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <debug.h>
#include <block_dev.h>
#include <mbr.h>

// MBR信息
mbr_info_t mbr_info;

// 块设备初始化
void block_dev_init(void)
{
        block_dev_t *ide_dev = &ide_main_dev;
        if (ide_dev->ops.init() == -1) {
                printk("Main IDE Device Not Found!\n");
                return;
        }

        if (ide_dev->ops.device_valid()) {
                printk_color(rc_black, rc_red, "Found IDE Driver: %u (sectors)  Desc: %s\n",
                         ide_dev->ops.get_nr_block(), ide_dev->ops.get_desc());
                
                read_mbr_info();
        }
}

// 读取分区信息
void read_mbr_info(void)
{
        io_request_t request;

        request.io_type = IO_READ;
        request.secno = 0;
        request.nsecs = 1;
        request.buffer = &mbr_info;
        request.bsize = sizeof(mbr_info);

        block_dev_t *ide_dev = &ide_main_dev;
        if (ide_dev->ops.request(&request) == 0) {
                printk_color(rc_black, rc_red, "\nPartition Info:\n");
                for (int i = 0; i < PARTITION_COUNT; ++i) {
                        if (mbr_info.part[i].partition_type == 0) {
                                continue;
                        }
                        printk_color(rc_black, rc_red, "Active: %02X  ", mbr_info.part[i].active_flag);
                        printk_color(rc_black, rc_red, "Type: %02X  ", mbr_info.part[i].partition_type);
                        printk_color(rc_black, rc_red, "SCHS: %02X%02X%02X  ",  mbr_info.part[i].start_chs[0],
                                        mbr_info.part[i].start_chs[1], mbr_info.part[i].start_chs[2]);
                        printk_color(rc_black, rc_red, "ECHS: %02X%02X%02X  ", mbr_info.part[i].end_chs[0],
                                        mbr_info.part[i].end_chs[1], mbr_info.part[i].end_chs[2]);
                        printk_color(rc_black, rc_red, "Start: %04u  ", mbr_info.part[i].start_sector);
                        printk_color(rc_black, rc_red, "Count: %05u\n", mbr_info.part[i].nr_sectors);  
                }
        } else {
                printk("Read MBR Info Error!");
        }
}

