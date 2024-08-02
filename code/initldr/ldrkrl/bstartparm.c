#include "cmctl.h"

// 初始化machbstart_t结构体, 清0, 并设置标志
void machbstart_t_init(machbstart_t* initp)
{
    memset(initp, 0, sizeof(machbstart_t));
    initp->mb_migc = MBS_MIGC;
    return;
}

void init_bstartparm()
{
    machbstart_t* mbsp = MBSPADR; // 1MB的内存地址
    
    // 初始化机器信息
    machbstart_t_init(mbsp);
    
    // 检查CPU
    init_chkcpu(mbsp);

    // 获取内存布局
    init_mem(mbsp);

    // 初始化内核栈
    init_krlinitstack(mbsp);

    // 放置内核文件
    init_krlfile(mbsp);

    // 放置字库文件
    init_defutfont(mbsp);

    // 将init_mem中获取到的内存信息转存到字体文件之后
    init_meme820(mbsp);

    // 建立MMU页表
    init_bstartpages(mbsp);

    // 设置图形模式
    init_graph(mbsp);

    return;
}


/**
 * 
 * 
 * 
 */

int adrzone_is_ok(u64_t sadr, u64_t slen, u64_t kadr, u64_t klen)
{
    if(kadr >= sadr && kadr <= (sadr + slen)) {
        return -1;
    }

    if(kadr <= sadr && (kadr + klen >= sadr)) {
        return -2;
    }

    return 0;
}

int chkadr_is_ok(machbstart_t *mbsp, u64_t chkadr, u64_t cksz)
{
    //u64_t len=chkadr+cksz;
    if (adrzone_is_ok((mbsp->mb_krlinitstack - mbsp->mb_krlitstacksz), mbsp->mb_krlitstacksz, chkadr, cksz) != 0)
        return -1;
    
    if (adrzone_is_ok(mbsp->mb_imgpadr, mbsp->mb_imgsz, chkadr, cksz) != 0)
        return -2;

    if (adrzone_is_ok(mbsp->mb_krlimgpadr, mbsp->mb_krlsz, chkadr, cksz) != 0)
        return -3;
    
    if (adrzone_is_ok(mbsp->mb_bfontpadr, mbsp->mb_bfontsz, chkadr, cksz) != 0)
        return -4;
    
    if (adrzone_is_ok(mbsp->mb_e820padr, mbsp->mb_e820sz, chkadr, cksz) != 0)
        return -5;
    
    if (adrzone_is_ok(mbsp->mb_memznpadr, mbsp->mb_memznsz, chkadr, cksz) != 0)
        return -6;
    
    if (adrzone_is_ok(mbsp->mb_memmappadr, mbsp->mb_memmapsz, chkadr, cksz) != 0)
        return -7;

    return 0;
}
