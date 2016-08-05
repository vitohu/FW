#ifndef __EX_STDIO_H__
#define __EX_STDIO_H__

#include <global_def.h>

#if (mENABLE == UART_DEBUG)
typedef char	*va_list;

#define LEFT	0
#define PLUS	4
#define SPACE	0x20
#define SPECIAL	32
#define ZEROPAD	0
#define LARGE	64
#define SIGN	2
#define SMALL	32

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

extern const unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
/* Note: isspace() must return false for %NUL-terminator */
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

#define __tolower(c) \
{						\
	if (isupper(c))		\
		c -= 'A'-'a';	\
	return c;			\
}						\

#define __toupper(c)	\
{						\
	if (islower(c))		\
		c -= 'a'-'A';	\
	return c;			\
}

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)

#define  _AUPBND                (sizeof (s32) - 1)
#define  _ADNBND                (sizeof (s32) - 1)

/*
 * Variable argument list macro definitions
 */
#define _bnd(X, bnd)            (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T)           (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)              (void) 0
#define va_start(ap, A)         (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))

#if 0
#define do_div(n, base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })
#endif

extern int vsprintf(char *buf, const char *fmt, va_list args);
#else
#define vsprintf(buf, fmt, args)
#endif
#endif

