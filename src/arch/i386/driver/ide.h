/*
 * =====================================================================================
 *
 *       Filename:  ide.h
 *
 *    Description:  IDE 设备驱动
 *
 *        Version:  1.0
 *        Created:  2015年02月03日 14时40分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_IDE_H_
#define INCLUDE_IDE_H_

#include <block_dev.h>

// IDE 设备结构
extern block_dev_t ide_main_dev;

#endif  // INCLUDE_IDE_H_
