/*
 * =====================================================================================
 *
 *       Filename:  console.h
 *
 *    Description:  屏幕输出函数
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 10时49分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_CONSOLE_H_
#define INCLUDE_CONSOLE_H_

#include <types.h>

typedef
enum real_color {
        rc_black = 0,
        rc_blue = 1,
        rc_green = 2,
        rc_cyan = 3,
        rc_red = 4,
        rc_magenta = 5,
        rc_brown = 6,
        rc_light_grey = 7,
        rc_dark_grey = 8,
        rc_light_blue = 9,
        rc_light_green = 10,
        rc_light_cyan = 11,
        rc_light_red = 12,
        rc_light_magenta = 13,
        rc_light_brown  = 14,
        rc_white = 15
} real_color_t;

// 屏幕显示初始化
void console_init(void);

// 清屏操作
void console_clear(void);

// 屏幕打印一个以 \0 结尾的字符串(默认黑底白字)
void console_write(char *cstr);

// 屏幕打印一个以 \0 结尾的字符串(带颜色)
void console_write_color(char *cstr, real_color_t back, real_color_t fore);

// 屏幕显示向上移动n行
void console_view_up(uint16_t offset);

// 屏幕显示向下移动n行
void console_view_down(uint16_t offset);

#endif  // INCLUDE_CONSOLE_H_
