/**
 * 
 * 输入/输出头文件io.h
 * 
 */

#ifndef _IO_H
#define _IO_H

#include "type.h"

/**
 * 
 * 基本输入输出函数
 * 
 */

/// @brief 将一个8/16位无符号整数写到指定的IO端口
/// @param port IO端口
/// @param val  待输出的值
KLINE void out_u8(const u16_t port, const u8_t val)
{
    // __asm__: 内嵌汇编
    // __volatile__: 表示不会被编译器优化
    __asm__ __volatile__("outb %1, %0\n"    // oubt指令: 将一个字节(val)写入port
             :                              // 输出操作数: 无
             : "dN"(port), "a"(val));       // 输入操作数: 将val和port赋值给al和dx
}

KLINE void out_u16(const u16_t port, const u16_t val)
{
    __asm__ __volatile__("outw %1, %0\n"
             :
             : "dN"(port), "a"(val));
}


/// @brief 从指定IO端口读取一个8/16位无符号整数
/// @param port IO端口
/// @return 
KLINE u8_t in_u8(const u16_t port)
{
    u8_t tmp;
    __asm__ __volatile__("inb %1, %0\n"     // inb指令: 从port端口读取一个字节
             : "=a"(tmp)                    // 输出操作数: 将读取的字节赋值给tmp
             : "dN"(port));                 // 输入操作数: 将port赋值给dx寄存器
    return tmp;
}

KLINE u16_t in_u16(const u16_t port)
{
    u16_t tmp;
    __asm__ __volatile__("inw %1, %0\n"
             : "=a"(tmp)
             : "dN"(port));
    return tmp;
}


/**
 * 
 * 内存操作函数
 * 
 */

/// @brief 同C标准函数中的memset
KLINE void memset(void* src, u8_t val, uint_t len)
{
    u8_t* s = src;
    for(size_t i = 0; i < len; i++) {
        s[i] = val;
    }
}

/// @brief 将src赋值到dest, 需要考虑src与dest可能会有重叠部分
/// @return 返回长度
KLINE sint_t m2mcopy(void* src, void* dest, sint_t len)
{
    if(src == NULL || dest == NULL) {
        return 0;
    }

    u8_t* s = src, *d = dest;

    // 若src与dest相同, 无需传送
    if(s == d) {
        return len;
    }

    // 若src比dest地址小
    if(s < d) {
        for(sint_t i = len - 1; i >= 0; i--)
            d[i] = s[i];
        return len;
    }

    // 若src比dest地址大
    for(sint_t i = 0; i < len; i++) 
        d[i] = s[i];
    return len;    
}

#endif