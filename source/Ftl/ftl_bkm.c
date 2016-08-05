/*******************************************************************************
*Filename:<ftl_bkm.c>
*
*Brief   :ftl block management.include empty block, data block, garbage block
*         use rb tree to sort and search block based on version and erase cnt
*
*Owner   :Vito first created at:2016-06-30
*
*Revision:V1.0
*******************************************************************************/
#include <global_def.h>
#include <ftl.h>
#include <ftl_ap.h>
#include <ftl_bkm.h>

#define         MAX_BKM_NODE_CNT       100

#define COMP_SMALL      0
#define COMP_BIG        1
#define COMP_EQUAL      2

#define BPC_MODE_MSK    0x01
#define BPC_MODE_OFST   0x00
#define BLK_TYPE_MSK    0x03
#define BLK_TYPE_OFST   0x01

typedef union
{
    u8      byte;
    struct
    {   
        u8  bpc_mode    :1;
        u8  blk_type    :4;
        u8  rev         :3;
    }bit;
}t_bkm_type;

//each bkm_node need 5DW(20Bytes) to manager.
//so total sram need MAX_BLOCK_CNT*24 Bytes to manager bkm module
//such as 4096 blocks, need 80KB SRAM;1024 blocks, need 20KB SRAM
typedef struct st_bkm_node
{
    s_list      node;           //s list node           //1DW
    rb_node     rb_node;        //node                  //3DW
    u32         val;            //use to record blk num //1DW
}bkm_node;

#if 0
static bkm_node     at_bkm_node[MAX_BKM_NODE_CNT]; //do not need to malloc SRAM any more

static s_list       free_node_list;

static rb_root      empty_bkm_tree;
static rb_root      data_bkm_tree;
static rb_root      gbg_bkm_tree;
#endif

typedef bool (*comp_fun)(u32 , u32 );

typedef struct
{
    bkm_node        *pt_bkm_node;   //buff to alloc bkm node pointer
    t_bkm_type      *pt_bkm_ctl;    //buff to manager bkm info
    s_list          free_node_list; //list head;used to manager all empty s_list node
    
    rb_root         empty_bkm_tree; //used to store empty block
    rb_root         data_bkm_tree;  //used to store data block
    rb_root         gbg_bkm_tree;   //used to store garbage block

    u16             w_empty_blk_cnt;
    u16             w_data_blk_cnt;
    u16             w_gbg_blk_cnt;
    u16             w_rev;
    
    bool            f_is_init;
}t_bkm_mng_type;

static t_bkm_mng_type t_bkm_mng;
#if 0
{
    .f_is_init = FALSE
};
#endif
/*******************************************************************************
*function name:ftl_bkm_init
*
*description  :init bkm module.
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_init(void)
{
    u16 w_idx;
    
    s_list_init_head(&t_bkm_mng.free_node_list);
    
    t_bkm_mng.pt_bkm_node = (bkm_node *)malloc(sizeof(bkm_node) * MAX_BKM_NODE_CNT, FALSE);
    t_bkm_mng.pt_bkm_ctl = (t_bkm_type *)malloc(sizeof(t_bkm_type) * ((BLOCK_CNT + DWRD_BYTE_MSK) & (~DWRD_BYTE_MSK)), FALSE);
    memset_u32((u32 *)t_bkm_mng.pt_bkm_ctl, INVALID32, ((BLOCK_CNT + DWRD_BIT_MSK) >> DWRD_BYTE_EXP));
    for(w_idx = 0; w_idx < MAX_BKM_NODE_CNT; w_idx++)
    {
        s_list_add(&t_bkm_mng.free_node_list, &t_bkm_mng.pt_bkm_node[w_idx].node);
    }

    t_bkm_mng.empty_bkm_tree.rb_node    = NULL;
    t_bkm_mng.data_bkm_tree.rb_node     = NULL;
    t_bkm_mng.gbg_bkm_tree.rb_node      = NULL;
    
    t_bkm_mng.w_empty_blk_cnt           = 0;
    t_bkm_mng.w_data_blk_cnt            = 0;
    t_bkm_mng.w_gbg_blk_cnt             = 0;
    t_bkm_mng.f_is_init = TRUE;

    for(w_idx = 0; w_idx < 100; w_idx++)
    {
        ftl_bkm_add_empty_blk(0, w_idx);
    }
    
}

/*******************************************************************************
*function name:ftl_bkm_rb_search
*
*description  :search the bkm_node wht same val of @d_val
*
*parameter    :@d_val: compare val; @root,pointer of rb root 
*              @func: function used to compare 
*
*return value :@t_ret_node, if matched, return pointer of bkm_node
*                           else,return NULL.
*              user should check the return value.
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
bkm_node *ftl_bkm_rb_search(u32 d_val, rb_root *root, comp_fun func)
{
    bkm_node    *t_ret_node = NULL;
    rb_node     *t_rb_node = root->rb_node;
    bool        f_cmp_ret;

    while(t_rb_node)
    {
        t_ret_node = rb_entry(t_rb_node, bkm_node, rb_node);
        
        f_cmp_ret = func(d_val, t_ret_node->val);
        if(COMP_SMALL == f_cmp_ret)
            t_rb_node       = t_rb_node->left;
        else if(COMP_BIG == f_cmp_ret)
            t_rb_node       = t_rb_node->right;
        else 
            return t_ret_node;
    }
    
    return NULL;
}

/*******************************************************************************
*function name:ftl_bkm_rb_insert
*
*description  :insert rb node which is in struct @t_blk_node to rb tree
*
*parameter    :@t_blk_node, contain rb_node and val; @root:the pointer of rb tree
*              @func:   function used to compare
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_rb_insert(bkm_node *t_blk_node, rb_root *root, comp_fun func)
{
    rb_node     **new = &(root->rb_node), *parent = NULL;
    bkm_node    *t_tmp_node;
    bool        f_ret;

    //find where to insert new node
    while(*new) 
    {
        t_tmp_node  = rb_entry(*new, bkm_node, rb_node);
        f_ret       = func(t_blk_node->val, t_tmp_node->val);
        parent      = *new;
        
        if(COMP_SMALL == f_ret)
            new         = &((*new)->left);
        else if(COMP_BIG == f_ret)
            new         = &((*new)->right);
        else 
            ASSERT_LVL(BKM, ASSERT_LVL0, 0); //this is impossible
    }

    /* Add new node and rebalance tree. */
    rb_link_node(&t_blk_node->rb_node, parent, new);
    rb_insert_color(&t_blk_node->rb_node, root);
}

/*******************************************************************************
*function name:ftl_bkm_rb_delete
*
*description  :delete rb node from @root rb tree
*
*parameter    :@t_bkm_node:src node which contain rb node
*              @root: the pointer of rb tree
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_rb_delete(bkm_node *t_bkm_node, rb_root *root)
{
    rb_erase(&t_bkm_node->rb_node, root);
    s_list_add(&t_bkm_mng.free_node_list, &t_bkm_node->node);
}

/*******************************************************************************
*function name:ftl_bkm_empty_blk_cmp
*
*description  :compare function for empty tree
*
*parameter    :@d_val0: src for compare;@d_val1: tar fro compare
*
*return value :COMP_SMALL; COMP_BIG; COMP_EQUAL
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
static bool ftl_bkm_empty_blk_cmp(u32 d_val0, u32 d_val1)
{ //TODO
    if(d_val0 < d_val1)
        return COMP_SMALL;
    else if(d_val0 > d_val1)
        return COMP_BIG;
    else
        return COMP_EQUAL;
}

/*******************************************************************************
*function name:ftl_bkm_data_blk_cmp
*
*description  :compare function for data tree
*
*parameter    :@d_val0: src for compare;@d_val1: tar fro compare
*
*return value :COMP_SMALL; COMP_BIG; COMP_EQUAL
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
static bool ftl_bkm_data_blk_cmp(u32 d_val0, u32 d_val1)
{ //TODO
    if(d_val0 < d_val1)
        return COMP_SMALL;
    else if(d_val0 > d_val1)
        return COMP_BIG;
    else
        return COMP_EQUAL;
}

/*******************************************************************************
*function name:ftl_bkm_gbg_blk_cmp
*
*description  :compare function for garbage tree
*
*parameter    :@d_val0: src for compare;@d_val1: tar fro compare
*
*return value :COMP_SMALL; COMP_BIG; COMP_EQUAL
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
static bool ftl_bkm_gbg_blk_cmp(u32 d_val0, u32 d_val1)
{ //TODO
    if(d_val0 < d_val1)
        return COMP_SMALL;
    else if(d_val0 > d_val1)
        return COMP_BIG;
    else
        return COMP_EQUAL;
}

/*******************************************************************************
*function name:ftl_bkm_del_empty_blk
*
*description  :delete block from empty rb tree
*
*parameter    :@b_bank @w_blk
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_del_empty_blk(u8 b_bank, u16 w_blk)
{
    u32      d_val = w_blk + (b_bank << WORD_BIT_CNT);
    bkm_node *t_bkm_node = ftl_bkm_rb_search(d_val, &t_bkm_mng.empty_bkm_tree, ftl_bkm_empty_blk_cmp);
    
    ftl_bkm_rb_delete(t_bkm_node, &t_bkm_mng.empty_bkm_tree);
}

/*******************************************************************************
*function name:ftl_bkm_del_data_blk
*
*description  :delete block from data rb tree
*
*parameter    :@b_bank @w_blk
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_del_data_blk(u8 b_bank, u16 w_blk)
{
    u32      d_val = w_blk + (b_bank << WORD_BIT_CNT);
    bkm_node *t_bkm_node = ftl_bkm_rb_search(d_val, &t_bkm_mng.data_bkm_tree, ftl_bkm_data_blk_cmp);
    
    ftl_bkm_rb_delete(t_bkm_node, &t_bkm_mng.data_bkm_tree);
}

/*******************************************************************************
*function name:ftl_bkm_del_gbg_blk
*
*description  :delete block from garbage rb tree
*
*parameter    :@b_bank @w_blk
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_del_gbg_blk(u8 b_bank, u16 w_blk)
{
    u32      d_val = w_blk + (b_bank << WORD_BIT_CNT);
    bkm_node *t_bkm_node = ftl_bkm_rb_search(d_val, &t_bkm_mng.gbg_bkm_tree, ftl_bkm_gbg_blk_cmp);
    
    ftl_bkm_rb_delete(t_bkm_node, &t_bkm_mng.gbg_bkm_tree);
}

/*******************************************************************************
*function name:ftl_bkm_add_empty_blk
*
*description  :add block to empty rb tree
*
*parameter    :@b_bank @w_blk
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_add_empty_blk(u8 b_bank, u16 w_blk)
{
    u32      d_val       = w_blk + (b_bank << WORD_BIT_CNT);
    //search gbg block tree
    bkm_node *t_bkm_node = ftl_bkm_rb_search(d_val, &t_bkm_mng.gbg_bkm_tree, ftl_bkm_gbg_blk_cmp);

    if(t_bkm_node)
    {   //delete node from gbg blk tree
        ftl_bkm_rb_delete(t_bkm_node, &t_bkm_mng.gbg_bkm_tree);

        ftl_bkm_update_gbg_blk(1, mUPDATE_DEC);
    }
    else
    {
        t_bkm_node      = s_list_entry(s_list_get_entry(&t_bkm_mng.free_node_list), bkm_node, node); 
        t_bkm_node->val = d_val;
    }

    ftl_bkm_rb_insert(t_bkm_node, &t_bkm_mng.empty_bkm_tree, ftl_bkm_empty_blk_cmp);

    ftl_bkm_update_empty_blk(1, mUPDATE_INC);
    ftl_bkm_set_blk_type(b_bank, w_blk, mUSER_NULL);
}

/*******************************************************************************
*function name:ftl_bkm_add_data_blk
*
*description  :add block to data rb tree
*
*parameter    :@b_bank @w_blk
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_add_data_blk(u8 b_bank, u16 w_blk)
{
    u32      d_val       = w_blk + (b_bank << WORD_BIT_CNT);
    //search data block tree
    bkm_node *t_bkm_node = ftl_bkm_rb_search(d_val, &t_bkm_mng.empty_bkm_tree, ftl_bkm_empty_blk_cmp);

    if(t_bkm_node)
    {   ////delete node from empty blk tree
        ftl_bkm_rb_delete(t_bkm_node, &t_bkm_mng.empty_bkm_tree);

        ftl_bkm_update_empty_blk(1, mUPDATE_DEC);
    }
    else
    {
        t_bkm_node      = s_list_entry(s_list_get_entry(&t_bkm_mng.free_node_list), bkm_node, node); 
        t_bkm_node->val = d_val;
    }
    
    ftl_bkm_rb_insert(t_bkm_node, &t_bkm_mng.data_bkm_tree, ftl_bkm_data_blk_cmp);

    ftl_bkm_update_data_blk(1, mUPDATE_INC);
}

/*******************************************************************************
*function name:ftl_bkm_add_gbg_blk
*
*description  :add block to garbage rb tree
*
*parameter    :@b_bank @w_blk
*
*return value :none
*
*revise       :Vito created at:2016-06-30
*******************************************************************************/
void ftl_bkm_add_gbg_blk(u8 b_bank, u16 w_blk)
{
    u32      d_val       = w_blk + (b_bank << WORD_BIT_CNT);
    //search data block tree
    bkm_node *t_bkm_node = ftl_bkm_rb_search(d_val, &t_bkm_mng.data_bkm_tree, ftl_bkm_data_blk_cmp);

    if(t_bkm_node)
    {   //delete node from data blk tree
        ftl_bkm_rb_delete(t_bkm_node, &t_bkm_mng.data_bkm_tree);

        ftl_bkm_update_data_blk(1, mUPDATE_DEC);
    }
    else
    {
        t_bkm_node      = s_list_entry(s_list_get_entry(&t_bkm_mng.free_node_list), bkm_node, node); 
        t_bkm_node->val = d_val;
    }
    
    ftl_bkm_rb_insert(t_bkm_node, &t_bkm_mng.gbg_bkm_tree, ftl_bkm_gbg_blk_cmp);

    ftl_bkm_update_gbg_blk(1, mUPDATE_INC);
}

/*******************************************************************************
*function name:ftl_bkm_alloc_new_block
*
*description  :alloc new block from empty block tree, 
*
*parameter    :@b_user, USER_HOST, USER_GC, USER_FTL, use least version&erase cnt block
*                       USER_WL use last version&ec block
*
*return value :block num
*
*revise       :Vito created at:2016-07-01
*******************************************************************************/
u16 ftl_bkm_alloc_new_block(u8 b_user)
{
    bkm_node    *t_bkm_node;
    u16         w_blk;
    switch(b_user)
    {
        case mUSER_HOST:
        case mUSER_GC:
        case mUSER_FTL:
            t_bkm_node = rb_entry(rb_first(&t_bkm_mng.empty_bkm_tree), bkm_node, rb_node);
            break;
        case mUSER_WL:
            t_bkm_node = rb_entry(rb_last(&t_bkm_mng.empty_bkm_tree), bkm_node, rb_node);
            break;
        default:
            ASSERT_LVL(BKM, ASSERT_LVL0, 0);
    }
    w_blk = t_bkm_node->val & ((1 << WORD_BIT_CNT) - 1);
    ftl_bkm_add_data_blk(t_bkm_node->val >> WORD_BIT_CNT ,w_blk);
    ftl_bkm_set_blk_type(0, w_blk, b_user);
    DBG_MESSAGE("alloc :%3d User:%1d\n", w_blk, b_user);
    return w_blk;
}

/*******************************************************************************
*function name:ftl_bkm_get_data_blk_cnt, ftl_bkm_get_empty_blk_cnt, 
*              ftl_bkm_get_gbg_blk_cnt 
*
*description  :get data/empty/gbg block cnt 
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-07-01
*******************************************************************************/
u16 ftl_bkm_get_data_blk_cnt(void)
{
    return t_bkm_mng.w_data_blk_cnt;
}

u16 ftl_bkm_get_empty_blk_cnt(void)
{
    return t_bkm_mng.w_empty_blk_cnt;
}

u16 ftl_bkm_get_gbg_blk_cnt(void)
{
    return t_bkm_mng.w_gbg_blk_cnt;
}

/*******************************************************************************
*function name:ftl_bkm_update_data_blk, ftl_bkm_update_empty_blk, ftl_bkm_update_gbg_blk
*
*description  :update data/empty/gbg block cnt according to @b_type
*
*parameter    :@w_cnt:          @b_type:mUPDATE_INC , mUPDATE_DEC, mUPDATE_SET
*
*return value :none
*
*revise       :Vito created at:2016-07-01
*******************************************************************************/
void ftl_bkm_update_data_blk(u16 w_cnt, u8 b_type)
{
    switch(b_type)
    {
        case mUPDATE_INC:
            t_bkm_mng.w_data_blk_cnt += w_cnt;
            break;
        case mUPDATE_DEC:
            ASSERT_LVL(BKM, ASSERT_LVL0, t_bkm_mng.w_data_blk_cnt > 0);
            t_bkm_mng.w_data_blk_cnt -= w_cnt;
            break;
        case mUPDATE_SET:
            t_bkm_mng.w_data_blk_cnt = w_cnt;
            break;
        default:
            ASSERT_LVL(BKM, ASSERT_LVL0, 0); // not support
            break;
    }
    ASSERT_LVL(BKM, ASSERT_LVL0, t_bkm_mng.w_data_blk_cnt <= BLOCK_CNT);
}

void ftl_bkm_update_empty_blk(u16 w_cnt, u8 b_type)
{
    switch(b_type)
    {
        case mUPDATE_INC:
            t_bkm_mng.w_empty_blk_cnt += w_cnt;
            break;
        case mUPDATE_DEC:
            ASSERT_LVL(BKM, ASSERT_LVL0, t_bkm_mng.w_empty_blk_cnt > 0);
            t_bkm_mng.w_empty_blk_cnt -= w_cnt;
            break;
        case mUPDATE_SET:
            t_bkm_mng.w_empty_blk_cnt = w_cnt;
            break;
        default:
            ASSERT_LVL(BKM, ASSERT_LVL0, 0); // not support
            break;
    }
    ASSERT_LVL(BKM, ASSERT_LVL0, t_bkm_mng.w_empty_blk_cnt <= BLOCK_CNT);
}

void ftl_bkm_update_gbg_blk(u16 w_cnt, u8 b_type)
{
    switch(b_type)
    {
        case mUPDATE_INC:
            t_bkm_mng.w_gbg_blk_cnt += w_cnt;
            break;
        case mUPDATE_DEC:
            ASSERT_LVL(BKM, ASSERT_LVL0, t_bkm_mng.w_gbg_blk_cnt > 0);
            t_bkm_mng.w_gbg_blk_cnt -= w_cnt;
            break;
        case mUPDATE_SET:
            t_bkm_mng.w_gbg_blk_cnt = w_cnt;
            break;
        default:
            ASSERT_LVL(BKM, ASSERT_LVL0, 0); // not support
            break;
    }
    ASSERT_LVL(BKM, ASSERT_LVL0, t_bkm_mng.w_gbg_blk_cnt <= BLOCK_CNT);
}

u8 ftl_bkm_get_blk_bpc_mode(u8 b_bank, u16 w_blk)
{
    if(mENABLE != PAR_BPC_MODE)
    {
        return MODE_MLC;
    }
    return t_bkm_mng.pt_bkm_ctl[w_blk].byte & BPC_MODE_MSK;
}

void ftl_bkm_set_blk_bpc_mode(u8 b_bank, u16 w_blk, u8 b_bpc_mode)
{
    if(mENABLE != PAR_BPC_MODE)
    {
        return;
    }
    if(MODE_SLC == b_bpc_mode)
    {
        t_bkm_mng.pt_bkm_ctl[w_blk].byte |= (1 << BPC_MODE_OFST);
    }
    else 
    {
        t_bkm_mng.pt_bkm_ctl[w_blk].byte &= ~(1 << BPC_MODE_OFST);
    }
}

void ftl_bkm_set_blk_type(u8 b_bank, u16 w_blk, u8 b_type)
{
    t_bkm_mng.pt_bkm_ctl[w_blk].bit.blk_type = b_type;
}

u8 ftl_bkm_get_blk_type(u8 b_bank, u16 w_blk)
{
    return t_bkm_mng.pt_bkm_ctl[w_blk].bit.blk_type;
}
#if (mENABLE == FTL_BKM_UT_TEST)
void ut_dbg_print_result(rb_root *root)
{
    bkm_node *t_bkm_node;
    rb_node  *t_rb_node = rb_first(root);
    u8       b_idx      = 0;
    DBG_MESSAGE("print <%s> blk list\n", ((u32)root == (u32)(&t_bkm_mng.data_bkm_tree)) ? "DATA" : \
                            (((u32)root == (u32)(&t_bkm_mng.empty_bkm_tree)) ? "EMPTY" : \
                            (((u32)root == (u32)(&t_bkm_mng.gbg_bkm_tree)) ? "GBG" : "NULL")));
    while(t_rb_node)
    {
        t_bkm_node = rb_entry(t_rb_node, bkm_node, rb_node);
        DBG_MESSAGE("%2d Blk:%3d ||", b_idx, t_bkm_node->val);
        b_idx ++;
        if((b_idx % 10) == 0)
        {
            DBG_MESSAGE("\n");
        }
        t_rb_node = rb_next(t_rb_node);
    }
    DBG_MESSAGE("END\n");
}
void ftl_bkm_data_blk_test(void)
{
    u16 w_empty[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                     18,22,28,36,48,88,120};
    u16 w_data[20] = {18, 12, 2, 28, 120, 5, 88, 7, 9, 8, 10, 36, 1,
                     0,22,3,11,48,6,4};
    u16 w_gbg[20] = {18, 9, 120, 28, 36, 5, 88, 7, 4, 12, 10, 2, 1,
                     0,22,3,11,48,6,8};
    u8 b_empty_idx, b_data_idx, b_gbg_idx;
    u16 w_type;
//init bkm
    ftl_bkm_init();
//start test
    for(b_empty_idx = 0; b_empty_idx < 20; b_empty_idx++)
    {
        ftl_bkm_add_empty_blk(0, w_empty[b_empty_idx]);
    }
    ut_dbg_print_result(&t_bkm_mng.empty_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.data_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.gbg_bkm_tree);
    DBG_MESSAGE("ADD Blk to Data List\n");
    for(b_data_idx = 0; b_data_idx < 10; b_data_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_data_idx, w_data[b_data_idx]);
        if((b_data_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_data_blk(0, w_data[b_data_idx]);
        //ftl_bkm_del_empty_blk(0, w_data[b_data_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.empty_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.data_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.gbg_bkm_tree);

    DBG_MESSAGE("ADD Blk to Data List\n");
    for(; b_data_idx < 20; b_data_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_data_idx, w_data[b_data_idx]);
        if((b_data_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_data_blk(0, w_data[b_data_idx]);
        //ftl_bkm_del_empty_blk(0, w_data[b_data_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.empty_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.data_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.gbg_bkm_tree);

    DBG_MESSAGE("ADD Blk to GBG List\n");
    for(b_gbg_idx = 0; b_gbg_idx < 20; b_gbg_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_gbg_idx, w_gbg[b_gbg_idx]);
        if((b_gbg_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_gbg_blk(0, w_gbg[b_gbg_idx]);
        //ftl_bkm_del_data_blk(0, w_gbg[b_gbg_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.empty_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.data_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.gbg_bkm_tree);

    DBG_MESSAGE("ADD Blk to EMPTY List\n");
    for(b_gbg_idx = 0; b_gbg_idx < 20; b_gbg_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_gbg_idx, w_gbg[b_gbg_idx]);
        if((b_gbg_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_empty_blk(0, w_gbg[b_gbg_idx]);
        //ftl_bkm_del_gbg_blk(0, w_gbg[b_gbg_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.empty_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.data_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.gbg_bkm_tree);

    DBG_MESSAGE("Host:%3d GC:%3d FTL:%3d WL:%3d\n", ftl_bkm_alloc_new_block(mUSER_HOST),\
        ftl_bkm_alloc_new_block(mUSER_GC),ftl_bkm_alloc_new_block(mUSER_FTL),\
        ftl_bkm_alloc_new_block(mUSER_WL));
    ut_dbg_print_result(&t_bkm_mng.empty_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.data_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.gbg_bkm_tree);

    for(b_gbg_idx = 0; b_gbg_idx < 10; b_gbg_idx++)
    {
        ftl_bkm_alloc_new_block(mUSER_HOST);
    }
    ut_dbg_print_result(&t_bkm_mng.empty_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.data_bkm_tree);
    ut_dbg_print_result(&t_bkm_mng.gbg_bkm_tree);

    for(b_gbg_idx = 0; b_gbg_idx < 20; b_gbg_idx++)
    {
        w_type = ftl_bkm_get_blk_type(0, w_empty[b_gbg_idx]);
        DBG_MESSAGE("%1d --> %s ", w_empty[b_gbg_idx], (w_type == mUSER_HOST) ? "HOST" : \
            ((w_type == mUSER_GC) ? "  GC" : ((w_type == mUSER_FTL) ? " FTL" :\
            ((w_type == mUSER_WL) ? "  WL" : "EMPY"))));

        if((b_gbg_idx & 0x07) == 0x03)
        {
            DBG_MESSAGE("\n");
        }
    }
    DBG_MESSAGE("\n");
}
#endif
