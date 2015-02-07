/*
 * =====================================================================================
 *
 *       Filename:  elf.h
 *
 *    Description:  elf 相关的定义
 *
 *        Version:  1.0
 *        Created:  2014年11月04日 11时00分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Qianyi.lh (liuhuan), qianyi.lh@alibaba-inc.com
 *        Company:  Alibaba-Inc Aliyun
 *
 * =====================================================================================
 */

#ifndef INCLUDE_ELF_H_
#define INCLUDE_ELF_H_

#include <types.h>
#include <mboot.h>

#define ELF32_ST_TYPE(i) ((i)&0xf)

// ELF 格式区段头
typedef
struct elf_section_header_t {
        uint32_t name;
        uint32_t type;
        uint32_t flags;
        uint32_t addr;
        uint32_t offset;
        uint32_t size;
        uint32_t link;
        uint32_t info;
        uint32_t addralign;
        uint32_t entsize;
} __attribute__((packed)) elf_section_header_t;

// ELF 格式符号
typedef
struct elf_symbol_t {
        uint32_t name;
        uint32_t value;
        uint32_t size;
        uint8_t  info;
        uint8_t  other;
        uint16_t shndx;
} __attribute__((packed)) elf_symbol_t;

// ELF 信息
typedef
struct elf_t {
        elf_symbol_t *symtab;
        uint32_t symtabsz;
        const char *strtab;
        uint32_t strtabsz;
} elf_t;

#endif  // INCLUDE_ELF_H_
