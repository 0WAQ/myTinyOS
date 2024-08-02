#include "cmctl.h"

__attribute__((section(".data"))) cursor_t curs;

void kprint(const char_t* fmt, ...)
{
    char_t buf[512];
    va_list_t arg = (va_list_t)((char_t*)(&fmt) + sizeof(long));

    vsprintfk(buf, fmt, arg);
    GxH_strwrite(buf, &curs);
}

char_t* strcopy(char_t* buf, char* str)
{
    while(*str) {
        *buf = *str;
        buf++;
        str++;
    }
    return buf;
}

void vsprintfk(char_t* buf, const char_t* fmt, va_list_t args)
{
    char_t *p = buf;
    va_list_t next_arg = args;
    while (*fmt)
    {
        if (*fmt != '%') {
            *p++ = *fmt++;
            continue;
        }
        fmt++;

        switch (*fmt)
        {
        case 'x':
            p = numberk(p, *((long *)next_arg), 16);
            next_arg += sizeof(long);
            fmt++;

            break;

        case 'd':
            p = numberk(p, *((long *)next_arg), 10);
            next_arg += sizeof(long);
            fmt++;
            break;

        case 's':
            p = strcopy(p, (char_t *)(*((long *)next_arg)));
            next_arg += sizeof(long);
            fmt++;
            break;

        default:
            break;
        }

    }
    *p = 0;
}

char* numberk(char_t* str, uint_t n, sint_t base)
{
    register char_t *p;
    char_t strbuf[36];
    p = &strbuf[36];
    *--p = 0;

    if(n == 0) {
        *--p = '0';
    }
    else {
        do {
            *--p = "0123456789abcdef"[n % base];
        } while(n /= base);
    }

    while(*p != 0) {
        *str ++ = *p++;
    }
    return str;
}

void GxH_strwrite(char_t* str, cursor_t* cursptr)
{
    uint_t straddr = cursptr->x + cursptr->y * 80 * 2;
    char_t *p_start = (char_t*)(cursptr->vmem_s + straddr);
    u32_t tfindx = FALSE;

    while(*str)
    {
        if(*str == 10) {
            tfindx = TRUE;
            str++;

            if(*str == 0) {
                break;
            }
        }
    
        current_curs(cursptr, VGACHAR_DF_CFLG);

        *p_start = *str++;
        p_start += 2;
    }

    if(tfindx == TRUE) {
        current_curs(cursptr, VGACHAR_LR_CFLG);
    }
}

void clear_screen(u16_t srrv)
{
    curs.x = 0;
    curs.y = 0;

    u16_t* p = (u16_t*)VGASTR_RAM_BASE;

    for(size_t i = 0; i < 2001; i++)
        p[i] = srrv;

    close_curs();
}

void close_curs()
{
    out_u8(VGACTRL_REG_ADR, VGACURS_REG_INX);
    out_u8(VGACTRL_REG_DAT, VGACURS_CLOSE);
}