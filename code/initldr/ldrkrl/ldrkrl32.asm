_entry:
    cli
    lgdt [GDT_PTR]                  ;加载GDT地址到GDTR寄存器
    lidt [LDT_PTR]                  ;假造LDT地址到LDTR寄存器
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



;;;
realadr_call_entry:
    pushad                          ;保存通用寄存器
    push    ds                      ;
    push    es                      ;保存4个段寄存器
    push    fs                      ;
    push    gs                      ;
    call save_eip_jmp
    pop     gs                      ;
    pop     fs                      ;恢复4个段寄存器
    pop     es                      ;
    pop     ds                      ;
    popad                           ;恢复通用寄存器
    ret

;
save_eip_jmp:
    pop esi                         ;弹出调用save_eip_jmp时保存的eip到esp中
    mov [PM32_EIP_OFF], esi         ;将eip保存到特定的内存空间中
    mov [PM32_ESP_OFF], esp         ;将esp保存到特定的内存空间中
    jmp dword far [cpmty_mode]      ;长跳转, 将cpmty_mode处的第一个4字节装入eip, 把随后的2字节装入cs

;
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


LDT_PTR:
IDTLEN dw 0x3ff
IDTBAS dd 0                         ;BIOS中断表的地址和长度