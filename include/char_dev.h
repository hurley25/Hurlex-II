/*
 * =====================================================================================
 *
 *       Filename:  char_dev.h
 *
 *    Description:  字符设备定义
 *
 *        Version:  1.0
 *        Created:  2015年02月07日 11时16分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_CHAR_DEV_H_
#define INCLUDE_CHAR_DEV_H_

#include <types.h>

// 字符设备接口
typedef
struct char_dev {
        const char *name;                               // 设备名称
        bool is_readable;                               // 设备是否可读
        bool is_writeable;                              // 设备是否可写
        struct char_ops {                               // 设备操作
                int (*init)(void);                      // 设备初始化
                bool (*device_valid)(void); 	        // 设备是否可用
                const char *(*get_desc)(void);          // 获取设备描述
                int (*read)(void *, uint32_t);          // 设备读取
                int (*write)(const void *, uint32_t);   // 设备写入
                int (*ioctl)(int, int);                 // 设备选项设置
        } ops;
        struct char_dev *next;                          // 字符设备链
} char_dev_t;

// 全局字符设备链表
extern char_dev_t *char_devs;

// 字符设备初始化
void char_dev_init(void);

// 内核注册字符设备
int add_char_dev(char_dev_t *cdev);

// Keyboard 设备结构
extern char_dev_t kboard_dev;

#endif  // INCLUDE_CHAR_DEV_H_
