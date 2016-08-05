#ifndef _NF_CMD_H_
#define _NF_CMD_H_




extern write_flash_cmd(u32 d_ptu, u32 *p_buff);
extern write_flash_dummy_cmd(u32 d_ptu);
extern void read_flash_cmd(u32 d_ptu, u32 *p_buff);
extern bool nf_wait_rb(void);
extern void nf_init_flash_interface(void);
#endif
