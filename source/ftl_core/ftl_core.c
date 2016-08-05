

#include <global_def.h>
#include <ftl.h>
#include <ftl_ap.h>
#include <ftl_l2p.h>
#include <ata_cmd.h>
#include <nfc_queue.h>
#include <ftl_ver.h>
#include <ftl_bkm.h>
#include <flash_reg.h>
struct list_head ftl_core_tran_list;
struct list_head ftl_host_write_list;

void ftl_core_init(void)
{
    INIT_LIST_HEAD(&ftl_core_tran_list);
    INIT_LIST_HEAD(&ftl_host_write_list);

    nfc_queue_init(); // for ut test
    ftl_ap_init();
    ftl_ver_init();
    ftl_bkm_init();
}

void ftl_core_rotation(void)
{
    u32                 d_lpt;
    t_ctrl_node_type    *p_ctrl_node;
    struct list_head    *nxt_node = NULL;

    u8 b_ce = *REGAccessByte(F0CEN);
    b_ce &= ~0x02;
    *REGAccessByte(F0CEN) = 0xFF;
    *REGAccessByte(F0CEN) = b_ce;
    while(FALSE == list_empty(&ftl_core_tran_list))
    {
        p_ctrl_node = list_first_entry(&ftl_core_tran_list,t_ctrl_node_type,node);
        DBG_MESSAGE("<FTL>(%2d %8x %3x)\n", p_ctrl_node->d_tag, p_ctrl_node->d_lba,\
                p_ctrl_node->w_len);

        p_ctrl_node->d_ltu  = p_ctrl_node->d_lba >> LTU_LBA_EXP;
        d_lpt               = p_ctrl_node->d_ltu >> L2P_ENT_BIT;
        ata_cmd_rotation();
        ftl_l2p_load_lpt(d_lpt);
        
        //nxt_node = p_ctrl_node->node.next;
        if(OP_HOST_WRITE == p_ctrl_node->w_op_type)
        {
            if(((p_ctrl_node->d_lba & LTU_LBA_MSK) != 0) || \
                (p_ctrl_node->w_len != LTU_LBA_CNT))
            {
                t_ctrl_node_type *p_tmp_ctrl_node = (t_ctrl_node_type *)malloc(sizeof(t_ctrl_node_type), TRUE);

                p_tmp_ctrl_node->b_user = mUSER_HOST;
                p_tmp_ctrl_node->d_lba = p_ctrl_node->d_lba;
                p_tmp_ctrl_node->w_len = p_ctrl_node->w_len;
                p_tmp_ctrl_node->w_op_type = OP_HOST_WMR;
                p_tmp_ctrl_node->p_buff = p_ctrl_node->p_buff;

                nfc_queue_add_read_list(p_tmp_ctrl_node);
                nfc_read_handle(TRUE);
            }
            p_ctrl_node->d_ptu = ftl_ap_alloc_new_ptu(mUSER_HOST);
            ftl_l2p_update_lpt(p_ctrl_node->d_ltu, p_ctrl_node->d_ptu); // bug!

            DBG_MESSAGE("<CORE>ltu:%8x -->ptu:%8x\n", p_ctrl_node->d_ltu,\
                p_ctrl_node->d_ptu);
            //add node to write list
            
            list_del(&p_ctrl_node->node);
            nfc_queue_add_write_list(p_ctrl_node);
        }
        else if(OP_HOST_READ == p_ctrl_node->w_op_type)
        {
            p_ctrl_node->d_ptu = ftl_l2p_lpt_get_ptu(p_ctrl_node->d_ltu);

            list_del(&p_ctrl_node->node);
            nfc_queue_add_read_list(p_ctrl_node);
        }
    }
    //
    b_ce |= 0x02;
    *REGAccessByte(F0CEN) = 0xFF;
    *REGAccessByte(F0CEN) = b_ce;
}

void ftl_core_add_trans_node(t_ctrl_node_type *p_ctrl_node)
{
    list_add_tail(&p_ctrl_node->node, &ftl_core_tran_list);
}

void ftl_core_force_dump_host_data(void)
{
    u32                 d_ptu   = ftl_ap_get_current_ptu_value(mUSER_HOST);
    t_ctrl_node_type    *p_ctrl_node;   
    while(PTU_GET_OFST(d_ptu) != 0x03)
    {
        
        p_ctrl_node = (t_ctrl_node_type *)malloc(sizeof(t_ctrl_node_type), TRUE);

        p_ctrl_node->b_user = mUSER_HOST;
        p_ctrl_node->d_lba  = DUMMY_LTU;
        p_ctrl_node->d_ptu  = ftl_ap_alloc_new_ptu(mUSER_HOST);
        
        DBG_MESSAGE("<FD> force dump host data ptu:%8x\n", p_ctrl_node->d_ptu);
        
        p_ctrl_node->p_buff = (u32 *)malloc(LTU_BYTE_CNT, FALSE);
        memset_u32(p_ctrl_node->p_buff, INVALID32, LTU_BYTE_CNT >> DWRD_BYTE_EXP);
        p_ctrl_node->w_op_type = OP_DUMMY_WRITE;
        
        nfc_queue_add_write_list(p_ctrl_node);
        
        d_ptu   = ftl_ap_get_current_ptu_value(mUSER_HOST);

        
    }
    
}

