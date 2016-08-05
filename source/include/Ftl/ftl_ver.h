#ifndef _FTL_VER_H_
#define _FTL_VER_H_

void ftl_ver_init(void);
u32 ftl_ver_get_last_ver(void);
void ftl_ver_inc_last_ver(u32 d_value);
void ftl_ver_set_last_ver(u32 d_value);
u32 ftl_ver_get_blk_ver(u8 b_bank, u16 w_block);
void ftl_ver_upd_blk_ver(u8 b_bank, u16 w_block, u32 d_ver);
u16 ftl_ver_get_max_ver_blk(u8 b_bank);
u16 ftl_ver_get_thre_ver_blk(u8 b_bank, u32 d_thre);
#endif
