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

#include <types.h>
#include <common.h>
#include <debug.h>

#include <block_dev.h>

#define SECTSIZE  512 // 默认扇区大小

/** 
 * bit 7 = 1  控制器忙         * bit 6 = 1  驱动器就绪
 * bit 5 = 1  设备错误         * bit 4      N/A
 * bit 3 = 1  扇区缓冲区错误   * bit 2 = 1  磁盘已被读校验
 * bit 1        N/A            * bit 0 = 1  上一次命令执行失败
 */
#define IDE_BSY                 0x80            // IDE驱动器忙 
#define IDE_DRDY                0x40            // IDE驱动器就绪
#define IDE_DF                  0x20            // IDE驱动器错误
#define IDE_ERR                 0x01            // 上一次命令失败

#define IDE_CMD_READ            0x20            // IDE读扇区命令
#define IDE_CMD_WRITE           0x30            // IDE写扇区命令
#define IDE_CMD_IDENTIFY        0xEC            // IDE识别命令

// IDE设备端口起始端口定义
#define IOBASE                  0x1F0             // 主IDE设备起始操作端口
#define IOCTRL                  0x3F4             // 主IDE控制起始控制端口

// IDE设备控制端口偏移量
#define ISA_DATA                0x00            // IDE数据端口偏移
#define ISA_ERROR               0x01            // IDE错误端口偏移
#define ISA_PRECOMP             0x01            
#define ISA_CTRL                0x02            // IDE控制端口偏移
#define ISA_SECCNT              0x02
#define ISA_SECTOR              0x03
#define ISA_CYL_LO              0x04
#define ISA_CYL_HI              0x05
#define ISA_SDH                 0x06            // IDE选择端口偏移
#define ISA_COMMAND             0x07            // IDE命令端口偏移
#define ISA_STATUS              0x07            // IDE状态端口偏移

// IDE设备限制值
#define MAX_NSECS               128             // IDE设备最大操作扇区数
#define MAX_DISK_NSECS          0x10000000      // IDE设备最大扇区号

// IDE设备身份信息在读取的信息块中的偏移
#define IDE_IDENT_SECTORS       20
#define IDE_IDENT_MODEL         54
#define IDE_IDENT_CAPABILITIES  98
#define IDE_IDENT_CMDSETS       164
#define IDE_IDENT_MAX_LBA       120
#define IDE_IDENT_MAX_LBA_EXT   200

#define IDE_DESC_LEN            40              // IDE设备描述信息尺寸

// IDE设备信息
static struct ide_device {
        uint8_t valid;                   // 是否可用
        uint32_t sets;                   // 命令支持
        uint32_t size;                   // 扇区数量
        char desc[IDE_DESC_LEN+1];       // IDE设备描述
} ide_device;

// 初始化IDE设备
static int ide_init(void);

// 检测IDE设备是否可用
static bool ide_device_valid(void);

// 获取IDE设备描述
static const char *ide_get_desc(void);

// 获得设备默认块数量
static int ide_get_nr_block(void);

// 设备操作请求
static int ide_request(io_request_t *req);

// 读取IDE设备若干扇区
static int ide_read_secs(uint32_t secno, void *dst, uint32_t nsecs);

// 写入IDE设备若干扇区
static int ide_write_secs(uint32_t secno, const void *src, uint32_t nsecs);

// IDE设备选项设置
static int ide_ioctl(int op, int flag);

// IDE 设备结构
block_dev_t ide_main_dev = {
        .name = "IDE_MAIN",
        .block_size = SECTSIZE,
        .ops = {
                .init = &ide_init,
                .device_valid = &ide_device_valid,
                .get_desc = &ide_get_desc,
                .get_nr_block = &ide_get_nr_block,
                .request = &ide_request,
                .ioctl = ide_ioctl
        }
};

// 等待IDE设备可用
static int32_t ide_wait_ready(uint16_t iobase, bool check_error)
{
        int r = 0;
        while ((r = inb(iobase + ISA_STATUS)) & IDE_BSY) {
                // Waiting ... Do nothing ...
        }
        if (check_error && (r & (IDE_DF | IDE_ERR)) != 0) {
                return -1;
        }

        return 0;
}

// 获取IDE设备描述
static const char *ide_get_desc(void)
{
        return (const char *)(ide_device.desc);
}

// 初始化IDE设备
static int ide_init(void)
{
        ide_wait_ready(IOBASE, 0);

        // 1: 选择要操作的设备
        outb(IOBASE + ISA_SDH, 0xE0);
        ide_wait_ready(IOBASE, 0);

        // 2: 发送IDE信息获取命令
        outb(IOBASE + ISA_COMMAND, IDE_CMD_IDENTIFY);
        ide_wait_ready(IOBASE, 0);

        // 3: 检查设备是否存在
        if (inb(IOBASE + ISA_STATUS) == 0 || ide_wait_ready(IOBASE, 1) != 0) {
                return -1;
        }

        ide_device.valid = 1;

        // 读取IDE设备信息
        uint32_t buffer[128];
        insl(IOBASE + ISA_DATA, buffer, sizeof(buffer) / sizeof(uint32_t));

        uint8_t *ident = (uint8_t *)buffer;
        uint32_t cmdsets = *(uint32_t *)(ident + IDE_IDENT_CMDSETS);
        uint32_t sectors;

        // 检查设备使用48-bits还是28-bits地址
        if (cmdsets & (1 << 26)) {
                sectors = *(uint32_t *)(ident + IDE_IDENT_MAX_LBA_EXT);
        }
        else {
                sectors = *(uint32_t *)(ident + IDE_IDENT_MAX_LBA);
        }
        ide_device.sets = cmdsets;
        ide_device.size = sectors;

        char *desc = ide_device.desc;
        char *data = (char *)((uint32_t)ident + IDE_IDENT_MODEL);

        // 复制设备描述信息
        int i, length = IDE_DESC_LEN;
        for (i = 0; i < length; i += 2) {
                desc[i] = data[i+1];
                desc[i+1] = data[i];
        }
        do {
                desc[i] = '\0';
        } while (i-- > 0 && desc[i] == ' ');

        return 0;
}

// 检测指定IDE设备是否可用
static bool ide_device_valid(void)
{
        return ide_device.valid == 1;
}

// 获得设备默认块数量
static int ide_get_nr_block(void)
{
        if (ide_device_valid()) {
                return ide_device.size;
        }

        return 0;
}

// 设备操作请求
static int ide_request(io_request_t *req)
{
        if (req->io_type == IO_READ) {
                if (req->bsize < SECTSIZE * req->nsecs) {
                        return -1;
                }
                return ide_read_secs(req->secno, req->buffer ,req->nsecs);
        } else if (req->io_type == IO_WRITE) {
                if (req->bsize < SECTSIZE * req->nsecs) {
                        return -1;
                }
                return ide_write_secs(req->secno, req->buffer ,req->nsecs);
        }

        return -1;
}

// 读取指定IDE设备若干扇区
static int ide_read_secs(uint32_t secno, void *dst, uint32_t nsecs)
{
        assert(nsecs <= MAX_NSECS && ide_device.valid == 1, "nsecs or ide error!");
        assert(secno < MAX_DISK_NSECS && secno + nsecs <= MAX_DISK_NSECS, "secno error!");

        ide_wait_ready(IOBASE, 0);

        outb(IOCTRL + ISA_CTRL, 0);
        outb(IOBASE + ISA_SECCNT, nsecs);
        outb(IOBASE + ISA_SECTOR, secno & 0xFF);
        outb(IOBASE + ISA_CYL_LO, (secno >> 8) & 0xFF);
        outb(IOBASE + ISA_CYL_HI, (secno >> 16) & 0xFF);
        outb(IOBASE + ISA_SDH, 0xE0 | ((secno >> 24) & 0xF));
        outb(IOBASE + ISA_COMMAND, IDE_CMD_READ);

        int ret = 0;
        for ( ; nsecs > 0; nsecs --, dst += SECTSIZE) {
                if ((ret = ide_wait_ready(IOBASE, 1)) != 0) {
                        return ret;
                }
                insl(IOBASE, dst, SECTSIZE / sizeof(uint32_t));
        }

        return ret;
}

// 写入指定IDE设备若干扇区
static int ide_write_secs(uint32_t secno, const void *src, uint32_t nsecs)
{
        assert(nsecs <= MAX_NSECS && ide_device.valid == 1, "nsecs or ide error");
        assert(secno < MAX_DISK_NSECS && secno + nsecs <= MAX_DISK_NSECS, "secno error!");

        ide_wait_ready(IOBASE, 0);

        outb(IOCTRL + ISA_CTRL, 0);
        outb(IOBASE + ISA_SECCNT, nsecs);
        outb(IOBASE + ISA_SECTOR, secno & 0xFF);
        outb(IOBASE + ISA_CYL_LO, (secno >> 8) & 0xFF);
        outb(IOBASE + ISA_CYL_HI, (secno >> 16) & 0xFF);
        outb(IOBASE + ISA_SDH, 0xE0 | ((secno >> 24) & 0xF));
        outb(IOBASE + ISA_COMMAND, IDE_CMD_WRITE);

        int ret = 0;
        for ( ; nsecs > 0; nsecs --, src += SECTSIZE) {
                if ((ret = ide_wait_ready(IOBASE, 1)) != 0) {
                        return ret;
                }
                outsl(IOBASE, src, SECTSIZE / sizeof(uint32_t));
        }

        return ret;
}

// IDE设备选项设置
static int ide_ioctl(int op, int flag)
{
        if (op != 0 && flag != 0) {
                return -1;
        }

        return 0;
}

