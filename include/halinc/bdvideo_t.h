/**********************************************************
        引导调式屏幕上显示输出头文件bdvideo_t.h
**********************************************************/
#ifndef _BDVIDEO_T_H
#define _BDVIDEO_T_H

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

/*typedef struct s_PIXCL
{
    u8_t cl_b;
    u8_t cl_g;
    u8_t cl_r;
    u8_t cl_a;
}__attribute__((packed)) pixcl_t;
*/

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
}__attribute__((packed)) bmfhead_t;

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

//typedef drvstus_t (*dftghp_ops_t)(void* ghpdevp,void* inp,void* outp,uint_t x,uint_t y,uint_t iocode);

typedef struct s_DFTGHOPS
{
    // 读写显存数据
    size_t (*dgo_read)(void* ghpdev,void* outp,size_t rdsz);
    size_t (*dgo_write)(void* ghpdev,void* inp,size_t wesz);
    sint_t (*dgo_ioctrl)(void* ghpdev,void* outp,uint_t iocode);

    // 刷新
    void   (*dgo_flush)(void* ghpdev);
    sint_t (*dgo_set_bank)(void* ghpdev, sint_t bnr);

    // 读写像素
    pixl_t (*dgo_readpix)(void* ghpdev,uint_t x,uint_t y);
    void   (*dgo_writepix)(void* ghpdev,pixl_t pix,uint_t x,uint_t y);

    // 直接读写像素
    pixl_t (*dgo_dxreadpix)(void* ghpdev,uint_t x,uint_t y);
    void   (*dgo_dxwritepix)(void* ghpdev,pixl_t pix,uint_t x,uint_t y);

    // 设置x、y坐标和偏移
    sint_t (*dgo_set_xy)(void* ghpdev,uint_t x,uint_t y);
    sint_t (*dgo_set_vwh)(void* ghpdev,uint_t vwt,uint_t vhi);
    sint_t (*dgo_set_xyoffset)(void* ghpdev,uint_t xoff,uint_t yoff);
   
    // 获取x、y坐标和偏移
    sint_t (*dgo_get_xy)(void* ghpdev,uint_t* rx,uint_t* ry);
    sint_t (*dgo_get_vwh)(void* ghpdev,uint_t* rvwt,uint_t* rvhi);
    sint_t (*dgo_get_xyoffset)(void* ghpdev,uint_t* rxoff,uint_t* ryoff);
}dftghops_t;

typedef struct s_DFTGRAPH
{
    spinlock_t gh_lock;
    u64_t gh_mode;          // 图形模式
    u64_t gh_x;             // 水平像素点
    u64_t gh_y;             // 垂直像素点
    u64_t gh_framphyadr;    // 显存物理地址
    u64_t gh_fvrmphyadr;    // 显存虚拟地址
    u64_t gh_fvrmsz;        // 显存大小
    u64_t gh_onepixbits;    // 一个像素点占用的数据位数
    u64_t gh_onepixbyte;
    u64_t gh_vbemodenr;     // vbe模式号
    u64_t gh_bank;          // 显存的bank数
    u64_t gh_curdipbnk;     // 当前bank
    u64_t gh_nextbnk;       // 下一个bank
    u64_t gh_banksz;        // bank大小
    u64_t gh_fontadr;       // 字库地址
    u64_t gh_fontsz;        // 字库大小
    u64_t gh_fnthight;      // 字体高度
    u64_t gh_nxtcharsx;     // 下一个字符显示的x坐标
    u64_t gh_nxtcharsy;     // 下一个字符显示的y坐标
    u64_t gh_linesz;        // 字符行高
    pixl_t gh_deffontpx;    // 默认字体大小
    u64_t gh_chardxw;
    u64_t gh_flush;
    u64_t gh_framnr;
    u64_t gh_fshdata;       // 刷新相关
    dftghops_t gh_opfun;    // 图形驱动操作函数指针结构体
}dftgraph_t;

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


#endif // BDVIDEO_T_H
