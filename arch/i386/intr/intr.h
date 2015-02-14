/*
 * =====================================================================================
 *
 *       Filename:  intr.h
 *
 *    Description:  中断描述符表相关
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 14时49分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef IDT_H_
#define IDT_H_

#include <types.h>

// 中断保存的寄存器类型
typedef
struct pt_regs_t {

        // 用于保存用户的数据段描述符
        uint16_t ds;
        uint16_t padding1;

        // 从 edi 到 eax 由 pusha 指令压栈
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t oesp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;

        // 中断号(内核代码自行压栈)
        uint32_t int_no;

        // 错误代码(有中断错误代码的中断会由CPU压栈)
        uint32_t err_code;

        // 以下由处理器自动压栈
        uint32_t eip;
        uint16_t cs;
        uint16_t padding2;
        uint32_t eflags;

        // 如果发生了特权级的切换CPU会压栈
        uint32_t esp;
        uint16_t ss;
        uint16_t padding3;
} pt_regs_t;

// 定义中断处理函数指针
typedef void (*interrupt_handler_t)(pt_regs_t *);

// 调用中断处理函数
void isr_handler(pt_regs_t *regs);

// 注册一个中断处理函数
void register_interrupt_handler(uint8_t n, interrupt_handler_t h);

// 中断号定义
#define INT_DIVIDE_ERROR         0
#define INT_DEBUG                1
#define INT_NMI                  2
#define INT_BREAKPOINT           3
#define INT_OVERFLOW             4
#define INT_BOUND                5
#define INT_INVALID_OPCODE       6
#define INT_DEVICE_NOT_AVAIL     7
#define INT_DOUBLE_FAULT         8
#define INT_COPROCESSOR          9
#define INT_INVALID_TSS         10
#define INT_SEGMENT             11
#define INT_STACK_FAULT         12
#define INT_GENERAL_PROTECT     13
#define INT_PAGE_FAULT          14

#define INT_X87_FPU             16
#define INT_ALIGNMENT           17
#define INT_MACHINE_CHECK       18
#define INT_SIMD_FLOAT          19
#define INT_VIRTUAL_EXCE        20

// 声明中断处理函数 0-19 属于 CPU 的异常中断
// ISR:中断服务程序(interrupt service routine)
void isr0();            // 0 #DE 除 0 异常 
void isr1();            // 1 #DB 调试异常 
void isr2();            // 2 NMI 
void isr3();            // 3 BP 断点异常 
void isr4();            // 4 #OF 溢出 
void isr5();            // 5 #BR 对数组的引用超出边界 
void isr6();            // 6 #UD 无效或未定义的操作码 
void isr7();            // 7 #NM 设备不可用(无数学协处理器) 
void isr8();            // 8 #DF 双重故障(有错误代码) 
void isr9();            // 9 协处理器跨段操作 
void isr10();           // 10 #TS 无效TSS(有错误代码) 
void isr11();           // 11 #NP 段不存在(有错误代码) 
void isr12();           // 12 #SS 栈错误(有错误代码) 
void isr13();           // 13 #GP 常规保护(有错误代码) 
void isr14();           // 14 #PF 页故障(有错误代码) 
void isr15();           // 15 CPU 保留 
void isr16();           // 16 #MF 浮点处理单元错误 
void isr17();           // 17 #AC 对齐检查 
void isr18();           // 18 #MC 机器检查 
void isr19();           // 19 #XM SIMD(单指令多数据)浮点异常

// 20-31 Intel 保留
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

// 32～255 用户自定义异常
void isr128();

// IRQ 处理函数
void irq_handler(pt_regs_t *regs);

// 定义IRQ
#define  IRQ0     32    // 电脑系统计时器
#define  IRQ1     33    // 键盘
#define  IRQ2     34    // 与 IRQ9 相接，MPU-401 MD 使用
#define  IRQ3     35    // 串口设备
#define  IRQ4     36    // 串口设备
#define  IRQ5     37    // 建议声卡使用
#define  IRQ6     38    // 软驱传输控制使用
#define  IRQ7     39    // 打印机传输控制使用
#define  IRQ8     40    // 即时时钟
#define  IRQ9     41    // 与 IRQ2 相接，可设定给其他硬件
#define  IRQ10    42    // 建议网卡使用
#define  IRQ11    43    // 建议 AGP 显卡使用
#define  IRQ12    44    // 接 PS/2 鼠标，也可设定给其他硬件
#define  IRQ13    45    // 协处理器使用
#define  IRQ14    46    // IDE0 传输控制使用
#define  IRQ15    47    // IDE1 传输控制使用

// 声明 IRQ 函数
// IRQ:中断请求(Interrupt Request)
void irq0();            // 电脑系统计时器
void irq1();            // 键盘
void irq2();            // 与 IRQ9 相接，MPU-401 MD 使用
void irq3();            // 串口设备
void irq4();            // 串口设备
void irq5();            // 建议声卡使用
void irq6();            // 软驱传输控制使用
void irq7();            // 打印机传输控制使用
void irq8();            // 即时时钟
void irq9();            // 与 IRQ2 相接，可设定给其他硬件
void irq10();           // 建议网卡使用
void irq11();           // 建议 AGP 显卡使用
void irq12();           // 接 PS/2 鼠标，也可设定给其他硬件
void irq13();           // 协处理器使用
void irq14();           // IDE0 传输控制使用
void irq15();           // IDE1 传输控制使用

// 初始化中断描述符表
void idt_init(void);

#endif  // IDT_H_
