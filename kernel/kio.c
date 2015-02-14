/*
 * =====================================================================================
 *
 *       Filename:  kio.c
 *
 *    Description:  输入输出的实现
 *
 *        Version:  1.0
 *        Created:  2015年02月08日 12时07分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <common.h>
#include <char_dev.h>
#include <kio.h>

// 读取一个字符
char getchar(void)
{
        char ch;
        char_dev_t *kb_dev = &kboard_dev;

        if (!kb_dev->ops.device_valid()) {
                return 0;
        }

        while (kb_dev->ops.read(&ch, 1) == 1) {
                cpu_hlt();
                return ch;
        }

        return 0;
}
