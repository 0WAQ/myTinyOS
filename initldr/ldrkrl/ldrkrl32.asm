%include "../include/base/ldrasm.inc"

global _start
global realadr_call_entry
global IDT_PTR
extern ldrkrl_entry

[section .text]
[bits 32]
_start:
_entry:
        cli
        lgdt [GDT_PTR]                  ;加载GDT地址到GDTR寄存器
        lidt [IDT_PTR]                  ;假造LDT地址到LDTR寄存器
        jmp dword 0x8 :_32bits_mode     ;长跳转影子寄存器


;;初始化相关寄存器
_32bits_mode:
        mov ax, 0x10                    ;数据段选择子(目的)
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        xor eax, eax
        xor ebx, ebx
        xor ecx, ecx
        xor edx, edx
        xor edi, edi
        xor esi, esi
        xor ebp, ebp
        xor esp, esp

        mov esp, 0x90000                ;栈底指针指向0x90000

        call ldrkrl_entry               ;调用ldrkrl_entry函数
        xor ebx, ebx                    ;清空ebx寄存器

        jmp 0x2000000
        jmp $



;;;功能: 在C环境中, 调用BIOS中断
realadr_call_entry:
        ;保存C语言的上下文
        pushad                          ;保存通用寄存器
        push    ds                      ;
        push    es                      ;保存4个段寄存器
        push    fs                      ;
        push    gs                      ;
        
        ;切换回实模式调用BIOS中断, 把中断返回的结果存放在内存中
        call save_eip_jmp
        
        ;恢复C语言的上下文
        pop     gs                      ;
        pop     fs                      ;恢复4个段寄存器
        pop     es                      ;
        pop     ds                      ;
        popad                           ;恢复通用寄存器
        ret

;
save_eip_jmp:

        ;将在realadr_call_entry中调用save_eip_jmp时保存在栈中的eip弹出
        pop esi ;保存在esi中, 方便在跳转到[0x0018:0x1000]之后回到realadr_call_entry
        mov [PM32_EIP_OFF], esi         ;将eip保存到特定的内存空间中
        mov [PM32_ESP_OFF], esp         ;将esp保存到特定的内存空间中
        
        ;跳转到[0x0018:0x1000]处执行代码, 0x18是段描述符索引
        jmp dword far [cpmty_mode]      ;在realintsave.asm中, 在inithead.c中被放入0x1000处

;存放
cpmty_mode:
        dd 0x1000
        dw 0x18
        jmp $



;;; 
GDT_START:
knull_dsc: dq 0                     ;a-e
kcode_dsc: dq 0x00cf9a000000ffff
kdata_dsc: dq 0x00cf92000000ffff
k16cd_dsc: dq 0x00009a000000ffff    ;16位代码段描述符
k16da_dsc: dq 0x000092000000ffff    ;16位数据段描述符
GDT_END:


GDT_PTR:
GDTLEN  dw GDT_END - GDT_START - 1  ;GDT界限
GDTBASE dd GDT_START


IDT_PTR:
IDTLEN dw 0x3ff
IDTBAS dd 0                         ;BIOS中断表的地址和长度