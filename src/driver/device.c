/*
 * =====================================================================================
 *
 *       Filename:  device.c
 *
 *    Description:  外部设备相关
 *
 *        Version:  1.0
 *        Created:  2015年02月08日 11时47分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <device.h>
#include <char_dev.h>
#include <block_dev.h>

// 外部设备初始化
void device_init(void)
{
        char_dev_init();
        block_dev_init();
}


