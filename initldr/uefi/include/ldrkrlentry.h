/**
 * 
 * 
 * 
 */

#ifndef _LDRKRLENTRY_H
#define _LDRKRLENTRY_H

#include "base/ldrtype.h"

LoaderInfo* GetLoaderinfo();
void ldrkrl_entry();
void kerror(char_t* kestr);
void die(u32_t dt);

#endif  // _LDRKRLENTRY_H