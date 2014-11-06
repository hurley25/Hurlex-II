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

#include <console.h>
#include <common.h>
#include <mm/mm.h>

/*
 * VGA(Video Graphics Array，视频图形阵列)是使用模拟信号的一种视频传输标准，
 * 内核可以通过它来控制屏幕上字符或者图形的显示。
 * 在默认的文本模式(Text-Mode)下，VGA控制器保留了一块内存(0x8b000~0x8bfa0)
 * 作为屏幕上字符显示的缓冲区，若要改变屏幕上字符的显示，只需要修改这块内存就好了。
 */
 
// VGA 的显示缓冲的起点是 0xB8000
static uint16_t *video_memory = (uint16_t *)(0xB8000 + PAGE_OFFSET);

// 屏幕"光标"的坐标
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// 屏幕是 80 * 25
#define CON_WIDTH  80
#define CON_HIGH   25

// VGA 内部的寄存器多达300多个，显然无法一一映射到I/O端口的地址空间。
// 对此 VGA 控制器的解决方案是，将一个端口作为内部寄存器的索引：0x3D4，
// 再通过 0x3D5 端口来设置相应寄存器的值。
#define VGA_IDX   0x3D4
#define VGA_SET   0x305

// 在这里用到的两个内部寄存器的编号为14与15，分别表示光标位置的高8位与低8位。
#define CUR_HIGH  14
#define CUR_LOW   15

// 移动光标
static void move_cursor(void)
{
        uint16_t cursorLocation = cursor_y * CON_WIDTH + cursor_x;
        
        outb(VGA_IDX, CUR_HIGH);                  // 告诉 VGA 我们要设置光标的高字节
        outb(VGA_SET, cursorLocation >> 8);       // 发送高 8 位
        outb(VGA_IDX, CUR_LOW);                   // 告诉 VGA 我们要设置光标的低字节
        outb(VGA_SET, cursorLocation);            // 发送低 8 位
}

// 屏幕滚动操作
static void scroll(void)
{
        // attribute_byte 被构造出一个黑底白字的描述格式
        uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
        uint16_t blank = 0x20 | (attribute_byte << 8);  // space 是 0x20

        // cursor_y 到 CON_HIGH 的时候，就该换行了
        if (cursor_y >= CON_HIGH) {
                // 将所有行的显示数据复制到上一行，第一行永远消失了...
                int i;
                for (i = 0 * CON_WIDTH; i < (CON_HIGH-1) * CON_WIDTH; i++) {
                      video_memory[i] = video_memory[i+CON_WIDTH];
                }

                // 最后的一行数据现在填充空格，不显示任何字符
                for (i = (CON_HIGH-1) * CON_WIDTH; i < CON_HIGH * CON_WIDTH; i++) {
                      video_memory[i] = blank;
                }

                // 向上移动了一行，所以 cursor_y 现在是 24
                cursor_y = CON_HIGH - 1;
        }
}

// 屏幕显示初始化
void init_console(void)
{
        console_clear();
}

// 清屏操作
void console_clear(void)
{
        uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
        uint16_t blank = 0x20 | (attribute_byte << 8);

        int i;
        for (i = 0; i < CON_WIDTH * CON_HIGH; i++) {
              video_memory[i] = blank;
        }

        cursor_x = 0;
        cursor_y = 0;
        move_cursor();
}

// 屏幕输出一个字符(带颜色)
void console_putc_color(char c, real_color_t back, real_color_t fore)
{
        uint8_t back_color = (uint8_t)back;
        uint8_t fore_color = (uint8_t)fore;

        uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
        uint16_t attribute = attribute_byte << 8;

        // 0x08 是 退格键 的 ASCII 码
        // 0x09 是 tab 键 的 ASCII 码
        if (c == 0x08 && cursor_x) {
              cursor_x--;
        } else if (c == 0x09) {
              cursor_x = (cursor_x+8) & ~(8-1);
        } else if (c == '\r') {
              cursor_x = 0;
        } else if (c == '\n') {
                cursor_x = 0;
                cursor_y++;
        } else if (c >= ' ') {
                video_memory[cursor_y*CON_WIDTH + cursor_x] = c | attribute;
                cursor_x++;
        }

        // 每 80 个字符一行，满80就必须换行了
        if (cursor_x >= CON_WIDTH) {
                cursor_x = 0;
                cursor_y ++;
        }

        // 如果需要的话滚动屏幕显示
        scroll();

        // 移动硬件的输入光标
        move_cursor();
}

// 屏幕打印一个以 \0 结尾的字符串(默认黑底白字)
void console_write(char *cstr)
{
        while (*cstr) {
              console_putc_color(*cstr++, rc_black, rc_white);
        }
}

// 屏幕打印一个以 \0 结尾的字符串(带颜色)
void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
        while (*cstr) {
              console_putc_color(*cstr++, back, fore);
        }
}

