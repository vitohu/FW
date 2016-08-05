#ifndef _MEMCTL_H_
#define _MEMCTL_H_



typedef enum
{
    mFREE_SUCCESS = 0,
    mPOINT_NOT_EXIST,
    mPARA_ERR
}e_free_result;


void mem_init(void);
void *malloc(u32 size, bool f_force_alloc_sram);
e_free_result free(void *p);
void test_mem_ctl_func(void);
u32 mem_get_dwrd(u32 d_ram_start_addr, u16 w_ofst);
void mem_set_dwrd(u32 d_ram_start_addr, u16 w_ofst, u32 d_val);
#endif
