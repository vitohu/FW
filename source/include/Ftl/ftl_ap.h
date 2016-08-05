#ifndef _FTL_AP_H_
#define _FTL_AP_H_

//order can not be switch!! mapping to m_P2L_AP
enum 
{
    mUSER_HOST = 0,
    mUSER_GC,
    mUSER_WL,
    mUSER_FTL,
    mUSER_CNT,
    mUSER_NULL = mUSER_CNT
};





void ftl_ap_init(void);
u32 ftl_ap_get_current_ptu_value(u8 b_user);
u32 ftl_ap_get_pre_ptu_value(u8 b_user);
u32 ftl_ap_alloc_new_ptu(u8 b_user);
void ftl_ap_update_meta_info(u8 b_user, u32 d_lpa, u32 d_ptu);

#endif
