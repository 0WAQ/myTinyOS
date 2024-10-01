/**
 * 
 * 
 * 
 */

#ifndef _GRAPH_H
#define _GRAPH_H

#include "base/ldrtype.h"

/// @brief 初始化图形模式
/// @param mbsp 机器信息结构 
void init_graph(machbstart_t* mbsp);

/// @brief 初始化图形数据结构
/// @param initp 图形数据结构, 存放在mbsp->mb_ghparm
void graph_t_init(graph_t* initp);

/// @brief 
/// @param vfar 
/// @return 
u32_t vfartolineadr(u32_t vfar);

void init_kinitfvram(machbstart_t* mbsp);
u32_t utf8_to_unicode(utf8_t* utfp, s32_t* retuib);

#endif  // _GRAPH_H