/*
 * =====================================================================================
 *
 *       Filename:  block_dev.h
 *
 *    Description:  块设备接口定义
 *
 *        Version:  1.0
 *        Created:  2015年02月07日 11时15分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_BLOCK_DEV_H_
#define INCLUDE_BLOCK_DEV_H_

#include <types.h>

typedef
enum io_type {
        IO_READ = 0,
        IO_WRITE = 1
} io_type_t;

typedef 
struct io_request {
        io_type_t io_type;       // IO操作类型
        uint32_t secno;          // 起始位置
        uint32_t nsecs;          // 读写数量
        void *buffer;            // 读写缓冲区
        uint32_t bsize;          // 缓冲区尺寸
} io_request_t;

// 块设备接口
typedef
struct block_dev {
        const char *name;               // 设备名称
        uint32_t block_size;            // 单位块大小
        struct block_ops {                              // 设备操作
                int (*init)(void);                      // 设备初始化
                bool (*device_valid)(void); 	        // 设备是否可用
                const char *(*get_desc)(void);          // 获取设备描述
                int (*get_nr_block)(void);              // 获得设备默认块数量
                int (*request)(io_request_t *);         // 设备操作请求
                int (*ioctl)(int, int);                 // 设备选项设置
        } ops;
        struct block_dev *next;                         // 块设备链
} block_dev_t;

// 全局块设备链表
extern block_dev_t *block_devs;

// 块设备初始化
void block_dev_init(void);

// 内核注册块设备
int add_block_dev(block_dev_t *bdev);

// IDE 设备结构
extern block_dev_t ide_main_dev;

#endif  // INCLUDE_BLOCK_DEV_H_
