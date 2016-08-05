#ifndef _DBG_CFG_H_
#define _DBG_CFG_H_

#define	UART_DEBUG			(mDISABLE)
#define UART_MESSAGE		(mDISABLE)
#define UART_ASSERT			(mDISABLE)
#define UART_DATA_PRINT		(mDISABLE)

/*********************** Debug Assert configuration begin *********************/
#if(UART_ASSERT == mENABLE)
enum
{
    ASSERT_LVL0 = 0,    /* High priority */
    ASSERT_LVL1,
    ASSERT_LVL2,
    ASSERT_LVL3,
    ASSERT_LVL4,        /* Low  priority */
};

#define MODULE_P2L_ASSERT_TH    ASSERT_LVL1
#define MODULE_META_ASSERT_TH   ASSERT_LVL1
#define MODULE_AP_ASSERT_TH     ASSERT_LVL1
#define MODULE_VER_ASSERT_TH    ASSERT_LVL1
#define MODULE_COM_ASSERT_TH    ASSERT_LVL1
#define MODULE_BKM_ASSERT_TH    ASSERT_LVL1
#define MODULE_MEM_ASSERT_TH    ASSERT_LVL1
#define MODULE_L2P_ASSERT_TH    ASSERT_LVL1
#define MODULE_ATA_ASSERT_TH    ASSERT_LVL4
#define MODULE_NFC_ASSERT_TH    ASSERT_LVL1
#endif  /* (UART_ASSERT == mENABLE) */
/*********************** Debug Assert configuration end ***********************/



/*********************** Debug Message configuration begin ********************/
#if(UART_MESSAGE == mENABLE)
enum
{
    MSG_LVL0 = 0,       /* High priority */
    MSG_LVL1,
    MSG_LVL2,
    MSG_LVL3,
    MSG_LVL4,           /* Low  priority */
};

#define MODULE_COM_MSG_TH    MSG_LVL0
#define MODULE_EMC_MSG_TH    MSG_LVL0
#define MODULE_DPM_MSG_TH    MSG_LVL0
#define MODULE_FTL_MSG_TH    MSG_LVL0
#define MODULE_NFC_MSG_TH    MSG_LVL0
#endif  /* (UART_MESSAGE == mENABLE) */
/*********************** Debug Message configuration end **********************/

#endif
