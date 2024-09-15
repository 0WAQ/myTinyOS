/**********************************************************
        物理内存区间文件memarea_t.h
**********************************************************/
#ifndef _MEMAREA_T_H
#define _MEMAREA_T_H

#define MMSTUS_ERR (0)
#define MMSTUS_OK (1)

typedef struct s_ARCLST
{
	list_h_t al_lru1;
	list_h_t al_lru2;
	uint_t al_lru1nr;
	uint_t al_lru2nr;
}arclst_t;

typedef struct s_MMAFRETS
{
	msadsc_t* mat_fist;
	uint_t mat_sz;
	uint_t mat_phyadr;
	u16_t mat_runmode;
	u16_t mat_gen;
	u32_t mat_mask;
}__attribute__((packed)) mmafrets_t;

struct s_MEMAREA;
typedef struct s_MAFUNCOBJS
{
	mmstus_t (*mafo_init)(struct s_MEMAREA* memarea, void* valp, uint_t val);
	mmstus_t (*mafo_exit)(struct s_MEMAREA* memarea);
	mmstus_t (*mafo_aloc)(struct s_MEMAREA* memarea, mmafrets_t* mafrspack, void* valp, uint_t val);
	mmstus_t (*mafo_free)(struct s_MEMAREA* memarea, mmafrets_t* mafrspack, void* valp, uint_t val);
	mmstus_t (*mafo_recy)(struct s_MEMAREA* memarea, mmafrets_t* mafrspack, void* valp, uint_t val);

}mafuncobjs_t;

#define BAFH_STUS_INIT 0
#define BAFH_STUS_ONEM 1
#define BAFH_STUS_DIVP 2
#define BAFH_STUS_DIVM 3

// 组织内存页(msadsc_t)的数据结构
typedef struct s_BAFHLST
{
	spinlock_t af_lock;		// 自旋锁
	u32_t af_stus;			// 状态
	uint_t af_oder;			// 页面数的偏移量
	uint_t af_oderpnr;		// oder对应的连续页面数，例如，oder为2，那么 1 << 2 = 4
	uint_t af_fobjnr;		// 空闲页面数
	//uint_t af_aobjnr;
	uint_t af_mobjnr;		// 当前总页面数
	uint_t af_alcindx;		// 分配计数
	uint_t af_freindx;		// 释放计数
	list_h_t af_frelst;		// 挂载此结构的空闲msadsc_t结构
	list_h_t af_alclst;		// 挂在此结构的已经分配的msadsc_t结构
	list_h_t af_ovelst;
}bafhlst_t;

#define MDIVMER_ARR_LMAX 52
#define MDIVMER_ARR_BMAX 11
#define MDIVMER_ARR_OMAX 9

// 组织(bafhlst_t)的数据结构
typedef struct s_MEMDIVMER
{
	spinlock_t dm_lock;
	u32_t dm_stus;
	uint_t dm_dmmaxindx;
	uint_t dm_phydmindx;
	uint_t dm_predmindx;
	uint_t dm_divnr;		// 内存分割(分配)次数
	uint_t dm_mernr;		// 内存合并(释放)次数
	//bafhlst_t dm_mdmonelst[MDIVMER_ARR_OMAX];
	//bafhlst_t dm_mdmblklst[MDIVMER_ARR_BMAX];
	bafhlst_t dm_mdmlielst[MDIVMER_ARR_LMAX];	// 结构体数组，每个元素挂载的msadsc_t的数量是其下标左移一位
	bafhlst_t dm_onemsalst;						// 单个的结构体
}memdivmer_t;

#define MA_TYPE_INIT 0
#define MA_TYPE_HWAD 1
#define MA_TYPE_KRNL 2
#define MA_TYPE_PROC 3
#define MA_TYPE_SHAR 4
#define MEMAREA_MAX 4
#define MA_HWAD_LSTART 0
#define MA_HWAD_LSZ 0x2000000
#define MA_HWAD_LEND (MA_HWAD_LSTART+MA_HWAD_LSZ-1)
#define MA_KRNL_LSTART 0x2000000
#define MA_KRNL_LSZ (0x400000000-0x2000000)
#define MA_KRNL_LEND (MA_KRNL_LSTART+MA_KRNL_LSZ-1)
#define MA_PROC_LSTART 0x400000000
#define MA_PROC_LSZ (0xffffffffffffffff-0x400000000)
#define MA_PROC_LEND (MA_PROC_LSTART+MA_PROC_LSZ)
//0x400000000  0x40000000

// 内存分区，逻辑上分为硬件区、内核区、应用区
typedef struct s_MEMAREA
{
	list_h_t ma_list;			// 链表
	spinlock_t ma_lock;			// 自旋锁
	uint_t ma_stus;				// 状态
	uint_t ma_flgs;				// 标志
	uint_t ma_type;				// 类型
	sem_t ma_sem;				// 信号量
	wait_l_head_t ma_waitlst;	// 等待队列
	uint_t ma_maxpages;			// 总页面数
	uint_t ma_allocpages;		// 分配的页面数
	uint_t ma_freepages;		// 空闲的页面数
	uint_t ma_resvpages;		// 保留的页面数
	uint_t ma_horizline;		// 分配时的水位线
	adr_t ma_logicstart;		// 开始地址
	adr_t ma_logicend;			// 结束地址
	uint_t ma_logicsz;			// 大小
	adr_t ma_effectstart;
	adr_t ma_effectend;
	uint_t ma_effectsz;
	list_h_t ma_allmsadsclst;
	uint_t ma_allmsadscnr;
	arclst_t ma_arcpglst;
	mafuncobjs_t ma_funcobj;
	memdivmer_t ma_mdmdata;
	void* ma_privp;
	/*
	*这个结构至少占用一个页面，当然
	*也可以是多个连续的的页面，但是
	*该结构从第一个页面的首地址开始
	*存放，后面的空间用于存放实现分
	*配算法的数据结构，这样每个区可
	*方便的实现不同的分配策略，或者
	*有天你觉得我的分配算法是渣渣，
	*完全可以替换mafuncobjs_t结构
	*中的指针，指向你的函数。
	*/
}memarea_t;

#endif