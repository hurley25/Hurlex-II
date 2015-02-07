/*
 * =====================================================================================
 *
 *       Filename:  io_request.h
 *
 *    Description:  IO请求结构定义
 *
 *        Version:  1.0
 *        Created:  2015年02月07日 11时38分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_IO_REQUEST_H_
#define INCLUDE_IO_REQUEST_H_

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

#endif  // INCLUDE_IO_REQUEST_H_
