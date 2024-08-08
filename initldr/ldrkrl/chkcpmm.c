#include "../include/cmctl.h"

/**
 * 
 * CPU相关
 * 
 */

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
    return;
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
    for(u32_t i = 0; i < enr; i++) 
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
    if(move_krlimg(mbsp, (u64_t)(0x8f000), 0x1001) < 1) {
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
