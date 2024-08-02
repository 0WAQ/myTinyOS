/**
 * 
 * 
 * 
 */

#ifndef _GRAPH_H
#define _GRAPH_H

#include "ldrtype.h"

/// @brief 初始化图形模式
/// @param mbsp 机器信息结构 
void init_graph(machbstart_t* mbsp);

/// @brief 初始化图形数据结构
/// @param initp 图形数据结构, 存放在mbsp->mb_ghparm
void graph_t_init(graph_t* initp);

/// @brief 获取VBE模式
/// @param mbsp 机器信息结构
void get_vbemode(machbstart_t* mbsp);

/// @brief 获取VBE模式的具体信息
/// @param mbsp 机器信息结构
void get_vbemodeinfo(machbstart_t* mbsp);

/// @brief 设置VBE模式 
void set_vbemodeinfo();


u32_t vfartolineadr(u32_t vfar);
void init_kinitfvram(machbstart_t* mbsp);
void bga_write_reg(u16_t index, u16_t data);
u16_t bag_read_reg(u16_t index);
u32_t get_bgadevice();
u32_t chk_bgamaxver();
void init_bgadevice(machbstart_t* mbsp);
u32_t utf8_to_unicode(utf8_t* utfp, s32_t* retulib);

#endif