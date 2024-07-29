### 目录结构

```bash
initldr/
|
|--build/
|
|--include/
| |
| |--base/                  # 最基本的头文件, 定义了各种类型和宏
| | |--chkcpmm_t.h
| | |--imgmgrhead.h
| | |--io.h
| | |--idrasm.inc
| | |--ldrtype.h
| | |--realparm.h
| | |--type.h
| | |--vgastr_t.h
| |
| |--bstartparm.h           # ldrkrl/ 目录中各个.c文件的头
| |--chkcpmm.h
| |--cmctl.h
| |--fs.h
| |--graph.h
| |--inithead.h
| |--ldrkrlentry.h
| |--vgastr.h
|
|--ldrkrl/
| |--bstartparm.c           # 实现收集机器信息, 建立页面数据
| |--chkcpmm.c              # 实现检查CPU工作模式和内存视图
| |--fs.c                   # 实现解析映像文件的功能
| |--graph.c                # 实现切换显卡图形模式
| |--imginithead.asm        # GRUB头的汇编部分
| |--inithead.c             # GRUB头的C语言部分, 用于防止二级引导器到指定内存中
| |--ldrkrl32.asm           # 二级引导器核心入口汇编部分
| |--ldrkrlentry.c          # 二级引导器核心入口C语言部分
| |--realintsave.asm        # 实现调用BIOS中断的功能
| |--vgastr.c               # 实现字符串输出
|
|--Makefile
|--README.md
```