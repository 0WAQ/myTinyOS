/**
 * 
 * 基本类型文件type.h
 * 
 */

#ifndef _TYPE_H
#define _TYPE_H

/**
 * 
 * 基本数据类型
 * 
 */
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long uint_t;
typedef unsigned long long u64_t;

typedef char s8_t;
typedef short s16_t;
typedef int s32_t;
typedef long sint_t;
typedef long long s64_t;

typedef char char_t;
typedef const char* str_t;


/**
 * 
 * 特殊数据类型
 * 
 */
typedef u64_t cpuflg_t;

typedef uint_t adr_t;
typedef uint_t size_t;
typedef uint_t drv_t;
typedef uint_t mrv_t;

typedef sint_t drvstus_t;

typedef u64_t dev_t;
typedef u64_t bool_t;


/**
 *
 * 回调函数类型
 * 
 */
typedef void (*inthandler)();
typedef drv_t (*i_handler_t)(uint_t int_nr);
typedef drv_t (*f_handler_t)(uint_t int_nr, void* sframe);
typedef void (*syscall_t)();
typedef drv_t (*dev_op_t)();
typedef mrv_t (*msg_op_t)();
typedef void* exception_t;
typedef char_t* va_list_t;


/**
 * 
 * 特殊属性
 * 
 */
#define KLINE static inline

#define public
#define private static
#define EXTERN extern
#define KEXTERN extern

#define NULL  0
#define TRUE  1
#define FALSE 0

#define REGCALL __attribute__((regparm(3)))

#define ALIGN(x, a)     (((x) + (a) - 1) & ~((a) - 1))
#define P4K_ALIGN(x) ALIGN(x, 0x1000)

#endif
