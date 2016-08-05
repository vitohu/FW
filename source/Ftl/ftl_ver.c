/*******************************************************************************
*Filename:<ftl_ver.c>
*
*Brief   :version management. we use version to indicate the write sequence of each block
*
*Owner   :Vito first created at:2016-06-28
*
*Revision:V1.0
*******************************************************************************/
#include <global_def.h>
#include <ftl_ver.h>
#include <ftl.h>
//TODO
#define     VER_TBL_SIZE   (0x1000)

typedef struct
{
    u32     d_last_ver;
    u32     *p_ver_tbl;
    
    bool    f_is_init;
}t_ver_type;

static t_ver_type t_ver_ctl;
#if 0
    = {
   // .f_is_init = FALSE
};
#endif

/*******************************************************************************
*function name:ftl_ver_init
*
*description  :init ver module
*
*parameter    :@b_user:the ap point which need to update p2l
*              :@d_ptu:
*              :@d_ltu       
*
*return value :none
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
void ftl_ver_init(void)
{
    t_ver_ctl.d_last_ver = 0;

    t_ver_ctl.p_ver_tbl = (u32 *)malloc(VER_TBL_SIZE, FALSE);
    
    t_ver_ctl.f_is_init  = TRUE;
}

/*******************************************************************************
*function name:ftl_ver_get_last_ver
*
*description  :get the value of last ver
*
*parameter    :none  
*
*return value :last version value
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
u32 ftl_ver_get_last_ver(void)
{
    ASSERT_LVL(VER, ASSERT_LVL0, t_ver_ctl.f_is_init == TRUE);
    return t_ver_ctl.d_last_ver;
}

/*******************************************************************************
*function name:ftl_ver_inc_last_ver
*
*description  :inc the value of last ver
*
*parameter    :@d_value:   
*
*return value :none
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
void ftl_ver_inc_last_ver(u32 d_value)
{
    ASSERT_LVL(VER, ASSERT_LVL0, t_ver_ctl.f_is_init == TRUE);
    t_ver_ctl.d_last_ver += d_value;
}

/*******************************************************************************
*function name:ftl_ver_set_last_ver
*
*description  :set the value of last ver
*
*parameter    :@d_value:   
*
*return value :none
*
*revise       :Vito created at:2016-06-29
*******************************************************************************/
void ftl_ver_set_last_ver(u32 d_value)
{
    ASSERT_LVL(VER, ASSERT_LVL0, t_ver_ctl.f_is_init == TRUE);
    t_ver_ctl.d_last_ver = d_value;
}

/*******************************************************************************
*function name:ftl_ver_get_blk_ver
*
*description  :get block version value
*
*parameter    :@b_bank, :@w_block   
*
*return value :block version value
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
u32 ftl_ver_get_blk_ver(u8 b_bank, u16 w_block)
{
    ASSERT_LVL(VER, ASSERT_LVL0, t_ver_ctl.f_is_init == TRUE);
    return t_ver_ctl.p_ver_tbl[w_block];
}

/*******************************************************************************
*function name:ftl_ver_upd_blk_ver
*
*description  :update block version value
*
*parameter    :@b_bank, :@w_block  :@d_ver
*
*return value :none
*
*revise       :Vito created at:2016-06-28
*******************************************************************************/
void ftl_ver_upd_blk_ver(u8 b_bank, u16 w_block, u32 d_ver)
{
    ASSERT_LVL(VER, ASSERT_LVL0, t_ver_ctl.f_is_init == TRUE);
    t_ver_ctl.p_ver_tbl[w_block] = d_ver;
}

/*******************************************************************************
*function name:ftl_ver_get_max_ver_blk
*
*description  :get the blk num of max version blk
*
*parameter    :@b_bank,
*
*return value :blk num
*
*revise       :Vito created at:2016-06-29
*******************************************************************************/
u16 ftl_ver_get_min_ver_blk(u8 b_bank)
{
    u16 w_blk,  w_blk_fet;
    u32 d_ver_min, d_ver_tmp;
    w_blk_fet = INVALID16;
    d_ver_min = INVALID32;
    d_ver_tmp = INVALID32;
    for(w_blk = 0; w_blk < BLOCK_CNT; w_blk++)
    {
        d_ver_tmp = ftl_ver_get_blk_ver(b_bank, w_blk);
        if(d_ver_tmp != INVALID32)
        {
            if(d_ver_tmp < d_ver_min)
            {
                d_ver_min = d_ver_tmp;
                w_blk_fet = w_blk;
            }
        }
    }
    return w_blk_fet;
}

/*******************************************************************************
*function name:ftl_ver_get_thre_ver_blk
*
*description  :get the blk num which is less than the thershold with last ver
*
*parameter    :@b_bank, @d_thre
*
*return value :blk num, if return INVALID16,means no blk matched the condition
*
*revise       :Vito created at:2016-06-29
*******************************************************************************/
u16 ftl_ver_get_thre_ver_blk(u8 b_bank, u32 d_thre)
{
    u16 w_blk,  w_blk_fet;
    u32 d_ver_tmp;
    u32 d_last_ver = ftl_ver_get_last_ver();
    if(d_last_ver < d_thre) //no blk ver is matched!
    {
        return INVALID16;
    }
    
    w_blk_fet = INVALID16;
    d_ver_tmp = INVALID32;
    for(w_blk = 0; w_blk < BLOCK_CNT; w_blk++)
    {
        d_ver_tmp = ftl_ver_get_blk_ver(b_bank, w_blk);
        if(d_ver_tmp != INVALID32)
        {
            if(d_ver_tmp <= (d_last_ver - d_thre))
            {
                w_blk_fet = w_blk;
            }
        }
    }
    return w_blk_fet;
}
