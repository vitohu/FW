
#include	<global_def.h>
#include    <memctl.h>
#include    <ftl_bkm.h>
#include    <ftl_l2p.h>
#include    <ftl_core.h>
#include    <ata_cmd.h>
#include    <nf_cmd.h>
int main(void)
{
    
#if UART_DEBUG
    uart_init(115200, 30000000);
#endif
    DBG_MESSAGE("This is main function\r\n");
#if 1
    mem_init();
    ftl_core_init();
    ftl_l2p_init();
    nf_init_flash_interface();
    
#else
    test_mem_ctl_func();
#endif
#if (mENABLE == FTL_BKM_UT_TEST)
    ftl_bkm_data_blk_test();
#endif

#if (FTL_L2P_UT_TEST_ENABLE == mENABLE)
    ftl_l2p_ut_test();
#endif

#if (ATA_CMD_UT_TEST_ENABLE == mENABLE)
    ata_cmd_ut_test();
#endif
    
    while(1);
}

