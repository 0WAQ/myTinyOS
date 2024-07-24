lodev=$(losetup -f)
sudo losetup $lodev hd.img

sudo mkfs.ext4 -q $lodev

sudo mkdir ./hdisk
sudo mkdir -o loop ./hd.img ./hdisk
sudo mkdir ./hdisk/boot

sudo grub-install --boot-directory=./hdisk/boot --force --allow-floppy $lodev


VBoxManage.exe convertfromraw ./hd.img --format VDI ./hd.vdi