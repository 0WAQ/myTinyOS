#include "../include/cmctl.h"

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
        kerror("r_file_to_padr error!\n");
    }

    // 放置完成后, 更新机器信息
    mbsp->mb_krlimgpadr = IMGKRNL_PHYADR;
    mbsp->mb_krlsz = sz;

    // mbsp->mb_nextwtpadr要始终指向下一段空闲内存的首地址
    mbsp->mb_nextwtpadr = P4K_ALIGN(mbsp->mb_krlimgpadr + mbsp->mb_krlsz);
    mbsp->mb_kalldendpadr = mbsp->mb_krlimgpadr + mbsp->mb_krlsz;
}

// 放置字库文件
void init_defutfont(machbstart_t* mbsp)
{
    u32_t dfadr = (u32_t)mbsp->mb_nextwtpadr;

    u64_t sz = r_file_to_padr(mbsp, dfadr, "font.fnt");
    if(sz == 0) {
        kerror("r_file_to_padr error!\n");
    }

    // 放置完成后, 更新机器信息
    mbsp->mb_bfontpadr = (u64_t)dfadr;
    mbsp->mb_bfontsz = sz;

    // 指向下一段空闲内存的首地址
    mbsp->mb_nextwtpadr = P4K_ALIGN(dfadr + sz);
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
        kerror("no mrddsc");
    }

    fhdsc_t *fhdsc_start = (fhdsc_t *)((u32_t)(mrddadrs->mdc_fhdbk_s) + ((u32_t)(mbsp->mb_imgpadr)));

    for (u64_t i = 0; i < mrddadrs->mdc_fhdnr; i++)
        if (strcmpl(fname, fhdsc_start[i].fhd_name) == 0)
            return &fhdsc_start[i];

    kerror("not find file in get_fileinfo()");
    return NULL;
}

u64_t r_file_to_padr(machbstart_t* mbsp, u32_t f2adr, char_t* fnm)
{
    // 判空
    if(f2adr == NULL || fnm == NULL || mbsp == NULL) {
        return 0;
    }

    u32_t fpadr = 0, sz = 0;

    // 获取文件的地址和大小
    get_file_rpadrandsz(fnm, mbsp, &fpadr, &sz);

    // 校验
    if (fpadr == 0 || sz == 0) return 0;

    // 检查内存
    if (NULL == chk_memsize((e820map_t *)((u32_t)mbsp->mb_e820padr), 
                            (u32_t)(mbsp->mb_e820nr), f2adr, sz)) {
        return 0;   
    }

    kprint("123");
    set_curs(0, 1);

    //FIXME: ERROR
    if(chkadr_is_ok(mbsp, f2adr, sz) != 0) {
        return 0;
    }

    kerror("123");

    // 将文件放到指定内存
    m2mcopy((void *)fpadr, (void *)f2adr, (sint_t)sz);
    return sz;
}

void get_file_rpadrandsz(char_t* fname, machbstart_t* mbsp, u32_t* retadr, u32_t* retsz)
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
    // TODO: 第一处
    u64_t padr = (u32_t)(fhdsc->fhd_intsf_s + mbsp->mb_imgpadr);
    if (padr > 0xffffffff) {
        *retadr = 0;
        return;
    }

    u64_t fsz = (u32_t)fhdsc->fhd_freal_sz;
    if (fsz > 0xffffffff) {
        *retadr = 0;
        return;
    }

    *retadr = (u32_t)padr;
    *retsz = (u32_t)fsz;
}

int move_krlimg(machbstart_t* mbsp, u64_t adr, u64_t sz)
{
    // 判断当前空间是否合法
    if(sz < 1 || adr + sz >= 0xffffffff) {
        return 0;
    }

    void* to_adr = (void*)((u32_t)(P4K_ALIGN(adr + sz)));
    sint_t to_sz = (sint_t)mbsp->mb_imgsz;

    if(adrzone_is_ok(mbsp->mb_imgpadr, mbsp->mb_imgsz, adr, sz) != 0) 
    {
        if(NULL == chk_memsize((e820map_t*)((u32_t)(mbsp->mb_e820padr)), 
                        (u32_t)mbsp->mb_e820nr, (u64_t)((u32_t)to_adr), (u64_t)to_sz)) 
        {
            return -1;
        }

        m2mcopy((void*)((u32_t)mbsp->mb_imgpadr), to_adr, to_sz);
        mbsp->mb_imgpadr = (u64_t)((u32_t)to_adr);
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