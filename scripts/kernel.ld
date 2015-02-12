/*
 *      kernel.ld -- 针对 kernel 格式所写的链接脚本
 *     
 *      qianyi.lh   2014/11/04  10:00:00
 */

ENTRY(start)
SECTIONS
{
        . = 0x100000;

        PROVIDE(kern_start = .);
        PROVIDE(kern_init_text_start = .);
        .init.text :
        {
                *(.init.text)
                . = ALIGN(4096);
        }
        
        PROVIDE(kern_init_text_end = .);
        PROVIDE(kern_init_data_start = .);
        .init.data :
        {
                *(.init.data)
                . = ALIGN(4096);
        }

        PROVIDE(kern_init_data_end = .);

        . += 0xC0000000;

        PROVIDE(kern_text_start = . - 0xC0000000);

        .text : AT(ADDR(.text) - 0xC0000000)
        {
                *(.text)
                . = ALIGN(4096);
        }

        PROVIDE(kern_text_end = . - 0xC0000000);
        PROVIDE(kern_data_start = . - 0xC0000000);

        .data : AT(ADDR(.data) - 0xC0000000)
        {
                *(.data)
                *(.rodata)
                . = ALIGN(4096);
        }
        .bss : AT(ADDR(.bss) - 0xC0000000)
        {
                *(.bss)
                . = ALIGN(4096);
        }

        PROVIDE(kern_data_end = . - 0xC0000000);

        .stab : AT(ADDR(.stab) - 0xC0000000)
        {
                *(.stab)
                . = ALIGN(4096);
        }
        .stabstr : AT(ADDR(.stabstr) - 0xC0000000)
        {
                *(.stabstr)
                . = ALIGN(4096);
        }

        PROVIDE(kern_end = . - 0xC0000000);

        /DISCARD/ : { *(.comment) *(.eh_frame) }
}

