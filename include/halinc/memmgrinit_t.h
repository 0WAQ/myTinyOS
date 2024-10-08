/**********************************************************
        物理内存管理器初始化文件memmgrinit_t.h
**********************************************************/
#ifndef _MEMMGRINIT_T_H
#define _MEMMGRINIT_T_H


// 用于存放内核的各种数据结构，是虚拟地址
typedef struct s_MEMMGROB
{
	list_h_t mo_list;
	spinlock_t mo_lock;		// 自旋锁
	uint_t mo_stus;			// 状态
	uint_t mo_flgs;			// 标志
	sem_t mo_sem;
	u64_t mo_memsz;			// 内存大小
	u64_t mo_maxpages;		// 内存最大页面数
	u64_t mo_freepages;		// 内存最大空闲页面数
	u64_t mo_alocpages;		// 内存最大分配页面数
	u64_t mo_resvpages;		// 内存保留页面数
	u64_t mo_horizline;		// 内存分配水位线
	phymmarge_t* mo_pmagestat;	// 内存空间布局结构指针
	u64_t mo_pmagenr;
	msadsc_t* mo_msadscstat;	// 内存页面结构指针
	u64_t mo_msanr;
	memarea_t* mo_mareastat;	// 内存区结构指针
	u64_t mo_mareanr;
	kmsobmgrhed_t mo_kmsobmgr;
	void* mo_privp;
	void* mo_extp;
}memmgrob_t;

#endif