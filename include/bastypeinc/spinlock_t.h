/**********************************************************
		自旋锁头文件spinlock.h
**********************************************************/
#ifndef _SPINLOCK_T_H
#define _SPINLOCK_T_H

typedef struct
{
	volatile u32_t lock;
} spinlock_t;

#endif
