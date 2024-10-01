#include "cmctl.h"

/**
 * 
 * 放置内核与字库文件
 * 
 */

// 放置内核文件
void init_krlfile(machbstart_t* mbsp)
{
    // 在映像文件中查找相应的文件, 复制到对应的地址, 并返回文件大小
    u64_t sz = r_file_to_padr(mbsp, IMGKRNL_PHYADR, "Cosmos.bin");
    if(sz == 0) {
        Debug();
        kerror("r_file_to_padr error!\n");
    }

    // 放置完成后, 更新机器信息
    mbsp->mb_krlimgpadr = IMGKRNL_PHYADR;
    mbsp->mb_krlsz = sz;
    // mbsp->mb_nextwtpadr = P4K_ALIGN(mbsp->mb_krlimgpadr + mbsp->mb_krlsz);
    mbsp->mb_kalldendpadr = mbsp->mb_krlimgpadr + mbsp->mb_krlsz;
}

void init_defutfont(machbstart_t *mbsp)
{
    uint_t dfadr = 0;

    u64_t sz = ReadFileToMemPhyAddr(mbsp, &dfadr, "font.fnt");
    if (sz == 0)
    {
        Debug();
        kerror("ReadFileToMemPhyAddr err");
    }

    // 放置完成后, 更新机器信息
    mbsp->mb_bfontpadr = (u64_t)(dfadr);
    mbsp->mb_bfontsz = sz;
    mbsp->mb_kalldendpadr = mbsp->mb_bfontpadr + mbsp->mb_bfontsz;
}

/**
 * 
 * 其它
 * 
 */

u64_t get_wt_imgfilesz(machbstart_t* mbsp)
{
    u64_t retsz = LDRFILEADR;
    imgfhdsc_t* mrddadrs = MRDDSC_ADR;

    if (mrddadrs->mdc_endgic  != MDC_ENDGIC ||
        mrddadrs->mdc_version != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2 ||
        mrddadrs->mdc_filbk_e < 0x4000)
    {
        return 0;
    }

    retsz += mrddadrs->mdc_filbk_e;
    retsz -= LDRFILEADR;
    mbsp->mb_imgpadr = LDRFILEADR;
    mbsp->mb_imgsz = retsz;
    return retsz;
}

fhdsc_t *get_fileinfo(char_t *fname, machbstart_t *mbsp)
{
    imgfhdsc_t *mrddadrs = (imgfhdsc_t*)((u32_t)(mbsp->mb_imgpadr + MLOSDSC_OFF));
    if (mrddadrs->mdc_endgic  != MDC_ENDGIC ||
        mrddadrs->mdc_version != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2)
    {
        Debug();
        kerror("no mrddsc");
    }

    fhdsc_t *fhdsc_start = (fhdsc_t *)((uint_t)(mrddadrs->mdc_fhdbk_s) + ((uint_t)(mbsp->mb_imgpadr)));

    for (u64_t i = 0; i < mrddadrs->mdc_fhdnr; i++)
        if (strcmpl(fname, fhdsc_start[i].fhd_name) == 0)
            return &fhdsc_start[i];

    kerror("not find file in get_fileinfo()");
    return NULL;
}

u64_t r_file_to_padr(machbstart_t *mbsp, uint_t f2adr, char_t *fnm)
{
    if (f2adr == NULL || fnm == NULL || mbsp == NULL) {
        return 0;
    }

    uint_t fpadr = 0, sz = 0;
    get_file_rpadrandsz(fnm, mbsp, &fpadr, &sz);
    if (fpadr == 0 || sz == 0) {
        return 0;
    }

    uint_t start = LoaderAllocMemory(sz);
    if(start == 0 || start != f2adr) {
        return 0;
    }

    if (chkadr_is_ok(mbsp, start, sz) != 0) {
        return 0;
    }

    m2mcopy((void *)fpadr, (void *)start, (sint_t)sz);
    return sz;
}

void get_file_rpadrandsz(char_t* fname, machbstart_t* mbsp, uint_t* retadr, uint_t* retsz)
{
    // 判空
    if (fname == NULL || mbsp == NULL) {
        *retadr = 0;
        return;
    }

    // 获取文件描述结构体
    fhdsc_t *fhdsc = get_fileinfo(fname, mbsp);
    if (fhdsc == NULL) {
        *retadr = 0;
        return;
    }

    // 结果
    u64_t padr = fhdsc->fhd_intsf_s + mbsp->mb_imgpadr;
    if (padr > 0xffffffff) {
        *retadr = 0;
        return;
    }

    u64_t fsz = fhdsc->fhd_freal_sz;
    if (fsz > 0xffffffff) {
        *retadr = 0;
        return;
    }

    *retadr = (uint_t)padr;
    *retsz = (uint_t)fsz;
}

int move_krlimg(machbstart_t* mbsp, u64_t adr, u64_t sz)
{
    // 判断当前空间是否合法
    if(sz < 1 || adr + sz >= 0xffffffff) {
        return 0;
    }

    void* to_adr = (void*)((uint_t)(P4K_ALIGN(adr + sz)));
    sint_t to_sz = (sint_t)mbsp->mb_imgsz;

    if(adrzone_is_ok(mbsp->mb_imgpadr, mbsp->mb_imgsz, adr, sz) != 0) 
    {
        if(NULL == chk_memsize((e820map_t*)((uint_t)(mbsp->mb_e820padr)), 
                        (uint_t)mbsp->mb_e820nr, (u64_t)((uint_t)to_adr), (u64_t)to_sz)) 
        {
            return -1;
        }

        m2mcopy((void*)((u32_t)mbsp->mb_imgpadr), to_adr, to_sz);
        mbsp->mb_imgpadr = (u64_t)((uint_t)to_adr);
        return 1;
    }
    return 2;
}

int strcmpl(const char* a, const char* b)
{
    while(*a && *b && (*a == *b)) {
        a++, b++;
    }
    return *b - *a;
}

void fs_entry()
{
    return;
}

u64_t get_filesz(char_t *filenm, machbstart_t *mbsp)
{
    if (filenm == NULL || mbsp == NULL)
    {
        return 0;
    }
    fhdsc_t *fhdscstart = get_fileinfo(filenm, mbsp);
    if (fhdscstart == NULL)
    {
        return 0;
    }
    return fhdscstart->fhd_freal_sz;
}

uint_t ReadFileToMemPhyAddr(machbstart_t *mbsp, uint_t* f2adr, char_t *fnm)
{
    if(fnm == NULL || mbsp == NULL || f2adr == NULL) {
        return 0;
    }

    uint_t fpadr = 0, sz = 0;
    get_file_rpadrandsz(fnm, mbsp, &fpadr, &sz);
    if(fpadr == 0 || sz == 0) {
        return 0;
    }

    uint_t start = *f2adr;
    if(start == 0)
    {
        start = LoaderAllocMemory(sz);
        if(start == 0) {
            return 0;
        }
    }
    else
    {
        if(MemoryIsFree(start, sz) == FALSE) {
            return 0;
        }
    }
    
    if(chkadr_is_ok(mbsp, start, sz) != 0) {
        return 0;
    }
    
    m2mcopy((void *)fpadr, (void *)start, (sint_t)sz);
    *f2adr = start;
    return sz;
}

u64_t ret_imgfilesz()
{
    u64_t retsz = LDRFILEADR;
    imgfhdsc_t *mrddadrs = MRDDSC_ADR;
    if (mrddadrs->mdc_endgic != MDC_ENDGIC ||
        mrddadrs->mdc_version != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2)
    {
        kerror("no mrddsc");
    }
    if (mrddadrs->mdc_filbk_e < 0x4000) {
        kerror("imgfile error");
    }

    retsz += mrddadrs->mdc_filbk_e;
    retsz -= LDRFILEADR;
    return retsz;
}
