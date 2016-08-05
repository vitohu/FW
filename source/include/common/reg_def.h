#ifndef _REG_DEF_H_
#define _REG_DEF_H_


//*********************************************************************
//Const Definition
#define		LowWORD 		0
#define		HighWORD		2

#define		BYTE0			0
#define		BYTE1			1
#define		BYTE2			2
#define		BYTE3			3

//*********************************************************************
//Register Access Macro Definition
#define		REGWord(BaseAddr,WordPos)	((BaseAddr) + (WordPos))
#define		REGBit(BaseAddr,BitOff,BitWidth)	((BaseAddr) + \
			(((1 << (BitWidth)) - 1) << (((BitOff) & 0x07) + 2)) + ((BitOff) >> 3))


#define		REGAccessByte(RegAddr)	((volatile u8 *)(RegAddr))
#define		REGAccessWord(RegAddr)	((volatile u16 *)(RegAddr))
#define		REGAccessDword(RegAddr)	((volatile u32 *)(RegAddr))


#define		REGValue(RegV,RegPos)	(RegV << (RegPos & 0x07))



#endif
