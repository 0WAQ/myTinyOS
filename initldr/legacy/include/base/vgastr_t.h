/**
 * 
 * 转换显示字符串头文件vgastr.h的类型vgastr_t.h
 * 
 */

#ifndef _VGASTR_T_H
#define _VGASTR_T_H

#include "type.h"

#define VGASTR_RAM_BASE (0xb8000)   // 显存段基地址
#define VGASTR_RAM_END  (0xbffff)   // 显存段结束地址
#define VGADP_DFVL  (0x0700)
#define VGADP_HLVL  (0x0f00)
#define VGACTRL_REG_ADR 0x03d4
#define VGACTRL_REG_DAT 0x03d5
#define VGACURS_REG_INX 0x0a
#define VGACURS_CLOSE   0x20
#define VGACHAR_LR_CFLG 10
#define VGACHAR_DF_CFLG 0
#define VGASADH_REG_INX 0x0c
#define VGASADL_REG_INX 0x0d

typedef struct s_CURSOR
{
    uint_t vmem_s;
    uint_t vmem_e;
    uint_t cvmem_adr;

    uint_t x;
    uint_t y;
}cursor_t;

#endif  // _VGASTR_T_H