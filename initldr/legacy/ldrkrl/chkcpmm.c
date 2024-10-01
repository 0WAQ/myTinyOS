/**********************************************************
		系统全局内存检查文件chkcpmm.c
**********************************************************/
#include "cmctl.h"

/**
 * 
 * CPU相关
 * 
 */

int chk_cpu_sse()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x1,%%eax \n\t"
        "cpuid \n\t"
        "bt $25,%%edx  \n\t" // sse mode  support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

int chk_cpu_sse2()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x1,%%eax \n\t"
        "cpuid \n\t"
        "bt $26,%%edx  \n\t" // sse mode  support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

int chk_cpu_sse3()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x1,%%eax \n\t"
        "cpuid \n\t"
        "bt $0,%%ecx  \n\t" // sse mode  support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

int chk_cpu_sse4()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x1,%%eax \n\t"
        "cpuid \n\t"
        "bt $19,%%ecx  \n\t" // sse mode  support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

int chk_cpu_cr4osxsave()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl %%cr4,%%eax \n\t"
        "bt $18,%%eax  \n\t" // osxsave support 位
        "setcb %%al \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

int chk_cpu_osxsave()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x1,%%eax \n\t"
        "cpuid \n\t"
        "bt $27,%%ecx  \n\t" // osxsave support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        : "c" (0)
        :);
    return rets;
}

int chk_cpu_avxbit()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x1,%%eax \n\t"
        "cpuid \n\t"
        "bt $28,%%ecx  \n\t" // avx support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        : "c" (0)
        :);
    return rets;
}

int chk_cpu_avx2bit()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x7,%%eax \n\t"
        "cpuid \n\t"
        "bt $5,%%ebx  \n\t" // avx2 support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        : "c" (0)
        :);
    return rets;
}

int chk_cpu_avx512fbit()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x7,%%eax \n\t"
        "cpuid \n\t"
        "bt $16,%%ebx  \n\t" // avx512f support 位
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        : "c" (0)
        :);
    return rets;
}

int chk_cpu_avx2()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x1,%%eax \n\t"
        "cpuid \n\t"
        "andl $0x018000000,%%ecx \n\t"
        "cmpl $0x018000000,%%ecx \n\t"
        "jne 1f \n\t"
        "movl $0,%%ecx \n\t"
        "xgetbv \n\t"
        "andl $0x06,%%eax \n\t"
        "cmpl $0x06,%%eax \n\t"
        "jne 1f \n\t"
        "movl $1,%%eax \n\t"
        "jmp 2f \n\t"
        "1: \n\t"
        "movl $0,%%eax \n\t"
        "2: \n\t"

        : "=a"(rets)
        :
        :);
    return rets;
}

void enable_cpu_osxsave()
{
    __asm__ __volatile__(
        "movl %%cr4,%%eax \n\t"
        // "btsl $9,%%eax\n\t"
        // "btsl $10,%%eax\n\t"
        "btsl $18,%%eax\n\t"//enable osxsave 位
        "movl %%eax,%%cr4  \n\t" 
        :
        :
        : "eax");
    return;   
}

int chk_cpu_avx512()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x7,%%eax \n\t"
        "movl $0x0,%%ecx \n\t"
        "cpuid \n\t"
        "bt $16,%%ebx  \n\t" // avx512  support 位
        "setcb %%al \n\t"
        "jnc 1f \n\t"
        "bt $26,%%ebx  \n\t" // avx512  support 位
        "setcb %%al \n\t"
        "jnc 1f \n\t"
        "bt $27,%%ebx  \n\t" // avx512  support 位
        "setcb %%al \n\t"
        "jnc 1f \n\t"
        "bt $17,%%ebx  \n\t" // avx512  support 位
        "setcb %%al \n\t"
        "jnc 1f \n\t"
        "bt $30,%%ebx  \n\t" // avx512  support 位
        "setcb %%al \n\t"
        "jnc 1f \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

int chk_cpu_avx512reg()
{
    int rets = 0;

    __asm__ __volatile__("xgetbv" : "=a" (rets) : "c" (0) : "%edx");

    if(((rets & 6) == 6) && ((rets & 0xe0) == 0xe0))
    {
        return 1;
    }
    return 0;
}

u32_t get_xcr0low32()
{
    u32_t rets = 0;

    __asm__ __volatile__("xgetbv\n\t" : "=a" (rets) : "c" (0) : "%edx");
    
    return rets;
}

void set_xcr0(u64_t xcr0)
{
    __asm__ __volatile__("xsetbv\n\t" : :"a"((u32_t)(xcr0 & 0xffffffff)), "d"((u32_t)(xcr0 >> 32)), "c" (0) :);
}


//检查CPU
void init_chkcpu(machbstart_t *mbsp)
{
    // 检查CPU是否CPUID
    if (!chk_cpuid())
    {
        kerror("Your CPU is not support CPUID sys is die!");
        CLI_HALT();
    }

    // 检查CPU是否支持长模式
    if (!chk_cpu_longmode())
    {
        kerror("Your CPU is not support 64bits mode sys is die!");
        CLI_HALT();
    }
    mbsp->mb_cpumode = 0x40;    //如果成功则设置机器信息结构的cpu模式为64位


    if (!chk_cpu_sse())
    {
        kprint("Your CPU is not support sse sys is die!");
        CLI_HALT();
    }
    if (!chk_cpu_sse2())
    {
        kprint("Your CPU is not support sse2 sys is die!");
        CLI_HALT();
    }
    if (!chk_cpu_sse3())
    {
        kprint("Your CPU is not support sse3 sys is die!");
        CLI_HALT();
    }
    if (!chk_cpu_sse4())
    {
        kprint("Your CPU is not support sse4 sys is die!");
        CLI_HALT();
    }

    enable_cpu_osxsave();
    set_xcr0(7);
    if (!chk_cpu_cr4osxsave())
    {
        kprint("Your CPU is not support cr4osxsave sys is die!");
        CLI_HALT();
    }
    
    if (!chk_cpu_osxsave())
    {
        kprint("Your CPU is not support osxsave sys is die!");
        CLI_HALT();
    }
    
    if (!chk_cpu_avxbit())
    {
        kprint("Your CPU is not support avx sys is die!");
        CLI_HALT();
    }
    
    if (!chk_cpu_avx2bit())
    {
        kprint("Your CPU is not support avx2 sys is die!");
        CLI_HALT();
    }
    u32_t xr0low = get_xcr0low32();
    kprint("get_xcr0low32:%d\n",xr0low);
}

//通过改写Eflags寄存器的第21位，观察其位的变化判断是否支持CPUID
int chk_cpuid()
{
    int rets = 0;
    __asm__ __volatile__(
        "pushfl \n\t"
        "popl %%eax \n\t"
        "movl %%eax,%%ebx \n\t"
        "xorl $0x0200000,%%eax \n\t"
        "pushl %%eax \n\t"
        "popfl \n\t"
        "pushfl \n\t"
        "popl %%eax \n\t"
        "xorl %%ebx,%%eax \n\t"
        "jz 1f \n\t"
        "movl $1,%0 \n\t"
        "jmp 2f \n\t"
        "1: movl $0,%0 \n\t"
        "2: \n\t"
        : "=c"(rets)
        :
        :);
    return rets;
}

//检查CPU是否支持长模式
int chk_cpu_longmode()
{
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x80000000,%%eax \n\t"
        "cpuid \n\t" //把eax中放入0x80000000调用CPUID指令
        "cmpl $0x80000001,%%eax \n\t"//看eax中返回结果
        "setnb %%al \n\t" //不为0x80000001,则不支持0x80000001号功能
        "jb 1f \n\t"
        "movl $0x80000001,%%eax \n\t"
        "cpuid \n\t"//把eax中放入0x800000001调用CPUID指令，检查edx中的返回数据
        "bt $29,%%edx  \n\t" //长模式 支持位  是否为1
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

unsigned int acpi_get_bios_ebda()
{
    /*
       * There is a real-mode segmented pointer pointing to the
        * 4K EBDA area at 0x40E.
       */
    unsigned int address = *(unsigned short *)0x40E;
    address <<= 4;
    return address; /* 0 means none */
}

int acpi_checksum(unsigned char *ap, s32_t len)
{
    int sum = 0;
    while (len--)
    {
        sum += *ap++;
    }
    return sum & 0xFF;
}

mrsdp_t *acpi_rsdp_isok(mrsdp_t *rdp)
{
    //kprint("acpi_rsdp_isok rnu\n");
    if (rdp->rp_len == 0 || rdp->rp_revn == 0)
    {
        return NULL;
    }
    if (0 == acpi_checksum((unsigned char *)rdp, (s32_t)rdp->rp_len))
    {

        return rdp;
    }

    return NULL;
}

mrsdp_t *findacpi_rsdp_core(void *findstart, u32_t findlen)
{
    if (NULL == findstart || 1024 > findlen)
    {
        return NULL;
    }
    //kprint("ACPI findstart:%x\n",(uint_t)findstart);

    u8_t *tmpdp = (u8_t *)findstart;
    //u64_t* sigp=NULL;
    mrsdp_t *retdrp = NULL;
    for (u64_t i = 0; i <= findlen; i++)
    {
        //sigp=(u64_t*)(&tmpdp[i]);
        //if((RSDP_SIGN)==(*sigp))
        if (('R' == tmpdp[i]) && ('S' == tmpdp[i + 1]) && ('D' == tmpdp[i + 2]) && (' ' == tmpdp[i + 3]) &&
            ('P' == tmpdp[i + 4]) && ('T' == tmpdp[i + 5]) && ('R' == tmpdp[i + 6]) && (' ' == tmpdp[i + 7]))
        {
            retdrp = acpi_rsdp_isok((mrsdp_t *)(&tmpdp[i]));
            if (NULL != retdrp)
            {
                return retdrp;
            }
        }
    }
    return NULL;
}

public mrsdp_t *find_acpi_rsdp()
{

    void *fndp = (void *)acpi_get_bios_ebda();
    mrsdp_t *rdp = findacpi_rsdp_core(fndp, 1024);
    if (NULL != rdp)
    {
        return rdp;
    }
    //0E0000h和0FFFFFH
    fndp = (void *)(0xe0000);
    rdp = findacpi_rsdp_core(fndp, (0xfffff - 0xe0000));
    if (NULL != rdp)
    {
        return rdp;
    }
    return NULL;
}

public void init_acpi(machbstart_t *mbsp)
{
    mrsdp_t *rdp = NULL;
    rdp = find_acpi_rsdp();
    if (NULL == rdp)
    {
        kerror("Your computer is not support ACPI!!");
    }
    m2mcopy(rdp, &mbsp->mb_mrsdp, (sint_t)((sizeof(mrsdp_t))));
    if (acpi_rsdp_isok(&mbsp->mb_mrsdp) == NULL)
    {
        kerror("Your computer is not support ACPI!!");
    }
    return;
}


/**
 * 
 * 内存相关
 * 
 */

// 1. 获取内存布局(e820map_t这个结构体数组), 利用mmap函数
// 2. 检查内存大小(因为内核对内存容量有要求, 不能太小)
void init_mem(machbstart_t* mbsp)
{
    e820map_t* arr; // 内存的结构体数组
    u32_t n = 0;    // 元素个数

    mbsp->mb_ebdaphyadr = acpi_get_bios_ebda();
    mmap(&arr, &n);

    // 没有内存段
    if(n == 0) {
        kerror("no e820map\n");
    }

    //根据结构体检查内存大小
    if(chk_memsize(arr, n, 0x100000, 0x8000000) == NULL) {
        kerror("Your computer is low on memory, the memory cannot be less than 128MB!");
    }

    //检查成功, 将相关信息传递给机器信息结构
    mbsp->mb_e820padr = (u64_t)((u32_t)arr);
    mbsp->mb_e820nr = (u64_t)n;
    mbsp->mb_e820sz = n * sizeof(e820map_t);
    mbsp->mb_memsz = get_memsize(arr, n); //计算总的内存大小
    init_acpi(mbsp);
}

void init_meme820(machbstart_t* mbsp)
{
    e820map_t* semp = (e820map_t*)((u32_t)(mbsp->mb_e820padr));
    u64_t senr = mbsp->mb_e820nr;

    e820map_t* demp = (e820map_t*)((u32_t)(mbsp->mb_nextwtpadr));
    if(move_krlimg(mbsp, (u64_t)((u32_t)demp), (senr * (sizeof(e820map_t)))) < 1) {
        kerror("move_krlimg error!\n");
    }

    m2mcopy(semp, demp, (sint_t)(senr * sizeof(e820map_t)));

    mbsp->mb_e820padr = (u64_t)((u32_t)demp);
    mbsp->mb_e820sz = senr * sizeof(e820map_t);
    mbsp->mb_nextwtpadr = P4K_ALIGN((u32_t)demp + (u32_t)(senr * sizeof(e820map_t)));
    mbsp->mb_kalldendpadr = mbsp->mb_e820padr + mbsp->mb_e820sz;
}

// 获取内存布局结构体
void mmap(e820map_t** retemp, u32_t* retemnr)
{
    // 调用BIOS中断, 并且根据参数在函数表(realintsave.asm)中执行对应的函数(_getmmap)
    realadr_call_entry(RLINTNR(0), 0, 0);
    *retemnr = *((u32_t*)(E80MAP_NR));
    *retemp = (e820map_t*)(*((u32_t*)(E80MAP_ADRADR)));
}

u64_t get_memsize(e820map_t* e8p, u32_t enr)
{
    // 数据不合法
    if(enr == 0 || e8p == NULL) {
        return 0;
    }

    u64_t tot = 0;
    for(size_t i = 0; i < enr; i++) 
    {
        // 确保内存大小可用
        if(e8p[i].type == RAM_USABLE)
            tot += e8p[i].lsize;
    }
    return tot;
}

e820map_t* chk_memsize(e820map_t* e8p, u32_t enr, u64_t sadr, u64_t size)
{
    if(enr == 0 || e8p == NULL) {
        return NULL;
    }

    u64_t len = sadr + size;
    for(size_t i = 0; i < enr; i++) 
    {
        if(e8p[i].type == RAM_USABLE) 
        {
            if(sadr >= e8p[i].saddr && len < (e8p[i].saddr + e8p[i].lsize))
                return &e8p[i];
        }
    }
    return NULL;
}


/**
 * 
 * 初始化内核栈
 * 
 */

// 给内核提供一个栈
void init_krlinitstack(machbstart_t* mbsp)
{
    // 判断内核栈空间(0x8f000 ~ 0x90001)是否和其它空间有冲突
    if(move_krlimg(mbsp, (u64_t)(IKSTACK_PHYADR - IKSTACK_SIZE), 0x1001) < 1) {
        kerror("iks_moveimg error!\n");
    }

    mbsp->mb_krlinitstack = IKSTACK_PHYADR; //栈顶地址
    mbsp->mb_krlitstacksz = IKSTACK_SIZE;   //栈大小, 4KB
}

/**
 * 
 * 建立MMU页表数据
 *   
 */

void init_bstartpages(machbstart_t* mbsp)
{
    // 顶级页目录
    u64_t *p = (u64_t*)(KINITPAGE_PHYADR);   // 16MB 地址处
    
    // 页目录指针
    u64_t* pdpte = (u64_t*)(KINITPAGE_PHYADR + 0x1000);

    // 页目录
    u64_t* pde = (u64_t*)(KINITPAGE_PHYADR + 0x2000);

    // 判断存放页的地址是否有冲突
    if(move_krlimg(mbsp, (u64_t)(KINITPAGE_PHYADR), (0x1000 * 16 + 0x2000)) < 1) {
        kerror("move_krlimg error!\n");
    }

    // 物理地址从 0 开始
    u64_t adr = 0;

    // 将顶级页目录 和 页指针的空间清0
    for(size_t i = 0; i < PGENTY_SIZE; i++) {
        p[i] = 0;
        pdpte[i] = 0;
    }

    // 映射
    for(size_t i = 0; i < 16; i++) {
        pdpte[i] = (u64_t)((u32_t)pde | KPDPTE_RW | KPDPTE_P);
        for(size_t j = 0; j < PGENTY_SIZE; j++) {
            pde[j] = 0 | adr | KPDE_PS | KPDE_RW | KPDE_P;
            adr += 0x200000;
        }
        pde = (u64_t*)((u32_t)pde + 0x1000);
    }

    // 让顶级页目录中第0项和第(...)指向同一个页目录指针
    p[((KRNL_VIRTUAL_ADDRESS_START) >> KPML4_SHIFT) & 0x1ff] = (u64_t)((u32_t)pdpte | KPML4_RW | KPML4_P);
    p[0] = (u64_t)((u32_t)pdpte | KPML4_RW | KPML4_P);

    // 将页表首地址保存在机器信息结构中
    mbsp->mb_pml4padr = (u64_t)(KINITPAGE_PHYADR);
    mbsp->mb_subpageslen = (u64_t)(0x1000 * 16 + 0x2000);
    mbsp->mb_kpmapphymemsz = (u64_t)(0x400000000);
}


void init_chkmm()
{

    e820map_t *map = (e820map_t *)EMAP_PTR;
    u16_t *map_nr = (u16_t *)EMAP_NR_PTR;
    u64_t mmsz = 0;

    //kprint("chkmm_start!");

    for (int j = 0; j < (*map_nr); j++)
    {
        if (map->type == RAM_USABLE)
        {
            mmsz += map->lsize;
        }
        map++;
    }

    if (mmsz < BASE_MEM_SZ) //0x3F00000
    {
        kprint("Your computer is low on memory, the memory cannot be less than 64MB!");
        CLI_HALT();
    }

    if (!chk_cpuid())
    {
        kprint("Your CPU is not support CPUID sys is die!");
        CLI_HALT();
    }

    if (!chk_cpu_longmode())
    {
        kprint("Your CPU is not support 64bits mode sys is die!");
        CLI_HALT();
    }
    
    ldr_createpage_and_open();
    //for(;;);
    return;
}

void out_char(char *c)
{

    char *str = c, *p = (char *)0xb8000;

    while (*str)
    {
        *p = *str;
        p += 2;
        str++;
    }

    return;
}

void init_bstartpagesold(machbstart_t *mbsp)
{

    if (1 > move_krlimg(mbsp, (u64_t)(PML4T_BADR), 0x3000))
    {
        kerror("ip_moveimg err");
    }

    pt64_t *pml4p = (pt64_t *)PML4T_BADR, *pdptp = (pt64_t *)PDPTE_BADR, *pdep = (pt64_t *)PDE_BADR; //*ptep=(pt64_t*)PTE_BADR;
    for (int pi = 0; pi < PG_SIZE; pi++)
    {
        pml4p[pi] = 0;
        pdptp[pi] = 0;

        pdep[pi] = 0;
    }

    pml4p[0] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;
    pdptp[0] = 0 | PDE_BADR | PDT_S_RW | PDT_S_PNT;
    pml4p[256] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;

    pt64_t tmpba = 0, tmpbd = 0 | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;

    for (int di = 0; di < PG_SIZE; di++)
    {
        pdep[di] = tmpbd;
        tmpba += 0x200000;
        tmpbd = tmpba | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;
    }
    mbsp->mb_pml4padr = (u64_t)((u32_t)pml4p);
    mbsp->mb_subpageslen = 0x3000;
    mbsp->mb_kpmapphymemsz = (0x200000 * 512);
    return;
}

void ldr_createpage_and_open()
{
    pt64_t *pml4p = (pt64_t *)PML4T_BADR, *pdptp = (pt64_t *)PDPTE_BADR, *pdep = (pt64_t *)PDE_BADR; //*ptep=(pt64_t*)PTE_BADR;
    //*pdptp1=(pt64_t*)PDPTE1_BADR,*pde2p=(pt64_t*)PDE2_BADR, *pde1p=(pt64_t*)PDE1_BADR,*ptep1=(pt64_t*)PTE1_BADR ,
    for (int pi = 0; pi < PG_SIZE; pi++)
    {
        pml4p[pi] = 0;
        pdptp[pi] = 0;
        //pdptp1[pi]=0;
        pdep[pi] = 0;
        //pde1p[pi]=0;
        //pde2p[pi]=0;
        //ptep[pi]=0;
        //ptep1[pi]=0;
    }

    pml4p[0] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;
    pdptp[0] = 0 | PDE_BADR | PDT_S_RW | PDT_S_PNT;
    //pdep[0]=0|PTE_BADR|PDT_S_RW|PDT_S_PNT;

    pml4p[256] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;

    pt64_t tmpba = 0, tmpbd = 0 | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;

    for (int di = 0; di < PG_SIZE; di++)
    {
        pdep[di] = tmpbd;
        //pde1p[di]=tmpbd;
        tmpba += 0x200000;
        tmpbd = tmpba | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;
    }

    /*pt64_t* adp,add=0,adttp=0;
	for(int ti=0;ti<33;ti++)
	{
		adp=(pt64_t*)((int)pdep[ti]&0xfffff000);
		for(int adi=0;adi<PG_SIZE;adi++)
		{
			adttp=add|PT_S_RW|PT_S_PNT;
			adp[adi]=adttp;
			add+=0x1000;
			
		}
	}*/
    //;;0x4000000
    return;
}
