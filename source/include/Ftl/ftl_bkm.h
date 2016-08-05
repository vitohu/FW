#ifndef _FTL_BKM_H_
#define _FTL_BKM_H_

#define     FTL_BKM_UT_TEST     mDISABLE

enum
{
    BKM_DATA = 0,
    BKM_EMPTY,
    BKM_GBG,
    BKM_CNT
};

enum
{
    MODE_MLC = 0,
    MODE_SLC,
    MODE_CNT
};
extern void ftl_bkm_init(void);
extern void ftl_bkm_add_gbg_blk(u8 b_bank, u16 w_blk);
extern void ftl_bkm_add_data_blk(u8 b_bank, u16 w_blk);
extern void ftl_bkm_add_empty_blk(u8 b_bank, u16 w_blk);
extern u16 ftl_bkm_get_data_blk_cnt(void);
extern u16 ftl_bkm_get_empty_blk_cnt(void);
extern u16 ftl_bkm_get_gbg_blk_cnt(void);
extern void ftl_bkm_update_data_blk(u16 w_cnt, u8 b_type);
extern void ftl_bkm_update_empty_blk(u16 w_cnt, u8 b_type);
extern void ftl_bkm_update_gbg_blk(u16 w_cnt, u8 b_type);
extern u8 ftl_bkm_get_blk_bpc_mode(u8 b_bank, u16 w_blk);
extern void ftl_bkm_set_blk_bpc_mode(u8 b_bank, u16 w_blk, u8 b_bpc_mode);
extern void ftl_bkm_set_blk_type(u8 b_bank, u16 w_blk, u8 b_type);
extern u8 ftl_bkm_get_blk_type(u8 b_bank, u16 w_blk);

#if (mENABLE == FTL_BKM_UT_TEST)
extern void ftl_bkm_data_blk_test(void);
#endif

extern u16 ftl_bkm_alloc_new_block(u8 b_user);
#endif
