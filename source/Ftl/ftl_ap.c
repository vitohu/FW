/*******************************************************************************
*Filename:<ftl_ap.c>
*
*Brief   :manager write ap point.support max 4 ap point,AP_HOST, AP_GC, AP_FTL, AP_WL
*         AP_WL is not supported in 8K logic page flash
*
*Owner   :Vito first created at:2016-06-24
*
*Revision:V1.0
*******************************************************************************/

#include <global_def.h>
#include <ftl.h>
#include <ftl_ver.h>
#include <ftl_srp.h>
#include <ftl_meta.h>
#include <ftl_bkm.h>
#include <ftl_p2l.h>
#include <ftl_bbm.h>
#include <ftl_ap.h>

typedef struct
{
    u32             d_cur_ptu;
    u32             d_pre_ptu;

    u16             w_block;
    u16             w_page;
    u8              b_bank;
    u8              b_ofst;
    u8              b_rev[2];

    u32             d_ver;
    t_meta_type     t_meta_info;
}t_ap_type;

static t_ap_type  t_ap_ctl[mUSER_CNT];

/*******************************************************************************
*function name:ftl_ap_init
*
*description  :init ap module
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
void ftl_ap_init(void)
{
    u8 b_user;
    for(b_user = 0; b_user < mUSER_CNT; b_user++)
    {
        t_ap_ctl[b_user].d_cur_ptu    = INVALID32;
        t_ap_ctl[b_user].d_pre_ptu    = INVALID32;
        t_ap_ctl[b_user].w_block      = INVALID16;
        t_ap_ctl[b_user].w_page       = PAGE_CNT - 1;
        t_ap_ctl[b_user].b_bank       = BANK_CNT - 1;
        t_ap_ctl[b_user].b_ofst       = OFST_CNT - 1;
        t_ap_ctl[b_user].d_ver        = INVALID32;
        memset_u32((u32 *)(&t_ap_ctl[b_user].t_meta_info), INVALID32, sizeof(t_meta_type) >> DWRD_BYTE_EXP);
    }
}

/*******************************************************************************
*function name:ftl_ap_get_current_ptu_value
*
*description  :get the value of current ptu of @m_ap
*
*parameter    :@m_ap
*
*return value :current ptu value
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
u32 ftl_ap_get_current_ptu_value(u8 b_user)
{
    ASSERT_LVL(AP,ASSERT_LVL0,b_user < mUSER_CNT);
    return t_ap_ctl[b_user].d_cur_ptu;
}

/*******************************************************************************
*function name:ftl_ap_get_pre_ptu_value
*
*description  :get the value of pre alloced ptu of @m_ap
*
*parameter    :@m_ap
*
*return value :pre ptu value
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
u32 ftl_ap_get_pre_ptu_value(u8 b_user)
{
    ASSERT_LVL(AP,ASSERT_LVL0,b_user < mUSER_CNT);
    return t_ap_ctl[b_user].d_pre_ptu;
}

/*******************************************************************************
*function name:ftl_ap_alloc_new_ptu
*
*description  :ftl translate a new ptu to other module
*
*parameter    :@m_ap
*
*return value :new ptu alloc for @m_ap,also t_ap_ctl[m_ap].d_cur_ptu
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
u32 ftl_ap_alloc_new_ptu(u8 b_user)
{
    t_ap_type *p_ap_ctl = &t_ap_ctl[b_user];
    ASSERT_LVL(AP,ASSERT_LVL0,b_user < mUSER_CNT);
    p_ap_ctl->d_pre_ptu = p_ap_ctl->d_cur_ptu;
    
NEXT_PTU:
    p_ap_ctl->b_ofst++;
    if(p_ap_ctl->b_ofst == OFST_CNT)
    {
        p_ap_ctl->b_ofst = 0;
        p_ap_ctl->b_bank ++;
        if(p_ap_ctl->b_bank == BANK_CNT)
        {
            p_ap_ctl->b_bank = 0;
            p_ap_ctl->w_page ++;
            if(p_ap_ctl->w_page == PAGE_CNT)
            {
                p_ap_ctl->w_page = 0;
                p_ap_ctl->w_block = ftl_bkm_alloc_new_block(b_user);
                p_ap_ctl->d_ver   = ftl_ver_get_last_ver();
                ftl_ver_inc_last_ver(1);
            }
        }
    }
    
    if(FALSE == ftl_bb_check_ptu_valid(b_user, p_ap_ctl->b_ofst, p_ap_ctl->b_bank, \
                    p_ap_ctl->w_page, p_ap_ctl->w_block))
    {
        goto NEXT_PTU;
    }
// TODO 
    p_ap_ctl->d_cur_ptu = COMBINE_PTU(p_ap_ctl->b_ofst, p_ap_ctl->b_bank,\
                            p_ap_ctl->w_page, p_ap_ctl->w_block);
    return p_ap_ctl->d_cur_ptu;
}


/*******************************************************************************
*function name:ftl_ap_update_meta_info
*
*description  :ftl update meta info according b_user
*           in AP_HOST,C0 record AP_GC,    C1 record AP_WL
*           in AP_GC  ,C0 record AP_HOST,  C1 record AP_WL
*           in AP_WL  ,C0 record AP_HOST,  C1 record AP_GC
*
*parameter    :@b_user @d_lpa @d_ptu
*
*return value :none
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
void ftl_ap_update_meta_info(u8 b_user, u32 d_ltu, u32 d_ptu)
{
    u8 b_idx                = PTU_GET_OFST(d_ptu);
    u32 *meta_base_addr     = (u32 *)&t_ap_ctl[b_user].t_meta_info;
    ASSERT_LVL(AP, ASSERT_LVL0, b_user < mUSER_CNT);
    ASSERT_LVL(AP, ASSERT_LVL0, d_ptu != INVALID32);
    ASSERT_LVL(AP, ASSERT_LVL0, d_ltu < MAX_LTU);
    meta_base_addr[b_idx] = d_ltu;
    if(b_user < mUSER_FTL)
    {
        if(0 == b_idx)
        {
            switch(b_user)
            {
                case mUSER_HOST:
                    meta_base_addr[PTUC0_BASE_OFST]     = t_ap_ctl[mUSER_GC].d_pre_ptu;
                    meta_base_addr[VERC0_BASE_OFST]     = t_ap_ctl[mUSER_GC].d_ver;
                    meta_base_addr[PTUC1_BASE_OFST]     = t_ap_ctl[mUSER_WL].d_pre_ptu;
                    meta_base_addr[VERC1_BASE_OFST]     = t_ap_ctl[mUSER_WL].d_ver;
                    break;
                case mUSER_GC:
                    meta_base_addr[PTUC0_BASE_OFST]     = t_ap_ctl[mUSER_HOST].d_pre_ptu;
                    meta_base_addr[VERC0_BASE_OFST]     = t_ap_ctl[mUSER_HOST].d_ver;
                    meta_base_addr[PTUC1_BASE_OFST]     = t_ap_ctl[mUSER_WL].d_pre_ptu;
                    meta_base_addr[VERC1_BASE_OFST]     = t_ap_ctl[mUSER_WL].d_ver;
                    break;
                case mUSER_WL:
                    meta_base_addr[PTUC0_BASE_OFST]     = t_ap_ctl[mUSER_HOST].d_pre_ptu;
                    meta_base_addr[VERC0_BASE_OFST]     = t_ap_ctl[mUSER_HOST].d_ver;
                    meta_base_addr[PTUC1_BASE_OFST]     = t_ap_ctl[mUSER_GC].d_pre_ptu;
                    meta_base_addr[VERC1_BASE_OFST]     = t_ap_ctl[mUSER_GC].d_ver;
                    break;
                default:
                    while(1);
            }
        }
        meta_base_addr[PRE_P2L_BASE_OFST]   = ftl_p2l_get_pre_p2l_ptu(b_user);
    }
    else  //ap ftl
    {
        if(0 == b_idx)
        {
            meta_base_addr[PRE_SRP_BASE_OFST] = ftl_srp_get_pre_srp();
        }
    }
}

