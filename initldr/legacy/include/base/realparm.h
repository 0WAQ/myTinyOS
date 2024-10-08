/**
 * 
 * 
 * 
 */

#ifndef _REALPARM_H
#define _REALPARM_H

#define ETYBAK_ADR 0x2000
#define PM32_EIP_OFF (ETYBAK_ADR)
#define PM32_ESP_OFF (ETYBAK_ADR + 4)
#define PM16_EIP_OFF (ETYBAK_ADR + 8)
#define PM16_ESP_OFF (ETYBAK_ADR + 12)

#define RWHDPACK_ADR (ETYBAK_ADR + 32)
#define E80MAP_NR (ETYBAK_ADR + 64)         // 保存e820map_t结构体数组元素个数的地址
#define E80MAP_ADRADR (ETYBAK_ADR + 68)     // 保存e820map_t结构体数组的首地址
#define E80MAP_ADR (0x5000)
#define VBEINFO_ADR (0x6000)
#define VBEMINFO_ADR (0x6400)

#define READHD_BUFADR (0x3000) 

#endif  // _REALPARM_H