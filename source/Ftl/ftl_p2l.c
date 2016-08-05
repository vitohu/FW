/*******************************************************************************
*Filename:<Ftl_P2l.c>
*
*Brief   :Physical to Logic mapping management.
*
*Owner   :Vito first created at:2016-06-24
*
*Revision:V1.0
*******************************************************************************/
#include <global_def.h>
#include <ftl_P2l.h>
#include <ftl.h>
#include <ftl_bbm.h>


typedef struct
{
    u32     *pd_p2l_buff;
    u16     w_pre_idx;

    u8      b_dump_idx;
    bool    f_need_dump;
}t_p2l_type;
typedef struct
{
    t_p2l_type 	t_p2l_ctl[m_P2L_AP_CNT];
    bool      	f_is_init;
}t_p2l_mng;

static t_p2l_mng t_p2l;
#if 0
=
{
	.f_is_init = FALSE
};
#endif
/*******************************************************************************
*function name:ftl_P2L_init
*
*description  :init p2l module,include alloc p2l buff,init p2l var
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-06-24
*******************************************************************************/
void ftl_p2l_init(void)
{
    u8 b_user;
    for(b_user = 0; b_user < m_P2L_AP_CNT; b_user++)
    {
        t_p2l.t_p2l_ctl[b_user].pd_p2l_buff = (u32 *)malloc(P2L_TBL_SIZE, FALSE);  // alloc a lpa size for p2l buff
        t_p2l.t_p2l_ctl[b_user].w_pre_idx   = 0;
        t_p2l.t_p2l_ctl[b_user].b_dump_idx  = 0;
        t_p2l.t_p2l_ctl[b_user].f_need_dump = FALSE;
    }
    
    t_p2l.f_is_init = TRUE;
}

/*******************************************************************************
*function name:ftl_update_p2l
*
*description  :update p2l table of b_user.
*
*parameter    :@b_user:the ap point which need to update p2l
*              @d_ptu:
*              @d_ltu       
*
*return value :none
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
void ftl_update_p2l(u8 b_user, u32 d_ptu, u32 d_ltu)
{
    t_p2l_type *p_p2l_ctl = &t_p2l.t_p2l_ctl[b_user];
    u8  b_ofst = PTU_GET_OFST(d_ptu);
    u8  b_bank = PTU_GET_BANK(d_ptu);
    u16 w_page = PTU_GET_PAGE(d_ptu);
    u16 w_ptu_idx = (b_ofst + (b_bank << OFST_BIT) + (w_page << (OFST_BIT + BANK_BIT)))&P2L_ENT_MSK;

    ASSERT_LVL(P2L, ASSERT_LVL0, t_p2l.f_is_init == TRUE);
    ASSERT_LVL(P2L, ASSERT_LVL0, b_user < m_P2L_AP_CNT);
    ASSERT_LVL(P2L, ASSERT_LVL0, d_ltu < MAX_LTU);
    ASSERT_LVL(P2L, ASSERT_LVL0, d_ptu != INVALID32);

    while(p_p2l_ctl->w_pre_idx < w_ptu_idx) //skip bb or bad page
    {
        p_p2l_ctl->pd_p2l_buff[p_p2l_ctl->w_pre_idx] = DUMMY_LTU;
        p_p2l_ctl->w_pre_idx++;
    }
    p_p2l_ctl->pd_p2l_buff[p_p2l_ctl->w_pre_idx] = d_ltu;
    p_p2l_ctl->w_pre_idx++;

    //check need dump ptu or not
    if(P2L_ENT_MSK == (p_p2l_ctl->w_pre_idx & P2L_ENT_MSK)) 
    {
        p_p2l_ctl->f_need_dump = TRUE;
    }
}

/*******************************************************************************
*function name:ftl_dump_p2l
*
*description  :dump p2l table to nandflash
*
*parameter    :@m_ap:the ap point which need to dump p2l     
*
*return value :none
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
void ftl_dump_p2l(u8 b_user)
{
    
}

u32 ftl_p2l_get_pre_p2l_ptu(u8 b_user)
{
    return INVALID32;
}

#if 0
/*******************************************************************************
*function name:ftl_p2l_check_is_dump_trig
*
*description  :according to input ptu,check next valid ptu is dump trig or not
*
*parameter    :@b_ap:the ap point which need to check dump p2l
*              @w_ptu:currnt ptu
*
*return value :w_p2l_ptu: next ptu which need to dump p2l
*
*revise       :Vito created at:2016-07-12
*******************************************************************************/
u32 ftl_p2l_check_is_dump_trig(u8 b_ap, u32 w_ptu)
{
    u32 d_p2l_ptu = INVALID32;
    u32 d_next_ptu;

    d_next_ptu = ftl_trans_get_next_valid_ptu(b_ap, w_ptu);
}
#endif

/*******************************************************************************
*function name:ftl_p2l_check_is_dump_trig
*
*description  :according to input ptu,check next valid ptu is dump trig or not
*
*parameter    :@b_ap:the ap point which need to check dump p2l
*              @w_ptu:currnt ptu
*
*return value :w_p2l_ptu: next ptu which need to dump p2l
*
*revise       :Vito created at:2016-07-12
*******************************************************************************/
u32 ftl_p2l_set_last_p2l(u8 b_ap, u32 w_ptu)
{
    u16 w_page  = PAGE_CNT - 1;
    u16 w_blk   = PTU_GET_BLK(w_ptu);
    u8  b_ofst  = OFST_CNT - 1;
    u8  b_bank  = BANK_CNT - 1;
    
    while(FALSE == ftl_bb_check_ptu_valid(b_ap, b_ofst, b_bank, w_page, w_blk))
    {
        if(0 == b_bank)
        {
            ASSERT_LVL(P2L, ASSERT_LVL0, w_page > 0);
            w_page--;
        }
        else
        {
            b_bank--;
        }
        
    }
    return COMBINE_PTU(b_ofst, b_bank, w_page, w_blk);
}


