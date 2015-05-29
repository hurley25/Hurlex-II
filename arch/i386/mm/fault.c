/*
 * =====================================================================================
 *
 *       Filename:  fault.c
 *
 *    Description:  页错误处理
 *
 *        Version:  1.0
 *        Created:  2014年11月05日 09时58分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <mm/mm.h>
#include <common.h>
#include <debug.h>
#include <arch.h>

void do_page_fault(pt_regs_t *regs)
{
        uint32_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r" (cr2));

        printk("Page fault at EIP: 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
        printk("Error code: %x\n", regs->err_code);

        // bit 0 为 0 指页面不存在内存里
        if ( !(regs->err_code & 0x1)) {
                cprintk(rc_black, rc_red, "Because the page wasn't present.\n");
        }
        // bit 1 为 0 表示读错误，为 1 为写错误
        if (regs->err_code & 0x2) {
                cprintk(rc_black, rc_red, "Write error.\n");
        } else {
                cprintk(rc_black, rc_red, "Read error.\n");
        }
        // bit 2 为 1 表示在用户模式打断的，为 0 是在内核模式打断的
        if (regs->err_code & 0x4) {
                cprintk(rc_black, rc_red, "In user mode.\n");
        } else {
                cprintk(rc_black, rc_red, "In kernel mode.\n");
        }
        // bit 3 为 1 表示错误是由保留位覆盖造成的
        if (regs->err_code & 0x8) {
                cprintk(rc_black, rc_red, "Reserved bits being overwritten.\n");
        }
        // bit 4 为 1 表示错误发生在取指令的时候
        if (regs->err_code & 0x10) {
                cprintk(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
        }

        while (1) {
                cpu_hlt();
        }
}

