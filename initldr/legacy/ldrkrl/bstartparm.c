#include "cmctl.h"

void init_bstartparm()
{
    machbstart_t* mbsp = MBSPADR; // 1MB的内存地址
    
    // 初始化机器信息
    machbstart_t_init(mbsp);
    
    // 检查CPU
    init_chkcpu(mbsp);

    // 获取内存布局
    init_mem(mbsp);

    // 获取映像文件
    if(get_wt_imgfilesz(mbsp) == 0) {
        kerror("imgfilesz 0");
    }

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

    die(0x400);
}

// 初始化machbstart_t结构体, 清0, 并设置标志
void machbstart_t_init(machbstart_t* initp)
{
    memset(initp, 0, sizeof(machbstart_t));
    initp->mb_migc = MBS_MIGC;
}

int adrzone_is_ok(u64_t sadr, u64_t slen, u64_t kadr, u64_t klen)
{
    if(kadr >= sadr && kadr <= (sadr + slen)) {
        return -1;
    }

    if(kadr <= sadr && (kadr + klen) >= sadr) {
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

void disp_mbsp(machbstart_t *mbsp)
{
    kprint("mbsp->mb_imgpadr:%x\n", mbsp->mb_imgpadr);
    kprint("mbsp->mb_imgsz:%x\n", mbsp->mb_imgsz);
    kprint("mbsp->mb_krlimgpadr:%x\n", mbsp->mb_krlimgpadr);
    kprint("mbsp->mb_krlimgsz:%x\n", mbsp->mb_krlsz);
    kprint("mbsp->mb_e820padr:%x\n", mbsp->mb_e820padr);
    kprint("mbsp->mb_e820nr:%x\n", mbsp->mb_e820nr);
    kprint("mbsp->mb_e820sz:%x\n", mbsp->mb_e820sz);
    kprint("mbsp->mb_nextwtpadr:%x\n", mbsp->mb_nextwtpadr);
    kprint("mbsp->mb_kalldendpadr:%x\n", mbsp->mb_kalldendpadr);
    kprint("mbsp->mb_pml4padr:%x\n", mbsp->mb_pml4padr);
    kprint("mbsp->mb_subpageslen:%x\n", mbsp->mb_subpageslen);
    kprint("mbsp->mb_kpmapphymemsz:%x\n", mbsp->mb_kpmapphymemsz);
    kprint("mbsp->mb_cpumode:%x\n", mbsp->mb_cpumode);
    kprint("mbsp->mb_memsz:%x\n", mbsp->mb_memsz);
    kprint("mbsp->mb_krlinitstack:%x\n", mbsp->mb_krlinitstack);
    kprint("mbsp->mb_krlitstacksz:%x\n", mbsp->mb_krlitstacksz);
    kprint("mbsp->mb_bfontpadr:%x\n", mbsp->mb_bfontpadr);
    kprint("mbsp->mb_bfontsz:%x\n", mbsp->mb_bfontsz);

    die(0);
}

void testm2m()
{
    u8_t *s = (u8_t *)0x400000;
    u8_t *d = (u8_t *)(0x400000 - 1);
    for (u8_t i = 0; i < 10; i++)
        s[i] = i;

    for (u8_t j = 0; j < 10; j++)
        kprint("s[%d]:%d\n", j, s[j]);

    m2mcopy(s, d, 10);
    
    for (u8_t k = 0; k < 10; k++)
        kprint("d[%d]:%d\n", k, d[k]);

    die(0);
}