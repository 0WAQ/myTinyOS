#include "cmctl.h"

LoaderInfo* GLInfo = NULL;

bool_t HeadMemoryIsFree(LoaderInfo* info, uint_t start, uint_t size)
{
    uint_t end = start + size;
    e820map_t* emap;

    if(info == NULL) {
        return FALSE;
    }

    emap = info->Mem.E820Map;

    for(uint_t i = 0; i < info->Mem.E820MapNR; i++)
    {
        if(emap[i].type == RAM_USABLE)
        {
            if((start >= emap[i].saddr) && 
                (emap[i].saddr + (emap[i].lsize - 1) > end))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void inithead_entry(LoaderInfo* info)
{
    GLInfo = info;
    init_curs(info);
    clear_screen(VGADP_DFVL);
    write_ldrkrlfile();
}

// 将initldrsve.bin文件写到0x1000处
void write_realintsvefile()
{
    fhdsc_t *fhdscstart = find_file("initldrsve.bin");
    if (fhdscstart == NULL) {
        error("not file initldrsve.bin");
    }

    if(HeadMemoryIsFree(GLInfo, REALDRV_PHYADR, (uint_t)fhdscstart->fhd_freal_sz) == FALSE) {
        error("not Memory space");
    }

    m2mcopy((void *)((uint_t)(fhdscstart->fhd_intsf_s) + LDRFILEADR),
            (void *)REALDRV_PHYADR, (sint_t)fhdscstart->fhd_freal_sz);
}

// 将initldrkrl.bin文件写到0x200000处
void write_ldrkrlfile()
{
    fhdsc_t *fhdscstart = find_file("initldrkrl.bin");
    if (fhdscstart == NULL) {
        error("not file initldrkrl.bin");
    }

    if(HeadMemoryIsFree(GLInfo, ILDRKRL_PHYADR, (uint_t)fhdscstart->fhd_freal_sz) == FALSE) {
        error("not Memory space");
    }
    
    m2mcopy((void *)((uint_t)(fhdscstart->fhd_intsf_s) + LDRFILEADR),
            (void *)ILDRKRL_PHYADR, (sint_t)fhdscstart->fhd_freal_sz);
}

// 在映像文件中查找对应的文件
fhdsc_t* find_file(char_t* fname)
{
    // 从内存中取出映像文件的映像文件头描述符
    imgfhdsc_t* mrddadrs = MRDDSC_ADR;

    if(mrddadrs->mdc_endgic  != MDC_ENDGIC || 
       mrddadrs->mdc_version != MDC_RVGIC  ||
       mrddadrs->mdc_fhdnr < 2 || 
       mrddadrs->mdc_filnr < 2)
    {
        error("no mrddsc in find_file()");
    }
    
    // 从 映像文件头描述符 中找出 文件头描述符 数组的首地址
    fhdsc_t* fhdsc_arr = (fhdsc_t*)((uint_t)(mrddadrs->mdc_fhdbk_s) + LDRFILEADR);
    
    // 遍历所有文件头描述符
    for(u64_t i = 0; i < mrddadrs->mdc_fhdnr; i++) {
        // 通过文件名找到该文件
        if(strcmpl(fname, fhdsc_arr[i].fhd_name) == 0)
            return &fhdsc_arr[i];
    }

    error("not find file");
    return NULL;
}

void error(char_t* msg)
{
    Debug();
    kprint("INITLDR DIE ERROR:%s\n", msg);
    for(;;);
}

int strcmpl(const char* a, const char* b)
{
    while(*a && *b && (*a == *b)) {
        a++, b++;
    }
    return *b - *a;
}