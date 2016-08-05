#ifndef _NFC_QUEUE_H_
#define _NFC_QUEUE_H_


//need delet
extern u32	gdCH0SpareData[32];		/* 硬件需要保存ECC code */
extern u32	gdCH1SpareData[32];


extern void nfc_queue_init(void);
extern void nfc_queue_add_write_list(t_ctrl_node_type *p_ctrl_node);
extern void nfc_queue_add_read_list(t_ctrl_node_type *p_ctrl_node);
extern void nfc_read_handle(bool f_wait_done);
extern void nfc_write_handle(bool f_wait_done);
extern void nfc_queue_rotation(bool f_wait_done);
#endif

