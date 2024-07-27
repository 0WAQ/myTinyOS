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
    machbstart_t_init(msbp);
    return;
}