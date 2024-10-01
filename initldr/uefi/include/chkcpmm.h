/**
 * 
 * 系统全局内存检查头文件
 * 
 */

#ifndef _CHKCPMM_H
#define _CHKCPMM_H

#include "base/ldrtype.h"

/// @brief 内存相关
/// @param mbsp 机器信息结构
void init_mem(machbstart_t* mbsp);

/// @brief 初始化内核栈
/// @param mbsp 机器信息结构
void init_krlinitstack(machbstart_t* mbsp);

/// @brief
/// @param mbsp 机器信息结构
void init_meme820(machbstart_t* mbsp);

/// @brief
/// @param mbsp 机器信息结构
void init_chkcpu(machbstart_t* mbsp);


/// @brief 获取内存布局信息
/// @param retemp e820map_t结构体数组的首地址
/// @param retemnr 结构体数组的元素个数
void mmap(e820map_t** retemp, u32_t* retemnr);

/// @brief 
/// @param e8p 
/// @param enr 
/// @param sadr 
/// @param size 
/// @return 
e820map_t* chk_memsize(e820map_t* e8p, uint_t enr, u64_t sadr, u64_t size);


/// @brief 根据e820map_t结构体数组, 计算总的可用内存大小
/// @param e8p e820map_t结构体数组
/// @param enr 数组元素个数
/// @return 等于0 ==> 数据不合法, 大于0 ==> 内存总大小
u64_t get_memsize(e820map_t* e8p, uint_t enr);

bool_t MemoryIsFree(uint_t start, uint_t enr);
uint_t LoaderAllocMemory(uint_t size);


void init_chkmm();
void out_char(char* c);

/// @brief 建立MMU页表数据
/// @param mbsp 机器信息结构
void init_bstartpages(machbstart_t* mbsp);

void ldr_createpage_and_open();

#define CLI_HALT() __asm__ __volatile__("cli; hlt": : :"memory");

#endif  // _CHKCPMM_H