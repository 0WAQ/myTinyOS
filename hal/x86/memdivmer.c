/**********************************************************
        物理内存分割合并文件memdivmer.c
**********************************************************/
#include "cosmostypes.h"
#include "cosmosmctrl.h"

// 更新内存管理器的页面数
void mm_update_memmgrob(uint_t realpnr, uint_t flgs)
{
	cpuflg_t cpuflg;
	if (flgs == 0)
	{
		knl_spinlock_cli(&memmgrob.mo_lock, &cpuflg);
		memmgrob.mo_alocpages += realpnr;
		memmgrob.mo_freepages -= realpnr;
		knl_spinunlock_sti(&memmgrob.mo_lock, &cpuflg);
	}

	if (flgs == 1)
	{
		knl_spinlock_cli(&memmgrob.mo_lock, &cpuflg);
		memmgrob.mo_alocpages -= realpnr;
		memmgrob.mo_freepages += realpnr;
		knl_spinunlock_sti(&memmgrob.mo_lock, &cpuflg);
	}
}

// 更新内存区页面数
void mm_update_memarea(memarea_t *malokp, uint_t pgnr, uint_t flgs)
{
	if (malokp == NULL) {
		return;
	}

	if (flgs == 0) {
		malokp->ma_freepages -= pgnr;
		malokp->ma_allocpages += pgnr;
	}

	if (flgs == 1) {
		malokp->ma_freepages += pgnr;
		malokp->ma_allocpages -= pgnr;
	}
}

KLINE sint_t retn_divoder(uint_t pages)
{
	sint_t pbits = search_64rlbits((uint_t)pages) - 1;
	if (pages & (pages - 1)) {
		pbits++;
	}

	return pbits;
}

memarea_t *onfrmsa_retn_marea(memmgrob_t *mmobjp, msadsc_t *freemsa, uint_t freepgs)
{
	if (freemsa->md_indxflgs.mf_olkty != MF_OLKTY_ODER || freemsa->md_odlink == NULL) {
		return NULL;
	}

	msadsc_t *fmend = (msadsc_t *)freemsa->md_odlink;
	if (((uint_t)(fmend - freemsa) + 1) != freepgs) {
		return NULL;
	}

	if (freemsa->md_indxflgs.mf_marty != fmend->md_indxflgs.mf_marty) {
		return NULL;
	}

	// 遍历内存区，若类新相同则返回
	for (uint_t mi = 0; mi < mmobjp->mo_mareanr; mi++)
	{
		if ((uint_t)(freemsa->md_indxflgs.mf_marty) == mmobjp->mo_mareastat[mi].ma_type)
			return &mmobjp->mo_mareastat[mi];
	}

	return NULL;
}

// 根据mrtype类型，返回不同内存区的结构体指针
memarea_t *onmrtype_retn_marea(memmgrob_t *mmobjp, uint_t mrtype)
{
	// 遍历所有内存区
	for (uint_t mi = 0; mi < mmobjp->mo_mareanr; mi++)
	{
		// 若类型相同则直接返回
		if (mmobjp->mo_mareastat[mi].ma_type == mrtype)
			return &mmobjp->mo_mareastat[mi];
	}

	return NULL;
}

bafhlst_t *onma_retn_maxbafhlst(memarea_t *malckp)
{
	for (s64_t li = (MDIVMER_ARR_LMAX - 1); li >= 0; li--)
	{
		if (malckp->ma_mdmdata.dm_mdmlielst[li].af_fobjnr > 0)
			return &malckp->ma_mdmdata.dm_mdmlielst[li];
	}

	return NULL;
}

// 设置一段msadsc数组的属性为已删除(释放)
msadsc_t *mm_divpages_opmsadsc(msadsc_t *msastat, uint_t mnr)
{
	if (msastat == NULL || mnr == 0) {
		return NULL;
	}

	if ((MF_OLKTY_ODER != msastat->md_indxflgs.mf_olkty &&
		 MF_OLKTY_BAFH != msastat->md_indxflgs.mf_olkty) ||
		NULL == msastat->md_odlink ||
		PAF_NO_ALLOC != msastat->md_phyadrs.paf_alloc)
	{
		system_error("mm_divpages_opmsadsc err1\n");
	}

	// 获取尾部的msadsc
	msadsc_t *mend = (msadsc_t *)msastat->md_odlink;
	if (MF_OLKTY_BAFH == msastat->md_indxflgs.mf_olkty) {
		mend = msastat;
	}

	if (mend < msastat) {
		system_error("mm_divpages_opmsadsc err2\n");
	}

	if ((uint_t)((mend - msastat) + 1) != mnr) {
		system_error("mm_divpages_opmsadsc err3\n");
	}
	
	if (msastat->md_indxflgs.mf_uindx > (MF_UINDX_MAX - 1) || mend->md_indxflgs.mf_uindx > (MF_UINDX_MAX - 1) ||
		msastat->md_indxflgs.mf_uindx != mend->md_indxflgs.mf_uindx)
	{
		system_error("mm_divpages_opmsadsc err4");
	}

	// 若只有一个msadsc
	if (msastat == mend) {

		msastat->md_indxflgs.mf_uindx++;
		msastat->md_phyadrs.paf_alloc = PAF_ALLOC;
		msastat->md_indxflgs.mf_olkty = MF_OLKTY_ODER;
		msastat->md_odlink = mend;

		return msastat;
	}

	// 多个msadsc时，只设置开始与末端
	msastat->md_indxflgs.mf_uindx++;
	msastat->md_phyadrs.paf_alloc = PAF_ALLOC;
	mend->md_indxflgs.mf_uindx++;
	mend->md_phyadrs.paf_alloc = PAF_ALLOC;
	
	msastat->md_indxflgs.mf_olkty = MF_OLKTY_ODER;
	msastat->md_odlink = mend;
	
	return msastat;
}

sint_t mm_merpages_opmsadsc(bafhlst_t *bafh, msadsc_t *freemsa, uint_t freepgs)
{
	if (NULL == bafh || NULL == freemsa || 1 > freepgs) {
		return 0;
	}

	if (MF_OLKTY_ODER != freemsa->md_indxflgs.mf_olkty ||
		NULL == freemsa->md_odlink) {
		system_error("mm_merpages_opmsadsc err1\n");
	}

	// 获取释放的页面数组的尾页面
	msadsc_t *fmend = (msadsc_t *)freemsa->md_odlink;
	if (fmend < freemsa) {
		system_error("mm_merpages_opmsadsc err2\n");
	}

	if (bafh->af_oderpnr != freepgs ||
		((uint_t)(fmend - freemsa) + 1) != freepgs) {
		system_error("mm_merpages_opmsadsc err3\n");
	}
	if (PAF_NO_ALLOC == freemsa->md_phyadrs.paf_alloc ||
		1 > freemsa->md_indxflgs.mf_uindx) {
		system_error("mm_merpages_opmsadsc err4\n");
	}

	if (PAF_NO_ALLOC == fmend->md_phyadrs.paf_alloc ||
		1 > fmend->md_indxflgs.mf_uindx) {
		system_error("mm_merpages_opmsadsc err5\n");
	}

	if (freemsa->md_indxflgs.mf_uindx != fmend->md_indxflgs.mf_uindx) {
		system_error("mm_merpages_opmsadsc err6\n");
	}

// 1.若释放的是一个单独的页面
	if (freemsa == fmend)
	{
		// 分配计数-1
		freemsa->md_indxflgs.mf_uindx--;

		// 若分配计数-1后仍然大于0，说明该页面是共享页面，返回1表示不需要进行下一步操作
		if (freemsa->md_indxflgs.mf_uindx > 0) {
			return 1;
		}

		// 设置未分配的标识
		freemsa->md_phyadrs.paf_alloc = PAF_NO_ALLOC;
		freemsa->md_indxflgs.mf_olkty = MF_OLKTY_BAFH;
		freemsa->md_odlink = bafh;	// 指向所属的bafhlst
		
		return 2;
	}

// 2. 若释放的不是单独的页面
	freemsa->md_indxflgs.mf_uindx--;
	fmend->md_indxflgs.mf_uindx--;

	// 共享页面
	if (freemsa->md_indxflgs.mf_uindx > 0) {
		return 1;
	}

	// 设置起始、结束页的未分配标识
	freemsa->md_phyadrs.paf_alloc = PAF_NO_ALLOC;
	fmend->md_phyadrs.paf_alloc = PAF_NO_ALLOC;
	freemsa->md_indxflgs.mf_olkty = MF_OLKTY_ODER;

	// 起始页面指向结束页面
	freemsa->md_odlink = fmend;
	fmend->md_indxflgs.mf_olkty = MF_OLKTY_BAFH;

	// 结束页面指向所属的bafhlst
	fmend->md_odlink = bafh;

	return 2;
}

// 根据请求分配的页面数在对应的内存区中找出请求分配、实际分配的bafhlst_t
bool_t onmpgs_retn_bafhlst(memarea_t *malckp, uint_t pages, bafhlst_t **retrelbafh, bafhlst_t **retdivbafh)
{
	if (malckp == NULL || pages < 1 || retrelbafh == NULL || retdivbafh == NULL) {
		return FALSE;
	}

	// 获取bafhlst_t数组的开始地址
	bafhlst_t *bafhstat = malckp->ma_mdmdata.dm_mdmlielst;
	
	// 根据请求分配的页面数计算出dm_mdmlielst数组的下标
	sint_t dividx = retn_divoder(pages); // 这个找出的是请求分配的
	
	// 下标超出范围
	if (dividx < 0 || dividx >= MDIVMER_ARR_LMAX) {
		*retrelbafh = NULL;
		*retdivbafh = NULL;
		return FALSE;
	}

	// 对应结构的连续页面数小于需求
	if (bafhstat[dividx].af_oderpnr < pages) { 
		*retrelbafh = NULL;
		*retdivbafh = NULL;
		return FALSE;
	}

	/**
	 * 
	 * retrelbafh是没有分配页时的初始状态，即理论上应该从该页起开始分配
	 * retdivbafh是实际上根据当前页的状态来分配实际的页面
	 */

	// 从dividx起，寻找实际要分配的页面
	for (sint_t idx = dividx; idx < MDIVMER_ARR_LMAX; idx++) {
		//  直至找到一个连续页面数确实满足条件的话就直接返回，若dividx实际不够分配，那么就会继续遍历
		if (bafhstat[idx].af_oderpnr >= pages && bafhstat[idx].af_fobjnr > 0)
		{
			*retrelbafh = &bafhstat[dividx];
			*retdivbafh = &bafhstat[idx];
			return TRUE;
		}
	}

	*retrelbafh = NULL;
	*retdivbafh = NULL;
	return FALSE;
}

bool_t onfpgs_retn_bafhlst(memarea_t *malckp, uint_t freepgs, bafhlst_t **retrelbf, bafhlst_t **retmerbf)
{
	if (malckp == NULL || freepgs < 1 || retrelbf == NULL || retmerbf == NULL) {
		return FALSE;
	}

	// 获取bafhlst数组
	bafhlst_t *bafhstat = malckp->ma_mdmdata.dm_mdmlielst;
	
	// 根据要释放的freepgs计算出理论的bafhlst下标
	sint_t dividx = retn_divoder(freepgs);
	if (dividx < 0 || dividx >= MDIVMER_ARR_LMAX) {
		*retrelbf = NULL;
		*retmerbf = NULL;
		return FALSE;
	}

	if (bafhstat[dividx].af_mobjnr >= (~0UL))
		system_error("onfpgs_retn_bafhlst af_mobjnr max");

	if (bafhstat[dividx].af_fobjnr >= (~0UL))
		system_error("onfpgs_retn_bafhlst af_fobjnr max");

	if (bafhstat[dividx].af_oderpnr != freepgs)
	{
		*retrelbf = NULL;
		*retmerbf = NULL;
		return FALSE;
	}

	// 返回 请求释放 和 最大释放 的结构指针
	*retrelbf = &bafhstat[dividx];
	*retmerbf = &bafhstat[MDIVMER_ARR_LMAX - 1];

	return TRUE;
}

msadsc_t *mm_divipages_onbafhlst(bafhlst_t *bafhp)
{
	if (NULL == bafhp)
		return NULL;
	if (1 > bafhp->af_fobjnr)
		return NULL;
	if (list_is_empty_careful(&bafhp->af_frelst) == TRUE)
		return NULL;

	msadsc_t *tmp = list_entry(bafhp->af_frelst.next, msadsc_t, md_list);
	list_del(&tmp->md_list);
	tmp->md_indxflgs.mf_uindx++;
	tmp->md_phyadrs.paf_alloc = PAF_ALLOC;
	bafhp->af_fobjnr--;
	bafhp->af_mobjnr--;
	bafhp->af_alcindx++;

	return tmp;
}

// 从bafhlst结构中获取自己的msadsc结构的开始与结束地址
bool_t mm_retnmsaob_onbafhlst(bafhlst_t *bafhp, msadsc_t **retmstat, msadsc_t **retmend)
{
	if (bafhp == NULL || retmstat == NULL || retmend == NULL) {
		return FALSE;
	}

	if (bafhp->af_mobjnr < 1 || bafhp->af_fobjnr < 1) {
		*retmstat = NULL;
		*retmend = NULL;
		return FALSE;
	}

	// 链表判空
	if (list_is_empty_careful(&bafhp->af_frelst) == TRUE) {
		*retmstat = NULL;
		*retmend = NULL;
		return FALSE;
	}

	// 从bafhlst中取出一个msadsc(其为首地址)，container_of
	msadsc_t *tmp = list_entry(bafhp->af_frelst.next, msadsc_t, md_list);
	
	// 从链表中删除，将首个删除，那么整个挂载的页都会被删除
	list_del(&tmp->md_list);

	// 更新相关计数
	bafhp->af_mobjnr--;		// 当前总页面数
	bafhp->af_fobjnr--;		// 空闲总页面数
	bafhp->af_freindx++;	// 释放数
	
	*retmstat = tmp;
	*retmend = (msadsc_t *)tmp->md_odlink;	// 该指针指向尾部的msadsc
	
	// 若是单个msadsc结构，那就是它本身
	if (tmp->md_indxflgs.mf_olkty == MF_OLKTY_BAFH) {
		*retmend = tmp;
	}

	return TRUE;
}

// 检查pages是否合法
bool_t scan_mapgsalloc_ok(memarea_t *malckp, uint_t pages)
{
	if (malckp == NULL || pages < 1) {
		return FALSE;
	}

	// 内存区的空闲页面和最大页面都足够
	if (malckp->ma_freepages >= pages && malckp->ma_maxpages >= pages) {
		return TRUE;
	}

	return FALSE;
}

msadsc_t *mm_maxdivpages_onmarea(memarea_t *malckp, uint_t *retrelpnr)
{
	bafhlst_t *bafhp = onma_retn_maxbafhlst(malckp);
	if (NULL == bafhp) {
		*retrelpnr = 0;
		return NULL;
	}

	msadsc_t *retmsa = NULL;

	msadsc_t *retmstat = NULL, *retmend = NULL;
	bool_t rets = mm_retnmsaob_onbafhlst(bafhp, &retmstat, &retmend);
	if (FALSE == rets || NULL == retmstat || NULL == retmend) {
		*retrelpnr = 0;
		return NULL;
	}
	retmsa = mm_divpages_opmsadsc(retmstat, bafhp->af_oderpnr);

	if (NULL == retmsa)
	{
		*retrelpnr = 0;
		return NULL;
	}
	*retrelpnr = bafhp->af_oderpnr;

	return retmsa;
}

uint_t chek_divlenmsa(msadsc_t *msastat, msadsc_t *msaend, uint_t mnr)
{
	uint_t ok = 0;
	msadsc_t *ms = msastat, *me = msaend;
	if (NULL == msastat || NULL == msaend || 0 == mnr) {
		return 0;
	}

	if ((uint_t)(msaend - msastat + 1) != mnr) {
		return 0;
	}

	if (1 == mnr)
	{
		if (0 < msastat->md_indxflgs.mf_uindx) {
			return 0;
		}

		if (PAF_NO_ALLOC != msastat->md_phyadrs.paf_alloc) {
			return 0;
		}
		
		if (list_is_empty_careful(&msastat->md_list) == FALSE) {
			return 0;
		}

		return ok + 1;
	}

	for (; ms < me; ms++)
	{
		if (ms->md_indxflgs.mf_uindx != 0 ||
			(ms + 1)->md_indxflgs.mf_uindx != 0) {
			return 0;
		}

		if (PAF_NO_ALLOC != ms->md_phyadrs.paf_alloc ||
			PAF_NO_ALLOC != (ms + 1)->md_phyadrs.paf_alloc) {
			return 0;
		}

		if (list_is_empty_careful(&ms->md_list) == FALSE || list_is_empty_careful(&((ms + 1)->md_list)) == FALSE)
			return 0;
		if (PAGESIZE != (((ms + 1)->md_phyadrs.paf_padrs << PSHRSIZE) - (ms->md_phyadrs.paf_padrs << PSHRSIZE)))
			return 0;

		ok++;
	}

	if (0 == ok) {
		return 0;
	}

	if ((ok + 1) != mnr) {
		return 0;
	}

	return ok;
}

bool_t mrdmb_add_msa_bafh(bafhlst_t *bafhp, msadsc_t *msastat, msadsc_t *msaend)
{
	if (bafhp == NULL || msastat == NULL || msaend == NULL) {
		return FALSE;
	}

	uint_t mnr = (msaend - msastat) + 1;
	if ((uint_t)bafhp->af_oderpnr != mnr) {
		return FALSE;
	}

	if (msastat->md_indxflgs.mf_uindx > 0 || msaend->md_indxflgs.mf_uindx > 0) {
		return FALSE;
	}

	if (PAF_NO_ALLOC != msastat->md_phyadrs.paf_alloc ||
		PAF_NO_ALLOC != msaend->md_phyadrs.paf_alloc) {
		return FALSE;
	}

	if (msastat == msaend && 1 != mnr && 1 != bafhp->af_oderpnr) {
		return FALSE;
	}

	// 把一段连续的msadsc结构加入到其所对应的bafhlst_t中
	msastat->md_indxflgs.mf_olkty = MF_OLKTY_ODER;
	msastat->md_odlink = msaend;

	msaend->md_indxflgs.mf_olkty = MF_OLKTY_BAFH;
	msaend->md_odlink = bafhp;
	list_add(&msastat->md_list, &bafhp->af_frelst);
	bafhp->af_mobjnr++;
	bafhp->af_fobjnr++;

	return TRUE;
}

// relbfl
// divvfl
msadsc_t *mm_reldpgsdivmsa_bafhl(memarea_t *malckp, uint_t pages, uint_t *retrelpnr, bafhlst_t *relbfl, bafhlst_t *divbfl)
{
	msadsc_t *retmsa = NULL;
	bool_t rets = FALSE;
	msadsc_t *retmstat = NULL, *retmend = NULL;
	if (malckp == NULL || pages < 1 || retrelpnr == NULL || relbfl == NULL || divbfl == NULL) {
		return NULL;
	}

	if (relbfl > divbfl) {
		*retrelpnr = 0;
		return NULL;
	}

// 1.处理相等的情况
	if (relbfl == divbfl)
	{	
		// 从bafhlst中获取msadsc的开始地址与结束地址
		rets = mm_retnmsaob_onbafhlst(relbfl, &retmstat, &retmend);
		if (rets == FALSE || retmstat == NULL || retmend == NULL) {
			*retrelpnr = 0; 
			return NULL;
		}

		// 
		if ((uint_t)((retmend - retmstat) + 1) != relbfl->af_oderpnr) {
			*retrelpnr = 0;
			return NULL;
		}

		// 设置msadsc结构的相关信息，表示已删除
		retmsa = mm_divpages_opmsadsc(retmstat, relbfl->af_oderpnr);
		if (NULL == retmsa) {
			*retrelpnr = 0;
			return NULL;
		}

		*retrelpnr = relbfl->af_oderpnr;
		return retmsa;
	}


// 2.处理不相等的情况

	// ... 
	rets = mm_retnmsaob_onbafhlst(divbfl, &retmstat, &retmend);
	if (rets == NULL || retmstat == NULL || retmend == NULL) {
		*retrelpnr = 0;
		return NULL;
	}

	if ((uint_t)((retmend - retmstat) + 1) != divbfl->af_oderpnr) {
		*retrelpnr = 0;
		return NULL;
	}

	// 获取要释放的页面数
	uint_t divnr = divbfl->af_oderpnr;
	
	/**
	 * 假设用户只请求的1个页面，理论上要将第0个bafhlst中的页面分配出去，但实际上这个页面已经被分配出去了
	 * 所以要向后寻找，直至找到连续页面数足够的bafhlst，以上操作有前面的函数执行
	 * 
	 * 当上述操作的函数返回后，发现relbl和divbl不相同，那么说明需要进行分割操作，
	 * 即将实际分配到的页面数（假设4个，即第2个bafhlst元素）分割，变成两个2，将其中一个
	 * 挂载到bafhlst[1]上，然后继续循环，直到第bafhlst[0](即relbl)个，
	 * 将这1个msadsc挂载到bafhlst[0]上， 另外一个分配出去
	 */

	// 从div到rel遍历
	for (bafhlst_t *tmpbfl = divbfl - 1; tmpbfl >= relbfl; tmpbfl--)
	{
		// 分割连续的msadsc结构，操作对应上述描述
		if (mrdmb_add_msa_bafh(tmpbfl, &retmstat[tmpbfl->af_oderpnr], (msadsc_t *)retmstat->md_odlink) == FALSE)
			system_error("mrdmb_add_msa_bafh fail\n");

		// 不断更新末端msadsc
		retmstat->md_odlink = &retmstat[tmpbfl->af_oderpnr - 1];
		// 还需释放的页面数
		divnr -= tmpbfl->af_oderpnr;
	}

	retmsa = mm_divpages_opmsadsc(retmstat, divnr);
	if (retmsa == NULL) {
		*retrelpnr = 0;
		return NULL;
	}

	*retrelpnr = relbfl->af_oderpnr;
	return retmsa;
}

// 
msadsc_t *mm_reldivpages_onmarea(memarea_t *malckp, uint_t pages, uint_t *retrelpnr)
{
	if (malckp == NULL || pages < 1 || retrelpnr == NULL) {
		return NULL;
	}

	// 检查pages是否合法
	if (scan_mapgsalloc_ok(malckp, pages) == FALSE) {
		*retrelpnr = 0;
		return NULL;
	}
	
	bafhlst_t *retrelbhl = NULL, *retdivbhl = NULL;

	// 根据页面数在对应的内存区中找出请求分配页面数 理论上 和 实际上 分配的bafhlst_t
	bool_t rets = onmpgs_retn_bafhlst(malckp, pages, &retrelbhl, &retdivbhl);
	if (rets == FALSE) {
		*retrelpnr = 0;
		return NULL;
	}

	uint_t retpnr = 0;
	// 实际分配内存页面
	msadsc_t *retmsa = mm_reldpgsdivmsa_bafhl(malckp, pages, &retpnr, retrelbhl, retdivbhl);
	if (retmsa == NULL) {
		*retrelpnr = 0;
		return NULL;
	}

	*retrelpnr = retpnr;
	return retmsa;
}

msadsc_t *mm_prcdivpages_onmarea(memarea_t *malckp, uint_t pages, uint_t *retrelpnr)
{
	if (NULL == malckp || NULL == retrelpnr || 1 != pages) {
		return NULL;
	}

	if (MA_TYPE_PROC != malckp->ma_type) {
		*retrelpnr = 0;
		return NULL;
	}

	if (scan_mapgsalloc_ok(malckp, pages) == FALSE) {
		*retrelpnr = 0;
		return NULL;
	}

	bafhlst_t *prcbfh = &malckp->ma_mdmdata.dm_onemsalst;
	bool_t rets = FALSE;
	msadsc_t *retmsa = NULL, *retmstat = NULL, *retmend = NULL;
	rets = mm_retnmsaob_onbafhlst(prcbfh, &retmstat, &retmend);
	if (FALSE == rets || NULL == retmstat || NULL == retmend)
	{
		*retrelpnr = 0;
		return NULL;
	}

	if ((uint_t)((retmend - retmstat) + 1) != prcbfh->af_oderpnr)
	{
		*retrelpnr = 0;
		return NULL;
	}

	retmsa = mm_divpages_opmsadsc(retmstat, prcbfh->af_oderpnr);
	if (NULL == retmsa)
	{
		*retrelpnr = 0;
		return NULL;
	}

	*retrelpnr = prcbfh->af_oderpnr;

	return retmsa;
}

// 内存页面分配的核心函数
msadsc_t *mm_divpages_core(memarea_t *mareap, uint_t pages, uint_t *retrealpnr, uint_t flgs)
{
	uint_t retpnr = 0;
	msadsc_t *retmsa = NULL;
	cpuflg_t cpuflg;
	if (flgs != DMF_RELDIV && flgs != DMF_MAXDIV) {
		*retrealpnr = 0;
		return NULL;
	}

	if (mareap->ma_type != MA_TYPE_KRNL && mareap->ma_type != MA_TYPE_HWAD) {
		*retrealpnr = 0;
		return NULL;
	}

	// 内存区加锁
	knl_spinlock_cli(&mareap->ma_lock, &cpuflg);

	// 分配内存页面
	if (flgs == DMF_MAXDIV) {
		retmsa = mm_maxdivpages_onmarea(mareap, &retpnr);
	}
	else if (flgs == DMF_RELDIV) {
		retmsa = mm_reldivpages_onmarea(mareap, pages, &retpnr);
	}
	else {
		retmsa = NULL;
		retpnr = 0;
	}

	// 分配完成后更新内存相关信息
	if (retmsa != NULL && retpnr != 0)
	{
		mm_update_memarea(mareap, retpnr, 0);
		mm_update_memmgrob(retpnr, 0);
	}

	// 内存区解锁
	knl_spinunlock_sti(&mareap->ma_lock, &cpuflg);

	*retrealpnr = retpnr;	
	return retmsa;
}

// 内存页面分配的框架函数
// 参数描述见函数 mm_division_pages
msadsc_t *mm_divpages_fmwk(memmgrob_t *mmobjp, uint_t pages, uint_t *retrelpnr, uint_t mrtype, uint_t flgs)
{
	// 返回rtype对应的内存区结构
	memarea_t *marea = onmrtype_retn_marea(mmobjp, mrtype);
	if (marea == NULL) {
		*retrelpnr = 0;
		return NULL;
	}

	uint_t retpnr = 0;

	// 内存分配的核心函数
	msadsc_t *retmsa = mm_divpages_core(marea, pages, &retpnr, flgs);
	if (retmsa == NULL) {
		*retrelpnr = 0;
		return NULL;
	}

	*retrelpnr = retpnr;

	return retmsa;
}

/// @brief 内存页面分配的接口函数
/// @param mmobjp 管理内存的数据结构指针
/// @param pages 请求分配的内存页面数
/// @param retrealpnr 存放实际分配的内存页面数的指针
/// @param mrtype 请求分配的内存页面的内存区类型
/// @param flgs 请求分配的内存页面的标志位
/// @return 返回msadsc_t结构体数组
msadsc_t *mm_division_pages(memmgrob_t *mmobjp, uint_t pages, uint_t *retrealpnr, uint_t mrtype, uint_t flgs)
{
	if (mmobjp == NULL || retrealpnr == NULL || mrtype == 0) {
		return NULL;
	}

	uint_t retpnr = 0;
	
	// 调用mm_divpages_fmwk框架函数 去分配内存页面
	msadsc_t *retmsa = mm_divpages_fmwk(mmobjp, pages, &retpnr, mrtype, flgs);
	if (retmsa == NULL) {
		*retrealpnr = 0;
		return NULL;
	}

	*retrealpnr = retpnr;
	
	return retmsa;
}

memarea_t *retn_procmarea(memmgrob_t *mmobjp)
{
	if (NULL == mmobjp) {
		return NULL;
	}

	for (uint_t mi = 0; mi < mmobjp->mo_mareanr; mi++) {
		if (MA_TYPE_PROC == mmobjp->mo_mareastat[mi].ma_type) {
			return &mmobjp->mo_mareastat[mi];
		}
	}

	return NULL;
}

msadsc_t *divpages_procmarea_core(memmgrob_t *mmobjp, uint_t pages, uint_t *retrealpnr)
{
	cpuflg_t cpuflg;
	uint_t retpnr = 0;						  
	msadsc_t *retmsa = NULL, *retmsap = NULL; 
	if (NULL == mmobjp || 1 != pages || NULL == retrealpnr) {
		return NULL;
	}

	memarea_t *marp = retn_procmarea(mmobjp);
	if (NULL == marp) {
		*retrealpnr = 0;
		return NULL;
	}

	knl_spinlock_cli(&marp->ma_lock, &cpuflg);
	if (scan_mapgsalloc_ok(marp, pages) == FALSE) {
		retmsap = NULL;
		retpnr = 0;
		goto ret_step;
	}

	retmsa = mm_prcdivpages_onmarea(marp, pages, &retpnr);

	if (NULL != retmsa && 0 != retpnr) {
		mm_update_memarea(marp, retpnr, 0);
		mm_update_memmgrob(retpnr, 0);
		retmsap = retmsa;
		goto ret_step;
	}

	retpnr = 0;
	retmsap = NULL;

ret_step:
	knl_spinunlock_sti(&marp->ma_lock, &cpuflg);
	*retrealpnr = retpnr;
	
	return retmsap;
}

msadsc_t *mm_divpages_procmarea(memmgrob_t *mmobjp, uint_t pages, uint_t *retrealpnr)
{
	msadsc_t *retmsa = NULL;
	uint_t retpnr = 0;
	if (NULL == mmobjp || 1 != pages || NULL == retrealpnr) {
		return NULL;
	}

	retmsa = divpages_procmarea_core(mmobjp, pages, &retpnr);
	if (NULL != retmsa) {
		*retrealpnr = retpnr;
		return retmsa;
	}

	retmsa = mm_division_pages(mmobjp, pages, &retpnr, MA_TYPE_KRNL, DMF_RELDIV);
	if (NULL == retmsa) {
		*retrealpnr = 0;
		return NULL;
	}

	*retrealpnr = retpnr;
	
	return retmsa;
}

bool_t scan_freemsa_isok(msadsc_t *freemsa, uint_t freepgs)
{
	if (freemsa == NULL || freepgs < 1) {
		return FALSE;
	}

	if (freemsa->md_indxflgs.mf_olkty != MF_OLKTY_ODER ||
		freemsa->md_odlink == NULL || 
		freemsa->md_indxflgs.mf_uindx < 1) 
	{
		return FALSE;
	}

	msadsc_t *end = (msadsc_t *)freemsa->md_odlink;

	if (freemsa->md_phyadrs.paf_alloc != PAF_ALLOC ||
		end->md_phyadrs.paf_alloc != PAF_ALLOC ||
		end->md_indxflgs.mf_uindx < 1)
	{
		return FALSE;
	}

	if (((uint_t)((end - freemsa) + 1)) != freepgs) {
		return FALSE;
	}

	return TRUE;
}

sint_t mm_cmsa1blk_isok(bafhlst_t *bafh, msadsc_t *_1ms, msadsc_t *_1me)
{
	if (NULL == bafh || NULL == _1ms || NULL == _1me) {
		return 0;
	}

	if (_1me < _1ms) {
		return 0;
	}

	if (_1ms == _1me)
	{
		if (MF_OLKTY_BAFH != _1me->md_indxflgs.mf_olkty) {
			return 0;
		}

		if (bafh != (bafhlst_t *)_1me->md_odlink) {
			return 0;
		}

		if (PAF_NO_ALLOC != _1me->md_phyadrs.paf_alloc) {
			return 0;
		}

		if (0 != _1me->md_indxflgs.mf_uindx) {
			return 0;
		}

		if ((_1me->md_phyadrs.paf_padrs - _1ms->md_phyadrs.paf_padrs) != (uint_t)(_1me - _1ms)) {
			return 0;
		}
		
		return 2;
	}

	if (MF_OLKTY_ODER != _1ms->md_indxflgs.mf_olkty)
		return 0;

	if (_1me != (msadsc_t *)_1ms->md_odlink)
		return 0;

	if (PAF_NO_ALLOC != _1ms->md_phyadrs.paf_alloc)
		return 0;

	if (0 != _1ms->md_indxflgs.mf_uindx)
		return 0;

	if (MF_OLKTY_BAFH != _1me->md_indxflgs.mf_olkty)
		return 0;

	if (bafh != (bafhlst_t *)_1me->md_odlink)
		return 0;

	if (PAF_NO_ALLOC != _1me->md_phyadrs.paf_alloc)
		return 0;

	if (0 != _1me->md_indxflgs.mf_uindx)
		return 0;

	if ((_1me->md_phyadrs.paf_padrs - _1ms->md_phyadrs.paf_padrs) != (uint_t)(_1me - _1ms))
		return 0;

	return 2;
}

sint_t mm_cmsa2blk_isok(bafhlst_t *bafh, msadsc_t *_1ms, msadsc_t *_1me, msadsc_t *_2ms, msadsc_t *_2me)
{
	if (NULL == bafh || NULL == _1ms || NULL == _1me ||
		NULL == _2ms || NULL == _2me ||
		_1ms == _2ms || _1me == _2me) {
		return 0;
	}

	sint_t ret1s = 0, ret2s = 0;
	ret1s = mm_cmsa1blk_isok(bafh, _1ms, _1me);
	if (0 == ret1s) {
		system_error("mm_cmsa1blk_isok ret1s == 0\n");
	}

	ret2s = mm_cmsa1blk_isok(bafh, _2ms, _2me);
	if (0 == ret2s) {
		system_error("mm_cmsa1blk_isok ret2s == 0\n");
	}

	if (2 == ret1s && 2 == ret2s)
	{
		if (_1ms < _2ms && _1me < _2me)
		{
			if ((_1me + 1) != _2ms) {
				return 1;
			}

			if ((_1me->md_phyadrs.paf_padrs + 1) != _2ms->md_phyadrs.paf_padrs) {
				return 1;
			}
			return 2;
		}

		if (_1ms > _2ms && _1me > _2me)
		{
			if ((_2me + 1) != _1ms) {
				return 1;
			}

			if ((_2me->md_phyadrs.paf_padrs + 1) != _1ms->md_phyadrs.paf_padrs) {
				return 1;
			}

			return 4;
		}

		return 0;
	}

	return 0;
}

bool_t chek_cl2molkflg(bafhlst_t *bafh, msadsc_t *_1ms, msadsc_t *_1me, msadsc_t *_2ms, msadsc_t *_2me)
{
	if (NULL == bafh || NULL == _1ms || NULL == _1me || NULL == _2ms || NULL == _2me) {
		return FALSE;
	}
	
	if (_1ms == _2ms || _1me == _2me) {
		return FALSE;
	}

	if (((uint_t)(_2me - _1ms) + 1) != bafh->af_oderpnr) {
		return FALSE;
	}

	if (_1ms == _1me && _2ms == _2me) 
	{
		if (MF_OLKTY_ODER != _1ms->md_indxflgs.mf_olkty || (msadsc_t *)_1ms->md_odlink != _2me)
			return FALSE;

		if (MF_OLKTY_BAFH != _2me->md_indxflgs.mf_olkty || (bafhlst_t *)_2me->md_odlink != bafh)
			return FALSE;

		return TRUE;
	}

	if (MF_OLKTY_ODER != _1ms->md_indxflgs.mf_olkty || (msadsc_t *)_1ms->md_odlink != _2me)
		return FALSE;
	if (MF_OLKTY_INIT != _1me->md_indxflgs.mf_olkty || NULL != _1me->md_odlink)
		return FALSE;
	if (MF_OLKTY_INIT != _2ms->md_indxflgs.mf_olkty || NULL != _2ms->md_odlink)
		return FALSE;
	if (MF_OLKTY_BAFH != _2me->md_indxflgs.mf_olkty || (bafhlst_t *)_2me->md_odlink != bafh)
		return FALSE;

	return TRUE;
}

bool_t mm_clear_2msaolflg(bafhlst_t *bafh, msadsc_t *_1ms, msadsc_t *_1me, msadsc_t *_2ms, msadsc_t *_2me)
{
	if (NULL == bafh || NULL == _1ms || NULL == _1me || NULL == _2ms || NULL == _2me)
		return FALSE;
	if (_1ms == _2ms || _1me == _2me)
		return FALSE;

	_1me->md_indxflgs.mf_olkty = MF_OLKTY_INIT;
	_1me->md_odlink = NULL;
	_2ms->md_indxflgs.mf_olkty = MF_OLKTY_INIT;
	_2ms->md_odlink = NULL;
	_1ms->md_indxflgs.mf_olkty = MF_OLKTY_ODER;
	_1ms->md_odlink = _2me;
	_2me->md_indxflgs.mf_olkty = MF_OLKTY_BAFH;
	_2me->md_odlink = bafh;

	return TRUE;
}

sint_t mm_find_cmsa2blk(bafhlst_t *fbafh, msadsc_t **rfnms, msadsc_t **rfnme)
{
	if (NULL == fbafh || NULL == rfnms || NULL == rfnme) {
		return 0;
	}

	msadsc_t *freemstat = *rfnms;
	msadsc_t *freemend = *rfnme;
	if (1 > fbafh->af_fobjnr) {
		return 1;
	}

	list_h_t *tmplst = NULL;
	msadsc_t *tmpmsa = NULL, *blkms = NULL, *blkme = NULL;
	sint_t rets = 0;

	list_for_each(tmplst, &fbafh->af_frelst)
	{
		tmpmsa = list_entry(tmplst, msadsc_t, md_list);
		rets = mm_cmsa2blk_isok(fbafh, freemstat, freemend, tmpmsa, &tmpmsa[fbafh->af_oderpnr - 1]);
		if (2 == rets || 4 == rets)
		{
			blkms = tmpmsa;
			blkme = &tmpmsa[fbafh->af_oderpnr - 1];
			list_del(&tmpmsa->md_list);
			fbafh->af_fobjnr--;
			fbafh->af_mobjnr--;

			goto step1;
		}
	}

step1:
	if (0 == rets || 1 == rets) {
		return 1;
	}

	if (2 == rets)
	{
		if (mm_clear_2msaolflg(fbafh + 1, freemstat, freemend, blkms, blkme) == TRUE)
		{
			if (chek_cl2molkflg(fbafh + 1, freemstat, freemend, blkms, blkme) == FALSE)
				system_error("chek_cl2molkflg err1\n");
			*rfnms = freemstat;
			*rfnme = blkme;
			
			return 2;
		}

		return 0;
	}

	if (4 == rets)
	{
		if (mm_clear_2msaolflg(fbafh + 1, blkms, blkme, freemstat, freemend) == TRUE)
		{
			if (chek_cl2molkflg(fbafh + 1, blkms, blkme, freemstat, freemend) == FALSE)
				system_error("chek_cl2molkflg err2\n");
			*rfnms = blkms;
			*rfnme = freemend;

			return 2;
		}
		return 0;
	}

	return 0;
}

bool_t mpobf_add_msadsc(bafhlst_t *bafhp, msadsc_t *freemstat, msadsc_t *freemend)
{
	if (bafhp == NULL || freemstat == NULL || freemend == NULL)
		return FALSE;
	
	if (freemend < freemstat)
		return FALSE;
	
	if (bafhp->af_oderpnr != ((uint_t)(freemend - freemstat) + 1))
		return FALSE;

	if ((~0UL) <= bafhp->af_fobjnr || (~0UL) <= bafhp->af_mobjnr) {
		system_error("(~0UL)<=bafhp->af_fobjnr\n");
		return FALSE;
	}

	freemstat->md_indxflgs.mf_olkty = MF_OLKTY_ODER;

	// 设置起始页面指向结束页面
	freemstat->md_odlink = freemend;
	freemend->md_indxflgs.mf_olkty = MF_OLKTY_BAFH;
	
	// 将结束页面指向bafhlst
	freemend->md_odlink = bafhp;

	// 将起始页面挂载到所属的bafhlst中
	list_add(&freemstat->md_list, &bafhp->af_frelst);
	
	// 增加计数
	bafhp->af_fobjnr++;
	bafhp->af_mobjnr++;
	
	return TRUE;
}

// 合并页面
bool_t mm_merpages_onbafhlst(msadsc_t *freemsa, uint_t freepgs, bafhlst_t *relbf, bafhlst_t *merbf)
{
	sint_t rets = 0;

	// 要合并的msadsc首页面和尾页面
	msadsc_t *mnxs = freemsa, *mnxe = &freemsa[freepgs - 1];

	// 从relbf开始遍历
	bafhlst_t *tmpbf = relbf;
	for (; tmpbf < merbf; tmpbf++)
	{
		// 查看最大的连续的且空闲的msadsc结构
		rets = mm_find_cmsa2blk(tmpbf, &mnxs, &mnxe);
		if (rets == 1) {
			break;
		}

		if (rets == 0) {
			system_error("mm_find_cmsa2blk retn 0\n");
		}
	}

	// 将合并的msadsc加入到对应的bafhlst结构中
	if (mpobf_add_msadsc(tmpbf, mnxs, mnxe) == FALSE) {
		return FALSE;
	}

	return TRUE;
}

bool_t mm_merpages_onmarea(memarea_t *malckp, msadsc_t *freemsa, uint_t freepgs)
{
	if (malckp == NULL || freemsa == NULL || freepgs < 1) {
		return FALSE;
	}

	bafhlst_t *prcbf = NULL;
	sint_t pocs = 0;
	if (malckp->ma_type == MA_TYPE_PROC)
	{
		prcbf = &malckp->ma_mdmdata.dm_onemsalst;
		pocs = mm_merpages_opmsadsc(prcbf, freemsa, freepgs);
		if (pocs == 2)
		{
			if (mpobf_add_msadsc(prcbf, freemsa, &freemsa[freepgs - 1]) == FALSE) {
				system_error("mm_merpages_onmarea proc memarea merge fail\n");
			}
			mm_update_memarea(malckp, freepgs, 1);
			mm_update_memmgrob(freepgs, 1);
			return TRUE;
		}

		if (pocs == 1) {
			return TRUE;
		}

		if (pocs == 0) {
			return FALSE;
		}

		return FALSE;
	}

	bafhlst_t *retrelbf = NULL, *retmerbf = NULL;
	bool_t rets = FALSE;

	// 根据要释放的freepgs返回 请求释放的 和 最大释放的bafhlst，类似于分配时的
	rets = onfpgs_retn_bafhlst(malckp, freepgs, &retrelbf, &retmerbf);
	if (rets == FALSE) {
		return FALSE;
	}

	if (retrelbf == NULL || retmerbf == NULL) {
		return FALSE;
	}

	// 设置msadsc结构的信息，完成释放，返回1表示不需要下一步合并操作，返回2表示要进行合并操作
	sint_t mopms = mm_merpages_opmsadsc(retrelbf, freemsa, freepgs);
	
	// 合并
	if (mopms == 2)
	{
		// 调用合并函数
		rets = mm_merpages_onbafhlst(freemsa, freepgs, retrelbf, retmerbf);

		if (rets == TRUE) {
			mm_update_memarea(malckp, freepgs, 1);
			mm_update_memmgrob(freepgs, 1);
			return rets;
		}

		return FALSE;
	}

	if (mopms == 1) {
		return TRUE;
	}

	if (mopms == 0) {
		return FALSE;
	}

	return FALSE;
}

// 释放内存核心函数
bool_t mm_merpages_core(memarea_t *marea, msadsc_t *freemsa, uint_t freepgs)
{
	if (marea == NULL || freemsa == NULL || freepgs < 1) {
		return FALSE;
	}

	if (scan_freemsa_isok(freemsa, freepgs) == FALSE) {
		return FALSE;
	}

	bool_t rets = FALSE;
	cpuflg_t cpuflg;

	// 加锁
	knl_spinlock_cli(&marea->ma_lock, &cpuflg);

	// 释放内存
	rets = mm_merpages_onmarea(marea, freemsa, freepgs);

	// 解锁	
	knl_spinunlock_sti(&marea->ma_lock, &cpuflg);

	return rets;
}

// 释放内存框架函数
bool_t mm_merpages_fmwk(memmgrob_t *mmobjp, msadsc_t *freemsa, uint_t freepgs)
{
	// 获取要释放页的所在内存区
	memarea_t *marea = onfrmsa_retn_marea(mmobjp, freemsa, freepgs);
	if (NULL == marea) {
		return FALSE;
	}

	// 调用核心函数
	bool_t rets = mm_merpages_core(marea, freemsa, freepgs);
	if (FALSE == rets) {
		return FALSE;
	}

	return rets;
}

// 释放内存接口函数
// freemsa: 释放内存对应的首个msadsc结构指针
// freepgs: 请求释放的内存页面数
bool_t mm_merge_pages(memmgrob_t *mmobjp, msadsc_t *freemsa, uint_t freepgs)
{
	if (mmobjp == NULL || freemsa == NULL || freepgs < 1) {
		return FALSE;
	}

	// 调用框架函数
	bool_t rets = mm_merpages_fmwk(mmobjp, freemsa, freepgs);
	if (rets == FALSE) {
		return FALSE;
	}

	return rets;
}

void mchkstuc_t_init(mchkstuc_t *initp)
{
	list_init(&initp->mc_list);
	initp->mc_phyadr = 0;
	initp->mc_viradr = 0;
	initp->mc_sz = 0;
	initp->mc_chkval = 0;
	initp->mc_msa = NULL;
	initp->mc_chksadr = NULL;
	initp->mc_chkeadr = NULL;
}

void write_one_mchkstuc(msadsc_t *msa, uint_t pnr)
{
	if (NULL == msa || 0 == pnr) {
		system_error("write_one_mchkstuc msa pnr 0\n");
	}

	u64_t phyadr = msa->md_phyadrs.paf_padrs << PSHRSIZE;
	uint_t viradr = (uint_t)phyadr_to_viradr((adr_t)phyadr);
	uint_t sz = pnr << PSHRSIZE;

	mchkstuc_t *mchks = (mchkstuc_t *)((uint_t)viradr);
	mchkstuc_t_init(mchks);
	mchks->mc_phyadr = phyadr;
	mchks->mc_viradr = viradr;
	mchks->mc_sz = sz;
	mchks->mc_chkval = phyadr;
	mchks->mc_msa = msa;
	mchks->mc_chksadr = (u64_t *)(mchks + 1);
	mchks->mc_chkeadr = (u64_t *)((uint_t)(viradr + sz - 1)); 

	list_add(&mchks->mc_list, &memmgrob.mo_list);
}

bool_t chek_one_mchks(mchkstuc_t *mchs)
{
	if (NULL == mchs) {
		return FALSE;
	}

	msadsc_t *mstat = mchs->mc_msa, *mend = NULL;
	if (MF_OLKTY_ODER != mstat->md_indxflgs.mf_olkty || NULL == mstat->md_odlink)
	{
		kprint("chek_one_mchks 1\n");
		return FALSE;
	}

	mend = (msadsc_t *)mstat->md_odlink;
	if (((uint_t)(mend - mstat) + 1) != (mchs->mc_sz >> PSHRSIZE))
	{
		kprint("chek_one_mchks 2\n");
		return FALSE;
	}
	
	return TRUE;
}

void cmp_mchkstuc(mchkstuc_t *smchs, mchkstuc_t *dmchs)
{
	if (chek_one_mchks(smchs) == FALSE)
		system_error("chek_one_mchks smchs fail\n");
	if (chek_one_mchks(dmchs) == FALSE)
		system_error("chek_one_mchks dmchs fail\n");
	if (smchs->mc_chkval == dmchs->mc_chkval)
		system_error("cmp_mchkstuc smchschkval==dmchschkval\n");
}

void free_one_mchkstuc(mchkstuc_t *mchs)
{
	if (NULL == mchs) {
		system_error("free_one_mchkstuc mchs NULL\n");
	}

	uint_t relnr = (uint_t)(mchs->mc_sz >> PSHRSIZE);
	if (mm_merge_pages(&memmgrob, mchs->mc_msa, relnr) == FALSE) {
		kprint("mm_merge_pages adr:%x,pnr:%x\n", mchs->mc_msa->md_phyadrs.paf_padrs << PSHRSIZE, relnr);
		system_error("free_one_mchkstuc mm_merge_pages ret FALSE\n");
	}

	list_del(&mchs->mc_list);
}

void free_all_mchkstuc()
{
	list_h_t *tmlst = NULL;
	mchkstuc_t *mchs = NULL;
	list_for_each_head_dell(tmlst, &memmgrob.mo_list) {
		mchs = list_entry(tmlst, mchkstuc_t, mc_list);
		free_one_mchkstuc(mchs);
	}

	kprint("free_all_mchkstuc ok\n");
}

void chek_all_one_mchkstuc(mchkstuc_t *mchs)
{
	mchkstuc_t *mchsp = NULL;
	list_h_t *tmplst;
	list_for_each(tmplst, &memmgrob.mo_list)
	{
		mchsp = list_entry(tmplst, mchkstuc_t, mc_list);
		if (mchs != mchsp) {
			cmp_mchkstuc(mchs, mchsp);
		}
	}
}

void chek_all_mchkstuc()
{
	mchkstuc_t *mchs = NULL;
	list_h_t *tmplst;
	uint_t i = 0;
	list_for_each(tmplst, &memmgrob.mo_list)
	{
		mchs = list_entry(tmplst, mchkstuc_t, mc_list);
		chek_all_one_mchkstuc(mchs);
		i++;
		kprint("检查完第%d个内存空间块:正确.....sz:%d:pgs:%d\n", i, mchs->mc_sz, mchs->mc_sz >> PSHRSIZE);
	}
	kprint("全部检查完毕:正确!!\n");
}

uint_t retn_test_sec()
{
	memarea_t *mrp = memmgrob.mo_mareastat, *mar = NULL;
	for (uint_t i = 0; i < memmgrob.mo_mareanr; i++)
	{
		if (mrp[i].ma_type == MA_TYPE_KRNL)
		{
			mar = &mrp[i];
			break;
		}
	}

	if (mar == NULL) {
		return 0;
	}

	return (((uint_t)(mar->ma_maxpages) / 2) / 60) / 60;
}

void test_proc_marea(memarea_t *mr)
{
	msadsc_t *retmsa = NULL;
	uint_t retpnr = 0, pages = 1;
	u64_t stsc = 0, etsc = 0;

	for (;;)
	{
		if (MA_TYPE_PROC != mr->ma_type) {
			break;
		}

		stsc = x86_rdtsc();
		retmsa = mm_divpages_procmarea(&memmgrob, 1, &retpnr);
		etsc = x86_rdtsc();
		if (NULL == retmsa) {
			break;
		}
		write_one_mchkstuc(retmsa, retpnr);
		kprint("所分配连续物理内存页面的首地址:%x,连续物理内存页面数:%d,连续物理内存大小:%dMB,CPU时钟周期:%d,PAGES:%d\n",
			   (uint_t)(retmsa->md_phyadrs.paf_padrs << 12), (uint_t)retpnr, (uint_t)((retpnr << 12) >> 20), (uint_t)(etsc - stsc), (uint_t)pages);
	}

	kprint("TEST PROC_MAREA OK\n");
}

void test_maxone_marea(memarea_t *mr)
{
	msadsc_t *retmsa = NULL;
	uint_t retpnr = 0, pages = 1;
	u64_t stsc = 0, etsc = 0;
	//kprint("mr:%x\n",(uint_t)mr);
	//die(0);
	for (; mr->ma_freepages > pages;)
	{
		/*if(MA_TYPE_PROC==mr->ma_type)
		{
			stsc=x86_rdtsc();
			retmsa=mm_divpages_procmarea(&memmgrob,1,&retpnr);
			etsc=x86_rdtsc();

		}else*/
		{
			stsc = x86_rdtsc();
			retmsa = mm_division_pages(&memmgrob, pages, &retpnr, mr->ma_type, DMF_RELDIV);
			etsc = x86_rdtsc();
		}

		if (NULL != retmsa)
		{
			write_one_mchkstuc(retmsa, retpnr);
			kprint("所分配连续物理内存页面的首地址:%x,连续物理内存页面数:%d,连续物理内存大小:%dMB,CPU时钟周期:%d,PAGES:%d\n",
				   (uint_t)(retmsa->md_phyadrs.paf_padrs << 12), (uint_t)retpnr, (uint_t)((retpnr << 12) >> 20), (uint_t)(etsc - stsc), (uint_t)pages);
		}
		pages++;
	}
}

void test_onedivmer_all(memarea_t *ma)
{
	uint_t pages = 1, retpnr = 0;
	u64_t stsc = 0, etsc = 0;
	msadsc_t *retmsa = NULL;
	//for(;pages<ma->ma_maxpages;pages++)
	{
		for (; pages < ma->ma_maxpages;)
		{
			stsc = x86_rdtsc();
			retmsa = mm_division_pages(&memmgrob, pages, &retpnr, ma->ma_type, DMF_RELDIV);
			etsc = x86_rdtsc();
			if (NULL == retmsa) {
				kprint("in for retmsa==NULL:%x\n", pages);
				break;
			}

			write_one_mchkstuc(retmsa, retpnr);
			kprint("所分配连续物理内存页面的首地址:%x,连续物理内存页面数:%d,连续物理内存大小:%dMB,CPU时钟周期:%d,PAGES:%d\n",
				   (uint_t)(retmsa->md_phyadrs.paf_padrs << 12), (uint_t)retpnr, (uint_t)((retpnr << 12) >> 20), (uint_t)(etsc - stsc), (uint_t)pages);
		}
		chek_all_mchkstuc();
		free_all_mchkstuc();
	}
	kprint("test_onedivmer_all 0k\n");
}

void test_maxdiv_all()
{
	for (uint_t i = 0; i < 3; i++) {
		/*if(MA_TYPE_PROC==memmgrob.mo_mareastat[i].ma_type)
		{
			test_proc_marea(&memmgrob.mo_mareastat[i]);
		}*/
		test_onedivmer_all(&memmgrob.mo_mareastat[i]);
		//test_maxone_marea(&memmgrob.mo_mareastat[i]);
	}
}


void test_divsion_pages()
{
	//uint_t max=2;
	test_maxdiv_all();
	//chek_all_mchkstuc();
	free_all_mchkstuc();

	/*
	
	uint_t pages=1;
	msadsc_t* retmsa=NULL;
	u64_t stsc=0,etsc=0;
	uint_t retpnr=0;
	
	stsc=x86_rdtsc();
	retmsa=mm_division_pages(&memmgrob, pages,&retpnr, MA_TYPE_KRNL,DMF_RELDIV);
	etsc=x86_rdtsc();
	if(NULL==retmsa) {
		system_error("test_divsion_pages retmsa NULL\n");
	}
	
	write_one_mchkstuc(retmsa,retpnr);
	kprint("所分配连续物理内存页面的首地址:%x,连续物理内存页面数:%d,连续物理内存大小:%dMB,CPU时钟周期:%d,PAGES:%d\n",
		((uint_t)retmsa->md_phyadrs.paf_padrs<<12),(uint_t)retpnr,(uint_t)((retpnr<<12)>>20),(uint_t)(etsc-stsc),(uint_t)pages);
	*/

	// uint_t pi=1;pi<max;pi++

	/*
	uint_t retpnr=0;
	uint_t max=2;
	uint_t pages=1;
	msadsc_t* retmsa=NULL;
	uint_t stsc=0,etsc=0;

	kprint("下面开始测试内存分配代码的正确性,根据您的内存容量估算测试时间大约为:%d小时\n您可以选择喝杯茶或者看一看窗外的风景.......\n",retn_test_sec());
	die(0x800);		// 执行内存分配函数所用的

	for(;pages<2000;pages++)
	{
		for(;;pages++)
		{
			stsc=x86_rdtsc();
			retmsa=mm_division_pages(&memmgrob, pages,&retpnr, MA_TYPE_KRNL,DMF_RELDIV);
			etsc=x86_rdtsc();

			if(NULL==retmsa) {
				break;//system_error("test_divsion_pages retmsa NULL\n");
			}
			
			write_one_mchkstuc(retmsa,retpnr);
			kprint("所分配连续物理内存页面的首地址:%x,连续物理内存页面数:%d,连续物理内存大小:%dMB,CPU时钟周期:%d,PAGES:%d\n",
			retmsa->md_phyadrs.paf_padrs<<12,retpnr,(retpnr<<12)>>20,etsc-stsc,pages);
			
			if(mm_merge_pages(&memmgrob,retmsa,retpnr,MA_TYPE_KRNL)==FALSE) {
				system_error("test_divsion_pages free FALSE\n");
			}
		}
	}
	*/

	//chek_all_mchkstuc();
	//kprint("剩余物理内存总量:%d页面 :%d逼 :%d兆逼\n",memmgrob.mo_freepages,memmgrob.mo_freepages<<PSHRSIZE,(memmgrob.mo_freepages<<PSHRSIZE)>>20);
}
