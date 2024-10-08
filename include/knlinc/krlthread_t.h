/**********************************************************
        线程管理头文件krlthread_t.h
**********************************************************/
#ifndef _KRLTHREAD_T_H
#define _KRLTHREAD_T_H

#define TDSTUS_RUN 0
#define TDSTUS_SLEEP 3
#define TDSTUS_WAIT 4
#define TDSTUS_NEW 5
#define TDSTUS_ZOMB 6
#define TDSTUS_EXIT 12

#define TDFLAG_FREE (1)
#define TDFLAG_BUSY (2)


#define TDRUN_TICK 20

#define PRITY_MAX 64
#define PRITY_MIN 0
#define PRILG_SYS 0
#define PRILG_USR 5

#define MICRSTK_MAX 4

#define THREAD_MAX (4)

#define THREAD_NAME_MAX (64)
#define KERNTHREAD_FLG 0
#define USERTHREAD_FLG 3

#if((defined CFG_X86_PLATFORM)) 
#define DAFT_TDUSRSTKSZ 0x8000
#define DAFT_TDKRLSTKSZ 0x8000
#endif


#if((defined CFG_X86_PLATFORM)) 
#define TD_HAND_MAX 8
#define DAFT_SPSR 0x10
#define DAFT_CPSR 0xd3
#define DAFT_CIDLESPSR 0x13   
#endif

#define K_CS_IDX    0x08
#define K_DS_IDX    0x10
#define U_CS_IDX    0x1b
#define U_DS_IDX    0x23
#define K_TAR_IDX   0x28
#define UMOD_EFLAGS 0x1202
#define KMOD_EFLAGS	0x202

typedef struct s_MICRSTK
{
    uint_t msk_val[MICRSTK_MAX];
}micrstk_t;



typedef struct s_CONTEXT
{  
    uint_t       ctx_nextrip;
    uint_t       ctx_nextrsp;
    x64tss_t*    ctx_nexttss;
}context_t;

typedef struct s_THREAD
{
    spinlock_t  td_lock;
    list_h_t    td_list;
    uint_t      td_flgs;
    uint_t      td_stus;
    uint_t      td_cpuid;
    uint_t      td_id;
    uint_t      td_tick;
    uint_t      td_sumtick;
    uint_t      td_privilege;
    uint_t      td_priority;
    uint_t      td_runmode;
    adr_t       td_krlstktop;
    adr_t       td_krlstkstart;
    adr_t       td_usrstktop;
    adr_t       td_usrstkstart;
    mmadrsdsc_t* td_mmdsc;
    void*       td_resdsc;
    void*       td_privtep;
    void*       td_extdatap;
    char_t*     td_appfilenm;
    uint_t      td_appfilenmlen;
    context_t   td_context;
    objnode_t*  td_handtbl[TD_HAND_MAX];
    char_t      td_name[THREAD_NAME_MAX];
}thread_t;

#endif // KRLTHREAD_T_H
