/*
 * =====================================================================================
 *
 *       Filename:  partition.c
 *
 *    Description:  存储设备分区信息获取
 *
 *        Version:  1.0
 *        Created:  2015年02月04日 10时53分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */


#include <debug.h>
#include <partition.h>

#include "ide.h"

// MBR信息
mbr_info_t mbr_info;

// 读取分区信息
void read_partition_info(void)
{
        if (ide_device_valid(M_IDE_NO)) {
                if (ide_read_secs(M_IDE_NO, 0, &mbr_info, 1) == 0) {
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
        } else {
                printk("Main IDE Device Not Found!\n");
        }
}

