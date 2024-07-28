#include "cmctl.h"

void inithead_entry()
{
    write_realintsvefile();
    write_ldrkrlfile();
    return;
}

// 将initldrsve.bin文件写到特定的内存中
void write_realintsvefile()
{
    fhdsc_t* fhdsc_start = find_file("initldrsve.bin");
    if(fhdsc_start == NULL) {
        error("not file initldrsve.bin");
    }
    m2mcpy((void*)((u32_t)(fhdsc_start->fhd_intsf_s) + LDRFILEADR), 
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
    m2mcpy((void*)((u32_t)(fhdsc_start->fhd_intsf_s) + LDRFILEADR), 
           (void*)ILDRKRL_PHYADR, (sint_t)fhdsc_start->fhd_freal_sz);
    return;
}

// 在映像文件中查找对应的文件
fhdsc_t* find_file(char_t* fname)
{
    imgfhdsc_t* mrddadrs = MRDDSC_ADR;
    if(mrddadrs->mdc_endgic  != MDC_ENDGIC || 
       mrddadrs->mdc_version != MDC_RVGIC  ||
       mrddadrs->mdc_fhdnr < 2 || 
       mrddadrs->mdc_filnr < 2)
    {
        error("no mrddsc");
    }

    s64_t rethn = -1;
    fhdsc_t* fhdsc_start = (fhdsc_t*)((u32_t)(mrddadrs->mdc_fhdbk_s) + LDRFILEADR);
    for(u64_t i = 0; i < mrddadrs->mdc_fhdnr; i++)
    {
        if(strcmpl(fname, fhdsc_start[i].fhd_name) == 0)
        {
            rethn = (s64_t)i;
            goto ok_l;
        }
    }
    rethn = -1;

ok_l:
    if(rethn < 0) {
        error("not find file");
    }
    return &fhdsc_start[rethn];
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