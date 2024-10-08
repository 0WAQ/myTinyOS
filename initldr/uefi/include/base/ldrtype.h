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
#define BFH_ONERW_SZ 0x1000
#define BFH_RWONE_OK 1
#define BFH_RWONE_ER 2
#define BFH_RWALL_OK 3

#define FHDSC_NMAX 192
#define FHDSC_SZMAX 256
#define MDC_ENDGIC 0xaaffaaffaaffaaff
#define MDC_RVGIC 0xffaaffaaffaaffaa
#define REALDRV_PHYADR 0x1000
#define ILDRKRL_PHYADR 0x200000
#define IMGSHEL_PHYADR 0x30000

// 内核的 栈顶地址(0x8fff0) 和 栈大小(64kB), 范围从0x8f000 ~ 0x8fff0
#define IKSTACK_PHYADR (0x90000-0x10)
#define IKSTACK_SIZE 0x1000

#define IMGFILE_PHYADR 0x4000000
#define IMGKRNL_PHYADR 0x2000000

// MMU页表相关
#define KINITPAGE_PHYADR 0x1000000  // 顶级页目录地址

#define KINITFRVM_PHYADR 0x800000
#define KINITFRVM_SZ 0x400000

// 映像文件的物理地址
#define LDRFILEADR IMGFILE_PHYADR
#define MLOSDSC_OFF (0x1000)

// 映像文件头描述符的物理地址, 前0x1000个字节正好是4KB, 是GRUB头
#define MRDDSC_ADR (imgfhdsc_t*)(LDRFILEADR+0x1000)

#define KRNL_VIRTUAL_ADDRESS_START 0xffff800000000000
#define KPML4_P (1<<0)
#define KPML4_RW (1<<1)
#define KPML4_US (1<<2)
#define KPML4_PWT (1<<3)
#define KPML4_PCD (1<<4)
#define KPML4_A (1<<5)

#define KPDPTE_P (1<<0)
#define KPDPTE_RW (1<<1)
#define KPDPTE_US (1<<2)
#define KPDPTE_PWT (1<<3)
#define KPDPTE_PCD (1<<4)
#define KPDPTE_A (1<<5)

#define KPDE_P (1<<0)
#define KPDE_RW (1<<1)
#define KPDE_US (1<<2)
#define KPDE_PWT (1<<3)
#define KPDE_PCD (1<<4)
#define KPDE_A (1<<5)
#define KPDE_D (1<<6)
#define KPDE_PS (1<<7)
#define KPDE_G (1<<8)
#define KPDE_PAT (1<<12)

#define KPML4_SHIFT 39
#define KPDPTTE_SHIFT 30
#define KPDP_SHIFT 21
#define PGENTY_SIZE 512     

#define LOADER_MIGC (0x55ffccddeeaabb55)

#define RAM_USABLE 1
#define RAM_RESERV 2
#define RAM_ACPIREC 3
#define RAM_ACPINVS 4
#define RAM_AREACON 5
#define RAM_MEGER 6

typedef struct s_e820{
    u64_t saddr;    /* start of memory segment8 */
    u64_t lsize;    /* size of memory segment8 */
    u32_t type;    /* type of memory segment 4*/
}__attribute__((packed)) e820map_t;

typedef struct {
  ///
  /// Type of the memory region.
  /// Type EFI_MEMORY_TYPE is defined in the
  /// AllocatePages() function description.
  ///
  u32_t                  Type;
  ///
  /// Physical address of the first byte in the memory region. PhysicalStart must be
  /// aligned on a 4 KiB boundary, and must not be above 0xfffffffffffff000. Type
  /// EFI_PHYSICAL_ADDRESS is defined in the AllocatePages() function description
  ///
  uint_t    PhysicalStart;
  ///
  /// Virtual address of the first byte in the memory region.
  /// VirtualStart must be aligned on a 4 KiB boundary,
  /// and must not be above 0xfffffffffffff000.
  ///
  uint_t     VirtualStart;
  ///
  /// NumberOfPagesNumber of 4 KiB pages in the memory region.
  /// NumberOfPages must not be 0, and must not be any value
  /// that would represent a memory page with a start address,
  /// either physical or virtual, above 0xfffffffffffff000.
  ///
  u64_t                  NumberOfPages;
  ///
  /// Attributes of the memory region that describe the bit mask of capabilities
  /// for that memory region, and not necessarily the current settings for that
  /// memory region.
  ///
  u64_t                  Attribute;
} EFI_MEMORY_DESCRIPTOR;


typedef struct MemMAPData
{
    uint_t                       MemoryMapSize;
    EFI_MEMORY_DESCRIPTOR*       MemoryMap;
    uint_t                       MapKey;
    uint_t                       DescriptorSize;
    uint_t                       DescriptorNR;
    u32_t                        DescriptorVersion;
    e820map_t*                   E820Map;
    uint_t                       E820MapSize;
    uint_t                       E820MapNR;    
    uint_t                       E820MapMerNR;    
    uint_t                       MemSize;    
    uint_t                       Status;
}MemMAPData;


typedef struct LoaderInfo
{
    u64_t Migc;
    u64_t FrameBase;
    u64_t With;
    u64_t High;
    u64_t PerPixBits;
    u64_t PerPixRGBMode;
    MemMAPData Mem;
}LoaderInfo;

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
    u8_t dp_pecyder;
    u32_t dp_pslba;
    u32_t dp_plbasz;

}__attribute__((packed)) dpt_t;


typedef struct s_MBR
{
    char_t mb_byte[446];
    dpt_t mb_part[4];
    u16_t mb_endmgic;
}__attribute__((packed)) mbr_t;
typedef struct s_EBR
{
    char_t eb_byte[446];
    dpt_t eb_part[4];
    u16_t eb_endmgic;
}__attribute__((packed)) ebr_t;

typedef struct s_IDTR
{
        u16_t idtlen;
        u32_t idtbas;
}idtr_t;



typedef struct s_RWHDPACK
{
    //hd_addr_packet:		db	0x10		; [ 0] Packet size in bytes.
      //          db	0		; [ 1] Reserved, must be 0.
    //hd_sect_nr:		db	0		; [ 2] Nr of blocks to transfer.
   //             db	0		; [ 3] Reserved, must be 0.
   //o_ffset:		dw	0		; [ 4] Addr of transfer - Offset
    //s_eg:			dw	0		; [ 6] buffer.          - Seg
    //lba_l:			dd	0		; [ 8] LBA. Low  32-bits.
    //lba_h:			dd	0		; [12] LBA. High 32-bits.
    u8_t rwhpk_sz;
    u8_t rwhpk_rv;
    u8_t rwhpk_sn;
    u8_t rwhpk_rv1;
    u16_t rwhpk_of;
    u16_t rwhpk_se;
    u32_t rwhpk_ll;
    u32_t rwhpk_lh;

}__attribute__((packed)) rwhdpack_t;



typedef struct s_VBEINFO
{
        char vbesignature[4];
        u16_t vbeversion;
        u32_t oemstringptr;
        u32_t capabilities;
        u32_t videomodeptr;
        u16_t totalmemory;
        u16_t oemsoftwarerev;
        u32_t oemvendornameptr;
        u32_t oemproductnameptr;
        u32_t oemproductrevptr;
        u8_t reserved[222];
        u8_t oemdata[256];
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

typedef struct s_PIXCL
{
    u8_t cl_b;
    u8_t cl_g;
    u8_t cl_r;
    u8_t cl_a;
}__attribute__((packed)) pixcl_t;
#define BGRA(r,g,b) ((0|(r<<16)|(g<<8)|b))
typedef u32_t pixl_t;
#define VBEMODE 1
#define GPUMODE 2
#define BGAMODE 3
typedef struct s_GRAPH
{
    uint_t gh_mode;
    uint_t gh_x;
    uint_t gh_y;
    uint_t gh_framphyadr;
    uint_t gh_onepixbits;
    uint_t gh_vbemodenr;
    uint_t gh_vifphyadr;
    uint_t gh_vmifphyadr;
    uint_t gh_bank;
    uint_t gh_curdipbnk;
    uint_t gh_nextbnk;
    uint_t gh_banksz;
    uint_t gh_logophyadrs;
    uint_t gh_logophyadre;
    uint_t gh_fontadr;
    uint_t gh_ftsectadr;
    uint_t gh_ftsectnr;
    uint_t gh_fnthight;
    uint_t gh_nxtcharsx;
    uint_t gh_nxtcharsy;
    uint_t gh_linesz;
    vbeinfo_t gh_vbeinfo;
    vbeominfo_t gh_vminfo;
}__attribute__((packed)) graph_t;

typedef struct s_BMFHEAD
{
    u16_t bf_tag;   //0x4d42
    u32_t bf_size;
    u16_t bf_resd1;
    u16_t bf_resd2;
    u32_t bf_off;
}__attribute__((packed)) bmfhead_t;//14

typedef struct s_BITMINFO
{
    u32_t bi_size;
    s32_t bi_w;
    s32_t bi_h;
    u16_t bi_planes;
    u16_t bi_bcount;
    u32_t bi_comp;
    u32_t bi_szimg;
    s32_t bi_xpelsper;
    s32_t bi_ypelsper;
    u32_t bi_clruserd;
    u32_t bi_clrimport;
}__attribute__((packed)) bitminfo_t;//40

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
    u8_t  fh_mgic[4];
    u32_t fh_fsize;
    u8_t  fh_sectnr;
    u8_t  fh_fhght;
    u16_t fh_wcpflg;
    u16_t fh_nchars;
    u8_t  fh_rev[2];
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
    u32_t ui_chwith:6;
}__attribute__((packed)) uftindx_t;
#define MAX_CHARBITBY 32
typedef struct s_FNTDATA
{
    u8_t fntwxbyte;
    u8_t fnthx;
    u8_t fntwx;
    u8_t fntchmap[MAX_CHARBITBY];
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
    u32_t rp_rsdtphyadr;
    u32_t rp_len;
    u64_t rp_xsdtphyadr;
    u8_t rp_echksum;
    u8_t rp_resv[3];
}__attribute__((packed)) mrsdp_t;
typedef struct s_MACHBSTART
{
    u64_t   mb_migc;          //LMOSMBSP//0
    u64_t   mb_chksum;//8
    u64_t   mb_krlinitstack;//16
    u64_t   mb_krlitstacksz;//24
    u64_t   mb_imgpadr;
    u64_t   mb_imgsz;
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
    u64_t   mb_bfontpadr;
    u64_t   mb_bfontsz;
    u64_t   mb_fvrmphyadr;
    u64_t   mb_fvrmsz;
    u64_t   mb_cpumode;
    u64_t   mb_memsz;
    u64_t   mb_e820padr;
    u64_t   mb_e820nr;
    u64_t   mb_e820sz;
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
    u64_t   mb_pml4padr;
    u64_t   mb_subpageslen;
    u64_t   mb_kpmapphymemsz;
    u64_t   mb_ebdaphyadr;
    mrsdp_t mb_mrsdp;
    graph_t mb_ghparm;
}__attribute__((packed)) machbstart_t;

#define MBSPADR ((machbstart_t*)(0x100000))
#define VBE_DISPI_IOPORT_INDEX (0x01CE)
#define VBE_DISPI_IOPORT_DATA (0x01CF)
#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)
#define BGA_DEV_ID0 (0xB0C0) //- setting X and Y resolution and bit depth (8 BPP only), banked mode
#define BGA_DEV_ID1 (0xB0C1) //- virtual width and height, X and Y offset0
#define BGA_DEV_ID2 (0xB0C2) //- 15, 16, 24 and 32 BPP modes, support for linear frame buffer, support for retaining memory contents on mode switching
#define BGA_DEV_ID3 (0xB0C3) //- support for getting capabilities, support for using 8 bit DAC
#define BGA_DEV_ID4 (0xB0C4) //- VRAM increased to 8 MB
#define BGA_DEV_ID5 (0xB0C5) //- VRAM increased to 16 MB? [TODO: verify and check for other changes]
#define VBE_DISPI_BPP_4 (0x04)
#define VBE_DISPI_BPP_8 (0x08)
#define VBE_DISPI_BPP_15 (0x0F)
#define VBE_DISPI_BPP_16 (0x10)
#define VBE_DISPI_BPP_24 (0x18)
#define VBE_DISPI_BPP_32 (0x20)
#define VBE_DISPI_DISABLED (0x00)
#define VBE_DISPI_ENABLED (0x01)
#define VBE_DISPI_LFB_ENABLED (0x40)


void REGCALL realadr_call_entry(u16_t callint,u16_t val1,u16_t val2);
#endif // LDRTYPE_H
