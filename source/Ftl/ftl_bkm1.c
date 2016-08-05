
#if 0
#include <global_def.h>
#include <ftl_bkm1.h>

#define     MAX_BKM_NODE_CNT        100

#define COMP_SMALL      0
#define COMP_BIG        1
#define COMP_EQUAL      2

typedef struct st_bkm_node
{
    s_list      node;           //s list node           //1DW
    rb_node     rb_node;        //node                  //3DW
    u32         val;            //use to record blk num //1DW
}bkm_node;

typedef bool (*comp_fun)(u32 , u32 );

typedef struct
{
    bkm_node     *p_bkm_node;       //point to bkm node addr 
    s_list       free_node_list;    //single list used to manager empty bkm node
    rb_root      bkm_tree[BKM_CNT]; //rb tree for bkm management
    comp_fun     func[BKM_CNT];     //function used to compare val
    bool         f_is_init;
}t_bkm_mng_type;

static t_bkm_mng_type t_bkm_mng;

static bool ftl_bkm_data_blk_cmp(u32 d_val0, u32 d_val1);
static bool ftl_bkm_empty_blk_cmp(u32 d_val0, u32 d_val1);
static bool ftl_bkm_gbg_blk_cmp(u32 d_val0, u32 d_val1);


void ftl_bkm_init(void)
{
    u16 w_idx;
    t_bkm_mng.f_is_init = TRUE;
    t_bkm_mng.p_bkm_node = (bkm_node *)malloc(sizeof(bkm_node) * MAX_BKM_NODE_CNT);
    s_list_init_head(&t_bkm_mng.free_node_list);
    for(w_idx = 0; w_idx < MAX_BKM_NODE_CNT; w_idx++)
    {
        s_list_add(&t_bkm_mng.free_node_list, &t_bkm_mng.p_bkm_node[w_idx].node);
    }

    for(w_idx = 0; w_idx < BKM_CNT; w_idx++)
    {
        t_bkm_mng.bkm_tree[w_idx].rb_node   = NULL;
    }

    t_bkm_mng.func[BKM_DATA]                = ftl_bkm_data_blk_cmp;
    t_bkm_mng.func[BKM_EMPTY]               = ftl_bkm_empty_blk_cmp;
    t_bkm_mng.func[BKM_GBG]                 = ftl_bkm_gbg_blk_cmp;
}

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

void ftl_bkm_del_blk(u8 b_blk_type, u8 b_bank, u16 w_blk)
{
    rb_root  *bkm_tree      = &t_bkm_mng.bkm_tree[b_blk_type];
    comp_fun func           = t_bkm_mng.func[b_blk_type];
    u32      d_val          = w_blk + (b_bank << WORD_BIT_CNT);
    bkm_node *t_bkm_node    = ftl_bkm_rb_search(d_val, bkm_tree, func);
    
    ftl_bkm_rb_delete(t_bkm_node, bkm_tree);
}

void ftl_bkm_add_blk(u8 b_blk_type, u8 b_bank, u16 w_blk)
{
    u8       src_blk_type   = (b_blk_type + 2)%3;
    rb_root  *tar_bkm_tree  = &t_bkm_mng.bkm_tree[b_blk_type];
    rb_root  *src_bkm_tree  = &t_bkm_mng.bkm_tree[src_blk_type];
    comp_fun tar_func       = t_bkm_mng.func[b_blk_type];
    comp_fun src_func       = t_bkm_mng.func[src_blk_type];
    
    u32      d_val          = w_blk + (b_bank << WORD_BIT_CNT);
    //search block tree
    bkm_node *t_bkm_node    = ftl_bkm_rb_search(d_val, src_bkm_tree, src_func);

    if(t_bkm_node)
    {   //delete node from gbg blk tree
        ftl_bkm_rb_delete(t_bkm_node, src_bkm_tree);
    }
    else
    {
        t_bkm_node      = s_list_entry(s_list_get_entry(&t_bkm_mng.free_node_list), bkm_node, node); 
        t_bkm_node->val = d_val;
    }

    ftl_bkm_rb_insert(t_bkm_node, tar_bkm_tree, tar_func);
}


#if (mENABLE == FTL_BKM1_UT_TEST)
void ut_dbg_print_result(rb_root *root)
{
    bkm_node *t_bkm_node;
    rb_node  *t_rb_node = rb_first(root);
    u8       b_idx      = 0;
    DBG_MESSAGE("print <%s> blk list\n", ((u32)root == (u32)(&t_bkm_mng.bkm_tree[0])) ? "DATA" : \
                            (((u32)root == (u32)(&t_bkm_mng.bkm_tree[1])) ? "EMPTY" : \
                            (((u32)root == (u32)(&t_bkm_mng.bkm_tree[2])) ? "GBG" : "NULL")));
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
//init bkm
    ftl_bkm_init();
//start test
    for(b_empty_idx = 0; b_empty_idx < 20; b_empty_idx++)
    {
        ftl_bkm_add_blk(BKM_EMPTY, 0, w_empty[b_empty_idx]);
    }
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_EMPTY]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_DATA]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_GBG]);
    DBG_MESSAGE("ADD Blk to Data List\n");
    for(b_data_idx = 0; b_data_idx < 10; b_data_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_data_idx, w_data[b_data_idx]);
        if((b_data_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_blk(BKM_DATA, 0, w_data[b_data_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_EMPTY]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_DATA]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_GBG]);

    DBG_MESSAGE("ADD Blk to Data List\n");
    for(; b_data_idx < 20; b_data_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_data_idx, w_data[b_data_idx]);
        if((b_data_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_blk(BKM_DATA, 0, w_data[b_data_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_EMPTY]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_DATA]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_GBG]);

    DBG_MESSAGE("ADD Blk to GBG List\n");
    for(b_gbg_idx = 0; b_gbg_idx < 20; b_gbg_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_gbg_idx, w_gbg[b_gbg_idx]);
        if((b_gbg_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_blk(BKM_GBG, 0, w_gbg[b_gbg_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_EMPTY]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_DATA]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_GBG]);

    DBG_MESSAGE("ADD Blk to EMPTY List\n");
    for(b_gbg_idx = 0; b_gbg_idx < 20; b_gbg_idx++)
    {
        DBG_MESSAGE("%2d Blk:%3d ||", b_gbg_idx, w_gbg[b_gbg_idx]);
        if((b_gbg_idx % 10) == 9)
        {
            DBG_MESSAGE("\n");
        }
        ftl_bkm_add_blk(BKM_EMPTY, 0, w_gbg[b_gbg_idx]);
        //ftl_bkm_del_gbg_blk(0, w_gbg[b_gbg_idx]);
    }
    DBG_MESSAGE("\n");
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_EMPTY]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_DATA]);
    ut_dbg_print_result(&t_bkm_mng.bkm_tree[BKM_GBG]);
}
#endif

#endif

