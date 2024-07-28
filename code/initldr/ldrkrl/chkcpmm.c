#include "cmctl.h"

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

//检查CPU主函数
void init_chkcpu(machbstart_t *mbsp)
{
    if (!chk_cpuid())
    {
        kerror("Your CPU is not support CPUID sys is die!");
        CLI_HALT();
    }
    if (!chk_cpu_longmode())
    {
        kerror("Your CPU is not support 64bits mode sys is die!");
        CLI_HALT();
    }
    mbsp->mb_cpumode = 0x40;//如果成功则设置机器信息结构的cpu模式为64位
    return;
}

