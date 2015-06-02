/*
 * =====================================================================================
 *
 *       Filename:  fs.c
 *
 *    Description:  文件系统相关
 *
 *        Version:  1.0
 *        Created:  2015年02月08日 10时07分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <fs.h>
#include <device.h>

// vfs 初始化
void vfs_init(void);

// sfs 文件系统初始化
void sfs_init(void);

// 文件系统初始化
void fs_init(void)
{
        vfs_init();
        device_init();
        sfs_init();
}

// 打开文件
int do_open(const char *filename, uint32_t openflag)
{
        if (!filename || !openflag) {
                return 0;
        }

        return 0;
}

// 关闭文件
int do_close(int fd)
{
        return fd;
}

// 读取文件
int do_read(int fd, char *buff, size_t size)
{
        if (!buff || !size) {
                return 0;
        }

        return fd;
}

// 写入文件
int do_write(int fd, const char *buff, size_t size)
{
        if (!buff || !size) {
                return 0;
        }

        return fd;
}

