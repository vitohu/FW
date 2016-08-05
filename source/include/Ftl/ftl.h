#ifndef _FTL_H_
#define _FTL_H_

#define MAPPING_TBL_ENT_EXP         (LTU_BYTE_EXP - DWRD_BYTE_EXP)
#define MAPPING_TBL_ENT_CNT         (1 << MAPPING_TBL_ENT_EXP)
#define MAPPING_TBL_ENT_MSK         (MAPPING_TBL_ENT_CNT - 1)

#define MAPPING_TBL_BYTE_EXP        (LTU_BYTE_EXP)
#define MAPPING_TBL_BYTE_CNT        (1 << MAPPING_TBL_BYTE_EXP)
#define MAPPING_TBL_BYTE_MSK        (MAPPING_TBL_BYTE_CNT - 1)

#define OFST_BIT                    ( 2)
#define OFST_CNT                    (1 << OFST_BIT)
#define BANK_BIT                    ( 1)
#define BANK_CNT                    (1 << BANK_BIT)
#define PAGE_BIT                    ( 9)
#define PAGE_CNT                    (1 << PAGE_BIT)
#define BLOCK_BIT                   (10)
#define BLOCK_CNT                   (1 << BLOCK_BIT)

#define COMBINE_PTU(ofst, bank, page, block)                 (  \
                                    ((ofst) << (BANK_BIT + PAGE_BIT + BLOCK_BIT)) + \
                                    ((bank) << (PAGE_BIT + BLOCK_BIT)) + \
                                    ((page) << (BLOCK_BIT)) + (block))

#define PTU_GET_OFST(ptu)           (((ptu) >> (BANK_BIT + PAGE_BIT + BLOCK_BIT)) & ((1 << OFST_BIT) - 1))
#define PTU_GET_BANK(ptu)           (((ptu) >> (PAGE_BIT + BLOCK_BIT)) & ((1 << BANK_BIT) - 1))
#define PTU_GET_PAGE(ptu)           (((ptu) >> (BLOCK_BIT)) & ((1 << PAGE_BIT) - 1))
#define PTU_GET_BLK(ptu)            ((ptu) & ((1 << BLOCK_BIT) - 1))
#define PTU_GET_CH(ptu)             ((PTU_GET_BANK(ptu)) & (FLASH_CHANNEL_CNT - 1))

#define PLANE_ALIGNED(ptu)          ((PTU_GET_OFST(ptu) == (OFST_CNT - 1)) ? TRUE : FALSE)
//TODO
#define MAX_LTU                     INVALID32

#define DUMMY_LTU                   (INVALID32 - 10)

enum
{
    OP_HOST_READ = 0,
    OP_HOST_WRITE,
    OP_HOST_WMR,

    OP_FTL_READ,
    OP_FTL_WRITE,

    OP_DUMMY_WRITE,
    OP_DUMMY_READ
    
};

#include <ftl_para.h>
#include <ftl_dscpt.h>
#endif
