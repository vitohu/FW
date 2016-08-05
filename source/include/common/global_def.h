#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

typedef unsigned char               bool;

typedef unsigned char               u8;
typedef signed   char               s8;
typedef const unsigned char         cu8;
typedef const signed char           cs8;

typedef unsigned short              u16;
typedef signed   short              s16;
typedef const unsigned short        cu16;
typedef const signed short          cs16;

typedef unsigned int                u32;
typedef signed   int                s32;
typedef const unsigned int          cu32;
typedef const signed int            cs32;

typedef unsigned long long          u64;
typedef signed   long long          s64;
typedef const unsigned long long    cu64;
typedef const signed long long      cs64;

#define mENABLE		    	        0x01
#define mDISABLE		    	    0x00

#ifndef TRUE
#define TRUE    (1)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#define NO_BIT                      0x00000000
#define BIT0                        0x00000001
#define BIT1                        0x00000002
#define BIT2                        0x00000004
#define BIT3                        0x00000008
#define BIT4                        0x00000010
#define BIT5                        0x00000020
#define BIT6                        0x00000040
#define BIT7                        0x00000080
#define BIT8                        0x00000100
#define BIT9                        0x00000200
#define BIT10                       0x00000400
#define BIT11                       0x00000800
#define BIT12                       0x00001000
#define BIT13                       0x00002000
#define BIT14                       0x00004000
#define BIT15                       0x00008000
#define BIT16                       0x00010000
#define BIT17                       0x00020000
#define BIT18                       0x00040000
#define BIT19                       0x00080000
#define BIT20                       0x00100000
#define BIT21                       0x00200000
#define BIT22                       0x00400000
#define BIT23                       0x00800000
#define BIT24                       0x01000000
#define BIT25                       0x02000000
#define BIT26                       0x04000000
#define BIT27                       0x08000000
#define BIT28                       0x10000000
#define BIT29                       0x20000000
#define BIT30                       0x40000000
#define BIT31                       0x80000000

#define BYTE_BIT_CNT                (8)
#define BYTE_BIT_EXP                (3)     /* The exponent of 8 is 3, 1<<3 == 8*/
#define BYTE_BIT_MSK                ((1 << BYTE_BIT_EXP) - 1) // 7

#define WORD_BIT_CNT                (16)
#define WORD_BIT_EXP                ( 4)
#define WORD_BIT_MSK                ((1 << WORD_BIT_EXP) - 1) // 15


#define DWRD_BIT_CNT                (32)    /* One double word contain 32 bits */
#define DWRD_BIT_EXP                ( 5)    /* The exponent of 32 is 5, 1<<5 == 32*/
#define DWRD_BIT_MSK                ((1 << DWRD_BIT_EXP) - 1)  // 31


#define WORD_BYTE_EXP               (1) /* One word contain 2 bytes,the exponent of 2 is 1*/
#define WORD_BYTE_CNT               (1 <<(WORD_BIT_EXP - BYTE_BIT_EXP))
#define DWRD_BYTE_EXP               (2) /* One double word contain 4 bytes,the exp of 4 is 2
*/
#define DWRD_BYTE_CNT               (1 <<(DWRD_BIT_EXP - BYTE_BIT_EXP))
#define DWRD_BYTE_MSK               (DWRD_BYTE_CNT - 1) 

#define NULL                        0
#define INVALID32                   0xFFFFFFFF
#define INVALID16                   0xFFFF
#define INVALID8                    0xFF
#define INVALID_P                   NULL

#define PAGE_16K_BYTE               16384
#define PAGE_8K_BYTE                8192
#define PAGE_4K_BYTE                4096


#define LBA_BYTE_EXP                (9)
#define LBA_BYTE_CNT                (1 << LBA_BYTE_EXP)
#define LBA_BYTE_MSK                (LBA_BYTE_CNT - 1)

#define LTU_BYTE_EXP                (12)
#define LTU_BYTE_CNT                (1 << LTU_BYTE_EXP)
#define LTU_BYTE_MSK                (LTU_BYTE_CNT - 1)

#define LTU_LBA_EXP                 (LTU_BYTE_EXP - LBA_BYTE_EXP)
#define LTU_LBA_CNT                 (1 << LTU_LBA_EXP)
#define LTU_LBA_MSK                 (LTU_LBA_CNT - 1)

enum
{
    mATA_WAIT_DMA = 0,
    mATA_DMAING,
    mATA_DMA_DONE,

    mFTL_TRANS,
    mFTL_WMR,
    mFTL_NFC
};
enum
{
    mUPDATE_INC = 0,
    mUPDATE_DEC,
    mUPDATE_SET,
    mUPDATE_CNT
};



#include <uart.h>
#include <list.h>
#include <s_list.h>
#include <memop.h>
#include <memctl.h>
#include <rb_tree.h>
#include <reg_def.h>

#include <ftl_cfg.h>

typedef struct
{
    struct  list_head node;
    u32     d_tag;
    u32     d_lba;
    u16     w_len;
    u16     w_op_type;

    u32     d_ltu;
    u32     d_ptu;
    
    u32     *p_buff;
    u8      b_status;
    u8      b_user;
    u8      b_rev;
    bool    f_write_trig;
}t_ctrl_node_type;
#endif
