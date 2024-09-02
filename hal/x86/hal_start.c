/**********************************************************
        开始入口文件hal_start.c
**********************************************************/

#include "cosmostypes.h"
#include "cosmosmctrl.h"

void hal_start()
{
    // 1. 初始化hal(Hard Abstarct Lay, 硬件抽象层)
    init_hal();
    
    // 2. 初始化内核层 
    init_krl();

    for(;;);
}
