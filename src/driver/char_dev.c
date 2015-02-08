/*
 * =====================================================================================
 *
 *       Filename:  char_dev.c
 *
 *    Description:  字符设备相关
 *
 *        Version:  1.0
 *        Created:  2015年02月08日 10时13分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <debug.h>
#include <char_dev.h>

// 字符设备初始化
void char_dev_init(void)
{
        char_dev_t *kb_dev = &kboard_dev;
        kb_dev->ops.init();
        
        printk_color(rc_black, rc_red, "Init %s device ...\n\n", kb_dev->ops.get_desc());
}

