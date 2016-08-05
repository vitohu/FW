#ifndef _FTL_BBM_H_
#define _FTL_BBM_H_

extern bool ftl_bb_check_ptu_valid(u8 b_user, u8 b_ofst, u8 b_bank, u16 w_page, u16 w_block);
extern bool ftl_bb_check_is_bb(u8 b_bank, u16 w_block);
#endif
