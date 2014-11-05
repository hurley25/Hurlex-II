/*
 * =====================================================================================
 *
 *       Filename:  debug.c
 *
 *    Description:  调试相关的函数
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 10时54分39秒
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
#include <elf.h>
#include <lib/string.h>
#include <mm/mm.h>

static elf_t kernel_elf;

static void print_stack_trace();

static const char *elf_lookup_symbol(uint32_t addr, elf_t *elf);

void init_debug(void)
{
        // 从 GRUB 提供的信息中获取到内核符号表和代码地址信息
        kernel_elf = elf_from_multiboot(glb_mboot_ptr);
}

elf_t elf_from_multiboot(multiboot_t *mb)
{
        int i;
        elf_t elf;
        elf_section_header_t *sh = (elf_section_header_t *)mb->addr;

        uint32_t shstrtab = sh[mb->shndx].addr;
        for (i = 0; i < mb->num; i++) {
                const char *name = (const char *)(shstrtab + sh[i].name) + PAGE_OFFSET;
                // 在 GRUB 提供的 multiboot 信息中寻找内核 ELF 格式所提取的字符串表和符号表
                if (strcmp(name, ".strtab") == 0) {
                        elf.strtab = (const char *)sh[i].addr + PAGE_OFFSET;
                        elf.strtabsz = sh[i].size;
                }
                if (strcmp(name, ".symtab") == 0) {
                        elf.symtab = (elf_symbol_t *)(sh[i].addr + PAGE_OFFSET);
                        elf.symtabsz = sh[i].size;
                }
        }

        return elf;
}

void panic(const char *msg)
{
        printk("*** System panic: %s\n", msg);
        print_stack_trace();
        printk("***\n");
        
        // 致命错误发生后打印栈信息后停止在这里
        while(1) {
        	cpu_hlt();
        }
}

void print_stack_trace(void)
{
        uint32_t *ebp, *eip;

        asm volatile ("mov %%ebp, %0" : "=r" (ebp));
        while (ebp) {
                eip = ebp + 1;
                printk("   [0x%x] %s\n", *eip, elf_lookup_symbol(*eip, &kernel_elf));
                ebp = (uint32_t*)*ebp;
        }
}

static const char *elf_lookup_symbol(uint32_t addr, elf_t *elf)
{
        int i;

        for (i = 0; i < (elf->symtabsz / sizeof(elf_symbol_t)); i++) {
                if (ELF32_ST_TYPE(elf->symtab[i].info) != 0x2) {
                      continue;
                }
                // 通过函数调用地址查到函数的名字(地址在该函数的代码段地址区间之内)
                if ( (addr >= elf->symtab[i].value) && (addr < (elf->symtab[i].value + elf->symtab[i].size)) ) {
                        return (const char *)((uint32_t)elf->strtab + elf->symtab[i].name);
                }
        }

        return NULL;
}

void print_cur_status(void)
{
        static int round = 0;
        uint16_t reg1, reg2, reg3, reg4;

        asm volatile ( "mov %%cs, %0;"
                        "mov %%ds, %1;"
                        "mov %%es, %2;"
                        "mov %%ss, %3;"
                        : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

        // 打印当前的运行级别
        printk("%d: @ring %d\n", round, reg1 & 0x3);
        printk("%d:  cs = %x\n", round, reg1);
        printk("%d:  ds = %x\n", round, reg2);
        printk("%d:  es = %x\n", round, reg3);
        printk("%d:  ss = %x\n", round, reg4);
        ++round;
}

