#include "cmctl.h"

void inithead_entry()
{
    write_realintsvefile();
    write_ldrkrlfile();
    return;
}

// 将initldrsve.bin文件写到0x1000处
void write_realintsvefile()
{
    fhdsc_t* fhdsc_start = find_file("initldrsve.bin");
    if(fhdsc_start == NULL) {
        error("not file initldrsve.bin");
    }
    m2mcopy((void*)((u32_t)(fhdsc_start->fhd_intsf_s) + LDRFILEADR), 
           (void*)REALDRV_PHYADR, (sint_t)fhdsc_start->fhd_freal_sz);
    return;
}

// 将initldrkrl.bin写到特定的内存中
void write_ldrkrlfile()
{
    fhdsc_t* fhdsc_start = find_file("initldrkrl.bin");
    if(fhdsc_start == NULL) {
        error("not file initldrkrl.bin");
    }

    // 将该文件放到0x200000处, 之后再汇编中跳转到该位置, 就进入了二级引导器(即ldrkrl32.asm)
    m2mcopy((void*)((u32_t)(fhdsc_start->fhd_intsf_s) + LDRFILEADR), 
           (void*)ILDRKRL_PHYADR, (sint_t)fhdsc_start->fhd_freal_sz);
    return;
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
        error("no mrddsc");
    }
    
    // 从 映像文件头描述符 中找出 文件头描述符 数组的首地址
    fhdsc_t* fhdsc_arr = (fhdsc_t*)((u32_t)(mrddadrs->mdc_fhdbk_s) + LDRFILEADR);
    
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
    kprint("init loader die ERROR:%s\n", msg);
    do { } while(0);
    return;
}

int strcmpl(const char* a, const char* b)
{
    while(*a && *b && (*a == *b)) {
        a++, b++;
    }
    return *b - *a;
}