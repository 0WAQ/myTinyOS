/**********************************************************
        内核虚拟地址空间头文件krlvadrsmem_t.h
**********************************************************/
#ifndef _KRLVADRSMEM_T_H
#define _KRLVADRSMEM_T_H

#define RET4PT_PML4EVDR (1)
#define RET4PT_PDPTEVDR (2)
#define RET4PT_PDETEVDR (3)
#define RET4PT_PTETEVDR (4)
#define RET4PT_PFAMEPDR (5)

#define VMAP_MIN_SIZE (MSA_SIZE)

#define KMBOX_CACHE_MAX (0x1000)
#define KMBOX_CACHE_MIN (0x40)

typedef struct KVMCOBJMGR
{
	spinlock_t kom_lock;
	u32_t kom_flgs;
	uint_t kom_kvmcobjnr;
	list_h_t kom_kvmcohead;
	uint_t kom_kvmcocahenr;
	list_h_t kom_kvmcocahe;
	uint_t kom_kvmcodelnr;
	list_h_t kom_kvmcodelhead;
}kvmcobjmgr_t;

typedef struct KVMEMCOBJ
{
	list_h_t kco_list;
	spinlock_t kco_lock;
	u32_t kco_cont;
	u32_t kco_flgs;
	u32_t kco_type;
	uint_t kco_msadnr;
	list_h_t kco_msadlst;
	void* kco_filenode;
	void* kco_pager;
	void* kco_extp;
}kvmemcobj_t;


// 页面盒子的头
// 将页面盒子看作链表，那么该结构就是链表头，用于指明页民盒子这个链表
typedef struct KVMEMCBOXMGR 
{
	list_h_t kbm_list;
	spinlock_t kbm_lock;
	u64_t kbm_flgs;
	u64_t kbm_stus;	
	uint_t kbm_kmbnr;		// 页面盒子的数量
	list_h_t kbm_kmbhead;	// 链表，挂载页面盒子

	uint_t kbm_cachenr;		// 缓存空闲的页面盒子的个数
	uint_t kbm_cachemax;	// 最大缓存个数，超过了就要释放
	uint_t kbm_cachemin;	// 最小缓存个数
	list_h_t kbm_cachehead;
	void* kbm_ext;
}kvmemcboxmgr_t;

#define KMV_TEXT_TYPE 1
#define KMV_DATA_TYPE 2
#define KMV_BSS_TYPE 4
#define KMV_HEAP_TYPE 8
#define KMV_STACK_TYPE 16
#define KMV_BIN_TYPE 64

#define THREAD_HEAPADR_START 0x100000000

// 页面盒子
// 在实际使用虚拟地址区间时，将物理页面挂载到该结构
// 为什么要将该结构和虚拟地址区间(kmvarsdsc_t)分离？
// 		例如共享库，可能有多个虚拟地址区间使用相同的某些物理页面
typedef struct KVMEMCBOX 
{
	list_h_t kmb_list;		 // 链表，连接同类型结构三
	spinlock_t kmb_lock;
	refcount_t kmb_cont;	 // 共享的计数器
	u64_t kmb_flgs;
	u64_t kmb_stus;
	u64_t kmb_type;
	uint_t kmb_msanr;		 // 挂载的msadsc_t的数量
	list_h_t kmb_msalist;	 // 链表，挂载msadsc_t
	kvmemcboxmgr_t* kmb_mgr; // 指向上层结构
	void* kmb_filenode;		 // 指向文件节点描述符
	void* kmb_pager;		 // 指向分页器
	void* kmb_ext;
}kvmemcbox_t;

typedef struct VASLKNODE
{
	u32_t  vln_color;
	u32_t  vln_flags;
	void*  vln_left;
	void*  vln_right;
	void*  vln_prev;
	void*  vln_next;
}vaslknode_t;

typedef struct TESTSTC
{
	list_h_t tst_list;
	adr_t    tst_vadr;
	size_t   tst_vsiz;
	uint_t   tst_type;
	uint_t   tst_lime;
}teststc_t;

typedef struct PGTABPAGE
{
	spinlock_t ptp_lock;
	list_h_t   ptp_msalist;
	uint_t     ptp_msanr;
}pgtabpage_t;


// 在64位下，地址空间为: 0x0000_0000_0000_0000 ~ 0xFFFF_FFFF_FFFF_FFFF
// 其中：
// 		0x0000_0000_0000_0000 ~ 0x0000_7FFF_FFFF_FFFF 为用户空间
// 		0x0000_8000_0000_0000 ~ 0xFFFF_7FFF_FFFF_FFFF 为保留地址空间
//  	0xFFFF_8000_0000_0000 ~ 0xFFFF_FFFF_FFFF_FFFF 为内核空间
//		从内核空间的起始位置到 0xFFFF_8004_0000_0000 为线性映射区

// 表示虚拟地址区间的结构
// 例如，该结构可以表示用户空间中的代码区，内核空间的线性映射区等
typedef struct KMVARSDSC
{
	spinlock_t kva_lock;		// 保护自身的锁
	u32_t  kva_maptype;			// 映射类型
	list_h_t kva_list;			// 链表
	u64_t  kva_flgs;			// 相关状态
	u64_t  kva_limits;
	vaslknode_t kva_lknode;
	void*  kva_mcstruct;		// 指向它的上层结构

	// 当每个区的大小变化时，只需调整指针即可
	adr_t  kva_start;			// 虚拟地址的开始
	adr_t  kva_end;				// 虚拟地址的结束

	kvmemcbox_t* kva_kvmbox;	// 指向页面盒子，即用于挂载该虚拟地址区间所对应的物理页面
	void*  kva_kvmcobj;
}kmvarsdsc_t;

typedef struct s_MMADRSDSC mmadrsdsc_t;

// 表示虚拟地址空间的结构
typedef struct s_VIRMEMADRS
{
	spinlock_t vs_lock;
	u32_t  vs_resalin;
	list_h_t vs_list;	// 链表，连接虚拟地址区间，即上面的结构体
	uint_t vs_flgs;
	uint_t vs_kmvdscnr;	// 连接的虚拟地址区间的数量
	
	mmadrsdsc_t* vs_mm; // 指向它的上层数据结构，进程虚拟内存地址空间

	// 类似于C++ STL中deque的结构
	kmvarsdsc_t* vs_startkmvdsc;	// 开始的虚拟地址区间结构
	kmvarsdsc_t* vs_endkmvdsc;		// 结束的虚拟地址区间结构
	kmvarsdsc_t* vs_currkmvdsc;		// 进程当前正在使用的虚拟地址区间

	kmvarsdsc_t* vs_krlmapdsc;
	kmvarsdsc_t* vs_krlhwmdsc;
	kmvarsdsc_t* vs_krlolddsc;
	
	kmvarsdsc_t* vs_heapkmvdsc;
	kmvarsdsc_t* vs_stackkmvdsc;

	adr_t vs_isalcstart;	// 能分配的虚拟地址的开始
	adr_t vs_isalcend;		// 能分配的虚拟地址的结束
	void* vs_privte;
	void* vs_ext;
}virmemadrs_t;

typedef struct KVIRMEMADRS
{
	spinlock_t kvs_lock;
	u64_t  kvs_flgs;
	uint_t kvs_kmvdscnr;
	kmvarsdsc_t* kvs_startkmvdsc;
	kmvarsdsc_t* kvs_endkmvdsc;	
	kmvarsdsc_t* kvs_krlmapdsc;
	kmvarsdsc_t* kvs_krlhwmdsc;
	kmvarsdsc_t* kvs_krlolddsc;
	adr_t kvs_isalcstart;
	adr_t kvs_isalcend;
	void* kvs_privte;
	void* kvs_ext;
	list_h_t kvs_testhead;
	uint_t   kvs_tstcnr;
	uint_t   kvs_randnext;
	pgtabpage_t kvs_ptabpgcs;
	kvmcobjmgr_t kvs_kvmcomgr;
	kvmemcboxmgr_t kvs_kvmemcboxmgr;
}kvirmemadrs_t;

typedef struct s_THREAD thread_t;


// 进程直接拥有的结构，是对虚拟地址空间的封装
typedef struct s_MMADRSDSC
{
	spinlock_t msd_lock;
	list_h_t msd_list;
	uint_t msd_flag;
	uint_t msd_stus;
	uint_t msd_scount;		// 计数，该结构可能被共享
	thread_t* msd_thread;
	sem_t  msd_sem;					// 信号量
	mmudsc_t msd_mmu;				// MMU相关的信息

	virmemadrs_t msd_virmemadrs;	// 虚拟地址空间
	
	// 代码区的起始
	adr_t msd_stext;
	adr_t msd_etext;
	
	// 数据区的起始
	adr_t msd_sdata;
	adr_t msd_edata;
	
	// bss区的起始
	adr_t msd_sbss;
	adr_t msd_ebss;
	
	// 堆区的起始
	adr_t msd_sbrk;
	adr_t msd_ebrk;
}mmadrsdsc_t;


#define VADSZ_ALIGN(x) ALIGN(x,0x1000)
#define KVMCOBJ_FLG_DELLPAGE (1)
#define KVMCOBJ_FLG_UDELPAGE (2)

#endif