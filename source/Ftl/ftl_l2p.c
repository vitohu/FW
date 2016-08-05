

#include <global_def.h>
#include <ftl.h>
#include <ftl_ap.h>
#include <nf_cmd.h>
#include <ftl_core.h>
#include <nfc_queue.h>
#include <ftl_l2p.h>

#define CACHE_FLUSH_THRE            (6)

#define MAX_LPT_CNT                 (4)
#define MAX_MPT_CNT                 (2)

#define MAX_DIRTY_LPT_CNT           (4)
#define MAX_DIRTY_MPT_CNT           (2)

#define LPT_HASH_VALUE(lpt)         ((lpt)&(MAX_LPT_CNT - 1))
#define LPT_BIT                     (24)
#define LPT_MSK                     ((1 << LPT_BIT) - 1)
#define LPT_DIRTY_BIT_OFST          (31)
#define LPT_DIRTY_BIT               (1)

#define LPT_IS_DIRTY_CHECK(lpt)     (((lpt) >> LPT_DIRTY_BIT_OFST) & ((1 << LPT_DIRTY_BIT) - 1))    
#define LPT_IS_CLEAN_CHECK(lpt)     (!LPT_IS_DIRTY_CHECK(lpt))
#define LPT_SET_LPT_DIRTY(lpt)      (lpt |=(1 << LPT_DIRTY_BIT_OFST))
#define LPT_SET_LPT_CLEAN(lpt)      (lpt &=(1 << LPT_DIRTY_BIT_OFST) - 1)

#define MPT_HASH_VALUE(mpt)         ((mpt)&(MAX_MPT_CNT - 1))
#define MPT_BIT                     (12)
#define MPT_MSK                     ((1 << MPT_BIT) - 1)
#define MPT_DIRTY_BIT_OFST          (31)
#define MPT_DIRTY_BIT               (1)

#define MPT_IS_DIRTY_CHECK(mpt)     (((mpt) >> MPT_DIRTY_BIT_OFST) & ((1 << MPT_DIRTY_BIT) - 1))    
#define MPT_IS_CLEAN_CHECK(mpt)     (!MPT_IS_DIRTY_CHECK(mpt))
#define MPT_SET_MPT_DIRTY(mpt)      (mpt |=(1 << MPT_DIRTY_BIT_OFST))
#define MPT_SET_MPT_CLEAN(mpt)      (mpt &=(1 << MPT_DIRTY_BIT_OFST) - 1)

#define ALIGN_FORCE_MODE            TRUE
#define ALIGN_PASS_MODE             FALSE

enum
{
    CACHE_FLAG_LPT = 0,
    CACHE_FLAG_MPT,
    CACHE_FLAG_CPT,
    CACHE_FLAG_CNT
};
//data struct define
//for save SRAM Size.we use the MSB of p_lpt to indicate whether the lpt is dirty of clean.
//so the struct of p_lpt is 
/*
struct
{
    u32     lpt     : 24;   //for 2T¡¡drive, only need 512K lpt.only need 19bit. 24 bit can support 64TB
    u32     rev     :  7;
    u32     dirty   :  1;   //indicate current lpt is dirty of not .
}t_lpt;
*/
typedef struct
{
    struct list_head    node;
    u32                 d_tbl;  //store lpt/mpt/cpt
    u32                 d_ptu;
    u32                 *p_buff;
    u8                  b_cache_type;
    u8                  b_rev[3];
}t_cache_type;

typedef struct
{
    u32             **p_lpt_addr;   //the  sram addr of LPT which point the ram/buffer
    u32             *p_lpt;         //the value of each lpt;
    //u32 *p_lpu;         //the ptu which store lpt
    //u32 *p_mpt;         //indicater the lpt belong to which mpt 
    //u32 *p_bitmpa;      //bit map indicate lpt is loaded or not,,only valid in HMB Mode
                        //because SRAM only mode,we can check p_lpt to get this info.
    //u32 *c_lpt_map;     //used to store cache lpt -- lpu mapping
    u32             *p_dirty_queue; //for free buffer use
    t_cache_type    *p_cache_node;
    u16             w_queue_head;
    u16             w_queue_tail;
    u16             w_lpt_cnt;      //record the cnt of alloced lpt buffer
    u16             w_rev;          //for u32 aligned
}t_lpt_ctl_type;

typedef struct
{
    u32             **p_mpt_addr;   //sram buffer addr pointer of mpt
    u32             *p_mpt;         //the value of each mpt
    //u32 *p_mpu;         //the ptu of mpt
    u32             *p_dirty_queue; //use to free mpt use
    t_cache_type    *p_cache_node;
    u16             w_queue_head;
    u16             w_queue_tail;
    u16             w_mpt_cnt;      //record the cnt of alloced mpt buffer
    u16             w_rev;          //for u32 aligned
}t_mpt_ctl_type;

typedef struct
{
    u32             *p_cpt_addr;
    t_cache_type    *p_cache_node;
    bool            f_is_dirty;
    bool            rev[3];
}t_cpt_ctl_type;


t_lpt_ctl_type t_lpt_mng;
t_mpt_ctl_type t_mpt_mng;
t_cpt_ctl_type t_cpt_mng;

struct list_head tbl_cache_list;
u16    cache_node_cnt;

void ftl_l2p_add_tbl_to_cache(u8 b_tbl_type, u32 d_tbl, u32 d_ptu, u32 *p_buff);
bool ftl_l2p_check_tbl_in_cache(u8 b_type, u32 d_tbl);
/*******************************************************************************
*function name:ftl_l2p_init
*
*description  :init l2p module. alloc sram to struct t_lpt_mng
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-07-14
*******************************************************************************/
void ftl_l2p_init(void)
{
//init lpt ctl
    t_lpt_mng.p_lpt_addr    = (u32 **)malloc(sizeof(u32) * MAX_LPT_CNT, TRUE);
    t_lpt_mng.p_lpt         = (u32 *)malloc(sizeof(u32) * MAX_LPT_CNT, TRUE);
    t_lpt_mng.p_dirty_queue = (u32 *)malloc(sizeof(u32) * MAX_DIRTY_LPT_CNT, TRUE);
    t_lpt_mng.p_cache_node  = NULL;
    t_lpt_mng.w_queue_head  = 0;
    t_lpt_mng.w_queue_tail  = 0;
    t_lpt_mng.w_lpt_cnt     = 0;
    memset_u32((u32 *)t_lpt_mng.p_lpt_addr, 0, MAX_LPT_CNT);
    memset_u32(t_lpt_mng.p_lpt, INVALID32, MAX_LPT_CNT);
    memset_u32(t_lpt_mng.p_dirty_queue, INVALID32, MAX_DIRTY_LPT_CNT);
    
    #if 0
    if(PAR_HMB_SUPPORT)
        t_lpt_mng.p_bitmpa  = (u32)malloc(sizeof(u32) * ((TOTAL_L2P_CNT + DWRD_BIT_MSK) >> DWRD_BIT_EXP));
    else
        t_lpt_mng.p_bitmpa  = NULL;
    #endif
    
//init mpt ctl
    t_mpt_mng.p_mpt_addr    = (u32 **)malloc(sizeof(u32) * MAX_MPT_CNT, TRUE);
    t_mpt_mng.p_mpt         = (u32 *)malloc(sizeof(u32) * MAX_MPT_CNT, TRUE);
    t_mpt_mng.p_dirty_queue = (u32 *)malloc(sizeof(u32) * MAX_DIRTY_MPT_CNT, TRUE);
    t_mpt_mng.p_cache_node  = NULL;
    t_mpt_mng.w_queue_head  = 0;
    t_mpt_mng.w_queue_tail  = 0;
    t_mpt_mng.w_mpt_cnt     = 0;

    memset_u32((u32 *)t_mpt_mng.p_mpt_addr, 0, MAX_MPT_CNT);
    memset_u32(t_mpt_mng.p_mpt, INVALID32, MAX_MPT_CNT);
    memset_u32(t_mpt_mng.p_dirty_queue, INVALID32, MAX_DIRTY_MPT_CNT);

//init cpt ctl
    t_cpt_mng.p_cpt_addr    = (u32 *)malloc(sizeof(u32) * TOTAL_MPT_CNT, TRUE);
    t_cpt_mng.f_is_dirty    = FALSE;
    memset_u32(t_cpt_mng.p_cpt_addr, INVALID32, TOTAL_MPT_CNT);


    INIT_LIST_HEAD(&tbl_cache_list);
    cache_node_cnt = 0;
}

/*******************************************************************************
*function name:ftl_l2p_is_lpt_loaded
*
*description  :check is the lpt which include @d_ltu is loaded.
*             if hit cache list,return lpt buff
*parameter    :@d_lpt
*
*return value :TRUE: lpt is loaded, FALSE:LPT not loaded.
*
*revise       :Vito created at:2016-07-14
*******************************************************************************/
u32 ftl_l2p_is_lpt_loaded(u32 d_lpt)
{
    t_cache_type *p_cache_node;
    if((t_lpt_mng.p_lpt[LPT_HASH_VALUE(d_lpt)] & LPT_MSK) == d_lpt)
    {
        DBG_MESSAGE("<LL>lpt:%8x hit hash\n", d_lpt);
        list_for_each_entry_reverse(t_cache_type,p_cache_node,&tbl_cache_list,node)
        {
            if((CACHE_FLAG_LPT == p_cache_node->b_cache_type) && \
                (p_cache_node->d_tbl == d_lpt))
            {
                DBG_MESSAGE("<LL>lpt:%8x hit cache, p:%8x\n", d_lpt, p_cache_node->p_buff);
                //t_lpt_mng.p_cache_node = p_cache_node;
                break;
            }
        }
        return TBL_HIT_HASH;
    }
    //add check cache list
    list_for_each_entry_reverse(t_cache_type,p_cache_node,&tbl_cache_list,node)
    {
        if((CACHE_FLAG_LPT == p_cache_node->b_cache_type) && \
            (p_cache_node->d_tbl == d_lpt))
        {
            DBG_MESSAGE("<LL>lpt:%8x hit cache, p:%8x\n", d_lpt, p_cache_node->p_buff);
            //t_lpt_mng.p_cache_node = p_cache_node;
            return (((u32)p_cache_node->p_buff) | TBL_HIT_CACHE);
        }
    }
    DBG_MESSAGE("<LL>lpt:%8x miss all\n", d_lpt);
    t_lpt_mng.p_cache_node = NULL;
    return TBL_MISS_HIT;
}


/*******************************************************************************
*function name:ftl_l2p_is_lpt_loaded
*
*description  :check is the lpt which include @d_ltu is loaded.
*
*parameter    :@d_ltu
*
*return value :TRUE: lpt is loaded, FALSE:LPT not loaded.
*
*revise       :Vito created at:2016-07-14
*******************************************************************************/
void ftl_l2p_load_lpt(u32 d_lpt)
{
    u32 d_lpt_idx = LPT_HASH_VALUE(d_lpt);
    u32 d_ptu;
    u32 d_mpt;
    u32 d_ret;

    d_ret = ftl_l2p_is_lpt_loaded(d_lpt);
    if(TBL_HIT_HASH == d_ret)
    {
        return;
    }
//step1 : check lpt is dirty or not 
    if(INVALID32 != t_lpt_mng.p_lpt[d_lpt_idx])                  //need dump lpt to nand
    {   
        //dump dirty lpt   check dirty quenu and update mpt
        if(LPT_IS_DIRTY_CHECK(t_lpt_mng.p_lpt[d_lpt_idx]))
            ftl_l2p_dump_lpt(t_lpt_mng.p_lpt[d_lpt_idx] & LPT_MSK);
        else if(FALSE == ftl_l2p_check_tbl_in_cache(CACHE_FLAG_LPT, t_lpt_mng.p_lpt[d_lpt_idx] & LPT_MSK))
            free(t_lpt_mng.p_lpt_addr[d_lpt_idx]);
    }

//step 1.5: if tbl_hit_cache, directly set buff to current lpt buff
    if((d_ret & 0x03) == TBL_HIT_CACHE)
    {
        t_lpt_mng.p_lpt_addr[d_lpt_idx] = (u32 *)(d_ret&(~0x03));
        t_lpt_mng.p_lpt[d_lpt_idx]      = d_lpt & LPT_MSK;
        return;
    }
//step2 : load mpt

    d_mpt = d_lpt >> L2P_ENT_BIT;

    ftl_l2p_load_mpt(d_mpt);

//step 3: load lpt

    
    //if(NULL == t_lpt_mng.p_lpt_addr[d_lpt_idx])             //lpt table ram is not alloced
    {   
        DBG_MESSAGE("<LL>lpt %8x ", d_lpt);
        t_lpt_mng.p_lpt_addr[d_lpt_idx] = (u32 *)malloc(L2P_BYTE_CNT, FALSE);
    }
    d_ptu = ftl_l2p_mpt_get_lpu(d_lpt & L2P_ENT_MSK);
    
    if(INVALID32 != d_ptu)
    {
//send cmd to load lpt to buffer
        //read_flash_cmd(d_ptu, t_lpt_mng.p_lpt_addr[d_lpt_idx]);
        t_ctrl_node_type *p_ctrl_node = (t_ctrl_node_type *)malloc(sizeof(t_ctrl_node_type), TRUE);
        p_ctrl_node->b_user     = mUSER_FTL;
        p_ctrl_node->w_op_type  = OP_FTL_READ;
        p_ctrl_node->p_buff     = t_lpt_mng.p_lpt_addr[d_lpt_idx];
        p_ctrl_node->d_ptu      = d_ptu;
        nfc_queue_add_read_list(p_ctrl_node);
        nfc_read_handle(TRUE);
        
        DBG_MESSAGE("<LL>load lpt%x ptu:%x\n", d_lpt, d_ptu);
    }
    else
    {
        memset_u32(t_lpt_mng.p_lpt_addr[d_lpt_idx], INVALID32, L2P_BYTE_CNT >> DWRD_BYTE_EXP);
        DBG_MESSAGE("<LL>fill lpt%x ptu:%8x\n", d_lpt,d_ptu);
    }

//step 4: update lpt ctl info

    t_lpt_mng.p_lpt[d_lpt_idx] = d_lpt & LPT_MSK;
    
    
}

/*******************************************************************************
*function name:ftl_l2p_is_mpt_loaded
*
*description  :check is the mpt  is loaded or not.
*
*parameter    :@d_mpt
*
*return value :TRUE: mpt is loaded, FALSE:mpt not loaded.
*
*revise       :Vito created at:2016-07-15
*******************************************************************************/
u32 ftl_l2p_is_mpt_loaded(u32 d_mpt)
{
    t_cache_type *p_cache_node;
    
    if((t_mpt_mng.p_mpt[MPT_HASH_VALUE(d_mpt)] & MPT_MSK) == d_mpt)
    {
        DBG_MESSAGE("<ML>mpt:%8x hit hash\n", d_mpt);
        list_for_each_entry_reverse(t_cache_type,p_cache_node,&tbl_cache_list,node)
        {
            if((CACHE_FLAG_MPT == p_cache_node->b_cache_type) && \
                (p_cache_node->d_tbl == d_mpt))
            {
                DBG_MESSAGE("<ML>mpt:%8x hit cache, p:%8x\n", d_mpt, p_cache_node->p_buff);
                //t_mpt_mng.p_cache_node = p_cache_node;
                break;
            }
        }
        return TBL_HIT_HASH;
    }

    //add check cache list
    list_for_each_entry_reverse(t_cache_type,p_cache_node,&tbl_cache_list,node)
    {
        if((CACHE_FLAG_MPT == p_cache_node->b_cache_type) && \
            (p_cache_node->d_tbl == d_mpt))
        {
            DBG_MESSAGE("<ML>mpt:%8x hit cache, p:%8x\n", d_mpt, p_cache_node->p_buff);
            //t_mpt_mng.p_cache_node = p_cache_node;
            return (((u32)p_cache_node->p_buff) | TBL_HIT_CACHE);
        }
    }
    DBG_MESSAGE("<ML>mpt:%8x miss all\n", d_mpt);
    t_mpt_mng.p_cache_node = NULL;
    return TBL_MISS_HIT;
}

/*******************************************************************************
*function name:ftl_l2p_is_mpt_loaded
*
*description  :check is the mpt  is loaded or not.
*
*parameter    :@d_mpt
*
*return value :TRUE: mpt is loaded, FALSE:mpt not loaded.
*
*revise       :Vito created at:2016-07-15
*******************************************************************************/
void ftl_l2p_load_mpt(u32 d_mpt)
{
    u16 w_mpt_idx = MPT_HASH_VALUE(d_mpt);
    u32 d_mpu;
    u32 d_ret;
//step0: check d_mpt is loaded or not
    d_ret = ftl_l2p_is_mpt_loaded(d_mpt);
    if(TBL_HIT_HASH == d_ret)
    {
        return;
    }
    
//step1: check d_mpt is dirty or not
    if(INVALID32 != t_mpt_mng.p_mpt[w_mpt_idx])
    {
//step2: save dirty mpt,check dirty quenu and update cpt
        if(MPT_IS_DIRTY_CHECK(t_mpt_mng.p_mpt[w_mpt_idx]))
            ftl_l2p_dump_mpt(t_mpt_mng.p_mpt[w_mpt_idx] & MPT_MSK);
        else if(FALSE == ftl_l2p_check_tbl_in_cache(CACHE_FLAG_MPT, t_mpt_mng.p_mpt[w_mpt_idx] & MPT_MSK))
            free(t_mpt_mng.p_mpt_addr[w_mpt_idx]);
    }

    t_mpt_mng.p_mpt[w_mpt_idx] = d_mpt & MPT_MSK;
    if(TBL_HIT_CACHE == (d_ret & 0x03))
    {
        t_mpt_mng.p_mpt_addr[w_mpt_idx] = (u32 *)(d_ret & (~0x03));
        return;
    }
    
    d_mpu = ftl_l2p_cpt_get_mpu(d_mpt);

    //if(NULL == t_mpt_mng.p_mpt_addr[w_mpt_idx])
    {
        DBG_MESSAGE("<LM>mpt %8x ", d_mpt);
        t_mpt_mng.p_mpt_addr[w_mpt_idx] = (u32 *)malloc(L2P_BYTE_CNT, FALSE);
    }
//step3: load mpt
    if(INVALID32 != d_mpu)
    {
        //read_flash_cmd(d_mpu, t_mpt_mng.p_mpt_addr[w_mpt_idx]);
        t_ctrl_node_type *p_ctrl_node = (t_ctrl_node_type *)malloc(sizeof(t_ctrl_node_type), TRUE);
        p_ctrl_node->b_user     = mUSER_FTL;
        p_ctrl_node->w_op_type  = OP_FTL_READ;
        p_ctrl_node->p_buff     = t_mpt_mng.p_mpt_addr[w_mpt_idx];
        p_ctrl_node->d_ptu      = d_mpu;
        nfc_queue_add_read_list(p_ctrl_node);
        nfc_read_handle(TRUE);
         
        DBG_MESSAGE("<LM>load mpt:%8x ptu:%8x\n", d_mpt, d_mpu);
       
    }
    else
    {
        memset_u32(t_mpt_mng.p_mpt_addr[w_mpt_idx], INVALID32, L2P_BYTE_CNT >> DWRD_BYTE_EXP);
        DBG_MESSAGE("<LM>fill mpt:%8x ptu:%8x\n", d_mpt, d_mpu);
    }
    
}

/*******************************************************************************
*function name:ftl_l2p_cpt_get_mpu
*
*description  :get mpu of @d_mpt from cpt 
*
*parameter    :@d_mpt
*
*return value : mpu 
*
*revise       :Vito created at:2016-07-15
*******************************************************************************/
u32 ftl_l2p_cpt_get_mpu(u32 d_mpt)
{
    return mem_get_dwrd((u32 )t_cpt_mng.p_cpt_addr, d_mpt);
}

/*******************************************************************************
*function name:ftl_l2p_mpt_get_lpu
*
*description  :get lpu of @d_lpt from mpt 
*
*parameter    :@d_lpt
*
*return value : lpu 
*
*revise       :Vito created at:2016-07-15
*******************************************************************************/
u32 ftl_l2p_mpt_get_lpu(u32 d_lpt)
{
    u32 d_mpt     = d_lpt >> L2P_ENT_BIT;
    u32 d_mpt_idx = MPT_HASH_VALUE(d_mpt);
    u16 w_ofst    = d_lpt & L2P_ENT_MSK;

    return mem_get_dwrd((u32 )t_mpt_mng.p_mpt_addr[d_mpt_idx], w_ofst);
}

/*******************************************************************************
*function name:ftl_l2p_lpt_get_ptu
*
*description  :get ptu of @d_ltu from mpt 
*
*parameter    :@d_ltu
*
*return value : ptu 
*
*revise       :Vito created at:2016-07-15
*******************************************************************************/
u32 ftl_l2p_lpt_get_ptu(u32 d_ltu)
{
    u32 d_lpt       = d_ltu >> L2P_ENT_BIT;
    u32 d_lpt_idx   = LPT_HASH_VALUE(d_lpt);
    u16 w_ofst      = d_ltu & L2P_ENT_MSK;

    return mem_get_dwrd((u32 )t_lpt_mng.p_lpt_addr[d_lpt_idx], w_ofst);
}

/*******************************************************************************
*function name:ftl_l2p_update_lpt/ftl_l2p_update_mpt/ftl_l2p_update_cpt
*
*description  :update l2p/lpt/mpt/cpt 
*
*parameter    :xx
*
*return value : none 
*
*revise       :Vito created at:2016-07-15
*******************************************************************************/
void ftl_l2p_update_lpt(u32 d_ltu, u32 d_ptu)
{
    u32 d_lpt       = d_ltu >> L2P_ENT_BIT;
    u32 d_lpt_idx   = LPT_HASH_VALUE(d_lpt);
    u16 w_ofst      = d_ltu & L2P_ENT_MSK;

    ASSERT_LVL(L2P, ASSERT_LVL0, (t_lpt_mng.p_lpt[d_lpt_idx]&LPT_MSK) == d_lpt);

    DBG_MESSAGE("<UL>update lpt\\ltu:%8x --> ptu:%8x\n", d_ltu, d_ptu);
    mem_set_dwrd((u32 )t_lpt_mng.p_lpt_addr[d_lpt_idx], w_ofst, d_ptu);

#if 0
    if(NULL != t_lpt_mng.p_cache_node->p_buff)
    {
        t_lpt_mng.p_cache_node->p_buff = (u32 *)(((u32)t_lpt_mng.p_cache_node->p_buff) | 0x01);
    }
#endif

    if(LPT_IS_CLEAN_CHECK(t_lpt_mng.p_lpt[d_lpt_idx]))
    {
        LPT_SET_LPT_DIRTY(t_lpt_mng.p_lpt[d_lpt_idx]);
        DBG_MESSAGE("<UL>add dirty lpt:%8x\n", d_lpt);
        #if 0
        
        t_lpt_mng.p_dirty_queue[t_lpt_mng.w_queue_head++] = d_lpt;
        
        if(MAX_DIRTY_LPT_CNT == t_lpt_mng.w_queue_head)
        {
            t_lpt_mng.w_queue_head = 0;
        }

        if(t_lpt_mng.w_queue_head == t_lpt_mng.w_queue_tail)
        {
            ftl_l2p_dump_lpt(t_lpt_mng.p_dirty_queue[t_lpt_mng.w_queue_tail++]);
            if(MAX_DIRTY_LPT_CNT == t_lpt_mng.w_queue_tail)    
            {
                t_lpt_mng.w_queue_tail = 0;
            }
        }
        #endif
    }
}

void ftl_l2p_update_mpt(u32 d_lpt, u32 d_lpu)
{
    u32 d_mpt     = d_lpt >> L2P_ENT_BIT;
    u32 d_mpt_idx = MPT_HASH_VALUE(d_mpt);
    u16 w_ofst    = d_lpt & L2P_ENT_MSK;

    DBG_MESSAGE("<UM>update mpt\\ltu:%8x --> ptu:%8x\n", d_lpt, d_lpu);
    mem_set_dwrd((u32 )t_mpt_mng.p_mpt_addr[d_mpt_idx], w_ofst, d_lpu);

#if 0
    if(NULL != t_mpt_mng.p_cache_node->p_buff)
    {
        t_mpt_mng.p_cache_node->p_buff = (u32 *)(((u32)t_mpt_mng.p_cache_node->p_buff) | 0x01);
    }
#endif
    if(MPT_IS_CLEAN_CHECK(t_mpt_mng.p_mpt[d_mpt_idx]))
    {
        MPT_SET_MPT_DIRTY(t_mpt_mng.p_mpt[d_mpt_idx]);
        
        DBG_MESSAGE("<UM>add dirty mpt:%8x\n", d_mpt);
#if 0
        t_mpt_mng.p_dirty_queue[t_mpt_mng.w_queue_head++] = d_mpt;
        
        if(MAX_DIRTY_MPT_CNT == t_mpt_mng.w_queue_head)
        {
            t_mpt_mng.w_queue_head = 0;
        }

        if(t_mpt_mng.w_queue_head == t_mpt_mng.w_queue_tail)
        {
            //force to save dirty mpt
            ftl_l2p_dump_mpt(t_mpt_mng.p_dirty_queue[t_mpt_mng.w_queue_tail++]);
            if(MAX_DIRTY_MPT_CNT == t_mpt_mng.w_queue_tail)
            {
                t_mpt_mng.w_queue_tail = 0;
            }
        }
#endif
    }
    
    
}

void ftl_l2p_update_cpt(u32 d_mpt, u32 d_mpu)
{
    mem_set_dwrd((u32 )t_cpt_mng.p_cpt_addr, d_mpt, d_mpu);
    t_cpt_mng.f_is_dirty = TRUE;
}

/*******************************************************************************
*function name:ftl_l2p_dump_lpt
*
*description  :dump lpt to flash, if call this function, at least one plane 
*              data need to be flush to flash!
*               first, dump lpt,then check dirty lpt queue.
*
*parameter    :@d_lpt
*
*return value : none 
*
*revise       :Vito created at:2016-07-15
*******************************************************************************/
void ftl_l2p_dump_lpt(u32 d_lpt)
{
    t_ctrl_node_type *p_ctrl_node;
    u32 d_mpt;
    u32 d_ptu;

    if(LPT_IS_CLEAN_CHECK(t_lpt_mng.p_lpt[LPT_HASH_VALUE(d_lpt)]))
    {
        DBG_MESSAGE("<DL>lpt:%8x is clean, skip dump\n", d_lpt);
        return;
    }
    
DUMP_LPT:    
//step3: get new ptu
    d_ptu = ftl_ap_alloc_new_ptu(mUSER_FTL);
//step4: add lpt/ptu to cache list wait to dump to flash
    
    p_ctrl_node = (t_ctrl_node_type *)malloc(sizeof(t_ctrl_node_type), TRUE);

    p_ctrl_node->b_user     = mUSER_FTL;
    p_ctrl_node->d_ltu      = d_lpt;
    p_ctrl_node->d_ptu      = d_ptu;
    p_ctrl_node->p_buff     = t_lpt_mng.p_lpt_addr[LPT_HASH_VALUE(d_lpt)];
    p_ctrl_node->w_op_type  = OP_FTL_WRITE;
    
    nfc_queue_add_write_list(p_ctrl_node);
    //ftl_l2p_add_tbl_to_cache(CACHE_FLAG_LPT, d_lpt, d_ptu, t_lpt_mng.p_lpt_addr[LPT_HASH_VALUE(d_lpt)]);
    LPT_SET_LPT_CLEAN(t_lpt_mng.p_lpt[LPT_HASH_VALUE(d_lpt)]);    
    
    DBG_MESSAGE("<DL>save lpt:%8x ptu:%8x\n", d_lpt, d_ptu);
    
//step2: load mpt
    d_mpt = d_lpt >> L2P_ENT_BIT;
    ftl_l2p_load_mpt(d_mpt);
//step5: update lpt     
    ftl_l2p_update_mpt(d_lpt, d_ptu);
//step6: save mpt
    d_ptu = ftl_ap_alloc_new_ptu(mUSER_FTL);
    //write_flash_cmd(d_ptu, t_mpt_mng.p_mpt_addr[MPT_HASH_VALUE(d_mpt)]);
    ftl_l2p_add_tbl_to_cache(CACHE_FLAG_MPT, d_mpt, d_ptu, t_mpt_mng.p_mpt_addr[MPT_HASH_VALUE(d_mpt)]);
    DBG_MESSAGE("<DL>save mpt:%8x ptu:%8x\n", d_mpt, d_ptu);
    
    ftl_l2p_update_cpt(d_mpt, d_ptu);
    MPT_SET_MPT_CLEAN(t_mpt_mng.p_mpt[MPT_HASH_VALUE(d_mpt)]);    
#if 0
//step8: check is plane aligned, if not ,save dirty lpt, goto step 1
    while(!PLANE_ALIGNED(d_ptu))
    {
        if(t_lpt_mng.w_queue_head != t_lpt_mng.w_queue_tail)
        {
            d_lpt = t_lpt_mng.p_dirty_queue[t_lpt_mng.w_queue_tail++];
            if(MAX_DIRTY_LPT_CNT == t_lpt_mng.w_queue_tail)
            {
                t_lpt_mng.w_queue_tail = 0;
            }

            if((INVALID32 != t_lpt_mng.p_lpt[LPT_HASH_VALUE(d_lpt)]) && LPT_IS_DIRTY_CHECK(t_lpt_mng.p_lpt[LPT_HASH_VALUE(d_lpt)]))
            {
                DBG_MESSAGE("<DL>dump dirty lpt:%8x\n", t_lpt_mng.p_lpt[LPT_HASH_VALUE(d_lpt)]);
                goto DUMP_LPT;
            }
        }
        d_ptu = ftl_ap_alloc_new_ptu(mUSER_FTL);
        write_flash_dummy_cmd(d_ptu);
        DBG_MESSAGE("<DL>dump dirty ptu:%8x\n", d_ptu);
    }
    nfc_write_cmd_handle(mUSER_FTL);        
#endif
}

/*******************************************************************************
*function name:ftl_l2p_dump_mpt
*
*description  :dump mpt to flash, if call this function, at least one plane 
*              data need to be flush to flash!
*               first, dump mpt,then check dirty lpt queue.
*
*parameter    :@d_mpt
*
*return value : none 
*
*revise       :Vito created at:2016-07-18
*******************************************************************************/
void ftl_l2p_dump_mpt(u32 d_mpt)
{
    t_ctrl_node_type *p_ctrl_node;
    u32 d_mpu;

    if(MPT_IS_CLEAN_CHECK(t_mpt_mng.p_mpt[MPT_HASH_VALUE(d_mpt)]))
    {
        return;
    }
DUMP_MPT:
    d_mpu = ftl_ap_alloc_new_ptu(mUSER_FTL);
    
    //write_flash_cmd(d_mpu, t_mpt_mng.p_mpt_addr[MPT_HASH_VALUE(d_mpt)]);
    ftl_l2p_update_cpt(d_mpt, d_mpu);
    p_ctrl_node = (t_ctrl_node_type *)malloc(sizeof(t_ctrl_node_type), TRUE);

    p_ctrl_node->b_user     = mUSER_FTL;
    p_ctrl_node->d_ltu      = d_mpt;
    p_ctrl_node->d_ptu      = d_mpu;
    p_ctrl_node->p_buff     = t_mpt_mng.p_mpt_addr[MPT_HASH_VALUE(d_mpt)];
    p_ctrl_node->w_op_type  = OP_FTL_WRITE;
    
    nfc_queue_add_write_list(p_ctrl_node);
    //ftl_l2p_add_tbl_to_cache(CACHE_FLAG_MPT, d_mpt, d_mpu, t_mpt_mng.p_mpt_addr[MPT_HASH_VALUE(d_mpt)]);

    DBG_MESSAGE("<DM>dump mpt:%8x ptu:%8x\n", d_mpt, d_mpu);

    MPT_SET_MPT_CLEAN(t_mpt_mng.p_mpt[MPT_HASH_VALUE(d_mpt)]);

#if 0    
    while((!PLANE_ALIGNED(d_mpu)) && (TRUE == f_align_mode))
    {
        while(t_mpt_mng.w_queue_head != t_mpt_mng.w_queue_tail)
        {
            d_mpt = t_mpt_mng.p_dirty_queue[t_mpt_mng.w_queue_tail];
            
            t_mpt_mng.w_queue_tail++;
            if(MAX_DIRTY_MPT_CNT == t_mpt_mng.w_queue_tail)
            {
                t_mpt_mng.w_queue_tail = 0;
            }

            if((INVALID32 != t_mpt_mng.p_mpt[MPT_HASH_VALUE(d_mpt)]) && MPT_IS_DIRTY_CHECK(t_mpt_mng.p_mpt[MPT_HASH_VALUE(d_mpt)]))
            {
                DBG_MESSAGE("<DM>dump dirty mpt:%8x\n", t_mpt_mng.p_mpt[MPT_HASH_VALUE(d_mpt)]);
                goto DUMP_MPT;
            }
        }
        write_flash_dummy_cmd(d_mpu);
        DBG_MESSAGE("<DM>dump dirty ptu:%8x\n", d_mpu);
    }
#endif

}

void ftl_l2p_dump_cpt(void)
{
    u32 d_ptu;
    u16 w_cnt = 0;
    do
    {
        d_ptu = ftl_ap_alloc_new_ptu(mUSER_FTL);
        //write_flash_cmd(d_ptu, (u32 *)((u32)t_cpt_mng.p_cpt_addr + (w_cnt << L2P_BYTE_BIT)));
        ftl_l2p_add_tbl_to_cache(CACHE_FLAG_CPT, w_cnt, d_ptu, (u32 *)((u32)t_cpt_mng.p_cpt_addr + (w_cnt << L2P_BYTE_BIT)));
        w_cnt++;
    }while(w_cnt < TOTAL_CPT_CNT);
}

bool ftl_l2p_check_buff_in_hash_tbl(t_cache_type *p_cache_node)
{
    switch(p_cache_node->b_cache_type)
    {
        case CACHE_FLAG_LPT:
            if(t_lpt_mng.p_lpt_addr[LPT_HASH_VALUE(p_cache_node->d_tbl)] == p_cache_node->p_buff)
            {
                return TRUE;
            }
            break;
        case CACHE_FLAG_MPT:
            if(t_mpt_mng.p_mpt_addr[MPT_HASH_VALUE(p_cache_node->d_tbl)] == p_cache_node->p_buff)
            {
                return TRUE;
            }
            break;
        case CACHE_FLAG_CPT:
            ASSERT_LVL(L2P, ASSERT_LVL1, 0);
            break;
        default:
            ASSERT_LVL(L2P, ASSERT_LVL1, 0);
            break;
    }
    //ASSERT_LVL(L2P, ASSERT_LVL1, 0);
    return FALSE;
}

bool ftl_l2p_check_tbl_in_cache(u8 b_type, u32 d_tbl)
{
    t_cache_type *p_cache_node;
    
    list_for_each_entry_reverse(t_cache_type,p_cache_node,&tbl_cache_list,node)
    {
        if((b_type == p_cache_node->b_cache_type) && \
            (p_cache_node->d_tbl == d_tbl))
        {
            DBG_MESSAGE("<CK>tbl:%8x hit cache, p:%8x\n", d_tbl, p_cache_node->p_buff);
            return TRUE;
        }
    }
    return FALSE;
}

//need fix
void ftl_l2p_add_tbl_to_cache(u8 b_tbl_type, u32 d_tbl, u32 d_ptu, u32 *p_buff)
{
    t_cache_type *p_cache_node  = (t_cache_type *)malloc(sizeof(t_cache_type), TRUE);
    
    DBG_MESSAGE("<CT>add to cache:%8x %s (%8x ptu:%8x buf:%8x\n", (u32 )p_cache_node,b_tbl_type == CACHE_FLAG_LPT ? "LPT" : \
                (b_tbl_type == CACHE_FLAG_MPT ? "MPT" : "CPT"), d_tbl, d_ptu, (u32)p_buff);
    p_cache_node->b_cache_type  = b_tbl_type;
    p_cache_node->d_tbl         = d_tbl;
    p_cache_node->d_ptu         = d_ptu;
    p_cache_node->p_buff        = p_buff; //disable free current buff

    list_add_tail(&p_cache_node->node,&tbl_cache_list);
    cache_node_cnt ++;

    if(cache_node_cnt > CACHE_FLUSH_THRE)
    {
        u8 b_cnt = 0;

        //flush data node before dump tbl
        ftl_core_force_dump_host_data();
        while(b_cnt < 4)
        {
            //nf_handle_cmd();
            p_cache_node = list_first_entry(&tbl_cache_list,t_cache_type,node);

//if buff exist in hash table, skip free buff
            list_del(&p_cache_node->node);
            if((FALSE == ftl_l2p_check_buff_in_hash_tbl(p_cache_node)) && \
                //(0 == ((u32)p_cache_node->p_buff & 0x01)))
                (FALSE == ftl_l2p_check_tbl_in_cache(p_cache_node->b_cache_type, p_cache_node->d_tbl)))
            {
                free(p_cache_node->p_buff);
            }
            else
            {
                DBG_MESSAGE("SKIP Free Point:%8x\n", (u32)p_cache_node->p_buff);
            }
            
            free(p_cache_node);
            
            cache_node_cnt--;
            b_cnt++;
        }
        
    }
}


#if (FTL_L2P_UT_TEST_ENABLE == mENABLE)
#define TEST_CASE_CNT   40
void ftl_l2p_ut_test(void)
{                          
    typedef struct
    {
        u16 w_lpt;
        u16 w_mpt;
        u16 w_lpt_ofst;
    }t_ltu_case_type;

    t_ltu_case_type t_ltu_case[TEST_CASE_CNT] = {
      //case 0  //lpt matched //mpt match //lpt0 conflict //mpt0 conflict 
      {0, 0, 0}, {1, 6, 1},   {1, 1, 0},   {128, 0, 0} ,   {128, 6, 1},  
      {1, 2, 2}, {2, 0, 1},   {3, 2, 1},   {256, 3, 0},  {5, 2, 54},
      {45,4,15}, {34, 3, 78}, {9, 4, 165}, {54, 7, 6}, {890, 1, 49},
      {0, 5, 1},{1, 2, 5},{2, 0, 4},{3,3,4},{4,0,2},{0,1,1},{0,2,1},{0,3,4},
      {69,0,51},{34,1,6},{4,2,5},{645,3,34},{544,4,853},{054,5,45}
      
    };
    u32 d_ltu;
    u32 d_lpt;
    u32 d_mpt;
    u8  b_idx;
    u32 d_ptu;
    ftl_l2p_init();
    //ftl_ap_init();
    d_ptu = 0;
    for(b_idx = 0; b_idx < TEST_CASE_CNT; b_idx++)
    {
        d_ltu = (t_ltu_case[b_idx].w_lpt << L2P_ENT_BIT) + (t_ltu_case[b_idx].w_mpt << (L2P_ENT_BIT<< 1)) +\
            t_ltu_case[b_idx].w_lpt_ofst;
        d_ptu ++;
        d_lpt = d_ltu >> L2P_ENT_BIT;
        d_mpt = d_lpt >> L2P_ENT_BIT;
        DBG_MESSAGE("\nltu:%8x lpt%8x mpt:%8x ptu:%8x\n", d_ltu, d_lpt, d_mpt, d_ptu);

        ftl_l2p_load_lpt(d_lpt);
        ftl_l2p_update_lpt(d_ltu, d_ptu);
    }
}
#endif
