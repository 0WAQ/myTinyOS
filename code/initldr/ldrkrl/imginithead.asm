;
; 用于多引导协议信息结构标头的定义和初始化
; 同时支持GRUB和GRUB2两种引导程序的启动
;
; 主要功能: 初始化CPU的寄存器, 加载GDT, 切换到CPU的保护模式
;

;;; GRUB1和GRUB2需要的两个头结构
;; 符号定义与全局说明
        MBT_HDR_FLAGS EQU 0x00010003    ;定义多引导标头的标志
        MBT_HDR_MAGIC EQU 0x1BADB002    ;定义多引导标头的魔数, 用于识别多引导信息结构
        MBT2_MAGIC    EQU 0xe85250d6    ;定义多引导2表头的魔数

global _start                   ;声明一个全局符号, 可以被链接器和其它模块访问
extern inithead_entry           ;声明一个外部符号, 需要在其它地方定义



;;; 代码段与起始点
[section .text]                 ;声明代码段为.text段, 通常包含可执行代码
[bits 32]                       ;声明代码使用32位指令集


_start:                         ;_start入口点
        jmp _entry                  ;跳转到_entry



;;; GRUB1  
align 4     ;接下来的数据对齐到4字节边界

;定义多引导标头(multiboot header)
mbt_hdr:
        dd MBT_HDR_MAGIC            ;定义多引导魔数
        dd MBT_HDR_FLAGS            ;定义多引导标志
        dd -(MBT_HDR_MAGIC + MBT_HDR_FLAGS)     ;定义检查和, 确保标头的所有字段之和为0
        dd mbt_hdr                  ;指向多引导标头自身的指针
        dd _start                   ;指向程序入口_start
        dd 0                        ;
        dd 0                        ;其余字段填充为0或者_entry, 具体取决于实现需求
        dd _entry                   ;



;;; GRUB2
align 8     ;接下来的数据对齐到8字节边界

;定义多引导2标号
mbhdr:
        dd 0xE85250D6               ;定义多引导2魔数
        dd 0                        ;标头的架构字段(0: i386)
        dd mbhdrend - mbhdr          ;定义标头的总大小
        dd -(0xE85250D6 + 0 + (mbhdrend - mbhdr))    ;定义检查和, 确保标头的所有字段之和为0
        dw 2, 0                     ;定义多引导头部的版本为2
        dd 24                       ;指向头部字段的总大小(除了前8字节)
        dd mbhdr                    ;提供头部开始的指针
        dd _start                   ;.text段的开始指针
        dd 0
        dd 0
        dd 3, 0                     ;OS的类型(3: 兼容32和64位)
        dd 12
        dd _entry                   ;
        dd 0
        dw 0, 0
        dd 8                        ;最后一个标签的大小
mbhdrend:


;;;入口点与保护模式初始化
;;关中断并加载GDT
_entry:
        cli                         ;禁止中断

        ;; 设置RTC寄存器   
        in al, 0x70
        or al, 0x80
        out 0x70, al                ;关闭不可屏蔽中断

        lgdt [GDT_PTR]              ;加载GDT地址到GDTR寄存器
        jmp dword 0x8 :_32bits_mode ;长跳转刷新CS影子寄存器


;; 初始化 段寄存器, 通用寄存器, 栈寄存器, 为了调用inithead_entry这个C函数做准备
_32bits_mode:
        mov ax, 0x10
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
    
        mov esp, 0x7c00             ;设置堆栈指针, BIOS引导扇区加载到内存的0x7c00
        call inithead_entry         ;调用外部函数, 去初始化内核
        jmp 0x200000                ;跳转到地址, 开始执行内核代码(initldrkrl.bin文件)


;; GDT全局段描述符表
GDT_START:
knull_dsc: dq 0
kcode_dsc: dq 0x00cf9e000000ffff
kdata_dsc: dq 0x00cf92000000ffff
k16cd_dsc: dq 0x00009e000000ffff    ;16位代码段描述符
k16da_dsc: dq 0x000092000000ffff    ;16位数据段描述符
GDT_END:

;;GDT的基地址和长度
GDT_PTR:
GDTLEN  dw GDT_END - GDT_START - 1  ;GDT界限
GDTBASE dd GDT_START