
# 返回第一个可用的回环设备
lodev=$(losetup -f)

# 将hd.img关联到该回环设备上
sudo losetup $lodev ./hd.img

# 挂载hd.img到hdisk目录下
sudo mount -o loop ./hd.img ./hdisk

#
#   ...对硬盘内容的修改...
#

# 将hd.img打包成虚拟机能够识别的格式
# VBoxManage.exe convertfromraw ./hd.img --format VDI ./hd.vdi