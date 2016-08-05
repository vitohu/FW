

#include <global_def.h>
#include <nfc_queue.h>
#include <flash_reg.h>


void write_flash_cmd(u32 d_ptu, u32 *p_buff)
{
    
}

void write_flash_dummy_cmd(u32 d_ptu)
{
    
}

void read_flash_cmd(u32 d_ptu, u32 *p_buff)
{
    
}


bool nf_wait_rb(void)
{
	u8	lbReady;
	u8	lbTimeout;
	u16	lwDelayMS;
	lbTimeout = 100;
	
	while (lbTimeout--)
	{
		lwDelayMS = 100;
		while (lwDelayMS--)
		{

		    lbReady = *REGAccessByte(FRB0);
			if (lbReady)
			{
				return	TRUE;
			}
		}
	}
	return FALSE;
}

void nf_init_flash_interface(void)
{
	/* Flash 接口配置 */
	#if (DBG_SRAM_ERR)
	*REGAccessDword(GPIO0_DATA) |= 0xf;
	*REGAccessDword(GPIO1_DATA) |= 0xf;
	*REGAccessDword(GPIO0_DIR) &= 0xFFfffff0;	/* 设置F1D7-F1D0, CE为输出 */
	*REGAccessDword(GPIO1_DIR) &= 0xfffffff0;

	*REGAccessDword(GPIO0_DIR) &= (~(1 << FRB1_Pos));
	#else
	*REGAccessDword(GPIO0_DATA) |= 0xf;
	*REGAccessDword(GPIO1_DATA) |= 0xf;
	*REGAccessDword(GPIO0_DIR) &= 0xfffffff0;	/* 设置CE为输出 */
	*REGAccessDword(GPIO1_DIR) &= 0xfffffff0;
	#endif

	*REGAccessByte(SPI_GPIO_EN) = REGValue(1, SPI_GPIO_EN_Pos);
	
	*REGAccessByte(CH1_CFG_FLASH_SEL) = REGValue(1, CH_CFG_FLASH_SEL_POS);
	*REGAccessByte(CH0_CFG_FLASH_SEL) = 0;
	
	*REGAccessByte(FLASH_CLK_EN) = REGValue(1, FLASH_CLK_EN_POS);
	*REGAccessByte(ECC_CLK_SEL) = 0x0;			/* 100M */
	*REGAccessByte(ECC_CLK_EN) = REGValue(1, ECC_CLK_EN_POS);
	
	*REGAccessByte(CH1_CFG_BADCOL_EN) = 0;		/* 关闭bad column */
	*REGAccessByte(CH1_CFG_RDMZ_MODE) = REGValue(0, CH_CFG_RDMZ_MODE_POS);		/* 23bit mode */
	*REGAccessByte(CH1_CFG_RDMZ_EN) = REGValue(0, CH_CFG_RDMZ_EN_POS);		/* 使能加扰 */
	// *REGAccessByte(CH1_CFG_ECC_MODE) = REGValue(ECC_MODE_72bECC, CH_CFG_ECC_MODE_POS);		/* 72bit ECC */
	*REGAccessByte(CH1_CFG_ECC_EN) = REGValue(0, CH_CFG_ECC_EN_POS);			/* 使能ECC */
	*REGAccessByte(CH1_CFG_GATHER_MODE) = 0;	/* 不丢弃 */
	*REGAccessByte(CH1_CFG_DMA_BIND) = 0;		/* 独立的DMA */
	//*REGAccessByte(CH1_CFG_ALLONE_EN) = REGValue(1, CH_CFG_ALLONE_EN_POS);
	
	*REGAccessByte(CH0_CFG_BADCOL_EN) = 0;		/* 关闭bad column */
	*REGAccessByte(CH0_CFG_RDMZ_MODE) = REGValue(0, CH_CFG_RDMZ_MODE_POS);		/* 23bit mode */
	*REGAccessByte(CH0_CFG_RDMZ_EN) = REGValue(0, CH_CFG_RDMZ_EN_POS);		/* 使能加扰 */
	// *REGAccessByte(CH0_CFG_ECC_MODE) = REGValue(ECC_MODE_72bECC, CH_CFG_ECC_MODE_POS);		/* 72bit ECC */
	*REGAccessByte(CH0_CFG_ECC_EN) = REGValue(0, CH_CFG_ECC_EN_POS);			/* 使能ECC */
	*REGAccessByte(CH0_CFG_GATHER_MODE) = 0;	/* 不丢弃 */
	*REGAccessByte(CH0_CFG_DMA_BIND) = 0;		/* 独立的DMA */
	//*REGAccessByte(CH0_CFG_ALLONE_EN) = REGValue(1, CH_CFG_ALLONE_EN_POS);
	
	*REGAccessByte(CH1_FLASH_PAR_ROW_ADDR_LEN) = REGValue(2, CH_FLASH_PAR_ROW_ADDR_LEN_POS);
	*REGAccessByte(CH1_FLASH_PAR_COL_ADDR_LEN) = REGValue(1, CH_FLASH_PAR_COL_ADDR_LEN_POS);
	*REGAccessByte(CH1_FLASH_PAR_BAD_DATAL) = 0xFF;
	*REGAccessByte(CH1_FLASH_PAR_BAD_DATAH) = 0xFF;
	*REGAccessByte(CH1_FLASH_PAR_ONFICLKOUT_EN) = 0;
	*REGAccessByte(CH1_FLASH_PAR_FLASH_TYPE) = ASYNC8_FLASH;

	*REGAccessByte(CH0_FLASH_PAR_ROW_ADDR_LEN) = REGValue(2, CH_FLASH_PAR_ROW_ADDR_LEN_POS);
	*REGAccessByte(CH0_FLASH_PAR_COL_ADDR_LEN) = REGValue(1, CH_FLASH_PAR_COL_ADDR_LEN_POS);
	*REGAccessByte(CH0_FLASH_PAR_BAD_DATAL) = 0xFF;
	*REGAccessByte(CH0_FLASH_PAR_BAD_DATAH) = 0xFF;
	*REGAccessByte(CH0_FLASH_PAR_ONFICLKOUT_EN) = 0;
	*REGAccessByte(CH0_FLASH_PAR_FLASH_TYPE) = ASYNC8_FLASH;

	*REGAccessDword(CH0_SPARE_BAR) = (u32)gdCH0SpareData;		/* 配置Spare */
	*REGAccessDword(CH1_SPARE_BAR) = (u32)gdCH1SpareData;


    *REGAccessByte(FLASH_CLK_SEL) = FLASH_CLK_62M;
    *REGAccessByte(CH1_FLASH_TIMING_TWH) = REGValue(1, CH_FLASH_TIMING_TWH_POS);
	*REGAccessByte(CH1_FLASH_TIMING_TWP) = REGValue(1, CH_FLASH_TIMING_TWP_POS);
	*REGAccessByte(CH1_FLASH_TIMING_TREH) = REGValue(1, CH_FLASH_TIMING_TREH_POS);
	*REGAccessByte(CH1_FLASH_TIMING_TRP) = REGValue(1, CH_FLASH_TIMING_TRP_POS);
	*REGAccessByte(CH1_FLASH_TIMING_TCALS) = REGValue((1+1), CH_FLASH_TIMING_TCALS_POS);
	*REGAccessByte(CH1_FLASH_TIMING_TCALH) = REGValue((1+1), CH_FLASH_TIMING_TCALH_POS);

	*REGAccessByte(CH0_FLASH_TIMING_TWH) = REGValue(1, CH_FLASH_TIMING_TWH_POS);
	*REGAccessByte(CH0_FLASH_TIMING_TWP) = REGValue(1, CH_FLASH_TIMING_TWP_POS);
	*REGAccessByte(CH0_FLASH_TIMING_TREH) = REGValue(1, CH_FLASH_TIMING_TREH_POS);
	*REGAccessByte(CH0_FLASH_TIMING_TRP) = REGValue(1, CH_FLASH_TIMING_TRP_POS);
	*REGAccessByte(CH0_FLASH_TIMING_TCALS) = REGValue((1+1), CH_FLASH_TIMING_TCALS_POS);
	*REGAccessByte(CH0_FLASH_TIMING_TCALH) = REGValue((1+1), CH_FLASH_TIMING_TCALH_POS);

	*REGAccessByte(CH1_FLASH_TIMING_TDO) = REGValue(1, CH_FLASH_TIMING_TDO_POS);		/* rd sample */
	*REGAccessByte(CH0_FLASH_TIMING_TDO) = REGValue(1, CH_FLASH_TIMING_TDO_POS);		/* rd sample */

	
	//setRWCycle();
	//SetECCMode(SPARE_SIZE_25bECC);							/* 72bit ECC */

	/* just for FPGA debug,  */
	*REGAccessByte(DLL_MODE) = REGValue(1, DLL_MODE_POS);

    
    *REGAccessDword(CH0_FLASH_CMD) = FL_CMD_RESET;
}

