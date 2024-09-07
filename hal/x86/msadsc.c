/**********************************************************
        物理内存空间数组文件msadsc.c
**********************************************************/
#include "cosmostypes.h"
#include "cosmosmctrl.h"

// 初始化内存页结构(即初始化msdasc_t结构体中的变量)
void init_msadsc()
{
	u64_t coremdnr = 0;
	u64_t msadscnr = 0;					// 数组元素个数
	msadsc_t *msadscvp = NULL;			// 数组首地址
	machbstart_t *mbsp = &kmachbsp;

// 计算msadsc_t数组的开始地址和数组元素个数
	if (ret_msadsc_vadrandsz(mbsp, &msadscvp, &msadscnr) == FALSE)
		system_error("init_msadsc ret_msadsc_vadrandsz err\n");

// 初始化msadsc_t数组
	coremdnr = init_msadsc_core(mbsp, msadscvp, msadscnr);
	if (coremdnr != msadscnr)
		system_error("init_msadsc init_msadsc_core err\n");

// 将msadsc_t数组的相关信息写入全局machbstart中

	// 开始物理地址
	mbsp->mb_memmappadr = viradr_to_phyadr((adr_t)msadscvp);

	// 元素个数
	mbsp->mb_memmapnr = coremdnr;
	
	// 数组大小
	mbsp->mb_memmapsz = coremdnr * sizeof(msadsc_t);
	
	// 计算下一个空闲内存的起始地址
	mbsp->mb_nextwtpadr = PAGE_ALIGN(mbsp->mb_memmappadr + mbsp->mb_memmapsz);
}

u64_t init_msadsc_core(machbstart_t *mbsp, msadsc_t *msavstart, u64_t msanr)
{
	/**
	 * 扫描 phymmarge_t 数组, 若内存可用, 就建立一个msadsc_t
	 */

	// 获取phymmarge_t数组
	phymmarge_t *pmagep = (phymmarge_t *)phyadr_to_viradr((adr_t)mbsp->mb_e820expadr);

	u64_t mdidx = 0;

	// 遍历phymmarge数组
	for (u64_t i = 0; i < mbsp->mb_e820exnr; i++)
	{
		// 判断是不是可用内存
		if (pmagep[i].pmr_type == PMR_T_OSAPUSERRAM)
		{
			// 遍历这一块地址
			for (u64_t start = pmagep[i].pmr_saddr; start < pmagep[i].pmr_end; start += 4096)
			{
				// 若这块地址大小足够 1 个页(4KB), 就创建一个msadsc
				if (start + 4096 - 1 <= pmagep[i].pmr_end)
				{
					write_one_msadsc(&msavstart[mdidx], start);
					mdidx++;
				}
			}
		}
	}

	return mdidx;
}

// 创建一个msadsc
void write_one_msadsc(msadsc_t *msap, u64_t phyadr)
{
	// 初始化msadsc
	msadsc_t_init(msap);

	// 将64位物理地址转换为phyadrflgs
	phyadrflgs_t *tmp = (phyadrflgs_t *)(&phyadr);

	msap->md_phyadrs.paf_padrs = tmp->paf_padrs;
}

void msadsc_t_init(msadsc_t *initp)
{
	// 初始化链表和锁
	list_init(&initp->md_list);
	knl_spinlock_init(&initp->md_lock);

	initp->md_indxflgs.mf_olkty = MF_OLKTY_INIT;
	initp->md_indxflgs.mf_lstty = MF_LSTTY_LIST;
	initp->md_indxflgs.mf_mocty = MF_MOCTY_FREE;
	initp->md_indxflgs.mf_marty = MF_MARTY_INIT;
	initp->md_indxflgs.mf_uindx = MF_UINDX_INIT;
	initp->md_phyadrs.paf_alloc = PAF_NO_ALLOC;
	initp->md_phyadrs.paf_shared = PAF_NO_SHARED;
	initp->md_phyadrs.paf_swap = PAF_NO_SWAP;
	initp->md_phyadrs.paf_cache = PAF_NO_CACHE;
	initp->md_phyadrs.paf_kmap = PAF_NO_KMAP;
	initp->md_phyadrs.paf_lock = PAF_NO_LOCK;
	initp->md_phyadrs.paf_dirty = PAF_NO_DIRTY;
	initp->md_phyadrs.paf_busy = PAF_NO_BUSY;
	initp->md_phyadrs.paf_rv2 = PAF_RV2_VAL;
	initp->md_phyadrs.paf_padrs = PAF_INIT_PADRS;
	initp->md_odlink = NULL;
}

// 通过machbstart计算msadsc_t数组的起始地址和元素个数
bool_t ret_msadsc_vadrandsz(machbstart_t *mbsp, msadsc_t **retmasvp, u64_t *retmasnr)
{
	if (NULL == mbsp || NULL == retmasvp || NULL == retmasnr) {
		return FALSE;
	}

	if (mbsp->mb_e820exnr < 1 || 
		mbsp->mb_e820expadr == NULL || 
		mbsp->mb_e820exnr * sizeof(phymmarge_t) != mbsp->mb_e820exsz)
	{
		*retmasvp = NULL;
		*retmasnr = 0;
		return FALSE;
	}

	phymmarge_t *pmagep = (phymmarge_t *)phyadr_to_viradr((adr_t)mbsp->mb_e820expadr);
	u64_t usrmemsz = 0, msadnr = 0;
	for (u64_t i = 0; i < mbsp->mb_e820exnr; i++)
	{
		if (PMR_T_OSAPUSERRAM == pmagep[i].pmr_type)
		{
			usrmemsz += pmagep[i].pmr_lsize;
			msadnr += (pmagep[i].pmr_lsize >> 12);
		}
	}

	if (0 == usrmemsz || (usrmemsz >> 12) < 1 || msadnr < 1)
	{
		*retmasvp = NULL;
		*retmasnr = 0;
		return FALSE;
	}

	// msadnr = usrmemsz >> 12;
	if (0 != initchkadr_is_ok(mbsp, mbsp->mb_nextwtpadr, (msadnr * sizeof(msadsc_t)))) {
		system_error("ret_msadsc_vadrandsz initchkadr_is_ok err\n");
	}

	*retmasvp = (msadsc_t *)phyadr_to_viradr((adr_t)mbsp->mb_nextwtpadr);
	*retmasnr = msadnr;

	return TRUE;
}

// 查找内核之前占用的内存页面，在页结构和内存区被初始化后调用
void init_search_krloccupymm(machbstart_t *mbsp)
{
	if (search_krloccupymsadsc_core(mbsp) == FALSE) {
		system_error("search_krloccupymsadsc_core fail\n");
	}
}

// 查找内核已占用内存页面
bool_t search_krloccupymsadsc_core(machbstart_t *mbsp)
{
	u64_t retschmnr = 0;
	msadsc_t *msadstat = (msadsc_t *)phyadr_to_viradr((adr_t)mbsp->mb_memmappadr);
	u64_t msanr = mbsp->mb_memmapnr;
	
	// 查找 0 ～ 0x1000(BIOS中断表) 所对应的msadsc_t结构
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, 0, 0x1000);
	if(retschmnr == 0) return FALSE;
	
	// 查找内核栈
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, mbsp->mb_krlinitstack & (~(0xfffUL)), mbsp->mb_krlinitstack);
	if(retschmnr == 0) return FALSE;
	
	// 查找内核自己占用的内存页
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, mbsp->mb_krlimgpadr, mbsp->mb_nextwtpadr);
	if(retschmnr == 0) return FALSE;
	
	// 查找内核映像文件
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, mbsp->mb_imgpadr, mbsp->mb_imgpadr + mbsp->mb_imgsz);
	if(retschmnr == 0) return FALSE;

	return TRUE;
}

// 查找一段内存空间所对应的 msadsc_t 结构
u64_t search_segment_occupymsadsc(msadsc_t *msastart, u64_t msanr, u64_t ocpystat, u64_t ocpyend)
{
	u64_t mphyadr = 0, fsmsnr = 0;
	msadsc_t *fstatmp = NULL; // 以之后找到的第一个页为首元素的一个数组

	// 在内存页中找出对应的起始地址
	for (u64_t mnr = 0; mnr < msanr; mnr++)
	{
		// 判断物理页地址是否相同
		if ((msastart[mnr].md_phyadrs.paf_padrs << PSHRSIZE) == ocpystat)
		{
			// 对应内存段中的第一个页
			fstatmp = &msastart[mnr];
			break;
		}
	}

// 找到 或者 遍历完所有页面后...
	// 若遍历完没有对应的页
	if (fstatmp == NULL)
		return 0;
 
	fsmsnr = 0;
	// 遍历内存段，找出所有的页
	for (u64_t tmpadr = ocpystat; tmpadr < ocpyend; tmpadr += PAGESIZE, fsmsnr++)
	{
		// 获取页的物理地址
		mphyadr = fstatmp[fsmsnr].md_phyadrs.paf_padrs << PSHRSIZE;
		if (mphyadr != tmpadr)
			return 0;

		if (MF_MOCTY_FREE != fstatmp[fsmsnr].md_indxflgs.mf_mocty ||
			0 != fstatmp[fsmsnr].md_indxflgs.mf_uindx ||
			PAF_NO_ALLOC != fstatmp[fsmsnr].md_phyadrs.paf_alloc)
		{
			return 0;
		}

		// 设置页的属性
		fstatmp[fsmsnr].md_indxflgs.mf_mocty = MF_MOCTY_KRNL;
		fstatmp[fsmsnr].md_indxflgs.mf_uindx++;
		fstatmp[fsmsnr].md_phyadrs.paf_alloc = PAF_ALLOC;
	}

	// 正确性检验
	u64_t ocpysz = ocpyend - ocpystat;
	if ((ocpysz & 0xfff) != 0) 
	{
		if (((ocpysz >> PSHRSIZE) + 1) != fsmsnr)
			return 0;
		return fsmsnr;
	}

	if ((ocpysz >> PSHRSIZE) != fsmsnr)
		return 0;
	
	return fsmsnr;
}

void disp_one_msadsc(msadsc_t *mp)
{
	kprint("msadsc_t.md_f:_ux[%x],_my[%x],md_phyadrs:_alc[%x],_shd[%x],_swp[%x],_che[%x],_kmp[%x],_lck[%x],_dty[%x],_bsy[%x],_padrs[%x]\n",
		   (uint_t)mp->md_indxflgs.mf_uindx, (uint_t)mp->md_indxflgs.mf_mocty, (uint_t)mp->md_phyadrs.paf_alloc, (uint_t)mp->md_phyadrs.paf_shared, (uint_t)mp->md_phyadrs.paf_swap, (uint_t)mp->md_phyadrs.paf_cache, (uint_t)mp->md_phyadrs.paf_kmap, (uint_t)mp->md_phyadrs.paf_lock,
		   (uint_t)mp->md_phyadrs.paf_dirty, (uint_t)mp->md_phyadrs.paf_busy, (uint_t)(mp->md_phyadrs.paf_padrs << 12));
}

void disp_phymsadsc()
{
	u64_t coremdnr = 0;
	msadsc_t *msadscvp = NULL;
	machbstart_t *mbsp = &kmachbsp;

	msadscvp = (msadsc_t *)phyadr_to_viradr((adr_t)mbsp->mb_memmappadr);
	coremdnr = mbsp->mb_memmapnr;

	for (int i = 0; i < 10; ++i)
		disp_one_msadsc(&msadscvp[i]);

	for (u64_t i = coremdnr / 2; i < coremdnr / 2 + 10; ++i)
		disp_one_msadsc(&msadscvp[i]);

	for (u64_t i = coremdnr - 10; i < coremdnr; ++i)
		disp_one_msadsc(&msadscvp[i]);
}

void test_schkrloccuymm(machbstart_t *mbsp, u64_t ocpystat, u64_t sz)
{
	msadsc_t *msadstat = (msadsc_t *)phyadr_to_viradr((adr_t)mbsp->mb_memmappadr);
	u64_t msanr = mbsp->mb_memmapnr;
	u64_t chkmnr = 0;
	u64_t chkadr = ocpystat;
	if ((sz & 0xfff) != 0) {
		chkmnr = (sz >> PSHRSIZE) + 1;
	}
	else {
		chkmnr = sz >> PSHRSIZE;
	}

	msadsc_t *fstatmp = NULL;
	for (u64_t mnr = 0; mnr < msanr; mnr++)
	{
		if ((msadstat[mnr].md_phyadrs.paf_padrs << PSHRSIZE) == ocpystat)
		{
			fstatmp = &msadstat[mnr];
			goto step1;
		}
	}
step1:
	if (fstatmp == NULL) {
		system_error("fstatmp NULL\n");
	}

	for (u64_t i = 0; i < chkmnr; i++, chkadr += PAGESIZE)
	{
		if (chkadr != fstatmp[i].md_phyadrs.paf_padrs << PSHRSIZE)
			system_error("chkadr != err\n");
		if (PAF_ALLOC != fstatmp[i].md_phyadrs.paf_alloc)
			system_error("PAF_ALLOC err\n");
		if (1 != fstatmp[i].md_indxflgs.mf_uindx)
			system_error("mf_uindx err\n");
		if (MF_MOCTY_KRNL != fstatmp[i].md_indxflgs.mf_mocty)
			system_error("mf_olkty err\n");
	}
	if (chkadr != (ocpystat + (chkmnr * PAGESIZE)))
		system_error("test_schkrloccuymm err\n");
}
