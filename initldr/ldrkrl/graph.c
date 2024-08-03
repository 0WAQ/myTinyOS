#include "../include/cmctl.h"

void write_pixcolor(machbstart_t* mbsp, u32_t x, u32_t y, pixl_t pix)
{

    u8_t* p24bas;
    if(mbsp->mb_ghparm.gh_onepixbits == 24)
    {
            u32_t p24adr = (x + (y * mbsp->mb_ghparm.gh_x)) * 3;
            p24bas = (u8_t*)(p24adr + mbsp->mb_ghparm.gh_fram_phyadr);
            p24bas[0] = (u8_t)(pix);
            p24bas[1] = (u8_t)(pix >> 8);
            p24bas[2] = (u8_t)(pix >> 16);
            return;

    }
    u32_t* phybas = (u32_t*)mbsp->mb_ghparm.gh_fram_phyadr;
    phybas[x + (y * mbsp->mb_ghparm.gh_x)] = pix;
}

void bmp_print(void* bmfile,machbstart_t* mbsp)
{
    if(bmfile == NULL) {
        return;
    }

    pixl_t pix = 0;
    bmdbgr_t* bpixp = NULL;
    bmfhead_t* bmphdp = (bmfhead_t*)bmfile;
    bitminfo_t* binfp = (bitminfo_t*)(bmphdp + 1);
    u32_t img = (u32_t)bmfile + bmphdp->bf_off;
    
    bpixp = (bmdbgr_t*)img;
    
    int l = 0;
    int k = 0;
    int ilinebc = (((binfp->bi_w * 24) + 31) >> 5) << 2;
    
    for(int y = 639; y >= 129; y--, l++)
    {
        k = 0;
        for(int x = 322; x < 662; x++)
        {
            pix = BGRA(bpixp[k].bmd_r, bpixp[k].bmd_g, bpixp[k].bmd_b);
            write_pixcolor(mbsp, x, y, pix);
            k++;
        }
        bpixp = (bmdbgr_t*)(((int)bpixp) + ilinebc);
    } 
}

void logo(machbstart_t* mbsp)
{
    u32_t retadr = 0, sz = 0;

    get_file_rpadrandsz("logo.bmp", mbsp, &retadr, &sz);
    if(retadr == 0) {
        kerror("logo get_file_rpadrandsz error!\n");
    }

    bmp_print((void*)retadr, mbsp);
}

void init_graph(machbstart_t* mbsp)
{
    // 初始化图形数据结构
    graph_t_init(&mbsp->mb_ghparm);

    init_bgadevice(mbsp);

    if(mbsp->mb_ghparm.gh_mode != BGAMODE)
    {
        // 获取VBE模式, 通过BIOS中断
        get_vbemode(mbsp);

        // 获取一个具体的VBE模式的信息, 通过BIOS中断
        get_vbemodeinfo(mbsp);

        // 设置VBE模式, 通过BIOS中断
        set_vbemodeinfo();
    }

    init_kinitfvram(mbsp);

    logo(mbsp);
}

void init_bgadevice(machbstart_t* mbsp)
{
    u32_t retdevid = get_bgadevice();
    if(retdevid == 0)
        return;

    retdevid = chk_bgamaxver();
    if(retdevid == 0)
        return;

    bga_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_reg(VBE_DISPI_INDEX_XRES, 1024);
    bga_write_reg(VBE_DISPI_INDEX_YRES, 768);
    bga_write_reg(VBE_DISPI_INDEX_BPP, 0x20);
    bga_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED|(VBE_DISPI_LFB_ENABLED));
    
    mbsp->mb_ghparm.gh_mode=BGAMODE;
    mbsp->mb_ghparm.gh_vbe_mode_nr=retdevid;
    mbsp->mb_ghparm.gh_x=1024;
    mbsp->mb_ghparm.gh_y=768;
    mbsp->mb_ghparm.gh_fram_phyadr=0xe0000000;
    mbsp->mb_ghparm.gh_onepixbits=0x20;
    mbsp->mb_ghparm.gh_bank=4;
    mbsp->mb_ghparm.gh_curdip_bnk=0;
    mbsp->mb_ghparm.gh_next_bnk=0;
    mbsp->mb_ghparm.gh_bank_sz=(mbsp->mb_ghparm.gh_x*mbsp->mb_ghparm.gh_x*4);
    //test_bga();
    return; 
}

u32_t utf8_to_unicode(utf8_t* utfp, s32_t* retuib)
{
        u8_t uhd=utfp->utf_b1,ubyt=0;
    u32_t ucode=0,tmpuc=0;
    if(0x80>uhd)//0xbf&&uhd<=0xbf
    {
        ucode=utfp->utf_b1&0x7f;
        *retuib=1;
        return ucode;
    }
    if(0xc0<=uhd&&uhd<=0xdf)//0xdf
    {
        ubyt=utfp->utf_b1&0x1f;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=2;
        return ucode;
    }
    if(0xe0<=uhd&&uhd<=0xef)//0xef
    {
        ubyt=utfp->utf_b1&0x0f;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=3;
        return ucode;
    }
    if(0xf0<=uhd&&uhd<=0xf7)//0xf7
    {
        ubyt=utfp->utf_b1&0x7;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b4&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=4;
        return ucode;
    }
    if(0xf8<=uhd&&uhd<=0xfb)//0xfb
    {
        ubyt=utfp->utf_b1&0x3;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b4&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b5&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=5;
        return ucode;
    }
    if(0xfc<=uhd&&uhd<=0xfd)//0xfd
    {
        ubyt=utfp->utf_b1&0x1;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b4&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b5&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b6&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=6;
        return ucode;
    }
    *retuib=0;
    return 0;
}

u32_t get_bgadevice()
{
    u16_t bgaid = bga_read_reg(VBE_DISPI_INDEX_ID);
    if(BGA_DEV_ID0 <= bgaid && BGA_DEV_ID5 >= bgaid)
    {
        bga_write_reg(VBE_DISPI_INDEX_ID,bgaid);
        if(bga_read_reg(VBE_DISPI_INDEX_ID) != bgaid)
            return 0;
        return (u32_t)bgaid;
    }
    return 0;
}

u16_t bga_read_reg(u16_t index)
{
    out_u16(VBE_DISPI_IOPORT_INDEX, index);
    return in_u16(VBE_DISPI_IOPORT_DATA);
}

u32_t chk_bgamaxver()
{
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID5);
    if(bga_read_reg(VBE_DISPI_INDEX_ID) == BGA_DEV_ID5)
        return (u32_t)BGA_DEV_ID5;
    
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID4);
    if(bga_read_reg(VBE_DISPI_INDEX_ID) == BGA_DEV_ID4)
        return (u32_t)BGA_DEV_ID4;
    
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID3);
    if(bga_read_reg(VBE_DISPI_INDEX_ID) == BGA_DEV_ID3)
        return (u32_t)BGA_DEV_ID3;
    
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID2);
    if(bga_read_reg(VBE_DISPI_INDEX_ID) == BGA_DEV_ID2)
        return (u32_t)BGA_DEV_ID2;
    
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID1);
    if(bga_read_reg(VBE_DISPI_INDEX_ID) == BGA_DEV_ID1) 
        return (u32_t)BGA_DEV_ID1;
    
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID0);
    if(bga_read_reg(VBE_DISPI_INDEX_ID) == BGA_DEV_ID0)
        return (u32_t)BGA_DEV_ID0;
    
    return 0;
}

void bga_write_reg(u16_t index, u16_t data)
{
    out_u16(VBE_DISPI_IOPORT_INDEX, index);
    out_u16(VBE_DISPI_IOPORT_DATA, data);
}

void init_kinitfvram(machbstart_t* mbsp)
{
    mbsp->mb_fvrmphyadr = KINITRVM_PHYADR;
    mbsp->mb_fvrmsz = KINITRVM_SZ;
    memset((void*)KINITRVM_PHYADR, 0, KINITRVM_SZ);
}

void graph_t_init(graph_t* initp)
{
    memset(initp, 0, sizeof(graph_t));    
    return;
}

void get_vbemode(machbstart_t* mbsp)
{
    // 通过BIOS中断, 调用函数表中的_getvbemode
    realadr_call_entry(RLINTNR(2), 0, 0);

    vbeinfo_t* info = (vbeinfo_t*)VBEINFO_ADR;
    u16_t* mnm;

    if(info->vbe_signature[0] != 'V' ||
       info->vbe_signature[1] != 'E' ||
       info->vbe_signature[2] != 'S' ||
       info->vbe_signature[3] != 'A')
    {
        kerror("vbe is not VESA!\n");
    }

    // 打印并检查vbe版本
    kprint("vbe_version: %x.\n", info->vbe_version);
    if(info->vbe_version < 0x200) {
        kerror("vbe_version is not vbe3!n");
    }

    if(info->video_mode_ptr > 0xffff) {
        mnm = (u16_t*)vfartolineadr(info->video_mode_ptr);
    }
    else {
        mnm = (u16_t*)(info->video_mode_ptr);
    }

    int bm = 0;
    for(int i = 0; mnm[i] != 0xffff; i++) 
    {
        if(mnm[i] == 0x118)
            bm = 1;
        
        if(i > 0x1000)
            break;
    }

    if(bm == 0) {
        kerror("get_vbemode is not 118!\n");
    }

    mbsp->mb_ghparm.gh_mode = VBEMODE;
    mbsp->mb_ghparm.gh_vbe_mode_nr = 0x118;
    mbsp->mb_ghparm.gh_vif_phyadr = VBEINFO_ADR;
    m2mcopy(info, &mbsp->mb_ghparm.gh_vbeinfo, sizeof(vbeinfo_t));
}

void get_vbemodeinfo(machbstart_t* mbsp)
{
    // 通过BIOS中断, 调用函数表中的_getvbeonemodeinfo
    realadr_call_entry(RLINTNR(3), 0, 0);

    vbeominfo_t* info = (vbeominfo_t*)VBEMINFO_ADR;

    u32_t x = info->XResolution;
    u32_t y = info->YResolution;

    u32_t* phybase = (u32_t*)(info->PhysBasePtr);

    if(info->BitsPerPixel < 24) {
        kerror("info->BitsPerPixel != 32.\n");
    }

    if(x != 1024 || y != 768) {
        kerror("resolution is not 1024 * 768!\n");
    }

    if((u32_t)phybase < 0x100000) {
        kerror("phybase is not right!\n");
    }

    mbsp->mb_ghparm.gh_x = info->XResolution;
    mbsp->mb_ghparm.gh_y = info->YResolution;
    mbsp->mb_ghparm.gh_fram_phyadr = info->PhysBasePtr;
    mbsp->mb_ghparm.gh_onepixbits = info->BitsPerPixel;
    mbsp->mb_ghparm.gh_vmif_phyadr = VBEMINFO_ADR;
    m2mcopy(info, &mbsp->mb_ghparm.gh_vbeminfo, sizeof(vbeominfo_t));
}

void set_vbemodeinfo()
{
    realadr_call_entry(RLINTNR(4), 0, 0);
    return;
}

u32_t vfartolineadr(u32_t vfar)
{
    u32_t tmps = 0, sec = 0, off = 0;
    off = vfar & 0xffff;
    tmps = (vfar >> 16) & 0xffff;
    sec = tmps << 4;
    return (sec + off);
}