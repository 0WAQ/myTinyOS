### 目录结构
```bash
apps/
| |--helloworld.c
| |--love.c
| |--oneuser.c
|
build/
| |--lmkfbuild
| |--pretreatment.mkf
| |--vbox_win.mkf
| |--vbox.mkf
|
drivers/
| |--net/
| |--drvrfs.c
| |--drvtick.c
| |--drvuart.c
|
exckrnl/
| |--.keep
|
hal/
| |--x86/
| | |--hal_start.c
| | |--init_entry.asm
|
hdisk/
| |--.keep
|
include/
| |--bastypeinc/
| |--drvinc/
| |--halinc/
| |--knlinc/
| |--libinc/
| |--script/
| |--config.h
| |--cosmosmctrl.h
| |--cosmostypes.h
|
initldr/    # 二级引导器，用于解析内核文件和收集机器环境信息
| |--legacy/
| |--script/
| |--uefi/
| |--Makefile
| |--README.md
|
|
kernel/
|
|
lib/
|
|
ovmf/
| |--esp/
| |--OVMF_CODE.fd
| |--OVMF_VARS.fd
|
release/
| |--legacy
| |--uefi
|
|
script/
|
|
|--.gitignore
|--codeline.sh
|--gdbinit
|--Makefile
|--Makefile.x86
|--README.md
```

### 手工生产硬盘

#### 生产虚拟硬盘

```shell
dd bs=512 if=/dev/zero of=hd.img count=204800

# bs: block size, 块大小, 单位为Byte
# if: 表示输入文件, /dev/zero是Linux下返回0数据的文件, 读取就会返回0
# of: 表述输出文件, 即硬盘文件
# count: 表示输出的块总数
# 204800 * 512B = 2^10 * 2^10, 总计100MB
```

#### 格式化虚拟磁盘

1. 查找空闲的回环设备

```shell
sudo losetup -f
```
> 回环设备可以把文件虚拟成Linux块设备, 用来模拟文件系统

2. 将生成的虚拟硬盘关联到查找到的回环设备

```shell
sudo losetup /dev/loopX hd.img
```
> 这里的X是步骤1中找到的设备(范围从0 ~ 11)

3. 格式化这个回环设备, 在其中建立EXT4文件系统

```shell
sudo mkfs.ext4 -q /dev/loopX
```

4. 挂载hd.img文件

```shell
sudo mkdir ./hdisk                      # 创建hdisk目录
sudo mount -o loop ./hd.img ./hdisk     # 将hd.img挂载到hdisk下
sudo mkdir ./hdisk/boot                 # 创建boot目录
```

#### 安装GRUB

1. 安装grub

```shell
sudo grub-install --boot-directory=./hdisk/boot/ --force --allow-floppy /dev/loopX

# --boot-directory: 指定grub安装的目录
# --force: 发生警告不停止安装 
# --allow-floppy: 不安装软盘
# /dev/loopX: 指定虚拟硬盘设备
```

2. 建立grub.cfg文件, 并写入:

```cfg
menuentry 'HelloOS' {
insmod part_msdos
insmod ext2
set root='hd0'           # 硬盘只有一个分区
multiboot2 /boot/HelloOS.eki    # 加载boot目录下的HelloOS.eki文件
boot                            # 引导启动
}

set timeout_style=menu
if [ "${timeout}" = 0]; then
    set timeout = 10            # 等待10s启动
fi
```

#### 转换成虚拟硬盘格式

```shell
VBoxManage convertfromraw ./hd.img --format VDI ./hd.vdi

# convertfromraw: 指向原始格式文件
# --format VDI: 表示转换成虚拟的VDI格式
```
