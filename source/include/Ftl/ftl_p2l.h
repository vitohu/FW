#ifndef _FTL_P2L_H_
#define _FTL_P2L_H_

//order can not be switch!! mapping to m_AP
enum
{
    m_P2L_HOST = 0,
    m_P2L_GC,
    m_P2L_WL,
    m_P2L_AP_CNT
};


//P2l Tbl size is the same with min mapping table size
#define P2L_TBL_EXP     (MAPPING_TBL_BYTE_EXP)
#define P2L_TBL_SIZE    (1 << P2L_TBL_EXP)
#define P2L_TBL_MSK     (P2L_TBL_SIZE - 1)

#define P2L_ENT_EXP     (MAPPING_TBL_ENT_EXP)
#define P2L_ENT_CNT     (1 << P2L_ENT_EXP  )
#define P2L_ENT_MSK     (P2L_ENT_CNT - 1   )

void ftl_p2l_init(void);
void ftl_update_p2l(u8 b_user, u32 d_ptu, u32 d_ltu);
u32 ftl_p2l_get_pre_p2l_ptu(u8 b_user);
void ftl_dump_p2l(u8 b_user);
#endif
