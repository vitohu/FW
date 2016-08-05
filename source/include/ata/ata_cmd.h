#ifndef _ATA_CMD_H_
#define _ATA_CMD_H_

#define ATA_CMD_UT_TEST_ENABLE  mENABLE

#define ATA_CMD_READ            0x30
#define ATA_CMD_WRITE           0x35

typedef struct
{
    struct list_head node;
    u32 d_cmd_tag;
    u32 d_start_lba;
    u32 d_start_lba_bak;
    u16 w_len;
    u16 w_len_bak;
    
    u16 w_cmd_type;
    u16 w_rev;
}t_ata_cmd_type;



extern void ata_cmd_init(void);
extern void ata_cmd_add_new_cmd(u16 w_cmd_type, u16 w_len, u32 d_lba, u32 d_cmd_tag);
extern void ata_cmd_handle(void);
extern void ata_cmd_pre_handle(t_ata_cmd_type *p_ata_cmd);
extern void ata_cmd_read_handle(void);
extern void ata_cmd_write_handle(void);
extern void ata_cmd_other_handle(t_ata_cmd_type *p_ata_cmd);
extern void ata_cmd_rotation(void);
extern void ata_cmd_trans_node(t_ctrl_node_type *p_ctrl_node);
extern void ata_cmd_add_node_read_list(t_ctrl_node_type *p_ctrl_node);
extern void ata_cmd_add_node_cnt(void);
extern void ata_cmd_del_node_cnt(void);
#if (ATA_CMD_UT_TEST_ENABLE == mENABLE)
void ata_cmd_ut_test(void);
#endif


#endif
