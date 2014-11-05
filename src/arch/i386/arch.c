/*
 * =====================================================================================
 *
 *       Filename:  arch.c
 *
 *    Description:  架构相关的初始化
 *
 *        Version:  1.0
 *        Created:  2014年11月05日 09时50分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <arch.h>

void init_arch(void)
{
        init_gdt();
        init_idt();
        init_clock();
}
