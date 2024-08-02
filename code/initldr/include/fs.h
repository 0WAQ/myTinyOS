/**
 * 
 * 
 *  
 */

#ifndef _FS_H
#define _FS_H

#include "base/ldrtype.h"

// int strcmpl(const char* a, const char* b);

/// @brief 判断指定的内存空间是否与 mbsp 中的内存空间有冲突
/// @param mbsp 机器信息结构
/// @param adr 指定的内存空间首地址
/// @param sz 内存空间的大小
int move_krlimg(machbstart_t* mbsp, u64_t adr, u64_t sz);

/// @brief 放置内核文件与字库文件
/// @param mbsp 机器信息结构
void init_krlfile(machbstart_t* mbsp);
void init_defutfont(machbstart_t* mbsp);

fhdsc_t* get_fileinfo(char_t* fname, machbstart_t* mbsp);
void get_file_rpadrandsz(char_t* fname, machbstart_t* mbsp, u32_t retadr, u32_t* retsz);
u64_t get_filesz(char_t* filenm, machbstart_t* mbsp);
u64_t get_wt_imgfilesz(machbstart_t* mbsp);

/// @brief 从映像文件中找出对应的文件, 将其放入特定的内存中
/// @param mbsp 机器信息结构
/// @param f2adr 指定的内存
/// @param fnm 对应的文件名
/// @return 文件大小
u64_t r_file_to_padr(machbstart_t* mbsp, u32_t f2adr, char_t* fnm);

void set_rwhdhdpack(rwhdpack_t* setp, u8_t sn, u16_t off, u16_t seg, u32_t lbal, u32_t lbah);
void* read_sector(u64_t sector);
void ret_mbr();
int is_part(dpt_t* isp);
u64_t ret_imgfilesz();

#endif