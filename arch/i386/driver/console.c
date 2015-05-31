/*
 * =====================================================================================
 *
 *       Filename:  console.c
 *
 *    Description:  80 * 25 显示模式驱动程序
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 10时44分26秒
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
#include <sync.h>
#include <mm/mm.h>

#include "console.h"

/*
 * VGA(Video Graphics Array，视频图形阵列)是使用模拟信号的一种视频传输标准，
 * 内核可以通过它来控制屏幕上字符或者图形的显示。
 * 在默认的文本模式(Text-Mode)下，VGA控制器保留了一块内存(0x8b000~0x8bfa0)
 * 作为屏幕上字符显示的缓冲区，若要改变屏幕上字符的显示，只需要修改这块内存就好了。
 */
 
// VGA 的显示缓冲的起点是 0xB8000
static uint16_t *video_memory = (uint16_t *)(0xB8000 + PAGE_OFFSET);

// 屏幕"光标"的坐标
static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

// 屏幕是 80 * 25
#define CON_WIDTH  80
#define CON_HIGH   25

// VGA内存输入缓冲区 80 * 128
#define BUFF_WIDTH  80
#define BUFF_HIGH   128

// VGA 输出缓冲区
static uint16_t video_buffer[BUFF_WIDTH * BUFF_HIGH];

// buffer 输出的坐标
static uint16_t buffer_x = 0;
static uint16_t buffer_y = 0;

// 当前屏幕显示在缓冲区的起始位置
static uint16_t current_line = 0;

// VGA 内部的寄存器多达300多个，显然无法一一映射到I/O端口的地址空间。
// 对此 VGA 控制器的解决方案是，将一个端口作为内部寄存器的索引：0x3D4，
// 再通过 0x3D5 端口来设置相应寄存器的值。
#define VGA_IDX   0x3D4
#define VGA_SET   0x3D5

// 在这里用到的两个内部寄存器的编号为0xE与0xF，分别表示光标位置的高8位与低8位。
#define CUR_HIGH  0xE
#define CUR_LOW   0xF

// 移动光标
static void move_cursor(void);

// 刷新屏幕显示到当前输出位置
static void _flush_console_current(void);

// 屏幕显示初始化
void console_init(void)
{
        console_clear();
        cprintk(rc_black, rc_green, "Hello, Hurlex II kernel!\n\n");
}

// 清屏操作
void console_clear(void)
{
        uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
        uint16_t blank = 0x20 | (attribute_byte << 8);

        // 初始化 console 数据
        for (uint32_t i = 0; i < CON_WIDTH * CON_HIGH; ++i) {
              video_memory[i] = blank;
        }
        cursor_x = 0;
        cursor_y = 0;
        move_cursor();

        // 初始化 buffer 数据
        for (uint32_t i = 0; i < BUFF_WIDTH * BUFF_HIGH; ++i) {
              video_buffer[i] = blank;
        }
        
        buffer_x = 0;
        buffer_y = 0;
        
        current_line = 0;
}

// 移动光标
static void move_cursor(void)
{
        uint16_t cursor = cursor_y * CON_WIDTH + cursor_x;
        
        outb(VGA_IDX, CUR_HIGH);                  // 告诉 VGA 我们要设置光标的高字节
        outb(VGA_SET, (cursor >> 8) & 0xFF);      // 发送高 8 位
        outb(VGA_IDX, CUR_LOW);                   // 告诉 VGA 我们要设置光标的低字节
        outb(VGA_SET, cursor & 0xFF);             // 发送低 8 位
}

// 滚动缓冲区
static void scroll_buffer(void)
{
        // attribute_byte 被构造出一个黑底白字的描述格式
        uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
        uint16_t blank = 0x20 | (attribute_byte << 8);  // space 是 0x20

        // buffer_y 到 BUFF_HIGH - 1 的时候，就该换行了
        if (buffer_y == BUFF_HIGH - 1) {

                // 将所有行的显示数据复制到上一行，第一行永远消失了...
                for (uint32_t i = 0 * BUFF_WIDTH; i < (BUFF_HIGH-1) * BUFF_WIDTH; i++) {
                      video_buffer[i] = video_buffer[i+BUFF_WIDTH];
                }

                // 最后的一行数据现在填充空格，不显示任何字符
                for (uint32_t i = (BUFF_HIGH-1) * BUFF_WIDTH; i < BUFF_HIGH * BUFF_WIDTH; i++) {
                      video_buffer[i] = blank;
                }

                buffer_y--;
        }
}

// 屏幕输出一个字符(带颜色)
static void console_putc_color(char c, real_color_t back, real_color_t fore)
{
        uint8_t back_color = (uint8_t)back;
        uint8_t fore_color = (uint8_t)fore;

        uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
        uint16_t attribute = attribute_byte << 8;

        // 0x08 是 退格键 的 ASCII 码
        // 0x09 是 tab 键 的 ASCII 码
        if (c == 0x08 && buffer_x) {
              buffer_x--;
        } else if (c == 0x09) {
              buffer_x = (buffer_x+8) & ~(8-1);
        } else if (c == '\r') {
              buffer_x = 0;
        } else if (c == '\n') {
                buffer_x = 0;
                buffer_y++;
        } else if (c >= ' ') {
                video_buffer[buffer_y * BUFF_WIDTH + buffer_x] = c | attribute;
                buffer_x++;
        }

        // 每 80 个字符一行，满80就必须换行了
        if (buffer_x == BUFF_WIDTH) {
                buffer_x = 0;
                buffer_y ++;
        }

        // 滚动缓冲区
        scroll_buffer();
}

// 屏幕打印一个以 \0 结尾的字符串(默认黑底白字)
void console_write(char *cstr)
{
        bool intr_flag = false;
        local_intr_store(intr_flag);
        {
                while (*cstr) {
                        console_putc_color(*cstr++, rc_black, rc_white);
                }
                _flush_console_current();
        }
        local_intr_restore(intr_flag);
}

// 屏幕打印一个以 \0 结尾的字符串(带颜色)
void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
        bool intr_flag = false;
        local_intr_store(intr_flag);
        {
                while (*cstr) {
                        console_putc_color(*cstr++, back, fore);
                }
                _flush_console_current();
        }
        local_intr_restore(intr_flag);
}

// 刷新屏幕显示到指定位置
static void _flush_console(void)
{
        uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
        uint16_t blank = 0x20 | (attribute_byte << 8);
        uint16_t begin_line = 0, end_line = 0;

        begin_line = current_line;        
        end_line = buffer_y + 1;

        uint32_t i = 0;
        for (uint32_t j = begin_line * CON_WIDTH; j < end_line * CON_WIDTH; ++j) {
                video_memory[i] = video_buffer[j];
                i++;
        }

        while (i < CON_WIDTH * CON_HIGH) {
                video_memory[i] = blank;
                i++;
        }

        cursor_x = buffer_x;
        cursor_y = end_line - begin_line - 1;

        move_cursor();
}

// 刷新屏幕显示到当前输出位置
static void _flush_console_current(void)
{
        if (buffer_y >= CON_HIGH - 1) {
                current_line = buffer_y - CON_HIGH + 1; 
        } else {
                current_line = 0;
        }
        _flush_console();
}

// 屏幕显示向上移动n行
void console_view_up(uint16_t offset)
{
        if (current_line >= offset) {
                current_line -= offset;
        } else {
                current_line = 0;
        }
        _flush_console();
}

// 屏幕显示向下移动n行
void console_view_down(uint16_t offset)
{
        if (current_line + offset < buffer_y) {
                current_line += offset;
        } else {
                current_line = buffer_y;
        }
        _flush_console();
}

