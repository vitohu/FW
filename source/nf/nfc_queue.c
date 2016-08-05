
#include <global_def.h>
#include <ftl.h>
#include <ftl_ap.h>
#include <ata_cmd.h>
#include <ftl_core.h>
#include <nfc_queue.h>
#include <flash_reg.h>
#include <nf_cmd.h>

#define QUEUE_IDLE      0
#define QUEUE_READ      1
#define QUEUE_WRITE     2


struct list_head nfc_host_read_list[FLASH_CHANNEL_CNT];

typedef struct
{
    struct list_head    nfc_write_list[FLASH_CHANNEL_CNT][mUSER_CNT];
    struct list_head    nfc_write_queue[FLASH_CHANNEL_CNT];
    u32                 d_current_ptu[FLASH_CHANNEL_CNT];
}t_write_queue_mng_type;

typedef struct
{
    struct list_head    nfc_read_queue[FLASH_CHANNEL_CNT];
    u32                 d_current_ptu[FLASH_CHANNEL_CNT];
}t_read_queue_mng_type;

t_write_queue_mng_type  t_write_queue_mng;
t_read_queue_mng_type   t_read_queue_mng;
    
u8   b_excute_queue_state;

u32	gdCH0SpareData[32];		/* 硬件需要保存ECC code */
u32	gdCH1SpareData[32];


typedef void (*nfc_cmd_func)(t_ctrl_node_type *);
typedef enum 
{
    mNFC_READ_IDLE = 0,
    mNFC_READ_WAIT_READY,
    mNFC_READ_WAIT_DMA_DONE,

    mNFC_WRITE_IDLE,
    mNFC_WRITE_WAIT_DMA_DONE,
    mNFC_CMD_STATE_CNT,
    mNFC_CMD_DONE = mNFC_CMD_STATE_CNT
}nfc_cmd_state_type;

typedef struct
{
    nfc_cmd_state_type  state;
    nfc_cmd_func        func;
}nfc_cmd_struct_type;

void nfc_read_cmd_idle_handle(t_ctrl_node_type *p_ctrl_node);
void nfc_read_wait_ready_handle(t_ctrl_node_type *p_ctrl_node);
void nfc_read_wait_dma_handle(t_ctrl_node_type *p_ctrl_node);
void nfc_write_cmd_idle_handle(t_ctrl_node_type *p_ctrl_node);
void nfc_write_wait_dma_handle(t_ctrl_node_type *p_ctrl_node);


static nfc_cmd_struct_type nfc_cmd_handle[mNFC_CMD_STATE_CNT] = 
{
    {mNFC_READ_IDLE,             nfc_read_cmd_idle_handle},
    {mNFC_READ_WAIT_READY,       nfc_read_wait_ready_handle},
    {mNFC_READ_WAIT_DMA_DONE,    nfc_read_wait_dma_handle},

    {mNFC_WRITE_IDLE,             nfc_write_cmd_idle_handle},
    {mNFC_WRITE_WAIT_DMA_DONE,    nfc_write_wait_dma_handle}
};



void nfc_queue_init(void)
{
    u8 b_idx;
    u8 b_user;
    for(b_idx = 0; b_idx < FLASH_CHANNEL_CNT; b_idx++)
    {
        INIT_LIST_HEAD(&t_read_queue_mng.nfc_read_queue[b_idx]);
        t_read_queue_mng.d_current_ptu[b_idx] = INVALID32;
    }

    for(b_idx = 0; b_idx < FLASH_CHANNEL_CNT; b_idx++)
    {
        for(b_user = mUSER_HOST; b_user < mUSER_CNT; b_user++)
        {
            INIT_LIST_HEAD(&t_write_queue_mng.nfc_write_list[b_idx][b_user]);
        }
        INIT_LIST_HEAD(&t_write_queue_mng.nfc_write_queue[b_idx]);
        t_write_queue_mng.d_current_ptu[b_idx] = INVALID32;
    }
    b_excute_queue_state = QUEUE_IDLE;
}


void nfc_queue_add_write_list(t_ctrl_node_type *p_ctrl_node)
{
    t_ctrl_node_type    *t_ctrl_node;
    struct list_head    *p_write_list;
    u8                  b_idx, b_node_idx;
    u8                  b_ch = PTU_GET_CH(p_ctrl_node->d_ptu);
    
    p_write_list            = &t_write_queue_mng.nfc_write_list[b_ch][p_ctrl_node->b_user];

    if(PTU_GET_OFST(p_ctrl_node->d_ptu) == (OFST_CNT - 1))
        p_ctrl_node->f_write_trig = TRUE;
    else
        p_ctrl_node->f_write_trig = FALSE;
    p_ctrl_node->b_status   = mNFC_WRITE_IDLE;
    list_add_tail(&p_ctrl_node->node, p_write_list);
    DBG_MESSAGE("<WQ> add node to write list,%8x ch:%1d ptu:%8x\n", (u32)p_ctrl_node, \
        b_ch, p_ctrl_node->d_ptu);
    if(TRUE == p_ctrl_node->f_write_trig)
    {
        if(mUSER_FTL == p_ctrl_node->b_user)
        {
            ftl_core_force_dump_host_data();
        }
        while(1)
        {
            t_ctrl_node = list_first_entry(p_write_list, t_ctrl_node_type, node);

            DBG_MESSAGE("<WQ> add node to write queue,%8x\n", (u32)t_ctrl_node);
            list_del(&t_ctrl_node->node);
            list_add_tail(&t_ctrl_node->node,&t_write_queue_mng.nfc_write_queue[b_ch]);
            if(TRUE == t_ctrl_node->f_write_trig)
            {
                break;
            }
        }
        if(mUSER_FTL == p_ctrl_node->b_user)
        {
            nfc_write_handle(TRUE);
        }
        
    }
    
    #if 0
    if(TRUE == p_ctrl_node->f_write_trig)
    {
        b_idx = 0;
        //list_for_each_entry(t_ctrl_node_type,t_ctrl_node,&nfc_host_write_list,node)
        while(1)
        {
            t_ctrl_node = list_first_entry(&nfc_host_write_list, t_ctrl_node_type, node);
            b_node_idx = PTU_GET_OFST(t_ctrl_node->d_ptu) + (PTU_GET_BANK(t_ctrl_node->d_ptu) << OFST_BIT);
            ASSERT_LVL(NFC, ASSERT_LVL0, b_idx == (b_node_idx & 0x03));
            if(b_idx == 0)
            {
                //send write cmd
                //cal_row_addr();
                //send_write_cmd();
            }
            
            //start dma data
            //write_dma_start(); //dma 4KB data
            
            ata_cmd_rotation();
            list_del(&t_ctrl_node->node);
            free(t_ctrl_node->p_buff);
            free(t_ctrl_node);
            if(ATA_CMD_WRITE == t_ctrl_node->w_op_type)
            {
                ata_cmd_del_node_cnt();
            }
            
            //wait_write_dma_done();

            if((3 == b_idx) || (7 == b_idx))
            {
                if(TRUE == t_ctrl_node->f_write_trig)
                {
                    //send_write_cmd_10();
                    return;
                }
                else
                {
                    //send_write_cmd_11();
                    //change_plane_bit();
                    //send_write_cmd_81();
                }
                
            }
            b_idx++;
        }
        //wait_write_dma_done();
    }
    #endif   
}

void nfc_queue_add_read_list(t_ctrl_node_type *p_ctrl_node)
{
    u8 b_ch = PTU_GET_CH(p_ctrl_node->d_ptu);
    p_ctrl_node->b_status = mNFC_READ_IDLE;
    list_add_tail(&p_ctrl_node->node, &t_read_queue_mng.nfc_read_queue[b_ch]);

    DBG_MESSAGE("<RQ> add node to read queue,%8x user:%d ch:%1d ptu:%8x\n", (u32)p_ctrl_node, \
        p_ctrl_node->b_user, b_ch, p_ctrl_node->d_ptu);
    #if 0
    if(OP_HOST_READ == p_ctrl_node->w_op_type)
    {
        p_ctrl_node->b_status = mATA_WAIT_DMA;
        list_del(&p_ctrl_node->node);
        ata_cmd_add_node_read_list(p_ctrl_node);
    }
    else if(OP_HOST_WMR == p_ctrl_node->w_op_type)
    {
        list_del(&p_ctrl_node->node);
        free(p_ctrl_node);
    }
    #endif
}

void nfc_write_handle(bool f_wait_done)
{
    t_ctrl_node_type    *p_ctrl_node;
    struct list_head    *write_queue = NULL;
    u8 b_ce;
    if((QUEUE_IDLE != b_excute_queue_state) && (FALSE == f_wait_done))
    {
        return;
    }
    if(QUEUE_READ == b_excute_queue_state)
    {
        nfc_read_handle(TRUE);
    }
    b_excute_queue_state = QUEUE_WRITE;
    if(!list_empty(&t_write_queue_mng.nfc_write_queue[0]))
    {
        write_queue = &t_write_queue_mng.nfc_write_queue[0];
    }
    else if(!list_empty(&t_write_queue_mng.nfc_write_queue[1]))
    {
        write_queue = &t_write_queue_mng.nfc_write_queue[1];
    }

    
    while((!list_empty(&t_write_queue_mng.nfc_write_queue[0])) || (!list_empty(&t_write_queue_mng.nfc_write_queue[1])))
    {
        DBG_MESSAGE("<NFW> handle ch:%d\n", (write_queue == &t_write_queue_mng.nfc_write_queue[0]) ? 0 : 1);
        
        ASSERT_LVL(NFC, ASSERT_LVL0, 0 != write_queue);
        p_ctrl_node = list_first_entry(write_queue,t_ctrl_node_type,node);

        if(p_ctrl_node->b_status == mNFC_WRITE_WAIT_DMA_DONE)
        {
            ata_cmd_rotation();
            
            if(FALSE == f_wait_done)
            {
                ftl_core_rotation();
            }
        }

        b_ce = *REGAccessByte(F0CEN);
        b_ce &= ~0x04;
        *REGAccessByte(F0CEN) = 0xff;
        *REGAccessByte(F0CEN) = b_ce;
        nfc_cmd_handle[p_ctrl_node->b_status].func(p_ctrl_node);
        *REGAccessByte(F0CEN) = 0xff;
        b_ce |= 0x04;
        *REGAccessByte(F0CEN) = b_ce;
        
        if((!list_empty(&t_write_queue_mng.nfc_write_queue[0])) && (write_queue != &t_write_queue_mng.nfc_write_queue[0]))
        {
            write_queue = &t_write_queue_mng.nfc_write_queue[0];
        }
        else if((!list_empty(&t_write_queue_mng.nfc_write_queue[1])) && (write_queue != &t_write_queue_mng.nfc_write_queue[1]))
        {
            write_queue = &t_write_queue_mng.nfc_write_queue[1];
        }
    }


    b_excute_queue_state = QUEUE_IDLE;
    
}

void nfc_read_handle(bool f_wait_done)
{
    t_ctrl_node_type    *p_ctrl_node;
    struct list_head    *read_queue = NULL;

    if((QUEUE_IDLE != b_excute_queue_state) && (FALSE == f_wait_done))
    {
        return;
    }
    if(QUEUE_WRITE == b_excute_queue_state)
    {
        nfc_write_handle(TRUE);
    }
    b_excute_queue_state = QUEUE_READ;

    //two channel 
    if(!list_empty(&t_read_queue_mng.nfc_read_queue[0]))
    {
        read_queue = &t_read_queue_mng.nfc_read_queue[0];
    }
    else if(!list_empty(&t_read_queue_mng.nfc_read_queue[1]))
    {
        read_queue = &t_read_queue_mng.nfc_read_queue[1];
    }
    while((!list_empty(&t_read_queue_mng.nfc_read_queue[0])) || (!list_empty(&t_read_queue_mng.nfc_read_queue[1])))
    {
        
        ASSERT_LVL(NFC, ASSERT_LVL0, 0 != read_queue);
        p_ctrl_node = list_first_entry(read_queue,t_ctrl_node_type,node);

        DBG_MESSAGE("<NFR> handle ch:%d\n", (read_queue == &t_read_queue_mng.nfc_read_queue[0]) ? 0 : 1);

        if(mNFC_READ_IDLE != p_ctrl_node->b_status)
        {
            ata_cmd_rotation();
            if(FALSE == f_wait_done)
            {
                ftl_core_rotation();
            }
        }
        
        nfc_cmd_handle[p_ctrl_node->b_status].func(p_ctrl_node);
        
        if((!list_empty(&t_read_queue_mng.nfc_read_queue[0])) && (read_queue != &t_read_queue_mng.nfc_read_queue[0]))
        {
            read_queue = &t_read_queue_mng.nfc_read_queue[0];
        }
        else if((!list_empty(&t_read_queue_mng.nfc_read_queue[1])) && (read_queue != &t_read_queue_mng.nfc_read_queue[1]))
        {
            read_queue = &t_read_queue_mng.nfc_read_queue[1];
        }
    }
    b_excute_queue_state = QUEUE_IDLE;
}

void nfc_queue_rotation(bool f_wait_done)
{
    nfc_read_handle(f_wait_done);
    nfc_write_handle(f_wait_done);
}
void nfc_read_cmd_idle_handle(t_ctrl_node_type *p_ctrl_node)
{
    //check node in write list
    //cal row addr
    //send read cmd
    if(nf_wait_rb() == TRUE)
    {
        DBG_MESSAGE("<NR> wait ready\n");
        *REGAccessDword(CH0_FLASH_CMD) = FL_CMD_READ1;
        *REGAccessDword(CH0_FLASH_COL_ADDR) = 0;
        *REGAccessDword(CH0_FLASH_ROW_ADDR) = p_ctrl_node->d_ptu;
        *REGAccessDword(CH0_FLASH_CMD) = FL_CMD_READ2;
        
        p_ctrl_node->b_status = mNFC_READ_WAIT_READY;
    }
}

void nfc_read_wait_ready_handle(t_ctrl_node_type *p_ctrl_node)
{
    if(nf_wait_rb() == TRUE)
    {
        //start dma 
        *REGAccessByte(CH0_CFG_FLASH_WR) = 0;	/* DMA读 */
        *REGAccessDword(CH0_RDMZ_SEED) = 0;	/* randomizer种子 */
        *REGAccessWord(CH0_DATA_LEN_MAIN_LEN) = 1024;	/* 设置DMA数据长度 */
        *REGAccessByte(CH0_CFG_GATHER_MODE) = 0x0;		/* 不丢弃 */
        //DBG_MASSAGE("<DMA>Start Dma!\r\n");
        *REGAccessDword(CH0_DATA_BAR) = (u32) p_ctrl_node->p_buff; /* 设置DMA buffer地址 */	
        *REGAccessByte(CH0_CTRL_DMA_EN) = REGValue(1, CH_CTRL_DMA_EN_POS);	

        DBG_MESSAGE("<NR> start read dma\n");
        p_ctrl_node->b_status = mNFC_READ_WAIT_DMA_DONE;
    }
}

void nfc_read_wait_dma_handle(t_ctrl_node_type *p_ctrl_node)
{
    //if(flash_dma_done() == TRUE)
    if(0 == (*REGAccessByte(CH0_CTRL_DMA_EN)))
    {
        DBG_MESSAGE("<NR> read dma done\n");
        list_del(&p_ctrl_node->node);
        if(OP_HOST_READ == p_ctrl_node->w_op_type)
        {
            p_ctrl_node->b_status = mATA_WAIT_DMA;
            ata_cmd_add_node_read_list(p_ctrl_node);
        }
        else
        {
            free(p_ctrl_node);
        }
    }
}

void nfc_write_cmd_idle_handle(t_ctrl_node_type *p_ctrl_node)
{
    if(0 == PTU_GET_OFST(p_ctrl_node->d_ptu))
    {
        //if(0 == PTU_GET_BANK(p_ctrl_node->d_ptu))
        {
            if(nf_wait_rb() == TRUE)
            {
                //cal row addr
                //send write cmd
                *REGAccessDword(CH0_FLASH_CMD) = FL_CMD_PAGE_PROGRAM1;
                *REGAccessDword(CH0_FLASH_COL_ADDR) = 0;
                *REGAccessDword(CH0_FLASH_ROW_ADDR) = p_ctrl_node->d_ptu;
                //start dma data

                *REGAccessDword(CH0_RDMZ_SEED) = 0;	/* randomizer种子 */
                *REGAccessByte(CH0_CFG_GATHER_MODE) = 0x00;		/* 不丢弃 */ 
                *REGAccessByte(CH0_CFG_FLASH_WR) = REGValue(1, CH_CFG_FLASH_WR_POS);	/* DMA写 */
                *REGAccessDword(CH0_DATA_BAR) = (u32)p_ctrl_node->p_buff; /* 设置DMA buffer地址 */
                *REGAccessWord(CH0_DATA_LEN) = 1024;	/* 设置DMA数据长度 */
                gdCH0SpareData[0] = 0;
                *REGAccessByte(CH0_CTRL_DMA_EN) = REGValue(1, CH_CTRL_DMA_EN_POS);	/* 启动DMA */

                DBG_MESSAGE("<NW>send write cmd and dma start\n");
                p_ctrl_node->b_status = mNFC_WRITE_WAIT_DMA_DONE;
            }
        }
        #if 0
        else if(1 == PTU_GET_BANK(p_ctrl_node->d_ptu))
        {
            //set plane addr
            //send multiplane addr
            //start dma data
            DBG_MESSAGE("<NW>mult write cmd and dma start\n");
            p_ctrl_node->b_status = mNFC_WRITE_WAIT_DMA_DONE;
        }
        #endif
    }
    else
    {
        //if(wait_flash_dma_done() == TRUE)
        {
            //start dma data
            *REGAccessDword(CH0_RDMZ_SEED) = 0;	/* randomizer种子 */
            *REGAccessByte(CH0_CFG_GATHER_MODE) = 0x00;		/* 不丢弃 */ 
            *REGAccessByte(CH0_CFG_FLASH_WR) = REGValue(1, CH_CFG_FLASH_WR_POS);	/* DMA写 */
            *REGAccessDword(CH0_DATA_BAR) = (u32)p_ctrl_node->p_buff; /* 设置DMA buffer地址 */
            *REGAccessWord(CH0_DATA_LEN) = 1024;	/* 设置DMA数据长度 */
            gdCH0SpareData[0] = 0;
            *REGAccessByte(CH0_CTRL_DMA_EN) = REGValue(1, CH_CTRL_DMA_EN_POS);	/* 启动DMA */

            DBG_MESSAGE("<NW>write dma start\n");
            p_ctrl_node->b_status = mNFC_WRITE_WAIT_DMA_DONE;
        }
    }
    
}

void nfc_write_wait_dma_handle(t_ctrl_node_type *p_ctrl_node)
{
    t_ctrl_node_type *p_nxt_ctrl_node;
    if((*REGAccessByte(CH0_CTRL_DMA_EN)) == 0)
    {
        if(TRUE == p_ctrl_node->f_write_trig)
        {
            *REGAccessDword(CH0_FLASH_CMD) = FL_CMD_PAGE_PROGRAM2;
        }
        else
        {
            p_nxt_ctrl_node = list_entry(p_ctrl_node->node.next, t_ctrl_node_type, node);
            //start next write dma
            nfc_write_cmd_idle_handle(p_nxt_ctrl_node);
        }
        DBG_MESSAGE("<NW>write dma done\n");
        if((mUSER_HOST == p_ctrl_node->b_user) && (OP_HOST_WRITE == p_ctrl_node->w_op_type))
        {
            ata_cmd_del_node_cnt();
        }
        list_del(&p_ctrl_node->node);
        
		//*REGAccessByte(F0CEN) &= ~1;
        free(p_ctrl_node);
        //*REGAccessByte(F0CEN) = 0xF;			
        //*REGAccessByte(F0CEN) &= ~1;
        free(p_ctrl_node->p_buff);
        //*REGAccessByte(F0CEN) = 0xF;	
        
    }
}

