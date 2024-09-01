/**
 * 
 * 引导器类型文件ldrtype.h
 * 
 */

#ifndef _LDRTYPE_H
#define _LDRTYPE_H

#include "type.h"

#define BFH_RW_R 1
#define BFH_RW_W 2

#define BFH_BUF_SZ 0x1000
#define BFH_ONEPW_SZ 0x1000
#define BFH_RWONE_OK 1
#define BFH_RWONE_ER 2
#define BFH_RWALL_OK 3

#define FHDSC_NMAX 192
#define FHDSC_SZMAX 256
#define MDC_ENDGIC 0xaaffaaffaaffaaff
#define MDC_RVGIC  0xffaaffaaffaaffaa
#define REALDRV_PHYADR  0x1000
#define ILDRKRL_PHYADR  0x200000
#define IMSGHEL_PHYADR  0x30000

// 内核的 栈顶地址(0x8fff0) 和 栈大小(64kB), 范围从0x8f000 ~ 0x8fff0
#define IKSTACK_PHYADR  (0x90000 - 0x10)
#define IKSTACK_SIZE    0x1000

#define IMGFILE_PHYADR  0x4000000
#define IMGKRNL_PHYADR  0x2000000

// MMU页表相关
#define KINITPAGE_PHYADR 0x1000000  // 顶级页目录地址

#define KINITRVM_PHYADR 0x800000
#define KINITRVM_SZ     0x400000

// 映像文件的物理地址
#define LDRFILEADR  IMGFILE_PHYADR
#define MLOSDSC_OFF (0x1000)

// 映像文件头描述符的物理地址, 前0x1000个字节正好是4KB, 是GRUB头
#define MRDDSC_ADR  (imgfhdsc_t*)(LDRFILEADR + 0x1000)

#define KRNL_VIRTUAL_ADDRESS_START 0xffff800000000000
#define KPML4_P (1 << 0)
#define KPML4_RW (1 << 1)
#define KPML4_US (1 << 2)
#define KPML4_PWT (1 << 3)
#define KPML4_PCD (1 << 4)
#define KPML4_A (1 << 5)

#define KPDPTE_P (1 << 0)
#define KPDPTE_RW (1 << 1)
#define KPDPTE_US (1 << 2)
#define KPDPTE_PWT (1 << 3)
#define KPDPTE_PCD (1 << 4)
#define KPDPTE_A (1 << 5)

#define KPDE_P (1 << 0)
#define KPDE_RW (1 << 1)
#define KPDE_US (1 << 2)
#define KPDE_PWT (1 << 3)
#define KPDE_PCD (1 << 4)
#define KPDE_A (1 << 5)
#define KPDE_D (1 << 6)
#define KPDE_PS (1 << 7)
#define KPDE_G (1 << 8)
#define KPDE_PAT (1 << 12)

#define KPML4_SHIFT 39
#define KPDPTTE_SHIFT 30
#define KPDP_SHIFT 21
#define PGENTY_SIZE 512     // 顶级页的个数

// 文件头描述符
typedef struct s_fhdsc
{
    u64_t fhd_type;             //文件类型
    u64_t fhd_subtype;          //文件子类型
    u64_t fhd_status;           //文件状态
    u64_t fhd_id;               //文件id
    u64_t fhd_intsf_s;          //文件在映像文件的位置的开始偏移
    u64_t fhd_intsf_e;          //文件在映像文件的位置的结束偏移
    u64_t fhd_freal_sz;         //文件的实际大小
    u64_t fhd_fsum;             //文件校验和
    char  fhd_name[FHDSC_NMAX]; //文件名
}fhdsc_t;

// 映像文件头描述符
typedef struct s_imgfhdsc
{
    u64_t mdc_mgic;         //映像文件标识
    u64_t mdc_sfsum;        //未使用
    u64_t mdc_sfsoff;       //未使用
    u64_t mdc_sfeoff;       //未使用
    u64_t mdc_sfrlsz;       //未使用

    u64_t mdc_ldrbk_s;      //映像文件中二级引导器的开始偏移
    u64_t mdc_ldrbk_e;      //映像文件中二级引导器的结束偏移
    u64_t mdc_ldrbk_rsz;    //映像文件中二级引导器的实际大小
    u64_t mdc_ldrbk_sum;    //映像文件中二级引导器的校验和

    u64_t mdc_fhdbk_s;      //映像文件中文件头描述符数组的开始偏移
    u64_t mdc_fhdbk_e;      //映像文件中文件头描述符数组的结束偏移
    u64_t mdc_fhdbk_rsz;    //映像文件中文件头描述符的实际大小
    u64_t mdc_fhdbk_sum;    //映像文件中文件头描述符的校验和

    u64_t mdc_filbk_s;      //映像文件中文件数据的开始偏移
    u64_t mdc_filbk_e;      //映像文件中文件数据的结束偏移
    u64_t mdc_filbk_rsz;    //映像文件中文件数据的实际大小
    u64_t mdc_filbk_sum;    //映像文件在文件数据的校验和

    u64_t mdc_ldrcodenr;    //映像文件中文件头描述符的索引号
    u64_t mdc_fhdnr;        //映像文件中文件头描述符数组中元素的数量
    u64_t mdc_filnr;        //映像文件中文件头的数量??
    u64_t mdc_endgic;       //映像文件结束标志
    u64_t mdc_version;      //映像文件版本
}imgfhdsc_t;

#define RLINTNR(x) (x * 2)

typedef struct s_DPT
{
    u8_t dp_bmgic;
    u8_t dp_pshead;
    u8_t dp_pssector;
    u8_t dp_pscyder;
    u8_t dp_ptype;
    u8_t dp_pehead;
    u8_t dp_pesector;
    u8_t dp_pescyder;
    u32_t dp_pslba;
    u32_t dp_plbasz;
}__attribute__((packed)) dpt_t;

typedef struct s_MBR
{
    char_t mb_type[446];
    dpt_t mb_part[4];
    u16_t mb_endmgic;
}__attribute__((packed)) mbr_t;

typedef struct s_EBR
{
    char_t eb_type[446];
    dpt_t eb_part[4];
    u16_t eb_endmgic;
}__attribute__((packed)) ebr_t;

typedef struct s_IDTR
{
    u16_t idt_len;
    u32_t idt_base;
}idtr_t;

typedef struct s_RWHDPACK
{
    // hd_addr_packet:      db	0x10	; [ 0] Packet size in bytes.
    //                      db	0		; [ 1] Reserved, must be 0.
    // hd_sect_nr:		    db	0		; [ 2] Nr of blocks to transfer.
    //                      db	0		; [ 3] Reserved, must be 0.
    // o_ffset:		        dw	0		; [ 4] Addr of transfer - Offset
    // s_eg:			    dw	0		; [ 6] buffer.          - Seg
    // lba_l:			    dd	0		; [ 8] LBA. Low  32-bits.
    // lba_h:			    dd	0		; [12] LBA. High 32-bits.
    u8_t rwhdpk_sz;
    u8_t rwhdpk_rv;
    u8_t rwhdpk_sn;
    u8_t rwhdpk_rv1;
    u16_t rwhdpk_of;
    u16_t rwhdpk_se;
    u32_t rwhdpk_ll;
    u32_t rwhdpk_lh;
}__attribute__((packed)) rwhdpack_t;

// 以下宏是s_e820中的内存类型
#define RAM_USABLE 1    // 可用内存
#define RAM_RESERV 2    // 保留内存不可使用
#define RAM_ACPIREC 3   // ACPI表相关   
#define RAM_ACPINVS 4   // ACPI NVS空间
#define RAM_AREACON 5   // 包含坏内存

// 内存布局的结构体, 每一个结构体代表内存的一个段
typedef struct s_e820
{
    u64_t saddr;    // 内存开始地址
    u64_t lsize;    // 内存大小
    u32_t type;     // 内存类型
}__attribute__((packed)) e820map_t;

typedef struct s_VBEINFO
{
    char_t vbe_signature[4];
    u16_t vbe_version;
    u32_t oem_string_ptr;
    u32_t capabilities;
    u32_t video_mode_ptr;
    u16_t total_memory;
    u16_t oem_soft_ware_rev;
    u32_t oem_vendor_name_ptr;
    u32_t oem_product_name_ptr;
    u32_t oem_product_rev_ptr;
    u8_t  reserved[222];
    u8_t  oem_data[256];
}__attribute__((packed)) vbeinfo_t;

typedef struct s_VBEOMINFO
{
    u16_t ModeAttributes;
    u8_t  WinAAttributes;
    u8_t  WinBAttributes;
    u16_t WinGranularity;
    u16_t WinSize;
    u16_t WinASegment;
    u16_t WinBSegment;
    u32_t WinFuncPtr;
    u16_t BytesPerScanLine;
    u16_t XResolution;
    u16_t YResolution;
    u8_t  XCharSize;
    u8_t  YCharSize;
    u8_t  NumberOfPlanes;
    u8_t  BitsPerPixel;
    u8_t  NumberOfBanks;
    u8_t  MemoryModel;
    u8_t  BankSize;
    u8_t  NumberOfImagePages;
    u8_t  Reserved;
    u8_t  RedMaskSize;
    u8_t  RedFieldPosition;
    u8_t  GreenMaskSize;
    u8_t  GreenFieldPosition;
    u8_t  BlueMaskSize;
    u8_t  BlueFieldPosition;
    u8_t  RsvdMaskSize;
    u8_t  RsvdFieldPosition;
    u8_t  DirectColorModeInfo;
    u32_t PhysBasePtr;
    u32_t Reserved1;
    u16_t Reserved2;
    u16_t LinBytesPerScanLine;
    u8_t  BnkNumberOfImagePages;
    u8_t  LinNumberOfImagePages;
    u8_t  LinRedMaskSize;
    u8_t  LinRedFieldPosition;
    u8_t  LinGreenMaskSize;
    u8_t  LinGreenFieldPosition;
    u8_t  LinBlueMaskSize;
    u8_t  LinBlueFieldPosition;
    u8_t  LinRsvdMaskSize;
    u8_t  LinRsvdFieldPosition;
    u32_t MaxPixelClock;
    u8_t  Reserved3[189];
}__attribute__((packed)) vbeominfo_t;

// 以下为像素点的数据结构
typedef struct s_PIXCL
{
    u8_t cl_b;  // 蓝
    u8_t cl_g;  // 绿
    u8_t cl_r;  // 红
    u8_t cl_a;  // 透明
}__attribute__((packed)) pixcl_t;

typedef u32_t pixl_t;
#define BGRA(r, g, b) ((0 | (r << 16) | (g << 8) | b))

#define VBEMODE 1
#define GPUMODE 2
#define BGAMODE 3

typedef struct s_GRAPH
{
    u32_t gh_mode;
    u32_t gh_x;
    u32_t gh_y;
    u32_t gh_fram_phyadr;
    u32_t gh_onepixbits;
    u32_t gh_vbe_mode_nr;
    u32_t gh_vif_phyadr;
    u32_t gh_vmif_phyadr;
    u32_t gh_bank;
    u32_t gh_curdip_bnk;
    u32_t gh_next_bnk;
    u32_t gh_bank_sz;
    u32_t gh_logo_phyadrs;
    u32_t gh_logo_phyadre;
    u32_t gh_font_adr;
    u32_t gh_ftsect_adr;
    u32_t gh_ftsect_nr;
    u32_t gh_font_hight;
    u32_t gh_next_charsx;
    u32_t gh_next_charsy;
    u32_t gh_line_sz;
    vbeinfo_t gh_vbeinfo;
    vbeominfo_t gh_vbeminfo;
}__attribute((packed)) graph_t;

typedef struct s_BMFHEAD
{
    u16_t bf_tag;   // 0x4d42
    u32_t bf_size;
    u16_t bf_resd1;
    u16_t bf_resd2;
    u32_t bf_off;
}__attribute__((packed)) bmfhead_t;

typedef struct s_BITMINFO
{
    u32_t bi_size;
    u32_t bi_w;
    u32_t bi_h;
    u16_t bi_planes;
    u16_t bi_bcount;
    u32_t bi_comp;
    u32_t bi_szimg;
    s32_t bi_xpelsper;
    s32_t bi_ypelsper;
    u32_t bi_clruserd;
    u32_t bi_clrimport;
}__attribute__((packed)) bitminfo_t;

typedef struct s_BMDBGR
{
    u8_t bmd_b;
    u8_t bmd_g;
    u8_t bmd_r;
}__attribute__((packed)) bmdbgr_t;

typedef struct s_UTF8
{
    u8_t utf_b1;
    u8_t utf_b2;
    u8_t utf_b3;
    u8_t utf_b4;
    u8_t utf_b5;
    u8_t utf_b6;
}__attribute__((packed)) utf8_t;

typedef struct s_FONTFHDER
{
    u8_t fh_mgic[4];
    u32_t fh_fsize;
    u8_t fh_sect_nr;
    u8_t fh_fhght;
    u16_t fh_wcp_flg;
    u16_t fh_nchars;
    u8_t fh_rev[2];
}__attribute__((packed)) fontfhder_t;

typedef struct s_FTSECTIF
{
    u16_t fsf_fistuc;
    u16_t fsf_lastuc;
    u32_t fsf_off;
}__attribute__((packed)) ftsectif_t;

typedef struct s_UFTINDX
{
    u32_t ui_choff:26;
    u32_t uichwith:6;
}__attribute__((packed)) uftindx_t;

#define MAX_CHARBITBY 32

typedef struct s_FNTDATA
{
    u8_t fnt_wxbyte;
    u8_t fnt_hx;
    u8_t fnt_wx;
    u8_t fnt_chmap[MAX_CHARBITBY];
}__attribute__((packed)) fntdata_t;

typedef struct s_KLFOCPYMBLK
{
    u64_t sphyadr;
    u64_t ocymsz;
}__attribute__((packed)) klfocpymblk_t;

#define MBS_MIGC (u64_t)((((u64_t)'L')<<56)|(((u64_t)'M')<<48)|(((u64_t)'O')<<40)|(((u64_t)'S')<<32)|(((u64_t)'M')<<24)|(((u64_t)'B')<<16)|(((u64_t)'S')<<8)|((u64_t)'P'))

typedef struct s_MRSDP
{
    u64_t rp_sign;
    u8_t rp_chksum;
    u8_t rp_oemid[6];
    u8_t rp_revn;
    u32_t rp_rsdt_phyadr;
    u32_t rp_len;
    u64_t rp_xsdt_phyadr;
    u8_t rp_echksum;
    u8_t rp_resc[3];
}__attribute__((packed)) mrsdp_t;


// 机器信息结构, 由二级引导器收集
typedef struct s_MACHBSTART
{
    u64_t   mb_migc;
    u64_t   mb_chksum;
    u64_t   mb_krlinitstack;    //内核栈地址
    u64_t   mb_krlitstacksz;    //内核栈大小
    u64_t   mb_imgpadr;         //操作系统映像
    u64_t   mb_imgsz;           //操作系统映像大小
    u64_t   mb_krlimgpadr;
    u64_t   mb_krlsz;
    u64_t   mb_krlvec;
    u64_t   mb_krlrunmode;
    u64_t   mb_kalldendpadr;
    u64_t   mb_ksepadrs;
    u64_t   mb_ksepadre;
    u64_t   mb_kservadrs;
    u64_t   mb_kservadre;
    u64_t   mb_nextwtpadr;
    u64_t   mb_bfontpadr;       //操作系统字体地址
    u64_t   mb_bfontsz;         //操作系统字体大小
    u64_t   mb_fvrmphyadr;      //机器显存地址
    u64_t   mb_fvrmsz;          //机器显存大小
    u64_t   mb_cpumode;         //机器CPU工作模式
    u64_t   mb_memsz;           //机器内存大小
    u64_t   mb_e820padr;        //机器e820数组地址
    u64_t   mb_e820nr;          //机器e820数组元素个数
    u64_t   mb_e820sz;          //机器e820数组大小
    u64_t   mb_e820expadr;
    u64_t   mb_e820exnr;
    u64_t   mb_e820exsz;
    u64_t   mb_memznpadr;
    u64_t   mb_memznnr;
    u64_t   mb_memznsz;
    u64_t   mb_memznchksum;
    u64_t   mb_memmappadr;
    u64_t   mb_memmapnr;
    u64_t   mb_memmapsz;
    u64_t   mb_memmapchksum;
    u64_t   mb_pml4padr;        //机器页表数据地址
    u64_t   mb_subpageslen;     //机器页表个数
    u64_t   mb_kpmapphymemsz;   //操作系统映射空间大小
    u64_t   mb_ebdaphyadr;
    mrsdp_t mb_mrsdp;
    graph_t mb_ghparm;          //图形信息
}__attribute__((packed)) machbstart_t;  //取消struct在编译过程中的优化对齐


#define MBSPADR ((machbstart_t*)(0x100000))
#define VBE_DISPI_IOPORT_INDEX (0x01CE)
#define VBE_DISPI_IOPORT_DATA (0x01CF)
#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_WIDTH (6)
#define VBE_DISPI_INDEX_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)
#define BGA_DEV_ID0 (0xB0C0)
#define BGA_DEV_ID1 (0xB0C1)
#define BGA_DEV_ID2 (0xB0C2)
#define BGA_DEV_ID3 (0xB0C3)
#define BGA_DEV_ID4 (0xB0C4)
#define BGA_DEV_ID5 (0xB0C5)
#define VBE_DISPI_BPP_4 (0x04)
#define VBE_DISPI_BPP_8 (0x08)
#define VBE_DISPI_BPP_15 (0x0F)
#define VBE_DISPI_BPP_16 (0x10)
#define VBE_DISPI_BPP_24 (0x18)
#define VBE_DISPI_BPP_32 (0x20)
#define VBE_DISPI_DISABLED (0x00)
#define VBE_DISPI_ENABLED  (0x01)
#define VBE_DISPI_LFB_ENABLED (0x40)


// 会跳转到ldrkrl32.asm中的realadr_call_entry标号
// 功能: 在C环境中调用BIOS中断, 并根据参数选择执行对应的汇编函数
void REGCALL realadr_call_entry(u16_t callint, u16_t val1, u16_t val2);

#endif  // _LDRTYPE_H
