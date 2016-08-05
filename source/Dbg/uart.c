
#include <global_def.h>
#include <ex_stdio.h>
#include <memop.h>

#if	UART_DEBUG
//#include <stdio.h>
//#include <stdarg.h>
//#include <string.h>

#define CONFIG_UART_RX_BUF_SIZE		128
static u8 uart_rx[CONFIG_UART_RX_BUF_SIZE];
static struct uart_rx_buf uart_rx_buf;
#endif
static void uart_buf_init(void)
{
#if UART_DEBUG
	struct uart_rx_buf *rx = &uart_rx_buf;

	rx->buf = uart_rx;
	rx->cnt = 0;
	rx->wp = 0;
	rx->rp = 0;
#endif
}

u8 uart_rx_cnt(void)
{
#if UART_DEBUG
	return uart_rx_buf.cnt;
#else
    return 0;
#endif
}

u8 uart_read_char(void)
{
	
#if UART_DEBUG
    u8 c;
	struct uart_rx_buf *rx = &uart_rx_buf;

	c = rx->buf[rx->rp ++];
	rx->cnt--;

	rx->rp %= CONFIG_UART_RX_BUF_SIZE;
    return c;
#else
    return 0;
#endif
	
}

#define SYS_CLK		(0x1ace810)//25113605
//(0x1ace818) /*	OSC CLK */

u8 uart_init(u32 bandrate, u32 wclk)
{
#if UART_DEBUG
	u8 sta,c;
    u8 div = 0;
    u16 bpr = 0;
	u32 clk = wclk;

	u8 stopbits = 1;
	u8 pari = 'n';

    /* UART MODE ENABLE */
    //REG32(GPIO_CTRL) |= (1 << 8);

    switch(stopbits) {
    case 1:
        REG32(UART_CTRL) &= (~(1 << 4));
        break;
    case 2:
        REG32(UART_CTRL) |= (1 << 4);
        break;
    }

    switch(pari) {
    case 'p':
    case 'P':
        REG32(UART_CTRL) &= (~(0x03 << 2));
        REG32(UART_CTRL) |= (0x1 << 2);
        break;
    case 'o':
    case 'O':
        REG32(UART_CTRL) &= (~(0x03 << 2));
        REG32(UART_CTRL) |= (0x2 << 2);
        break;
    case 'n':
    case 'N':
        REG32(UART_CTRL) &= (~(0x03 << 2));
        break;
    }


    /* Uart tx & rx enable */
    REG32(UART_CTRL) |= 0x03;
#define UART_AUTO_BANDRATE
#ifndef UART_AUTO_BANDRATE
    /* Disable auto band mode */
    REG32(UART_CTRL) &= (~(1 << 5));

    REG32(UART_BPR) &= (~(0x7 >> 13));
    REG32(UART_BPR) &= (~0x3FF);

    while(1) {
        bpr = clk/bandrate - 1;
        if (bpr > 0x3FF) {
            div ++;
            clk /= (div + 1);
        } else
            break;
    }

    REG32(UART_BPR) |= (div << 13);
    REG32(UART_BPR) |= bpr;

    /* UART RX IE Enable */
    //REG32(UART_IE)  |= (0x01 << 6);
#else
	/* Enable auto band mode */
    REG32(UART_CTRL) |= (1 << 5);
	REG32(UART_BPR) |= (div << 13);
    
	while(1) {
        sta = REG8(REG_ACCESS_BYTE(UART_STAT, 0));
		if ((sta >> 3) & 0x01) {
			c = REG8(REG_ACCESS_BYTE(UART_RXD, 0));
			break;
		}
	}

#endif

	uart_buf_init();

#endif
    return 0;
}

void uart_putc(const char c)
{
#if (mENABLE == UART_DEBUG) 
	while(((REG32(UART_STAT) >> 4) & 0x01) == 0);

	REG8(REG_ACCESS_BYTE(UART_TXD, 0)) = c;
#endif
}

#if 0
static u8 uart_getc(void)
{
	while(((REG32(UART_STAT) >> 3) & 0x01) == 0);

	return REG8(REG_ACCESS_BYTE(UART_RXD, 0));
}
#endif

static void uart_puts(const char *buf)
{
#if UART_DEBUG
    while(*buf != '\0') {
        if (*buf == '\n') {
            uart_putc(0x0d);
            uart_putc(0x0a);
        } else
            uart_putc(*buf);
        buf++;
    }
#endif
}

void uart_print(const char *fmt, ...)
{
#if UART_DEBUG
    va_list ap;
    char string[128];
    va_start(ap,fmt);
	vsprintf(string,fmt,ap);
    va_end(ap);
    uart_puts(string);
    
#endif
}

static void uart_write_char(u8 c)
{
#if UART_DEBUG
	struct uart_rx_buf *rx = &uart_rx_buf;

	rx->buf[rx->wp ++] = c;

	rx->wp %= CONFIG_UART_RX_BUF_SIZE;
	rx->cnt ++;
#endif
}

void uart_rx_loop(void)
{
#if UART_DEBUG
	u8 c;
	u32 sta = REG32(UART_STAT);

	if ((sta >> 3) & 0x01) {
		c = REG8(REG_ACCESS_BYTE(UART_RXD, 0));
		uart_putc(c);
		uart_write_char(c);
	}
#endif
}

#if UART_ASSERT
void dbg_assert_func(cs8 *file, u16 line, cs8 *func)
{
#if UART_DEBUG
    uart_print("-Assert-:%s-%d-%s()\n", file, line, func);

    while(1);
#endif
}
#endif

void hex_dump(u8 *promt, u8 *dat, u32 len)
{
#if UART_DEBUG
	u32 i;

	dbg_mesg("%s:\n", promt);

	for (i = 0; i < len; i++) {
		dbg_mesg("0x%02X ",dat[i]);
		if (((i + 1) % 16) == 0)
			dbg_mesg("\n");
	}

	dbg_mesg("\n");
#endif
}


static char *fmtadj(char *src, u8 chcnt, u8 basecnt)
{
#if UART_DEBUG
	char *tmp = src + 6 + basecnt * chcnt;

	while(chcnt < 16)
	{
		*tmp++ = ' ';
		*tmp++ = ' ';
		*tmp++ = ' ';
		chcnt++;
	}
	*tmp = '\0';
#endif
	return src;
}


static void fmtrecovery(char *src, u8 chcnt, u8 basecnt)
{
#if UART_DEBUG
	char *tmp = src + 6 + basecnt * chcnt;

	while(chcnt < 16)
	{
		*tmp++ = '%';
		*tmp++ = '0';
		*tmp++ = '2';
		*tmp++ = 'x';
		*tmp++ = ' ';
		chcnt++;
	}
	*tmp = '\0';
#endif
}

void print_data(const u8 *const data, u32 const len)
{
#if UART_DEBUG
#if UART_DATA_PRINT
#define LINE_MAX_CH	16
#define P(n) (isprint((int)(p[n]))?(int)(p[n]):'.')

	u8 temp[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
				   0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	u8 *p = (u8 *)data;
	u32 i = 0;
	u32 left = len;
	s32 suppress_flag = -1; //whether suppress the same content
	s32 first_dup     = -1; //output first suppressed line, ignore the others

	//UART_ASSERT(NULL != data);
	//UART_ASSERT(len > 0);

	dbg_mesg("\n");
	for(i = 0; i < len; i+= 16 )
	{
		if(left < 16)
		{
			break;
		}

		p = (u8 *)data+i;
		if(0 == memcmp_u8(p, temp, 16))
		{
			suppress_flag = 1;
			if(first_dup == -1)
			{
				first_dup = 1;
			}
			else if(first_dup == 1)
			{
				first_dup = 2;
			}
			else if(first_dup == 2)
			{
				first_dup = 0;
			}
		}
		else
		{
			suppress_flag = -1;
			first_dup     = -1;
		}

		if(suppress_flag != 1)
		{
			dbg_mesg("%-x: %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x ",
				 i,
				 p[ 0], p[ 1], p[ 2], p[ 3], p[ 4], p[ 5], p[ 6], p[ 7],
				 p[ 8], p[ 9], p[10], p[11], p[12], p[13], p[14], p[15]);

			dbg_mesg("|%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c|\n",
				P( 0), P( 1), P( 2), P( 3), P( 4), P( 5), P( 6), P( 7),
				P( 8), P( 9), P(10), P(11), P(12), P(13), P(14), P(15));


		}
		else if((suppress_flag == 1) && (first_dup == 1))
		{
			dbg_mesg("%-x: \n", i);
		}
		else if((suppress_flag == 1) && (first_dup == 2))
		{
			dbg_mesg("\n");
		}
		memcpy_u8(temp, p, 16);
		left -= 16;
	}

	if((left < LINE_MAX_CH) && (left != 0))
	{
		u8 j;

		memcpy_u8(temp, p, left);
		//memset_u8(temp + left, '_', LINE_MAX_CH - left);
		p = (u8 *)temp;
		for (j = 15; j>= left; j--)
		{
			p[j] = ' ';
		}

		dbg_mesg(fmtadj("%-x: %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x %02x ", left, 5),
			 i,
			 p[ 0], p[ 1], p[ 2], p[ 3], p[ 4], p[ 5], p[ 6], p[ 7],
			 p[ 8], p[ 9], p[10], p[11], p[12], p[13], p[14], p[15]);

		dbg_mesg("|%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c|n",
			P( 0), P( 1), P( 2), P( 3), P( 4), P( 5), P( 6), P( 7),
			P( 8), P( 9), P(10), P(11), P(12), P(13), P(14), P(15));

		fmtrecovery("%-x: %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x ", left, 5);

		return;
	}
#endif
#endif
}

