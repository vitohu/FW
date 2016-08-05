#ifndef _GPIO_H_
#define _GPIO_H_




//*********************************************************************
//Register Address Definition

//*********************************************************************
#define		GPIO0_DATA					0x40030000


#define		FCEN0_Pos					0
#define		FCEN0						REGBit(GPIO0_DATA, FCEN0_Pos, 1)//Bit 0

#define		FCEN1_Pos					1
#define		FCEN1						REGBit(GPIO0_DATA, FCEN1_Pos, 1)//Bit 1

#define		FCEN2_Pos					2
#define		FCEN2						REGBit(GPIO0_DATA, FCEN2_Pos, 1)//Bit 2

#define		FCEN3_Pos					3
#define		FCEN3						REGBit(GPIO0_DATA, FCEN3_Pos, 1)//Bit 3

#define		FRB0_Pos					4
#define		FRB0						REGBit(GPIO0_DATA, FRB0_Pos, 1)//Bit 4

#define		FRB1_Pos					5
#define		FRB1						REGBit(GPIO0_DATA, FRB1_Pos, 1)//Bit 5

#define		F0DQS_Pos					6
#define		F0DQS						REGBit(GPIO0_DATA, F0DQS_Pos, 1)//Bit 6

#define		F1DQS_Pos					7
#define		F1DQS						REGBit(GPIO0_DATA, F1DQS_Pos, 1)//Bit 7

#define		F0RDN_Pos					8
#define		F0RDN						REGBit(GPIO0_DATA, F0RDN_Pos, 1)//Bit 8

#define		F0WRN_Pos					9
#define		F0WRN						REGBit(GPIO0_DATA, F0WRN_Pos, 1)//Bit 9

#define		F0CLE_Pos					10
#define		F0CLE						REGBit(GPIO0_DATA, F0CLE_Pos, 1)//Bit 10

#define		F0ALE_Pos					11
#define		F0ALE						REGBit(GPIO0_DATA, F0ALE_Pos, 1)//Bit 11

#define		F1RDN_Pos					12
#define		F1RDN						REGBit(GPIO0_DATA, F1RDN_Pos, 1)//Bit 12

#define		F1WRN_Pos					13
#define		F1WRN						REGBit(GPIO0_DATA, F1WRN_Pos, 1)//Bit 13

#define		F1CLE_Pos					14
#define		F1CLE						REGBit(GPIO0_DATA, F1CLE_Pos, 1)//Bit 14

#define		F1ALE_Pos					15
#define		F1ALE						REGBit(GPIO0_DATA, F1ALE_Pos, 1)//Bit 15

#define		F0DATA_Pos					16
#define		F0DATA						REGBit(GPIO0_DATA, F0DATA_Pos, 8)//Bit 16~23

#define		F1DATA_Pos					24
#define		F1DATA						REGBit(GPIO0_DATA, F1DATA_Pos, 8)//Bit 24~31


//*********************************************************************
#define		GPIO0_DIR					0x40030400


//*********************************************************************
#define		GPIO0_IE					0x40030800


//*********************************************************************
#define		GPIO1_DATA					0x40030C00


#define		SPI_SCK_Pos					0
#define		SPI_SCK						REGBit(GPIO1_DATA, SPI_SCK_Pos, 1)//Bit 0

#define		SPI_MOSI_Pos				1
#define		SPI_MOSI					REGBit(GPIO1_DATA, SPI_MOSI_Pos, 1)//Bit 1

#define		SPI_MISO_Pos				2
#define		SPI_MISO					REGBit(GPIO1_DATA, SPI_MISO_Pos, 1)//Bit 2

#define		SPI_CS_Pos					3
#define		SPI_CS						REGBit(GPIO1_DATA, SPI_CS_Pos, 1)//Bit 3

#define		Flash_WP_Pos				4
#define		Flash_WP					REGBit(GPIO1_DATA, Flash_WP_Pos, 1)//Bit 4

#define		LED_PIN_Status_Pos			5
#define		LED_PIN_Ststus				REGBit(GPIO1_DATA, LED_PIN_Status_Pos, 1)//Bit 5

#define		Packet_Sel_Pos				6
#define		Packet_Sel					REGBit(GPIO1_DATA, Packet_Sel_Pos, 1)//Bit 6

//Bit 7~31 are unused.


//*********************************************************************
#define		GPIO1_DIR					0x40031000


//*********************************************************************
#define		GPIO1_IE					0x40031400


//*********************************************************************
#define		PAD_CFG						0x40031800


#define		F0DATA_GPIO_EN_Pos			0
#define		F0DATA_GPIO_EN				REGBit(PAD_CFG, F0DATA_GPIO_EN_Pos, 1)//Bit 0

#define		F0CMD_GPIO_EN_Pos			1
#define		F0CMD_GPIO_EN				REGBit(PAD_CFG, F0CMD_GPIO_EN_Pos, 1)//Bit 1

#define		F0DQS_GPIO_EN_Pos			2
#define		F0DQS_GPIO_EN				REGBit(PAD_CFG, F0DQS_GPIO_EN_Pos, 1)//Bit 2

#define		F1DATA_GPIO_EN_Pos			3
#define		F1DATA_GPIO_EN				REGBit(PAD_CFG, F1DATA_GPIO_EN_Pos, 1)//Bit 3

#define		F1CMD_GPIO_EN_Pos			4
#define		F1CMD_GPIO_EN				REGBit(PAD_CFG, F1CMD_GPIO_EN_Pos, 1)//Bit 4

#define		F1DQS_GPIO_EN_Pos			5
#define		F1DQS_GPIO_EN				REGBit(PAD_CFG, F1DQS_GPIO_EN_Pos, 1)//Bit 5

#define		SPI_GPIO_EN_Pos				6
#define		SPI_GPIO_EN					REGBit(PAD_CFG, SPI_GPIO_EN_Pos, 1)//Bit 6

#define		LED_EN_Pos					7
#define		LED_EN						REGBit(PAD_CFG, LED_EN_Pos, 1)//Bit 7

#define		FLASH0_CE_END_Pos			8
#define		FLASH0_CE_END				REGBit(PAD_CFG, FLASH0_CE_END_Pos, 4)//Bit 8~11

#define		FLASH1_CE_END_Pos			12
#define		FLASH1_CE_END				REGBit(PAD_CFG, FLASH1_CE_END_Pos, 4)//Bit 12~15

//Bit 16~31 are unused.



#endif
