/**
 * 
 * 系统全局内存检查头文件
 * 
 */

#ifndef _CHKCPMM_H
#define _CHKCPMM_H

#include "ldrtype.h"

void init_mem(machbstart_t* mbsp);
void init_krlinitstack(machbstart_t* mbsp);
void init_meme820(machbstart_t* mbsp);
void init_chkcpu(machbstart_t* mbsp);

int chk_cpuid();
int chk_cpu_longmode();

/// @brief 
void mmap(e820map_t** retemp, u32_t* retemnr);

/// @brief 
e820map_t* chk_memsize(e820map_t* e8p, u32_t enr, u64_t sadr, u64_t size);

/// @brief 根据e820map_t结构体数组, 计算总的可用内存大小
u64_t get_memsize(e820map_t* e8p, u32_t enr);

void init_chkmm();
void out_char(char* c);
void init_bstartpages(machbstart_t* mbsp);
void ldr_createpage_and_open();

#define CLT_HALT() __asm__ __volatile__("cli; hlt": : :"memory");

#endif