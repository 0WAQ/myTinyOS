/**
 * 
 *
 *   
 */

#ifndef _BSTARTPARM_H
#define _BSTARTPARM_H

#include "base/ldrtype.h"

/// @brief 负责管理 检查CPU模式, 收集内存信息, 设置内核栈, 设置内核字体, 建立内核MMU页表数据
void init_bstartparm();

/// @brief 用于初始化machbstart_t指针
/// @param initp 
void machbstart_t_init(machbstart_t* initp);

int adrzone_is_ok(u64_t sadr, u64_t slen, u64_t kadr, u64_t klen);
int chkadr_is_ok(machbstart_t* mbsp, u64_t chkadr, u64_t cksz);

#endif  // _BSTARTPARM_H