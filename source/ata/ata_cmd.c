#include <global_def.h>
#include <ftl.h>
#include <ftl_core.h>
#include <ftl_ap.h>
#include <ata_cmd.h>
#include <nfc_queue.h>
#include <flash_reg.h>

#define CMD_QUEUE_CNT           (32)
#define MAX_CTRL_NODE_CNT       (5)

struct list_head        ata_cmd_list;
struct list_head        ata_read_cmd_list;
struct list_head        ata_read_dma_list;
struct list_head        ata_write_cmd_list;
struct list_head        ata_write_dma_list;

t_ata_cmd_type          *cur_ata_read_dma_cmd;
t_ata_cmd_type          *cur_ata_write_dma_cmd;

u8                      b_ctrl_node_cnt;
u8                      b_ata_cmd_cnt;

/*******************************************************************************
*function name:ata_cmd_init
*
*description  :init ata cmd module
*
*parameter    :none
*
*return value :none 
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_init(void)
{
    //t_ata_mng.b_ata_cmd_head = 0;
    //t_ata_mng.b_ata_cmd_tail = 0;
    ///memset_u32((u32 *)t_ata_mng.t_ata_cmd, (CMD_QUEUE_CNT * sizeof(t_ata_cmd_type)) >> DWRD_BYTE_EXP);

    INIT_LIST_HEAD(&ata_cmd_list);
    INIT_LIST_HEAD(&ata_read_cmd_list);
    INIT_LIST_HEAD(&ata_read_dma_list);
    INIT_LIST_HEAD(&ata_write_cmd_list);
    INIT_LIST_HEAD(&ata_write_dma_list);

    cur_ata_read_dma_cmd    = 0;
    cur_ata_write_dma_cmd   = 0;
    b_ctrl_node_cnt     = 0;
    b_ata_cmd_cnt       = 0;
}

/*******************************************************************************
*function name:ata_cmd_add_new_cmd
*
*description  :add new cmd to cmd list
*
*parameter    :ata cmd parament,@cmd_type @len, @lba, @cmd_tag
*
*return value :none 
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_add_new_cmd(u16 w_cmd_type, u16 w_len, u32 d_lba, u32 d_cmd_tag)
{
    t_ata_cmd_type *p_ata_cmd = (t_ata_cmd_type *)malloc(sizeof(t_ata_cmd_type), TRUE);

    DBG_MESSAGE("<NC>add new cmd %8x, (%2x, %2x, %4x, %4x)\n", (u32)p_ata_cmd, \
        w_cmd_type, w_len, d_lba, d_cmd_tag);
    
    ASSERT_LVL(ATA, ASSERT_LVL0, p_ata_cmd != NULL);
    
    p_ata_cmd->w_cmd_type   = w_cmd_type;
    p_ata_cmd->w_len        = w_len;
    p_ata_cmd->w_len_bak    = w_len;
    p_ata_cmd->d_start_lba  = d_lba;
    p_ata_cmd->d_start_lba_bak = d_lba;
    p_ata_cmd->d_cmd_tag    = d_cmd_tag;

    list_add_tail(&p_ata_cmd->node, &ata_cmd_list);
    b_ata_cmd_cnt++;
    ASSERT_LVL(ATA, ASSERT_LVL0, b_ata_cmd_cnt < CMD_QUEUE_CNT);
    
}

/*******************************************************************************
*function name:ata_cmd_check_queue_empty
*
*description  :check cmd list is empty or not
*
*parameter    :none
*
*return value :TRUE:cmd queue is empty, FALSE:cmd queue is not empty 
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
bool ata_cmd_check_queue_empty(void)
{
    return list_empty(&ata_cmd_list);
}

/*******************************************************************************
*function name:ata_cmd_handle
*
*description  :handle ata cmd according to cmd queue list
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_handle(void)
{
    t_ata_cmd_type *p_ata_cmd;
    if(TRUE == ata_cmd_check_queue_empty())
    {
        return; //no ata cmd to handle
    }
    p_ata_cmd = list_first_entry(&ata_cmd_list,t_ata_cmd_type,node);
    DBG_MESSAGE("<CH>cmd buff:%8x\n", (u32)p_ata_cmd);
    switch(p_ata_cmd->w_cmd_type)
    {
        case ATA_CMD_READ:
            ata_cmd_pre_handle(p_ata_cmd);
            ata_cmd_read_handle();
            break;
        case ATA_CMD_WRITE:
            ata_cmd_pre_handle(p_ata_cmd);
            ata_cmd_write_handle();
            break;
        default:
            ata_cmd_other_handle(p_ata_cmd);
            break;
    }
}

/*******************************************************************************
*function name:ata_cmd_add_node_read_list
*
*description  :add cmd node to read dma list,called by ftl/nfc module
*
*parameter    :@t_ctrl_node
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_add_node_read_list(t_ctrl_node_type *p_ctrl_node)
{
    list_add_tail(&p_ctrl_node->node, &ata_read_dma_list);
}

/*******************************************************************************
*function name:ata_cmd_add_node_write_list
*
*description  :add cmd node to write dma list,called ata module only
*
*parameter    :@t_ctrl_node
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_add_node_write_list(t_ctrl_node_type *t_ctrl_node)
{
    list_del(&t_ctrl_node->node);
    list_add_tail(&t_ctrl_node->node, &ata_write_dma_list);
}

/*******************************************************************************
*function name:ata_cmd_pre_handle
*
*description  :pre handle of ata cmd,alloc node and buff to read/write cmd
*
*parameter    :@p_ata_cmd
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_pre_handle(t_ata_cmd_type *p_ata_cmd)
{
    t_ata_cmd_type *p_read_ata_cmd = NULL;
    t_ctrl_node_type *t_ctrl_node;
    while(b_ctrl_node_cnt < MAX_CTRL_NODE_CNT)
    {
        t_ctrl_node = (t_ctrl_node_type *)malloc(sizeof(t_ctrl_node_type), TRUE);
        ASSERT_LVL(ATA,ASSERT_LVL0,t_ctrl_node != NULL);
        if(NULL == t_ctrl_node)
        {
            return;
        }
        
        //alloc buff
        t_ctrl_node->p_buff = (u32 *)malloc(LTU_BYTE_CNT, FALSE);
        ASSERT_LVL(ATA,ASSERT_LVL0,t_ctrl_node->p_buff != NULL);
        if(NULL == t_ctrl_node->p_buff)
        {
            free(t_ctrl_node);
            return;
        }
        t_ctrl_node->b_user     = mUSER_HOST;
        t_ctrl_node->d_lba      = p_ata_cmd->d_start_lba;
        t_ctrl_node->w_len      = p_ata_cmd->w_len > (LTU_LBA_CNT - (p_ata_cmd->d_start_lba & LTU_LBA_MSK)) ? \
                                   (LTU_LBA_CNT - (p_ata_cmd->d_start_lba & LTU_LBA_MSK)) : p_ata_cmd->w_len;
        
        t_ctrl_node->d_tag      = p_ata_cmd->d_cmd_tag;
        

        p_ata_cmd->d_start_lba  += t_ctrl_node->w_len;
        p_ata_cmd->w_len        -= t_ctrl_node->w_len;

        DBG_MESSAGE("<PH>tag:%2d lba:%8x len:%3x\n", t_ctrl_node->d_tag, t_ctrl_node->d_lba, \
            t_ctrl_node->w_len);
        if(ATA_CMD_READ == p_ata_cmd->w_cmd_type)
        {
            t_ctrl_node->w_op_type  = OP_HOST_READ;
            list_for_each_entry(t_ata_cmd_type, p_read_ata_cmd, &ata_read_cmd_list, node)
            {
                if(p_read_ata_cmd->d_cmd_tag == p_ata_cmd->d_cmd_tag)
                {
                    break;
                }
            }
            if(p_read_ata_cmd->d_cmd_tag != p_ata_cmd->d_cmd_tag)
            {
                p_read_ata_cmd = (t_ata_cmd_type *)malloc(sizeof(t_ata_cmd_type), TRUE);
                p_read_ata_cmd->w_cmd_type      = p_ata_cmd->w_cmd_type;
                p_read_ata_cmd->d_cmd_tag       = p_ata_cmd->d_cmd_tag;
                p_read_ata_cmd->d_start_lba_bak = p_ata_cmd->d_start_lba_bak;
                p_read_ata_cmd->w_len_bak       = p_ata_cmd->w_len_bak;
                list_add_tail(&p_read_ata_cmd->node, &ata_read_cmd_list);
            }
            ata_cmd_trans_node(t_ctrl_node);
        }
        else if(ATA_CMD_WRITE == p_ata_cmd->w_cmd_type)
        {
            t_ctrl_node->w_op_type  = OP_HOST_WRITE;
            t_ctrl_node->b_status   = mATA_WAIT_DMA;
            DBG_MESSAGE("<PH>add write dma list:%8x\n", (u32)t_ctrl_node);
            list_add_tail(&t_ctrl_node->node, &ata_write_dma_list);
        }
        ASSERT_LVL(ATA, ASSERT_LVL0, b_ctrl_node_cnt < MAX_CTRL_NODE_CNT);
        b_ctrl_node_cnt++;
        
        if(0 == p_ata_cmd->w_len)
        {
            DBG_MESSAGE("<PH>cmd slice done%8x\n", (u32)p_ata_cmd);
            list_del(&p_ata_cmd->node);
            if(ATA_CMD_READ == p_ata_cmd->w_cmd_type)
                //list_add_tail(&p_ata_cmd->node, &ata_read_cmd_list);
            {
                free(p_ata_cmd);
            }
            else if(ATA_CMD_WRITE == p_ata_cmd->w_cmd_type)
                list_add_tail(&p_ata_cmd->node, &ata_write_cmd_list); // maybe no use
            else
                ASSERT_LVL(ATA, ASSERT_LVL0, 0);

            return;
        }
        
    }
}

/*******************************************************************************
*function name:ata_cmd_read_handle
*
*description  :dma data to host handle
*
*parameter    :@none
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_read_handle(void)
{
    t_ctrl_node_type    *p_ctrl_node;
    t_ata_cmd_type      *p_ata_cmd;

    if(TRUE == list_empty(&ata_read_dma_list))
    {
        return;
    }
    
    list_for_each_entry(t_ata_cmd_type, p_ata_cmd, &ata_read_cmd_list, node)
    {
        if(0 != cur_ata_read_dma_cmd)
        {
            p_ata_cmd = cur_ata_read_dma_cmd; 
        }
        list_for_each_entry(t_ctrl_node_type,p_ctrl_node,&ata_read_dma_list,node)
        {
            if((p_ata_cmd->d_cmd_tag == p_ctrl_node->d_tag) && \
                (p_ata_cmd->d_start_lba_bak == p_ctrl_node->d_lba))
            {
                if(0 == cur_ata_read_dma_cmd)
                {
                    cur_ata_read_dma_cmd = p_ata_cmd;
                }
                DBG_MESSAGE("<RH>handle read cmd:(%2d %8x %3x)\n", p_ata_cmd->d_cmd_tag, \
                    p_ctrl_node->d_lba, p_ctrl_node->w_len);
                switch(p_ctrl_node->b_status)
                {
                    case mATA_WAIT_DMA:
                        //start dma;
                        DBG_MESSAGE("<RH>start dma\n");
                        p_ctrl_node->b_status = mATA_DMAING;
                        break;
                    case mATA_DMAING:
                        //wait dma done;
                        DBG_MESSAGE("<RH>dma done\n");
                        p_ctrl_node->b_status = mATA_DMA_DONE;
                        list_del(&p_ctrl_node->node);

                        p_ata_cmd->d_start_lba_bak  += p_ctrl_node->w_len;
                        p_ata_cmd->w_len_bak        -= p_ctrl_node->w_len;
                        if(0 == p_ata_cmd->w_len_bak)
                        {
                            //return status to host cmd finished
                            DBG_MESSAGE("<RH>free cmd (%2d)\n", p_ata_cmd->d_cmd_tag);
                            list_del(&p_ata_cmd->node);
                            free(p_ata_cmd);
                            cur_ata_read_dma_cmd = NULL;
                        }
                        free(p_ctrl_node->p_buff);
                        free(p_ctrl_node);
                        ASSERT_LVL(ATA, ASSERT_LVL0, b_ctrl_node_cnt != 0);
                        b_ctrl_node_cnt--;
                        break;
                }
                return;
            }
        }
        if(0 != cur_ata_read_dma_cmd)
        {
            return;
        }
    }
}

/*******************************************************************************
*function name:ata_cmd_write_handle
*
*description  :receive host data and trans node to ftl module
*
*parameter    :@none
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_write_handle(void)
{
    t_ctrl_node_type    *p_ctrl_node;
    t_ata_cmd_type      *p_ata_cmd;
    
    while(1)
    //list_for_each_entry(t_ctrl_node_type, p_ctrl_node, &ata_write_dma_list, node)
    {
        if(TRUE == list_empty(&ata_write_dma_list))
        {
            return;
        }
        p_ctrl_node = list_first_entry(&ata_write_dma_list, t_ctrl_node_type, node);

        DBG_MESSAGE("<WH>cmd buff:%8x, (tag:%2d LBA:%8x len:%3x)\n", (u32)p_ctrl_node,\
            p_ctrl_node->d_tag, p_ctrl_node->d_lba, p_ctrl_node->w_len);
        switch(p_ctrl_node->b_status)
        {
            case mATA_WAIT_DMA:
                //start receive host data
                DBG_MESSAGE("<WH>dma data\n");
                p_ctrl_node->b_status = mATA_DMAING;
                return;
            case mATA_DMAING:
                //check dma done
                DBG_MESSAGE("<WH>dma done\n");
                p_ctrl_node->b_status = mFTL_TRANS;
                //list_del(&p_ctrl_node->node);
                ata_cmd_trans_node(p_ctrl_node);
                break;
        }
        if(FALSE == list_empty(&ata_write_cmd_list))
        {
            p_ata_cmd   = list_first_entry(&ata_write_cmd_list, t_ata_cmd_type, node);
            if((p_ata_cmd->d_start_lba_bak + p_ata_cmd->w_len_bak) == \
                (p_ctrl_node->d_lba + p_ctrl_node->w_len))
            { //write cmd finished
                //retrun status to indicate write finished
                DBG_MESSAGE("<WH>Free cmd(%d)\n", p_ata_cmd->d_cmd_tag);
                list_del(&p_ata_cmd->node);
                free(p_ata_cmd);
            }
        }
    }
    
    
    
}

/*******************************************************************************
*function name:ata_cmd_other_handle
*
*description  :other cmd handle
*
*parameter    :@none
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_other_handle(t_ata_cmd_type *p_ata_cmd)
{
    
}

/*******************************************************************************
*function name:ata_cmd_rotation
*
*description  :handle read/write cmd,called by other module in busy time
*
*parameter    :@none
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_rotation(void)
{
    u8 b_ce = *REGAccessByte(F0CEN);
    b_ce &= ~0x01;

    *REGAccessByte(F0CEN) = 0xFF;
    *REGAccessByte(F0CEN) = b_ce;
    ata_cmd_handle();
    *REGAccessByte(F0CEN) = 0xFF;
    *REGAccessByte(F0CEN) = b_ce;
    //check read list
    ata_cmd_read_handle();
    *REGAccessByte(F0CEN) = 0xFF;
    *REGAccessByte(F0CEN) = b_ce;
    //check write list
    ata_cmd_write_handle();
    b_ce |= 0x01;
    *REGAccessByte(F0CEN) = 0xFF;
    *REGAccessByte(F0CEN) = b_ce;
}

/*******************************************************************************
*function name:ata_cmd_trans_node
*
*description  :trnas ctrl_node to ohter module or other module trans finished node 
*              to ata cmd module
*
*parameter    :@p_ctrl_node
*
*return value :none
*
*revise       :Vito created at:2016-07-25
*******************************************************************************/
void ata_cmd_trans_node(t_ctrl_node_type *p_ctrl_node)
{
    p_ctrl_node->b_status = mFTL_TRANS;
    if(p_ctrl_node->w_op_type == OP_HOST_WRITE)
    {
        list_del(&p_ctrl_node->node);
    }
    ftl_core_add_trans_node(p_ctrl_node);
}

void ata_cmd_add_node_cnt(void)
{
    ASSERT_LVL(ATA, ASSERT_LVL0, b_ctrl_node_cnt < MAX_CTRL_NODE_CNT);
    b_ctrl_node_cnt++;
}

void ata_cmd_del_node_cnt(void)
{
    ASSERT_LVL(ATA, ASSERT_LVL0, b_ctrl_node_cnt != 0);
    b_ctrl_node_cnt--;
}

#if (ATA_CMD_UT_TEST_ENABLE == mENABLE)
#define ATA_CMD_UT_CASE_CNT     9
void ata_cmd_ut_test(void)
{
    typedef struct
    {
        u32 d_lba;
        u32 d_cmd_tag;
        u16 w_len;
    }t_ata_cmd_ut_case_type;

    t_ata_cmd_ut_case_type t_ata_cmd_case[ATA_CMD_UT_CASE_CNT] ={
        {0, 1, 14}, {65536, 15, 20}, {1024, 2, 478}, {2048, 8, 63},
        {4096, 4, 3},{1024+1024*1024, 6, 7}, {1024, 7, 478},{1024*1024, 9, 14},
        {0, 12, 3}
    };
    u8 b_case_idx = 0;
    ata_cmd_init();
#if 1
    while(b_case_idx < ATA_CMD_UT_CASE_CNT)
    {
        ata_cmd_add_new_cmd(ATA_CMD_WRITE, t_ata_cmd_case[b_case_idx].w_len, \
            t_ata_cmd_case[b_case_idx].d_lba,t_ata_cmd_case[b_case_idx].d_cmd_tag);
        b_case_idx++;
    }
#endif
    while(1)
    {
        
        //ata_cmd_rotation();
        //ata_cmd_rotation();
        //ata_cmd_rotation();
        //ata_cmd_handle();

        //ftl translation

        //nfc handle
        //ata_cmd_rotation();
        //ata_cmd_rotation();
        ata_cmd_rotation();
        ftl_core_rotation();
        nfc_queue_rotation(FALSE);
    }
}
#endif
