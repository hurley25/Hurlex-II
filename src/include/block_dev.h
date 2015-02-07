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

#include "io_request.h"

// 块设备接口
typedef
struct block_dev {
        const char *name;       		 // 块设备名称
        uint32_t block_size;                     // 单位块大小
        struct block_ops {                       // 块设备操作
                int (*init)(void);               // 设备初始化
                bool (*device_valid)(void); 	 // 设备是否可用
                const char *(*get_desc)(void);   // 获取设备描述
                int (*get_nr_block)(void);       // 获得设备默认块数量
                int (*request)(io_request_t *);  // 设备操作请求
        } ops;
} block_dev_t;

#endif  // INCLUDE_BLOCK_DEV_H_
