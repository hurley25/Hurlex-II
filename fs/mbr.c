/*
 * =====================================================================================
 *
 *       Filename:  mbr.c
 *
 *    Description:  MBR 相关
 *
 *        Version:  1.0
 *        Created:  2015年02月08日 11时07分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <mbr.h>

 // MBR信息
mbr_info_t mbr_info;

// 读取分区信息
int read_mbr_info(block_dev_t *bdev)
{
        io_request_t request;

        request.io_type = IO_READ;
        request.secno = 0;
        request.nsecs = 1;
        request.buffer = &mbr_info;
        request.bsize = sizeof(mbr_info);

        if (bdev->ops.request(&request) == 0) {
                return 0;
        }

        return -1;
}

void show_partition_info(void)
{
        cprintk(rc_black, rc_red, "\nPartition Info:\n");
        for (int i = 0; i < PARTITION_COUNT; ++i) {
                if (mbr_info.part[i].partition_type != 0) {
                        cprintk(rc_black, rc_red, "Active: %02X  ", mbr_info.part[i].active_flag);
                        cprintk(rc_black, rc_red, "Type: %02X  ", mbr_info.part[i].partition_type);
                        cprintk(rc_black, rc_red, "SCHS: %02X%02X%02X  ",  mbr_info.part[i].start_chs[0],
                                        mbr_info.part[i].start_chs[1], mbr_info.part[i].start_chs[2]);
                        cprintk(rc_black, rc_red, "ECHS: %02X%02X%02X  ", mbr_info.part[i].end_chs[0],
                                        mbr_info.part[i].end_chs[1], mbr_info.part[i].end_chs[2]);
                        cprintk(rc_black, rc_red, "Start: %04u  ", mbr_info.part[i].start_sector);
                        cprintk(rc_black, rc_red, "Count: %05u\n", mbr_info.part[i].nr_sectors);  
                }
        }
}
