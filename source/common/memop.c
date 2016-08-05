
#include <global_def.h>
#include <memop.h>

void memcpy_u8(u8 *dst, u8 *src, u32 sz)
{
    ASSERT_LVL(COM, ASSERT_LVL0, dst!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, src!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, sz!=0);

    while( sz-- > 0)
    {
        *dst++ = *src++;
    }
}

void memcpy_u16(u16 *dst, u16 *src, u32 sz)
{
    ASSERT_LVL(COM, ASSERT_LVL0, dst!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, src!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, sz!=0);
    ASSERT_LVL(COM, ASSERT_LVL0, 0 == ((u32)dst & 0x01));
    ASSERT_LVL(COM, ASSERT_LVL0, 0 == ((u32)src & 0x01));

    while( sz-- > 0)
    {
        *dst++ = *src++;
    }
}

void memcpy_u32(u32 *dst, u32 *src, u32 sz)
{
    ASSERT_LVL(COM, ASSERT_LVL0, dst!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, src!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, sz!=0);
    ASSERT_LVL(COM, ASSERT_LVL0, 0 == ((u32)dst & 0x03));
    ASSERT_LVL(COM, ASSERT_LVL0, 0 == ((u32)src & 0x03));

    while( sz-- > 0)
    {
        *dst++ = *src++;
    }
}

void memset_u8(u8 *addr, u8 pattern, u32 sz)
{
	u32 i;
    ASSERT_LVL(COM, ASSERT_LVL0, addr!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, sz!=0);

    for(i = 0 ; i < sz; i ++)
    {
        addr[i] = pattern;
    }
}


void memset_u16(u16 *addr, u16 pattern, u32 sz)
{
    ASSERT_LVL(COM, ASSERT_LVL0, addr!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, sz!=0);
    ASSERT_LVL(COM, ASSERT_LVL0, 0 == ((u32)addr & 0x01));

    while( sz-- > 0)
    {
        *addr++ = pattern;
    }
}

void memset_u32(u32 *addr, u32 pattern, u32 sz)
{
	u32 i;
    ASSERT_LVL(COM, ASSERT_LVL0, addr!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, sz!=0);
    ASSERT_LVL(COM, ASSERT_LVL0, 0 == ((u32)addr & 0x03));

    for(i = 0 ; i < sz; i ++)
    {
        *(addr+ i) = pattern;
    }
}


bool memcmp_u8(u8 *str1, u8 *str2, u32 len)
{
	ASSERT_LVL(COM, ASSERT_LVL0, str1!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, str2!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, len!=0);

    while(len--)
    {
        if(*str1++ != *str2++)
        {
            return 1;
        }
    }

	//same return 0
    return 0;
}


bool memcmp_u16(u16 *str1, u16 *str2, u32 len)
{
    ASSERT_LVL(COM, ASSERT_LVL0, str1!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, str2!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, len!=0);

    while(len--)
    {
        if(*str1++ != *str2++)
        {
            return FALSE;
        }
    }

    return TRUE;
}

bool memcmp_u32(u32 *str1, u32 *str2, u32 len)
{
    ASSERT_LVL(COM, ASSERT_LVL0, str1!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, str2!=NULL);
	ASSERT_LVL(COM, ASSERT_LVL0, len!=0);

    while(len--)
    {
        if(*str1++ != *str2++)
        {
            return FALSE;
        }
    }

    return TRUE;
}

u32 str_len(u8 * s)
{
	const u8 *sc;

	ASSERT_LVL(COM, ASSERT_LVL0, NULL != s );

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return (u32)(sc - s);
}

