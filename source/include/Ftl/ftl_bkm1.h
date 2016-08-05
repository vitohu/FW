#ifndef _FTL_BKM1_H_
#define _FTL_BKM1_H_
#if 0
#define FTL_BKM1_UT_TEST    mENABLE

enum
{
    BKM_DATA = 0,
    BKM_GBG,
    BKM_EMPTY,
    BKM_CNT
};

extern void ftl_bkm_init(void);
extern void ftl_bkm_add_blk(u8 b_blk_type, u8 b_bank, u16 w_blk);
extern void ftl_bkm_data_blk_test(void);
#endif
#endif
