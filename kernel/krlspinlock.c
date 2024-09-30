/****************************************************************
        kernel自旋锁文件krlspinlock.c
****************************************************************/
#include "cosmostypes.h"
#include "cosmosmctrl.h"

void krlspinlock_init(spinlock_t* lock)
{
#if((defined CFG_X86_PLATFORM))       
    knl_spinlock_init(lock);
#endif  // (defined CFG_X86_PLATFORM)
}

void krlspinlock_lock(spinlock_t* lock)
{
#if((defined CFG_X86_PLATFORM))       
    knl_spinlock_lock(lock);
#endif  // (defined CFG_X86_PLATFORM)
}

void krlspinlock_unlock(spinlock_t* lock)
{
#if((defined CFG_X86_PLATFORM))   
    knl_spinlock_unlock(lock);
#endif  // (defined CFG_X86_PLATFORM)
}

void krlspinlock_cli(spinlock_t* lock,cpuflg_t* cpuflg)
{
#if((defined CFG_X86_PLATFORM))       
    knl_spinlock_cli(lock,cpuflg);
#endif  // (defined CFG_X86_PLATFORM)
}

void krlspinunlock_sti(spinlock_t* lock,cpuflg_t* cpuflg)
{
#if((defined CFG_X86_PLATFORM))       
    knl_spinunlock_sti(lock,cpuflg);
#endif  // (defined CFG_X86_PLATFORM)
}