/*
 * =====================================================================================
 *
 *       Filename:  ide.c
 *
 *    Description:  IDE 设备驱动
 *
 *        Version:  1.0
 *        Created:  2015年02月03日 14时55分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <common.h>
#include <debug.h>
#include "ide.h"

#define SECTSIZE  512 // 默认扇区大小

#define IDE_BSY                 0x80            // IDE设备忙碌
#define IDE_DRDY                0x40            // IDE设备可用
#define IDE_DF                  0x20
#define IDE_DRQ                 0x08
#define IDE_ERR                 0x01

#define IDE_CMD_READ            0x20            // IDE读扇区命令
#define IDE_CMD_WRITE           0x30            // IDE写扇区命令
#define IDE_CMD_IDENTIFY        0xEC            // IDE识别命令

#define IDE_IDENT_SECTORS       20
#define IDE_IDENT_MODEL         54
#define IDE_IDENT_CAPABILITIES  98
#define IDE_IDENT_CMDSETS       164
#define IDE_IDENT_MAX_LBA       120
#define IDE_IDENT_MAX_LBA_EXT   200

#define IO_BASE0                0x1F0           // 主IDE设备起始操作端口
#define IO_BASE1                0x170           // 从IDE设备起始操作端口
#define IO_CTRL0                0x3F4           // 主IDE控制起始控制端口
#define IO_CTRL1                0x374           // 从IDE设备起始控制端口

// IDE设备控制端口偏移量
#define ISA_DATA                0x00
#define ISA_ERROR               0x01
#define ISA_PRECOMP             0x01
#define ISA_CTRL                0x02
#define ISA_SECCNT              0x02
#define ISA_SECTOR              0x03
#define ISA_CYL_LO              0x04
#define ISA_CYL_HI              0x05
#define ISA_SDH                 0x06
#define ISA_COMMAND             0x07
#define ISA_STATUS              0x07

// IO 通道定义
static const struct {
        uint16_t base;        // I/O Base
        uint16_t ctrl;        // Control Base
} channels[2] = { {IO_BASE0, IO_CTRL0}, {IO_BASE1, IO_CTRL1} };

#define IO_BASE(ideno)          (channels[(ideno) >> 1].base)
#define IO_CTRL(ideno)          (channels[(ideno) >> 1].ctrl)

// IDE设备限制值
#define MAX_IDE                 4               // IDE设备最大设备号
#define MAX_NSECS               128             // IDE设备最大操作扇区数
#define MAX_DISK_NSECS          0x10000000      // IDE设备最大扇区号

// 判断IDE设备是否可用
#define VALID_IDE(ideno)        (((ideno) < MAX_IDE) && (ide_devices[ideno].valid == 1))

// IDE设备信息
static struct ide_device {
        uint8_t valid;        // 是否可用
        uint32_t sets;        // 命令支持
        uint32_t size;        // 扇区数量
        uint8_t desc[41];    // 描述
} ide_devices[MAX_IDE];

// 等待IDE设备可用
static int32_t ide_wait_ready(uint16_t iobase, bool check_error)
{
        int32_t r;

        while ((r = inb(iobase + ISA_STATUS)) & IDE_BSY) {
                // Waiting ... Do nothing ...
        }
        if (check_error && (r & (IDE_DF | IDE_ERR)) != 0) {
                return -1;
        }

        return 0;
}

// 初始化IDE设备
void ide_init(void)
{
        unsigned short ideno, iobase;
        for (ideno = 0; ideno < MAX_IDE; ++ideno) {
                iobase = IO_BASE(ideno);
                // 等待设备可用
                ide_wait_ready(iobase, 0);

                // step1: select drive
                outb(iobase + ISA_SDH, 0xE0 | ((ideno & 1) << 4));
                ide_wait_ready(iobase, 0);

                // step2: send ATA identify command
                outb(iobase + ISA_COMMAND, IDE_CMD_IDENTIFY);
                ide_wait_ready(iobase, 0);

                // step3: polling
                if (inb(iobase + ISA_STATUS) == 0 || ide_wait_ready(iobase, 1) != 0) {
                        continue ;
                }

                // 设置设备可用
                ide_devices[ideno].valid = 1;

                // read identification space of the device 
                unsigned int buffer[128];
                insl(iobase + ISA_DATA, buffer, sizeof(buffer) / sizeof(unsigned int));

                unsigned char *ident = (unsigned char *)buffer;
                unsigned int sectors;
                unsigned int cmdsets = *(unsigned int *)(ident + IDE_IDENT_CMDSETS);
                
                // device use 48-bits or 28-bits addressing
                if (cmdsets & (1 << 26)) {
                        sectors = *(unsigned int *)(ident + IDE_IDENT_MAX_LBA_EXT);
                }
                else {
                        sectors = *(unsigned int *)(ident + IDE_IDENT_MAX_LBA);
                }
                ide_devices[ideno].sets = cmdsets;
                ide_devices[ideno].size = sectors;

                uint8_t *desc = ide_devices[ideno].desc, *data = ident + IDE_IDENT_MODEL;
                uint32_t i, length = 40;
                for (i = 0; i < length; i += 2) {
                        desc[i] = data[i + 1], desc[i + 1] = data[i];
                }
                do {
                        desc[i] = '\0';
                } while (i -- > 0 && desc[i] == ' ');

                printk_color(rc_black, rc_red, "Found IDE Driver %d: %u (sectors)  Desc: %s\n",
                                ideno, ide_devices[ideno].size, ide_devices[ideno].desc);
        }
}

// 检测指定IDE设备是否可用
bool ide_device_valid(uint16_t ideno)
{
        return VALID_IDE(ideno);
}

// 获取指定IDE设备尺寸
uint32_t ide_device_size(uint16_t ideno)
{
        if (ide_device_valid(ideno)) {
                return ide_devices[ideno].size;
        }

        return 0;
}

// 读取指定IDE设备若干扇区
int32_t ide_read_secs(uint16_t ideno, uint32_t secno, void *dst, uint32_t nsecs)
{
        assert(nsecs <= MAX_NSECS && VALID_IDE(ideno), "nsecs or indeo error!");
        assert(secno < MAX_DISK_NSECS && secno + nsecs <= MAX_DISK_NSECS, "secno error!");

        uint16_t iobase = IO_BASE(ideno), ioctrl = IO_CTRL(ideno);

        ide_wait_ready(iobase, 0);

        // generate interrupt
        outb(ioctrl + ISA_CTRL, 0);
        outb(iobase + ISA_SECCNT, nsecs);
        outb(iobase + ISA_SECTOR, secno & 0xFF);
        outb(iobase + ISA_CYL_LO, (secno >> 8) & 0xFF);
        outb(iobase + ISA_CYL_HI, (secno >> 16) & 0xFF);
        outb(iobase + ISA_SDH, 0xE0 | ((ideno & 1) << 4) | ((secno >> 24) & 0xF));
        outb(iobase + ISA_COMMAND, IDE_CMD_READ);

        int ret;
        for (ret = 0; nsecs > 0; nsecs --, dst += SECTSIZE) {
                if ((ret = ide_wait_ready(iobase, 1)) != 0) {
                        return ret;
                }
                insl(iobase, dst, SECTSIZE / sizeof(uint32_t));
        }

        return ret;
}

// 写入指定IDE设备若干扇区
int32_t ide_write_secs(uint16_t ideno, uint32_t secno, const void *src, uint32_t nsecs)
{
        assert(nsecs <= MAX_NSECS && VALID_IDE(ideno), "nsecs or ideno error");
        assert(secno < MAX_DISK_NSECS && secno + nsecs <= MAX_DISK_NSECS, "secno error!");

        uint16_t iobase = IO_BASE(ideno), ioctrl = IO_CTRL(ideno);

        ide_wait_ready(iobase, 0);

        // generate interrupt
        outb(ioctrl + ISA_CTRL, 0);
        outb(iobase + ISA_SECCNT, nsecs);
        outb(iobase + ISA_SECTOR, secno & 0xFF);
        outb(iobase + ISA_CYL_LO, (secno >> 8) & 0xFF);
        outb(iobase + ISA_CYL_HI, (secno >> 16) & 0xFF);
        outb(iobase + ISA_SDH, 0xE0 | ((ideno & 1) << 4) | ((secno >> 24) & 0xF));
        outb(iobase + ISA_COMMAND, IDE_CMD_WRITE);

        int ret;
        for (ret = 0; nsecs > 0; nsecs --, src += SECTSIZE) {
                if ((ret = ide_wait_ready(iobase, 1)) != 0) {
                        return ret;
                }
                outsl(iobase, src, SECTSIZE / sizeof(uint32_t));
        }

        return ret;
}

