#ifndef __UART_H__
#define __UART_H__

#include <dbg_cfg.h>

#define REG8(addr)      (*(volatile unsigned char*)(addr))
#define REG16(addr)     (*(volatile unsigned short*)(addr))
#define REG32(addr)     (*(volatile unsigned int*)(addr))

#define REG_ACCESS_BYTE(addr32, Byte_n)     ((addr32) + (0x00FF << 2) + (Byte_n))


#define UART_CTRL_BASE      (0x40040000)

#define UART_CTRL           UART_CTRL_BASE
#define UART_BPR            (UART_CTRL_BASE + 0x400)
#define UART_IE             (UART_CTRL_BASE + 0x800)
#define UART_TXD            (UART_CTRL_BASE + 0xC00)
#define UART_RXD            (UART_CTRL_BASE + 0x1000)
#define UART_STAT           (UART_CTRL_BASE + 0x1400)

struct uart_rx_buf {
	u8	*buf;
	u8	wp;
	u8	rp;
	u8	cnt;
};

extern u8 uart_init(u32 bandrate, u32 clk);
extern void uart_putc(const char c);
extern void uart_print(const char *fmt, ...);
extern void dbg_assert_func(cs8 *file, u16 line, cs8 *func);
extern void uart_rx_loop(void);
extern u8 uart_rx_cnt(void);
extern u8 uart_read_char(void);

extern void hex_dump(u8 *promt, u8 *dat, u32 len);
extern void print_data(const u8 *const data, u32 const len);


//#define UART_MESSAGE      dbg_assert
#define DBG_MESSAGE     dbg_mesg
#if UART_MESSAGE
#define dbg_mesg(fmt, args...)		uart_print(fmt, ##args)
#else
#define dbg_mesg(fmt, args...)
#endif


#if(UART_ASSERT == mENABLE)
//declaration assert function
void dbg_assert_func(cs8 *file, u16 line, cs8 *func);

//definition assert macro
#define ASSERT_LVL(MODULE,lvl,is_ok)    ((MODULE_##MODULE##_ASSERT_TH < lvl) ? \
    ((void)0 ): (((bool)(is_ok) == TRUE) ? ((void)0) : \
    dbg_assert_func((cs8 *)__FILE__, (u16)__LINE__, (cs8 *)__FUNCTION__)))
#else
#define ASSERT_LVL(MODULE,lvl,is_ok)
#define dbg_assert_func(a,b,c)
#endif


#if(UART_MESSAGE == mENABLE)
//definition deug message macro
#define DBGMSG_LVL(MODULE,lvl, ...)     ((MODULE_##MODULE##_MSG_TH < lvl) ? \
    ((void)0 ) : (uart_print(__VA_ARGS__)))
#else

#define DBGMSG_LVL(MODULE,lvl, ...)
#endif

#endif
