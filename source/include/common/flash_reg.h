#ifndef _FLASH_REG_H_
#define _FLASH_REG_H_


#include <reg_def.h>
#include <gpio.h>

/* F0CEN0 ~ F0CEN3 通道0片选， GPIO */
#define		F0CEN_POS	0
#define		F0CEN		REGBit(GPIO0_DATA, F0CEN_POS, 4)

#define		F1CEN_POS	0
#define		F1CEN		REGBit(GPIO1_DATA, F1CEN_POS, 4)


/* GPIO1 bit7, TOGGLE_SEL */
#define		GPIO_RELIABLE_BOOT_POS	7
#define		GPIO_RELIABLE_BOOT	REGBit(GPIO1_DATA, GPIO_RELIABLE_BOOT_POS, 1)

#ifdef	PAD_CFG_DEF
/* Pad_Cfg *********************************************************/
#define		PAD_CFG				0x40030000+0x0400*6

/* bit0: F0DATA_GPIO_EN, */
#define		F0DATA_GPIO_EN		REGBit(PAD_CFG, 0, 1)
/* bit1: F0CMD_GPIO_EN,  */
#define		F0CMD_GPIO_EN		REGBit(PAD_CFG, 1, 1)
/* bit2: F0DQS_GPIO_EN,  */
#define		F0CMD_GPIO_EN		REGBit(PAD_CFG, 2, 1)
/* bit3: F1DATA_GPIO_EN */
#define		F0DATA_GPIO_EN		REGBit(PAD_CFG, 3, 1)
/* bit4: F1CMD_GPIO_EN,  */
#define		F0CMD_GPIO_EN		REGBit(PAD_CFG, 4, 1)
/* bit5: F1DQS_GPIO_EN,  */
#define		F0CMD_GPIO_EN		REGBit(PAD_CFG, 5, 1)
/* bit6: SPI_GPIO_EN */
#define		SPI_GPIO_EN			REGBit(PAD_CFG, 6, 1)
/* bit7: LED_EN */
#define		LED_EN				REGBit(PAD_CFG, 7, 1)
/* bit[11:8]: FLASH0_CE_END toggle flash 0 硬件拉高CEN, 仅当对应的FLASH_PAR寄存器CLE_CE置0时有效 */
#define		FLASH0_CE_END		REGBit(PAD_CFG, 8, 4)
/* bit[15:12]: FLASH1_CE_END toggle flash 1 硬件拉高CEN, 仅当对应的FLASH_PAR寄存器CLE_CE置0时有效 */
#define		FLASH1_CE_END		REGBit(PAD_CFG, 12, 4)

#endif	// PAD_CFG


/* Sys_Cfg *********************************************************/
#define		SYS_CFG				0x40030000+0x0400*7

/* [2:0] */
#define		FLASH_CLK_40M		0x0
#define		FLASH_CLK_83M		0x1
#define		FLASH_CLK_62M		0x2
#define		FLASH_CLK_100M		0x3
#define		FLASH_CLK_25M		0x4

#define		FLASH_CLK_SEL_POS	0
#define		FLASH_CLK_SEL		REGBit(SYS_CFG, FLASH_CLK_SEL_POS, 3)

/* [3]	FLASH_CLK_EN, Flash时钟使能 */
#define		FLASH_CLK_EN_POS	3
#define		FLASH_CLK_EN		REGBit(SYS_CFG, FLASH_CLK_EN_POS, 1)



#define		ECC_CLK_100M		0x0
#define		ECC_CLK_125M		0x1
#define		ECC_CLK_166M		0x2
#define		ECC_CLK_200M		0x3
#define		ECC_CLK_25M			0x4

/* [6:4]	00:96M,01:120M,10:160M,11:192M */
#define		ECC_CLK_SEL_POS	4
#define		ECC_CLK_SEL		REGBit(SYS_CFG, ECC_CLK_SEL_POS, 3)

/* [7]	ECC_CLK_EN, ECC 时钟使能 */
#define		ECC_CLK_EN_POS	7
#define		ECC_CLK_EN		REGBit(SYS_CFG, ECC_CLK_EN_POS, 1)


/* [8]	GLITCH_GATE_EN, 过滤Flash和ECC时钟毛刺 */
#define		GLITCH_GATE_EN_POS	8
#define		GLITCH_GATE_EN		REGBit(SYS_CFG, GLITCH_GATE_EN_POS, 1)

/* [9]	软件复位 */
#define		SOFT_RESET_POS	9
#define		SOFT_RESET		REGBit(SYS_CFG, SOFT_RESET_POS, 1)

#define		GPIO_ST_POS		10
#define		GPIO_ST			REGBit(SYS_CFG, GPIO_ST_POS, 1)

#define		GPIO_RED_POS	11
#define		GPIO_RED		REGBit(SYS_CFG, GPIO_RED_POS, 1)

#define		GPIO_REU_POS	12
#define		GPIO_REU		REGBit(SYS_CFG, GPIO_REU_POS, 1)

#define		GPIO_DS0_POS	13
#define		GPIO_DS0		REGBit(SYS_CFG, GPIO_DS0_POS, 1)

#define		GPIO_DS1_POS	14
#define		GPIO_DS1		REGBit(SYS_CFG, GPIO_DS1_POS, 1)

#define		GPIO_DS2_POS	15
#define		GPIO_DS2		REGBit(SYS_CFG, GPIO_DS2_POS, 1)

#define		OTP_RD_CLK_DIV_POS	16
#define		OTP_RD_CLK_DIV		REGBit(SYS_CFG, OTP_RD_CLK_DIV_POS, 3)

#define		OTP_RD_TRIM_POS	19
#define		OTP_RD_TRIM		REGBit(SYS_CFG, OTP_RD_TRIM_POS, 4)

#define		OTP_GATE_POS	23
#define		OTP_GATE		REGBit(SYS_CFG, OTP_GATE_POS, 1)


/* [25:24] 对应SPI模式0,1,2,3 */
#define		SPI_MODE_POS	24
#define		SPI_MODE		REGBit(SYS_CFG, SPI_MODE_POS, 2)

/* [29:26] Clk波特率 */
#define		SPI_BAUD_POS	26
#define		SPI_BAUD		REGBit(SYS_CFG, SPI_BAUD_POS, 4)

/* [30] 0:MSB,1:LSB */
#define		SPI_LSB_POS		30
#define		SPI_LSB		REGBit(SYS_CFG, SPI_LSB_POS, 1)

/* [31] SPI idle状态,无收发 */
#define		SPI_IDLE_POS	31
#define		SPI_IDLE		REGBit(SYS_CFG, SPI_IDLE_POS, 1)



/* LED_Cfg *********************************************************/
#define		LED_CFG				0x40030000+0x0400*8

/* [7:0] LED_ON_CNT, 每1M Cycle计数一次
	USB 2.0模式时，步长为1/30秒
	USB 3.0模式时，步长为1/125秒 */
#define		LED_ON_CNT_POS	0
#define		LED_ON_CNT		REGBit(LED_CFG, LED_ON_CNT_POS, 8)
/* [15:8]	LED_OFF_CNT , 每1M Cycle计数一次
	USB 2.0模式时，步长为1/30秒
	USB 3.0模式时，步长为1/125秒 */
#define		LED_OFF_CNT_POS	8
#define		LED_OFF_CNT		REGBit(LED_CFG, LED_OFF_CNT_POS, 8)

#define		OSC_CLK_EN_POS	16
#define		OSC_CLK_EN		REGBit(LED_CFG, OSC_CLK_EN_POS, 1)

#define		SUSPEND_CLK_SEL_POS	17
#define		SUSPEND_CLK_SEL	REGBit(LED_CFG, SUSPEND_CLK_SEL_POS, 1)

#define		PHY_DEBUG_MODE_POS	24
#define		PHY_DEBUG_MODE	REGBit(LED_CFG, PHY_DEBUG_MODE_POS, 1)

#define		DEBUG_MODE_POS	25
#define		DEBUG_MODE		REGBit(LED_CFG, DEBUG_MODE_POS, 1)

/* [26] DLL_MODE */
#define		DLL_MODE_POS	26
#define		DLL_MODE		REGBit(LED_CFG, DLL_MODE_POS, 1)


/* SPI_DATA: 不支持比特操作
	按Byte、Word、DWord读/写时，分别表示从/往SPI端口读/写一个、二个、4个字节。
	按Byte读写时，地址[9:2]需为0xff。*/
#define		SPI_DATA			0x40030000+0x0400*9



/* DLL_Cfg *********************************************************/
#define		DLL_CFG				0x40030000+0x0400*10

/* [0] DLL复位信号，低复位有效, 持续时间需50us */
#define		DLL_RESET_POS	0
#define		DLL_RESET		REGBit(DLL_CFG, DLL_RESET_POS, 1)

/* [1]	DLL_LOCK, 当DLL锁定时，此信号为1；没锁定时，为0. */
#define		DLL_LOCK_POS	1
#define		DLL_LOCK		REGBit(DLL_CFG, DLL_LOCK_POS, 1)

/* [2]	DLL通道0使能信号，用于关电进入低功耗模式，EN高为使能，低为关电 */
#define		DLL_EN0_POS		2
#define		DLL_EN0		REGBit(DLL_CFG, DLL_EN0_POS, 1)

/* [3]	DLL通道1使能信号，用于关电进入低功耗模式，EN高为使能，低为关电 */
#define		DLL_EN1_POS		3
#define		DLL_EN1		REGBit(DLL_CFG, DLL_EN1_POS, 1)

/* [9:4]	DLL_FINE_TURN0, 通道0输出延迟微调，
	高3bit为向后相位微调，000-111，向后延时逐级增大。
	低3bit为向前相位微调，000-111，提前相位（延时）逐级增大。
	一级typical延时为0.1ns左右。*/
#define		DLL_FINE_TURN0_POS	4
#define		DLL_FINE_TURN0		REGBit(DLL_CFG, DLL_FINE_TURN0_POS, 6)

/* [15:10]	DLL_FINE_TURN0, 通道1输出延迟微调，
	高3bit为向后相位微调，000-111，向后延时逐级增大。
	低3bit为向前相位微调，000-111，提前相位（延时）逐级增大。
	一级typical延时为0.1ns左右。*/
#define		DLL_FINE_TURN1_POS	10
#define		DLL_FINE_TURN1		REGBit(DLL_CFG, DLL_FINE_TURN1_POS, 6)

/* [17:16]	DLL_COARSE_TUNE0, 通道0相位调节：12级精度下，DQS_90输出相位选择
	00：默认值，选择90度
	01：90+30度
	10：90-30度
	11：90-60度 */
#define		DLL_COARSE_TUNE0_POS	16
#define		DLL_COARSE_TUNE0		REGBit(DLL_CFG, DLL_COARSE_TUNE0_POS, 2)

/* [19:18]	DLL_COARSE_TUNE1, 通道1输出延迟微调，
	高3bit为向后相位微调，000-111，向后延时逐级增大。
	低3bit为向前相位微调，000-111，提前相位（延时）逐级增大。
	一级typical延时为0.1ns左右。*/
#define		DLL_COARSE_TUNE1_POS	18
#define		DLL_COARSE_TUNE1		REGBit(DLL_CFG, DLL_COARSE_TUNE1_POS, 2)


/* PHY_STATUS ********************************************************/
#define		PHY_STATUS			0x40033800
#define		OSC_Cycle_0_POS		16
#define		OSC_Cycle_0			REGBit(PHY_STATUS, OSC_Cycle_0_POS, 8)

#define		OSC_Cycle_1_POS		24
#define		OSC_Cycle_1			REGBit(PHY_STATUS, OSC_Cycle_1_POS, 4)


/* SDMA_CTL ********************************************************/
#define		SDMA_CTL			0x40037000
#define		SDMA_CTL_DMAEN_POS	0
#define		SDMA_CTL_DMAEN		REGBit(SDMA_CTL, SDMA_CTL_DMAEN_POS, 1)


/* SDMA_BAR ********************************************************/
#define		SDMA_BAR			0x40037400
/* bit[15:0]: MAIN_BAR */
#define		SDMA_BAR_MAIN_BAR		REGWord(SDMA_BAR, LowWORD)



/* SDMA_LEN ********************************************************/
#define		SDMA_LEN			0x40037800
/* bit[10:0]: 主数据长度 */
#define		SDMA_LEN_MAIN_LEN		REGWord(SDMA_LEN, LowWORD)
/* bit[17:16]: 解扰模式 */
#define		SDMA_LEN_RDMZ_MODE_POS	16
#define		SDMA_LEN_RDMZ_MODE		REGBit(SDMA_LEN, SDMA_LEN_RDMZ_MODE_POS, 2)

/* SDMA_SEED *******************************************************/
#define		SDMA_SEED			0x40037C00
/* bit[28:0]: 解扰种子 */





/*
 *  Flash controller register
 *
 *
 */


/* CH_Ctrl *********************************************************/
#define		CH0_CTRL				0x40010000
#define		CH1_CTRL				0x40020000

/* bit 0: DMA传输使能。软件置1，硬件清0
 *	0: DMA 空闲,表示DMA完成 
 *	1: DMA 传输中 */
#define		CH_CTRL_DMA_EN_POS	0
#define		CH0_CTRL_DMA_EN		REGBit(CH0_CTRL, CH_CTRL_DMA_EN_POS, 1)
#define		CH1_CTRL_DMA_EN		REGBit(CH1_CTRL, CH_CTRL_DMA_EN_POS, 1)

/* bit1: 启动ECC预编码。软件置1，硬件清0。当DMA_Bind为1时，固件置1无效。详见DMA_BIND描述。
 *	1: ECC 预编码开始
 *	0: ECC 预编码完成 */
#define		CH_CTRL_PRE_ENCODE_POS	1
#define		CH0_CTRL_PRE_ENCODE		REGBit(CH0_CTRL, CH_CTRL_PRE_ENCODE_POS, 1)
#define		CH1_CTRL_PRE_ENCODE		REGBit(CH1_CTRL, CH_CTRL_PRE_ENCODE_POS, 1)

/* bit2: 启动ECC查找错误比特。软件置1，硬件清0。仅在DMA完成后，且ECC_STAT返回2'b01时，可启动该命令
 *  1:ECC查找错误开始
 *  0:ECC查找错误完成 */
#define		CH_CTRL_FIND_ERR_POS	2
#define		CH0_CTRL_FIND_ERR		REGBit(CH0_CTRL, CH_CTRL_FIND_ERR_POS, 1)
#define		CH1_CTRL_FIND_ERR		REGBit(CH1_CTRL, CH_CTRL_FIND_ERR_POS, 1)


/* CH_Cfg **********************************************************/
#define		CH0_CFG					0x40010000+0x0400*1
#define		CH1_CFG					0x40020000+0x0400*1

/* bit0: DMA读写方向。
 *  1:DMA写Flash
 *  0:DMA读Flash */
#define		CH_CFG_FLASH_WR_POS		0
#define		CH0_CFG_FLASH_WR		REGBit(CH0_CFG, CH_CFG_FLASH_WR_POS, 1)
#define		CH1_CFG_FLASH_WR		REGBit(CH1_CFG, CH_CFG_FLASH_WR_POS, 1)
	
/* bit1: 开启模板读写 */
#define		CH_CFG_TMPLT_EN_POS		1
#define		CH0_CFG_TMPLT_EN		REGBit(CH0_CFG, CH_CFG_TMPLT_EN_POS, 1)
#define		CH1_CFG_TMPLT_EN		REGBit(CH1_CFG, CH_CFG_TMPLT_EN_POS, 1)	

/* bit2: 开启ECC编码/解码 */
#define		CH_CFG_ECC_EN_POS	2
#define		CH0_CFG_ECC_EN		REGBit(CH0_CFG, CH_CFG_ECC_EN_POS, 1)
#define		CH1_CFG_ECC_EN		REGBit(CH1_CFG, CH_CFG_ECC_EN_POS, 1)
 
/* bit[5:3]: ECC 模式选择，仅ECC_EN=1时有效。Templet 随机数模式复用
 *			TMPLT_EN == 0	TMPLT_EN == 1
 *			ECC 模式		随机数模式
 *	000		ECC16模式；	32bit随机数1，算法同2095RDM的随机数产生算法
 *	001		ECC25模式；	同上取反
 *	010		ECC30模式；	32bit随机数2，算法同2095CRDM的随机数产生算法（一次完成32次2095RDM的计算）
 *	011		ECC39模式；	同上取反
 *	100		ECC44模式；	32 bit常数
 *	101		ECC63模式；	同上取反
 *	110		ECC72模式；	保留
 *	111		保留；  		保留 */
#define		CH_CFG_ECC_MODE_POS		3
#define		CH0_CFG_ECC_MODE		REGBit(CH0_CFG, CH_CFG_ECC_MODE_POS, 3)
#define		CH1_CFG_ECC_MODE		REGBit(CH1_CFG, CH_CFG_ECC_MODE_POS, 3)										 

#define		CH0_CFG_TEMPLET_MODE		CH0_CFG_ECC_MODE
#define		CH1_CFG_TEMPLET_MODE		CH1_CFG_ECC_MODE


/* bit[7:6]: 加扰/解扰模式选择，仅RDMZ_EN=1时有效。
 *	11: 三星提供
 *	01: 东芝提供
 *	10	29位加扰算法
 *	00	23位加扰算法 */
#define		CH_CFG_RDMZ_MODE_POS	6
#define		CH0_CFG_RDMZ_MODE		REGBit(CH0_CFG, CH_CFG_RDMZ_MODE_POS, 2)
#define		CH1_CFG_RDMZ_MODE		REGBit(CH1_CFG, CH_CFG_RDMZ_MODE_POS, 2)

/* bit[9:8]:
 *	00:不丢弃数据
 *	01:丢弃前512byte
 *	10:丢弃后512byte
 *	11:丢弃全部1024byte */
#define		CH_CFG_GATHER_MODE_POS	8
#define		CH0_CFG_GATHER_MODE		REGBit(CH0_CFG, CH_CFG_GATHER_MODE_POS, 2)
#define		CH1_CFG_GATHER_MODE		REGBit(CH1_CFG, CH_CFG_GATHER_MODE_POS, 2)

/* bit10:
 *	1: 开启软件纠错，关闭硬件纠错
 *	0:关闭软件纠错，开启硬件纠错 */
#define		CH_CFG_SOFTCRT_EN_POS	10
#define		CH0_CFG_SOFTCRT_EN		REGBit(CH0_CFG, CH_CFG_SOFTCRT_EN_POS, 1)
#define		CH1_CFG_SOFTCRT_EN		REGBit(CH1_CFG, CH_CFG_SOFTCRT_EN_POS, 1)

/* bit11:开启bad column */
#define		CH_CFG_BADCOL_EN_POS	11
#define		CH0_CFG_BADCOL_EN		REGBit(CH0_CFG, CH_CFG_BADCOL_EN_POS, 1)
#define		CH1_CFG_BADCOL_EN		REGBit(CH1_CFG, CH_CFG_BADCOL_EN_POS, 1)

/* bit12:开启全1检测 */
#define		CH_CFG_ALLONE_EN_POS	12
#define		CH0_CFG_ALLONE_EN		REGBit(CH0_CFG, CH_CFG_ALLONE_EN_POS, 1)
#define		CH1_CFG_ALLONE_EN		REGBit(CH1_CFG, CH_CFG_ALLONE_EN_POS, 1)

/* bit13:DMA绑定模式 */
#define		CH_CFG_DMA_BIND_POS		13
#define		CH0_CFG_DMA_BIND		REGBit(CH0_CFG, CH_CFG_DMA_BIND_POS, 1)
#define		CH1_CFG_DMA_BIND		REGBit(CH1_CFG, CH_CFG_DMA_BIND_POS, 1)

/* bit14: 通道flash选择。
 *	1:当前通道服务flash1
 *	0:当前通道服务flash0 */
#define		CH_CFG_FLASH_SEL_POS	14
#define		CH0_CFG_FLASH_SEL		REGBit(CH0_CFG, CH_CFG_FLASH_SEL_POS, 1)
#define		CH1_CFG_FLASH_SEL		REGBit(CH1_CFG, CH_CFG_FLASH_SEL_POS, 1)

/* bit15: 开启加扰/解扰 */
#define		CH_CFG_RDMZ_EN_POS		15
#define		CH0_CFG_RDMZ_EN			REGBit(CH0_CFG, CH_CFG_RDMZ_EN_POS, 1)
#define		CH1_CFG_RDMZ_EN			REGBit(CH1_CFG, CH_CFG_RDMZ_EN_POS, 1)

/* bit16: 多读写4字节数据从/到Spare区域 */
#define		CH_CFG_SPARE_EN_POS		16
#define		CH0_CFG_SPARE_EN		REGBit(CH0_CFG, CH_CFG_SPARE_EN_POS, 1)
#define		CH1_CFG_SPARE_EN		REGBit(CH1_CFG, CH_CFG_SPARE_EN_POS, 1)

/* bit17: 当ECC使能且ECC模式为25/43/50/57时，硬件自动填充一个Byte到最后 */
#define		CH_CFG_FILL_EN_POS		17
#define		CH0_CFG_FILL_EN			REGBit(CH0_CFG, CH_CFG_FILL_EN_POS, 1)
#define		CH1_CFG_FILL_EN			REGBit(CH1_CFG, CH_CFG_FILL_EN_POS, 1)


/* CH_Status *******************************************************/
#define		CH0_STATUS				0x40010000+0x0400*2
#define		CH1_STATUS				0x40020000+0x0400*2

/* bit0: DMA错误标志。硬件置位，软件清零 */
#define		CH_STATUS_DMA_ERR_POS	0
#define		CH0_STATUS_DMA_ERR		REGBit(CH0_STATUS, CH_STATUS_DMA_ERR_POS, 1)
#define		CH1_STATUS_DMA_ERR		REGBit(CH1_STATUS, CH_STATUS_DMA_ERR_POS, 1)

/* bit1: Flash通道空闲标志
 *	1: 可接受任何指令，且指令会立即得到执行
 *	0: 通道忙 */
#define		CH_STATUS_FLASH_READY_POS	1
#define		CH0_STATUS_FLASH_READY		REGBit(CH0_STATUS, CH_STATUS_FLASH_READY_POS, 1)
#define		CH1_STATUS_FLASH_READY		REGBit(CH1_STATUS, CH_STATUS_FLASH_READY_POS, 1)

/* bit2: DMA空闲标志
 *	1: 可接受ECC指令并执行（关于指令分类，请参考Flash操作说明）
 *	0: DMA忙，不能接受指令 */
#define		CH_STATUS_DMA_READY_POS		2
#define		CH0_STATUS_DMA_READY		REGBit(CH0_STATUS, CH_STATUS_DMA_READY_POS, 1)
#define		CH1_STATUS_DMA_READY		REGBit(CH1_STATUS, CH_STATUS_DMA_READY_POS, 1)

/* bit[4:3]: ECC纠错状态
 *	0: 没有错误
 *	1: 存在错误，但未查错
 *	2: 纠（查）错失败
 *	3: 纠（查）错成功 */
#define		CH_STATUS_ECC_STATE_POS		3
#define		CH0_STATUS_ECC_STATE		REGBit(CH0_STATUS, CH_STATUS_ECC_STATE_POS, 2)
#define		CH1_STATUS_ECC_STATE		REGBit(CH1_STATUS, CH_STATUS_ECC_STATE_POS, 2)

/* bit[14:8]ECC错误bit */
#define		CH0_STATUS_ECC_CNT		REGBit(CH0_STATUS, 8, 7)
#define		CH1_STATUS_ECC_CNT		REGBit(CH1_STATUS, 8, 7)

/* bit[29:16]统计1的bit数 */
#define		CH0_STATUS_ONE_CNT		REGWord(CH0_STATUS, HighWORD)
#define		CH1_STATUS_ONE_CNT		REGWord(CH1_STATUS, HighWORD)
#define		CHx_STATUS_ONE_CNT_MASK		0x3FFF0000


/* CHx_DATA_BAR ****************************************************/
#define		CH0_DATA_BAR			0x40010000+0x0400*3
#define		CH1_DATA_BAR			0x40020000+0x0400*3
/* bit[15:0]: MAIN_BAR */
#define		CH0_DATA_BAR_MAIN_BAR		REGWord(CH0_DATA_BAR, LowWORD)
#define		CH1_DATA_BAR_MAIN_BAR		REGWord(CH1_DATA_BAR, LowWORD)


/* Data_Len ********************************************************/
#define		CH0_DATA_LEN			0x40010000+0x0400*4
#define		CH1_DATA_LEN			0x40020000+0x0400*4
/* bit[15:0]: 主数据长度 */
#define		CH0_DATA_LEN_MAIN_LEN		REGWord(CH0_DATA_LEN, LowWORD)
#define		CH1_DATA_LEN_MAIN_LEN		REGWord(CH1_DATA_LEN, LowWORD)
/* bit[23:16]: 模板写spare长度 */
#define		CH_DATA_LEN_TEMPLET_SPARELEN_POS	16
#define		CH0_DATA_LEN_TEMPLET_SPARELEN		REGBit(CH0_DATA_LEN, CH_DATA_LEN_TEMPLET_SPARELEN_POS, 8)
#define		CH1_DATA_LEN_TEMPLET_SPARELEN		REGBit(CH1_DATA_LEN, CH_DATA_LEN_TEMPLET_SPARELEN_POS, 8)


/* CHx_SPARE_BAR ***************************************************/
#define		CH0_SPARE_BAR			0x40010000+0x0400*5
#define		CH1_SPARE_BAR			0x40020000+0x0400*5
/* bit[15:0]: SPARE_BAR, SPARE数据基地址，需四字节对齐 */
#define		CH0_SPARE_BAR_SB		REGWord(CH0_SPARE_BAR, LowWORD)
#define		CH1_SPARE_BAR_SB		REGWord(CH1_SPARE_BAR, LowWORD)

/* CHx_BADCOL_BAR **************************************************/
#define		CH0_BADCOL_BAR			0x40010000+0x0400*6		/* 7,BadColumnTable地址,Table以Page建表 */
#define		CH1_BADCOL_BAR			0x40020000+0x0400*6		/* 7,BadColumnTable地址,Table以Page建表 */
/* bit[15:0]: BADCOL_BAR, BAD COLUMN数据基地址，需四字节对齐 */
#define		CH0_BADCOL_BAR_BB		REGWord(CH0_BADCOL_BAR, LowWORD)
#define		CH1_BADCOL_BAR_BB		REGWord(CH1_BADCOL_BAR, LowWORD)


/* CHx_BADCOL_OFFSET ************************************************/
#define		CH0_BADCOL_OFFSET		0x40010000+0x0400*7		/* 8,BadColumn起始列地址,一般是frame(DMA+Spaer+ECC)起始地址 */
#define		CH1_BADCOL_OFFSET		0x40010000+0x0400*7		/* 8,BadColumn起始列地址,一般是frame(DMA+Spaer+ECC)起始地址 */
/* bit[14:0] BADCOL_OFFSET, BAD COLUMN数据偏移 */
#define		CH0_BADCOL_OFFSET_BO		REGWord(CH0_BADCOL_OFFSET, LowWORD)
#define		CH1_BADCOL_OFFSET_BO		REGWord(CH1_BADCOL_OFFSET, LowWORD)
#define		CHx_BADCOL_OFFSET_MASK		0x7fff


/* templet data, 模板读写初始化数据（常数或随机数种子，由TMPLT_MODE决定） */
#define		CH0_TEMPLET_SEED		0x40010000+0x0400*8		/* 9,tempelt data0 */
#define		CH1_TEMPLET_SEED		0x40020000+0x0400*8

/* randomizer seed, 不支持比特操作 *********************************/
#define		CH0_RDMZ_SEED			0x40010000+0x0400*9
#define		CH1_RDMZ_SEED			0x40020000+0x0400*9
/* bit[28:0], RDMZ_SEED, 加扰/解扰种子
	东芝算法有效范围13:0
	三星算法有效范围14:0 */
#define		CHx_RDMZ_SEED_RS_MASK		0x3fff
/* bit[18:16], ECC_FRAME_NUM, ECC帧号, 仅东芝算法需要 */
#define		CHx_RDMZ_SEED_ECC_FRAME_NUM_MASK		0x70000


/* ONE_THRD, 全一阈值 **********************************************/
#define		CH0_ONE_THRD			0x40010000+0x0400*10
#define		CH1_ONE_THRD			0x40020000+0x0400*10
/* bit[13:0], ONE_THRD */
#define		CH0_ONE_THRD_OT		REGWord(CH0_ONE_THRD, LowWORD)
#define		CH1_ONE_THRD_OT		REGWord(CH1_ONE_THRD, LowWORD)
#define		CHx_ONE_THRD_OT_MASK	0x1fff


/* ERR_LOC 不支持比特操作, 访问前必须开启ECC 时钟 ******************/
#define		CH0_ERR_LOC				0x40010000+0x0400*11
#define		CH1_ERR_LOC				0x40020000+0x0400*11
/* bit[13:0], ERR_LOC */
#define		CHx_ERR_LOC_MASK		0x1fff


/* flash command, 不支持比特操作, 访问前必须开启Flash 时钟 ********/
#define		CH0_FLASH_CMD			0x40010000+0x0400*12
#define		CH1_FLASH_CMD			0x40020000+0x0400*12
#define		CH_FLASH_CMD(n)			(0x40013000+(0x10000*(n)))
/* bit[7:0]: FLASH_CMD */
#define		CHx_FLASH_CMD_MASK		0xff


/* flash column addr, 不支持比特操作, 访问前必须开启Flash 时钟 *****/
#define		CH0_FLASH_COL_ADDR		0x40010000+0x0400*13
#define		CH1_FLASH_COL_ADDR		0x40020000+0x0400*13


/* flash row addr, 不支持比特操作, 访问前必须开启Flash 时钟 ********/
#define		CH0_FLASH_ROW_ADDR		0x40010000+0x0400*14
#define		CH1_FLASH_ROW_ADDR		0x40020000+0x0400*14


/* flash data, 不支持比特操作, 访问前必须开启Flash 时钟 ************/
#define		CH0_FLASH_DATA			0x40010000+0x0400*15
#define		CH1_FLASH_DATA			0x40020000+0x0400*15
/* bit[15:0], 读取/写入Flash数据, 写此寄存器时，向Flash写入数据; 读此寄存器时，从Flash读出数据
	8 bits异步Flash时，数据长度为一个byte
	16 bits或同步Flash时，数据长度为一个word */
#define		CHx_FLASH_DATA_MASK		0xffff


/* Flash_Para, 访问前必须开启Flash 时钟 ****************************/
#define		CH0_FLASH_PAR			0x40010000+0x0400*16
#define		CH1_FLASH_PAR			0x40020000+0x0400*16

/* bit[1:0]: Async Flash标志
	 00: ONFI Flash
	 01: 8bit Asynchronous Flash
	 10: Toggle Flash
	 11: 16bit Asynchronous Flash */
#define		CH_FLASH_PAR_FLASH_TYPE_POS		0
#define		CH0_FLASH_PAR_FLASH_TYPE		REGBit(CH0_FLASH_PAR, CH_FLASH_PAR_FLASH_TYPE_POS, 2)
#define		CH1_FLASH_PAR_FLASH_TYPE		REGBit(CH1_FLASH_PAR, CH_FLASH_PAR_FLASH_TYPE_POS, 2)

#define	ONFI_FLASH		0x0
#define	ASYNC8_FLASH	0x1
#define	TOGGLE_FLASH	0x2
#define	ASYNC16_FLASH	0x3


/* ONFI Flash时钟输出使能,仅为ONFI Flash时有效
	 0: 时钟输出关断
	 1: 时钟输出打开 */
#define		CH_FLASH_PAR_ONFICLKOUT_EN_POS	2
#define		CH0_FLASH_PAR_ONFICLKOUT_EN		REGBit(CH0_FLASH_PAR, CH_FLASH_PAR_ONFICLKOUT_EN_POS, 1)
#define		CH1_FLASH_PAR_ONFICLKOUT_EN		REGBit(CH1_FLASH_PAR, CH_FLASH_PAR_ONFICLKOUT_EN_POS, 1)



/* bit[4:3]列地址长度,对应FLASH_Col_ADDRx寄存器
	 0: 1 Byte
	 1: 2 Bytes
	 2: 3 Bytes
	 3: 4 Bytes */
#define		CH_FLASH_PAR_COL_ADDR_LEN_POS	3
#define		CH0_FLASH_PAR_COL_ADDR_LEN		REGBit(CH0_FLASH_PAR, CH_FLASH_PAR_COL_ADDR_LEN_POS, 2)
#define		CH1_FLASH_PAR_COL_ADDR_LEN		REGBit(CH1_FLASH_PAR, CH_FLASH_PAR_COL_ADDR_LEN_POS, 2)

/* bit[6:5]行地址长度,对应FLASH_ROW_ADDRx寄存器
	 0: 1 Byte
	 1: 2 Bytes
	 2: 3 Bytes
	 3: 4 Bytes */
#define		CH_FLASH_PAR_ROW_ADDR_LEN_POS	5
#define		CH0_FLASH_PAR_ROW_ADDR_LEN		REGBit(CH0_FLASH_PAR, CH_FLASH_PAR_ROW_ADDR_LEN_POS, 2)
#define		CH1_FLASH_PAR_ROW_ADDR_LEN		REGBit(CH1_FLASH_PAR, CH_FLASH_PAR_ROW_ADDR_LEN_POS, 2)

/* bit[7]利用CLE结束传输仅Toggle Flash有效
	 0: 利用CE控制
	 1: 利用CLE控制 */
#define		CH_FLASH_PAR_CLE_CE_POS		7
#define		CH0_FLASH_PAR_CLE_CE		REGBit(CH0_FLASH_PAR, CH_FLASH_PAR_CLE_CE_POS, 1)
#define		CH1_FLASH_PAR_CLE_CE		REGBit(CH1_FLASH_PAR, CH_FLASH_PAR_CLE_CE_POS, 1)

/* bit[23:8]bad column 填充数据 */
#define		CH0_FLASH_PAR_BAD_DATAL		REGBit(CH0_FLASH_PAR, 8, 8)
#define		CH0_FLASH_PAR_BAD_DATAH		REGBit(CH0_FLASH_PAR, 16, 8)

#define		CH1_FLASH_PAR_BAD_DATAL		REGBit(CH1_FLASH_PAR, 8, 8)
#define		CH1_FLASH_PAR_BAD_DATAH		REGBit(CH1_FLASH_PAR, 16, 8)




/* Flash_Timing, 访问前必须开启Flash 时钟 **************************/
#define		CH0_FLASH_TIMING		0x40010000+0x0400*17
#define		CH1_FLASH_TIMING		0x40020000+0x0400*17

/* bit[1:0]   */
#define		CH_FLASH_TIMING_TCALS_POS	0
#define		CH0_FLASH_TIMING_TCALS		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TCALS_POS, 2)
#define		CH1_FLASH_TIMING_TCALS		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TCALS_POS, 2)

/* bit[3:2]   */
#define		CH_FLASH_TIMING_TCALH_POS	2
#define		CH0_FLASH_TIMING_TCALH		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TCALH_POS, 2)
#define		CH1_FLASH_TIMING_TCALH		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TCALH_POS, 2)

/* bit[5:4]   */
#define		CH_FLASH_TIMING_TWP_POS		4
#define		CH0_FLASH_TIMING_TWP		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TWP_POS, 2)
#define		CH1_FLASH_TIMING_TWP		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TWP_POS, 2)

/* bit[7:6]   */
#define		CH_FLASH_TIMING_TWH_POS		6
#define		CH0_FLASH_TIMING_TWH		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TWH_POS, 2)
#define		CH1_FLASH_TIMING_TWH		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TWH_POS, 2)

/* bit[9:8]   */
#define		CH_FLASH_TIMING_TRP_POS		8
#define		CH0_FLASH_TIMING_TRP		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TRP_POS, 2)
#define		CH1_FLASH_TIMING_TRP		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TRP_POS, 2)

/* bit[11:10] */
#define		CH_FLASH_TIMING_TREH_POS	10
#define		CH0_FLASH_TIMING_TREH		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TREH_POS, 2)
#define		CH1_FLASH_TIMING_TREH		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TREH_POS, 2)

/* bit[13:12] */
#define		CH_FLASH_TIMING_TWPRE_POS	12
#define		CH0_FLASH_TIMING_TWPRE		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TWPRE_POS, 2)
#define		CH1_FLASH_TIMING_TWPRE		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TWPRE_POS, 2)

/* bit[15:14] */
#define		CH_FLASH_TIMING_TWPST_POS	14
#define		CH0_FLASH_TIMING_TWPST		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TWPST_POS, 2)
#define		CH1_FLASH_TIMING_TWPST		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TWPST_POS, 2)

/* bit[17:16] */
#define		CH_FLASH_TIMING_TRPRE_POS	16
#define		CH0_FLASH_TIMING_TRPRE		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TRPRE_POS, 2)
#define		CH1_FLASH_TIMING_TRPRE		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TRPRE_POS, 2)

/* bit[19:18] */
#define		CH_FLASH_TIMING_TRPST_POS	18
#define		CH0_FLASH_TIMING_TRPST		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TRPST_POS, 2)
#define		CH1_FLASH_TIMING_TRPST		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TRPST_POS, 2)

/* bit[21:20] */
#define		CH_FLASH_TIMING_TRPSTH_POS	20
#define		CH0_FLASH_TIMING_TRPSTH		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TRPSTH_POS, 2)
#define		CH1_FLASH_TIMING_TRPSTH		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TRPSTH_POS, 2)

/* bit[23:22] */
#define		CH_FLASH_TIMING_TCAD_POS	22
#define		CH0_FLASH_TIMING_TCAD		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TCAD_POS, 2)
#define		CH1_FLASH_TIMING_TCAD		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TCAD_POS, 2)

/* bit[25:24] */
#define		CH_FLASH_TIMING_TDO_POS		24
#define		CH0_FLASH_TIMING_TDO		REGBit(CH0_FLASH_TIMING, CH_FLASH_TIMING_TDO_POS, 2)
#define		CH1_FLASH_TIMING_TDO		REGBit(CH1_FLASH_TIMING, CH_FLASH_TIMING_TDO_POS, 2)




/* flash Command define */

#define		FL_CMD_READ_SPARE				0x50	/* Read Spare                             */
#define		FL_CMD_READ1					0x00	/* Read flash command                     */
#define		FL_CMD_READ2					0x30
#define		FL_CMD_MULTIPLANE_READ			0x60
#define		FL_CMD_SERIAL_DATA_OUT_1		0x05 
#define		FL_CMD_SERIAL_DATA_OUT_2		0xE0
#define		FL_CMD_NEXT_PAGE_READ_INC		0x31 
#define		FL_CMD_NEXT_PAGE_READ_NO_INC	0x3F


#define		FL_CMD_COPY_BACK_READ1			0x00	/* Read for Copy Back                     */
#define		FL_CMD_COPY_BACK_READ2			0x35	/* Read for Copy Back                     */
#define		FL_CMD_CACHE_READ				0x31	/* Cache Read command of T/I/M flashs     */
#define		FL_CMD_CACHE_READ_END			0x3F	/* Cache Read command end of T/I/M flashs */
#define		FL_CMD_CACHE_READ_2				0x3C
#define		FL_CMD_READID					0x90	/* Read ID                                */
#define		FL_CMD_READID_EXT				0x91	/* Read Extended ID                       */
#define		FL_CMD_GET_FEATURE				0xEE
#define		FL_CMD_SET_FEATURE				0xEF
#define		FL_CMD_RESET					0xFF	/*                                        */
#define		FL_CMD_SYNC_RESET				0xFC

#define		FL_CMD_PAGE_PROGRAM1 			0x80	/* Page Program                           */
#define		FL_CMD_PAGE_PROGRAM2 			0x10	/* Page Program                           */
#define		FL_CMD_COPY_BACK_PROGRAM1		0x85 	/* Copy-Back Program                      */
#define		FL_CMD_COPY_BACK_PROGRAM2		0x10 	/* Copy-Back Program                      */
#define		FL_CMD_BLOCK_ERASE1				0x60	/* Block Erase                            */
#define		FL_CMD_BLOCK_ERASE2				0xD0	/* Block Erase                            */
#define		FL_CMD_RANDOM_DATA_INPUT		0x85	/* Random Data Input*                     */

#define		FL_CMD_RANDOM_DATA_OUTPUT1_M2	0x60	/* Micron Random Data Output*             */
#define		FL_CMD_RANDOM_DATA_OUTPUT1		0x05	/* Random Data Output*                    */
#define		FL_CMD_RANDOM_DATA_OUTPUT2		0xE0	/* Random Data Output*                    */
#define		FL_CMD_CACHE_PROGRAM1			0x80	/* Cache program                          */
#define 	FL_CMD_CACHE_PROGRAM2			0x15	/* Cache program                          */
#define		FL_CMD_READ_STATUS				0x70   	/* Read Status                            */
#define     FL_CMD_ENHNACE_READ_STATUS      0x78    /* Enhance read status*/

#define		FL_CMD_MICRON_SEL_PLANE			0x06
#define		FL_CMD_MICRON_MULTIPANEREADCMD2	0x32


#define		TS_CMD_RELIABLEMODEACCESS		0xA2
#define		TS_CMD_SET1STPROGRAMMODE		0x09
#define		TS_CMD_SET2NDPROGRAMMODE		0x0D
#define		TS_CMD_LOWPAGESELEST			0x01
#define		TS_CMD_MIDDLEPAGESELEST			0x02
#define		TS_CMD_UPPERPAGESELEST			0x03
#define		TS_CMD_INPUTDATATOBUF			0x1A
#define		TS_CMD_COPYBACKPROGRAM			0x85

#define		FL_CMD_INVALID_RELIABLE_CMD		0x00


#define		FL_CMD_PAGE_LATCH				0xC0
#define		FL_CMD_POGRAME_CONFIRM			0x8B

#endif
