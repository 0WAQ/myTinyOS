/**********************************************************
        内核层内存管理文件krlmm.c
**********************************************************/
#include "cosmostypes.h"
#include "cosmosmctrl.h"

void init_krlmm()
{
    init_krlpagempol();

    // 初始化进程使用的虚拟地址空间
    init_kvirmemadrs();
}

adr_t krlnew(size_t mmsize)
{
    if (mmsize == MALCSZ_MIN || mmsize > MALCSZ_MAX)
    {
        return NULL;
    }
    return kmempool_new(mmsize);
}

bool_t krldelete(adr_t fradr, size_t frsz)
{
    if (fradr == NULL || frsz == MALCSZ_MIN || frsz > MALCSZ_MAX)
    {
        return FALSE;
    }
    return kmempool_delete(fradr, frsz);
}
