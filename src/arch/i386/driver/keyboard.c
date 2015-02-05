/*
 * =====================================================================================
 *
 *       Filename:  keyboard.c
 *
 *    Description:  键盘驱动
 *
 *        Version:  1.0
 *        Created:  2015年02月05日 13时33分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#include <common.h>

#include "keyboard.h"

/*
 * 我们定义的键盘当前状态信息(相关位被设置即为有效)
 *
 * 0: control
 * 1: alt
 * 2: alt-gr
 * 3: left shift
 * 4: right shift
 * 5: caps_lock
 * 6: scroll_lock
 * 7: num_lock
 */
#define CONTROL         0x1
#define ALT             0x2
#define ALTGR           0x4
#define LSHIFT          0x8
#define RSHIFT          0x10
#define CAPSLOCK        0x20
#define SCROLLLOCK      0x40
#define NUMLOCK         0x80

// 8 位的键盘扫描码的接通码使用前7位
// 其最高位置 1 即是其对应的断开码
// 该宏可以和获取的扫描码用来判断一个键是按下还是抬起
#define RELEASED_MASK 0x80

typedef uint8_t key_status_t;

typedef
struct keymap {
        uint8_t scancodes[128];         // 键盘扫描码的映射
        uint8_t capslock_scancodes[128];
        uint8_t shift_scancodes[128];
        uint8_t control_map[8];
        key_status_t controls;          // 键盘的控制状态信息
} keymap_t;

static keymap_t us = {
        //normal keys
        {
                /* first row - indices 0 to 14 */
                0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
                /* second row - indices 15 to 28 */
                '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', //Enter key
                /* 29 = Control, 30 - 41: third row */
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
                /* fourth row, indices 42 to 54, zeroes are shift-keys*/
                0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
                '*',
                /* Special keys */
                0,    // ALT - 56
                ' ', // Space - 57
                0,    // Caps lock - 58
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
                0, // Num lock - 69
                0, // Scroll lock - 70
                0, // Home - 71
                72, // Up arrow - 72  TODO
                0, // Page up - 73
                '-',
                0, // Left arrow - 75
                0,
                0, // Right arrow -77
                '+',
                0, // End - 79
                80, // Dowm arrow - 80  TODO
                0, // Page down - 81
                0, // Insert - 82
                0, // Delete - 83
                0, 0, 0,
                0, // F11 - 87
                0, // F12 - 88
                0, // All others undefined
        },
        // caps
        {
                /* first row - indices 0 to 14 */
                0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
                /* second row - indices 15 to 28 */
                '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
                /* 29 = Control, 30 - 41: third row */
                0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
                /* fourth row, indices 42 to 54, zeroes are shift-keys*/
                0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*',
                /* Special keys */
                0,   // ALT - 56
                ' ', // Space - 57
                0,   // Caps lock - 58
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
                0, // Num lock - 69
                0, // Scroll lock - 70
                0, // Home - 71
                0, // Up arrow - 72
                0, // Page up - 73
                '-',
                0, // Left arrow - 75
                0,
                0, // Right arrow -77
                '+',
                0, // End - 79
                0, // Dowm arrow - 80
                0, // Page down - 81
                0, // Insert - 82
                0, // Delete - 83
                0, 0, 0,
                0, // F11 - 87
                0, // F12 - 88
                0, // All others undefined
        },
        // shift
        {
                /* first row - indices 0 to 14 */
                0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
                /* second row - indices 15 to 28 */
                '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
                /* 29 = Control, 30 - 41: third row */
                0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
                /* fourth row, indices 42 to 54, zeroes are shift-keys*/
                0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
                /* Special keys */
                0,   // ALT - 56
                ' ', // Space - 57
                0,   // Caps lock - 58
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
                0, // Num lock - 69
                0, // Scroll lock - 70
                0, // Home - 71
                0, // Up arrow - 72
                0, // Page up - 73
                '-',
                0, // Left arrow - 75
                0,
                0, // Right arrow -77
                '+',
                0, // End - 79
                0, // Dowm arrow - 80
                0, // Page down - 81
                0, // Insert - 82
                0, // Delete - 83
                0, 0, 0,
                0, // F11 - 87
                0, // F12 - 88
                0, // All others undefined
        },
        // control_map
        {
                29, // Ctrl
                56, // Alt
                0,  // AltGr
                42, // left Shift
                54, // right Shift
                58, // Caps lock
                70, // Scroll lock
                69  // Num lock
        },
        // 键盘的控制键信息初始化为 0 
        0
};

// 当前使用的扫描码集
keymap_t *current_layout = &us;

// 键盘输入的缓冲区队列
static uint8_t keyboard_buffer[256];

// 键盘输入缓冲区队列的头尾标记
static uint32_t keyboard_buffer_start = 0;
static uint32_t keyboard_buffer_end = 0;

void keyboard_init(void)
{
        // 注册键盘中断处理函数
        register_interrupt_handler(IRQ1, &keyboard_handler);
}

uint8_t keyboard_getchar(void)
{
        // 队列中有数据则取出，否则返回 0
        if (keyboard_buffer_start != keyboard_buffer_end) {
                char ch = keyboard_buffer[keyboard_buffer_start++];
                keyboard_buffer_start %= 256;
                return ch;
        } else {
                return 0;
        }
}

void keyboard_handler(__attribute__((unused))pt_regs_t *regs)
{
        // 从键盘端口读入按下的键
        uint8_t scancode = inb(0x60);

        // 首先判断是按下还是抬起
        if (scancode & RELEASED_MASK) {
                uint32_t i;
                // 我们只检查前 5 个控制键，因为前五位 Ctrl Alt Shift 松开后不保留状态
                // 所以这些按键松开后必须清除按下标记
                for (i = 0; i < 5; i++) {
                        if(current_layout->control_map[i] == (scancode & ~RELEASED_MASK)) {
                                current_layout->controls &= ~(1 << i);
                                return;
                        }
                }
        // 当键被按下
        } else {
                uint32_t i;
                // 逐一检查各个控制位
                for (i = 0; i < 8; i++) {
                        // 如果当前键是控制键，则给相关控制位置 1
                        // 如果已有该标志位，则给相关控制位清 0
                        if (current_layout->control_map[i] == scancode) {
                                if (current_layout->controls & 1 << i) {
                                      current_layout->controls &= ~(1 << i);
                                } else {
                                      current_layout->controls |= (1 << i);
                                }
                                return;
                        }
                }
                uint8_t *scancodes = current_layout->scancodes;
        
                // 如果此时按下了 shift 键，切换到 shift 扫描码
                if ((current_layout->controls & (LSHIFT | RSHIFT)) && !(current_layout->controls & CONTROL)) {
                      scancodes = current_layout->shift_scancodes;
                }
                // 如果此时处于大写锁定状态，切换到大写锁定的扫描码
                if ((current_layout->controls & (CAPSLOCK)) && !(current_layout->controls & CONTROL)) {
                      scancodes = current_layout->capslock_scancodes;
                }
                // 如果队列不满则字符入队，否则丢弃
                if (keyboard_buffer_end != keyboard_buffer_start - 1) {
                        keyboard_buffer[keyboard_buffer_end++] = scancodes[scancode];
                        keyboard_buffer_end %= 256;
                }
        }
}


