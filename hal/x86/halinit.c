/****************************************************************
        Cosmos HAL全局初始化文件halinit.c
****************************************************************/

#include "cosmostypes.h"
#include "cosmosmctrl.h"


void init_hal()
{
    // 初始化平台
    init_halplatform();
    
    // 初始化内存
    move_img2maxpadr(&kmachbsp);
    init_halmm();
    
    // 初始化中断
    init_halintupt();

    kprint("HAL层初始化成功\n");
}
