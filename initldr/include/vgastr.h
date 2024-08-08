/**
 * 
 * 转换显示字符串头文件vgastr.h
 * 
 */

#ifndef _VGASTR_H
#define _VGASTR_H

#include "base/vgastr_t.h"

/// @brief 初始化光标
void init_curs();

/// @brief 设置光标位置
/// @param x 
/// @param y 
void set_curs(u32_t x, u32_t y);

/// @brief 像当前光标(cursptr)对应的位置写入str
/// @param str 写入的字符串
/// @param cursptr 光标
void GxH_strwrite(char_t* str, cursor_t* cursptr);

char* numberk(char_t* str, uint_t n, sint_t base);

/// @brief 清空屏幕
/// @param srrv 值一般为0x0700, 即黑底白字无闪烁无高亮, 无任何字符
void clear_screen(u16_t srrv);

/// @brief 
/// @param cr 
/// @param x 
/// @param y 
void put_one_char(char_t cr, uint_t x, uint_t y);

/// @brief 关闭光标
void close_curs();

/// @brief 解析可变参数列表
/// @param buf 最终形成的字符串
/// @param fmt 参数格式
/// @param args 参数列表
void vsprintfk(char_t* buf, const char_t* fmt, va_list_t args);

/// @brief 向屏幕上打印
/// @param fmt 参数格式
/// @param ... 参数列表
void kprint(const char_t* fmt, ...);


KLINE void current_curs(cursor_t* cp, u32_t c_flg)
{
    if(c_flg == VGACHAR_LR_CFLG)
    {
        cp->y++;
        cp->x = 0;
        if(cp->y > 24) {
            cp->y = 0;
            clear_screen(VGADP_DFVL);
        }
    }
    else if(c_flg == VGACHAR_DF_CFLG)
    {
        cp->x += 2;
        if(cp->x > 159)
        {
            cp->x = 0;
            cp->y++;
            if(cp->y > 24) {
                cp->y = 0;
                clear_screen(VGADP_DFVL);
            }
        }
    }
    return;
}

#endif
