%include "../include/base/ldrasm.inc"

[bits 16]
_start:
_16_mode:
        mov bp, 0x20                ;0x20是指向GDT中的16位数据段描述符
        mov ds, bp
        mov es, bp
        mov ss, bp
        mov ebp, cr0
        and ebp, 0xfffffffe
        mov cr0, ebp                ;Cr0.P=0 关闭保护模式
        jmp 0:real_entry            ;刷新CS影子寄存器, 进入实模式

;
real_entry:
        mov bp, cs
        mov ds, bp
        mov es, bp
        mov ss, bp                  ;重新设置实模式下的段寄存器, 都是CS中值:0
        mov sp, 0800h               ;设置栈
        mov bp, func_tabke
        add bp, ax
        call [bp]                   ;调用函数表中的汇编函数, ax是C函数传递进来的
        cli
        call disable_nmi
        mov ebp, cr0
        or  ebp, 1
        mov cr0, ebp                ;Cr0.P=1 开启保护模式
        jmp dword 0x8 :_32bits_mode

[BITS 32]
_32bits_mode:
        mov bp, 0x10
        mov ds, bp
        mov ss, bp                  ;重新设置保护模式下的段寄存器0x10是32位数据段描述符的索引
        mov esi, [PM32_EIP_OFF]     ;加载先前保存的EIP
        mov esp, [PM32_ESP_OFF]     ;加载先前保存的ESP
        jmp esi                     ;eip=esi 回到realadr_call_entry函数中

;
func_tabke:                     ;函数表
    dw _getmmap                 ;获取内存布局视图的函数
    dw _read                    ;读取硬盘的函数
        dw _getvbemode          ;获取显卡VBE模式
        dw _getvbeonemodeinfo   ;获取显卡VBE模式的数据
        dw _setvbemode          ;设置显卡VBE模式