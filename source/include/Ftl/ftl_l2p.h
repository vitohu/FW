#ifndef _FTL_L2P_H_
#define _FTL_L2P_H_

#define FTL_L2P_UT_TEST_ENABLE  mDISABLE

#define Cap         (0x3B800000)  //unit is sector ,,320GB

#define L2P_CNT_MSK         (FTL_MAPPING_SIZE - 1)
#define L2P_CNT_BIT         (FTL_MAPPING_SIZE_BIT)


#define L2P_ENT_BIT         (L2P_CNT_BIT + LBA_BYTE_EXP - DWRD_BYTE_EXP)
#define L2P_ENT_CNT         (1 << L2P_ENT_BIT)
#define L2P_ENT_MSK         (L2P_ENT_CNT - 1)

#define L2P_BYTE_BIT        (L2P_ENT_BIT + DWRD_BYTE_EXP)
#define L2P_BYTE_CNT        (1 << L2P_BYTE_BIT)
#define L2P_BYTE_MSK        (L2P_BYTE_CNT - 1)

#define TOTAL_L2P_CNT       ((Cap + L2P_CNT_MSK) >> L2P_CNT_BIT)
#define TOTAL_LPT_CNT       ((TOTAL_L2P_CNT + L2P_ENT_MSK)  >> L2P_ENT_BIT)
#define TOTAL_MPT_CNT       ((TOTAL_LPT_CNT + L2P_ENT_MSK)  >> L2P_ENT_BIT)
#define TOTAL_CPT_CNT       ((TOTAL_MPT_CNT + L2P_ENT_MSK)  >> L2P_ENT_BIT)

#define TBL_MISS_HIT        0
#define TBL_HIT_HASH        1
#define TBL_HIT_CACHE       2

//function statement
extern void ftl_l2p_init(void);
//extern u32 ftl_l2p_is_lpt_loaded(u32 d_lpt);
extern void ftl_l2p_load_lpt(u32 d_lpt);
extern void ftl_l2p_load_mpt(u32 d_mpt);
extern u32 ftl_l2p_cpt_get_mpu(u32 d_mpt);
extern u32 ftl_l2p_mpt_get_lpu(u32 d_lpt);
extern u32 ftl_l2p_lpt_get_ptu(u32 d_ltu);
extern void ftl_l2p_update_lpt(u32 d_ltu, u32 d_ptu);
extern void ftl_l2p_update_mpt(u32 d_lpt, u32 d_lpu);
extern void ftl_l2p_update_cpt(u32 d_mpt, u32 d_mpu);
extern void ftl_l2p_dump_lpt(u32 d_lpt);
extern void ftl_l2p_dump_mpt(u32 d_mpt);
extern void ftl_l2p_dump_cpt(void);


#if (FTL_L2P_UT_TEST_ENABLE == mENABLE)
void ftl_l2p_ut_test(void);
#endif

#endif
