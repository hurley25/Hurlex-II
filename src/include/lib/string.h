/*
 * =====================================================================================
 *
 *       Filename:  string.h
 *
 *    Description:  字符串相关函数
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 11时02分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_LIB_STRING_H_
#define INCLUDE_LIB_STRING_H_

#include <types.h>

void memcpy(void *dest, void *src, uint32_t len);

void memset(void *dest, uint8_t val, uint32_t len);

void bzero(void *dest, uint32_t len);

int strcmp(const char *str1, const char *str2);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, uint32_t len);

char *strcat(char *dest, const char *src);

int strlen(const char *src);

#endif  // INCLUDE_LIB_STRING_H_
