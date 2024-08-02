%include "../include/base/ldrasm.inc"

global _start
[section .text]

[bits 16]

; 存放在0x0018:0x1000处, 被ldrkrl32.asm中的save_eip_jmp(被realadr_call_entry调用)调用
_start:

;进入实模式, 参数在ax中
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
        mov ss, bp                  ;重新设置实模式下的段寄存器, 都是CS中的值, 为0
        mov sp, 0x08000             ;设置栈指针寄存器
        
        mov bp, func_table          ;函数表
        add bp, ax                  ;根据ax的值选择函数表中的哪一个函数
        call [bp]                   ;调用对应的函数
        
        ;调用完之后, 再次进入保护模式

        cli     ;关中断
        call disable_nmi
        mov ebp, cr0
        or  ebp, 1
        mov cr0, ebp                ;Cr0.P=1 开启保护模式
        jmp dword 0x08:_32bits_mode


[bits 32]
_32bits_mode:
        mov bp, 0x10
        mov ds, bp
        mov ss, bp                  ;重新设置保护模式下的段寄存器, 0x10是32位数据段描述符的索引
        mov esi, [PM32_EIP_OFF]     ;加载先前保存的EIP
        mov esp, [PM32_ESP_OFF]     ;加载先前保存的ESP
        jmp esi                     ;eip=esi 回到realadr_call_entry函数中

[bits 16]
;;
DispStr:
    mov bp, ax
    mov cx, 23
    mov ax, 0x1301
    mov bx, 0x000c
    mov dh, 10
    mov dl, 25
    mov bl, 15

    int 0x10
    ret


;;
clearDisp:
    mov ax, 0x0600
    mov bx, 0x0700
    mov cx, 0
    mov dx, 0x184f

    int 0x10
    ret


;;
_getmmap:
        push ds
        push es
        push ss

        mov esi, 0
        mov dword [E80MAP_NR], esi              ;结构体数组的元素个数, 初始为0
        mov dword [E80MAP_ADRADR], E80MAP_ADR   ;e820map结构体的开始地址
 
        xor ebx, ebx    ;清0
        mov edi, E80MAP_ADR
        
    loop:
            mov eax, 0xe820         ;获取e820map结构参数
            mov ecx, 20             ;e820map结构大小
            mov edx, 0x534d4150     ;获取e820map结构参数必须是这个数据
        
            int 0x15                ;BIOS的0x15中断
            jc .1
        
            add edi, 20 ;edi加20
            cmp edi, E80MAP_ADR + 0x1000  ;若edi大于该值, 则出错
            jg .1

            inc esi  ;元素个数自增1

            cmp ebx, 0
            jne loop
        
        jmp .2
    
    .1:
        mov esi, 0          ;出错处理, e820map结构体数组元素个数为0
    .2:
        mov dword [E80MAP_NR], esi  ;写入元素个数

        pop ss
        pop es
        pop ds
        ret


;;
_read:
    push ds
    push es
    push ss

    xor eax, eax
    mov ah, 0x42
    mov dl, 0x80
    mov si, RWHDPACK_ADR

    int 0x13
    jc .err

    pop ss
    pop es
    pop ds
    ret

 .err:
    mov ax, int131_errmsg
    call DispStr

    jmp $
    pop ss
    pop es
    pop ds
    ret


;;
_getvbemode:
    push es
    push ax
    push di

    mov di, VBEINFO_ADR
    mov ax, 0
    mov es, ax
    mov ax, 0x4f00

    int 0x10
    cmp ax, 0x004f
    jz .ok
    
    mov ax, getvbemode_errmsg
    call DispStr
    jmp $

 .ok:
    pop di
    pop ax
    pop es
    ret


;;
_getvbeonemodeinfo:
    push es
    push ax
    push di
    push cx

    mov di, VBEINFO_ADR
    mov ax, 0
    mov es, ax
    mov cx, 0x118
    mov ax, 0x4f01

    int 0x10
    cmp ax, 0x004f
    jz .ok

    mov ax, getvbemodeinfo_errmsg
    call DispStr
    jmp $

 .ok:
    pop cx
    pop di
    pop ax
    pop es
    ret


;;
_setvbemode:
    push ax
    push bx

    mov bx, 0x4118
    mov ax, 0x4f02

    int 0x10
    cmp ax, 0x004f
    jz .ok

    mov ax, setvbemode_errmsg
    call DispStr
    jmp $

 .ok:
    pop bx
    pop ax
    ret


;;
disable_nmi:
    push ax
    
    in al, 0x70
    or al, 0x80
    out 0x70, al
    
    pop ax
    ret


;函数表
func_table: 
    dw _getmmap                 ;获取内存布局视图的函数
    dw _read                    ;读取硬盘的函数
    dw _getvbemode              ;获取显卡VBE模式
    dw _getvbeonemodeinfo       ;获取显卡VBE模式的数据
    dw _setvbemode              ;设置显卡VBE模式


;;
error_message:
 int131_errmsg:
    db "int 131 read hard disk error!"
    db 0
 
 getvbemode_errmsg:
    db "get vbe_mode error!"
    db 0

 getvbemodeinfo_errmsg:
    db "get vbe_mode_info error!"
    db 0

 setvbemode_errmsg:
    db "set vbe_mode error!"
    db 0