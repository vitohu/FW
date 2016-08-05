#ifndef _MEMOP_H_
#define _MEMOP_H_

void memcpy_u8(u8 *dst, u8 *src, u32 sz);
void memcpy_u16(u16 *dst, u16 *src, u32 sz);
void memcpy_u32(u32 *dst, u32 *src, u32 sz);
void memset_u8(u8 *addr, u8 pattern, u32 sz);
void memset_u16(u16 *addr, u16 pattern, u32 sz);
void memset_u32(u32 *addr, u32 pattern, u32 sz);
bool memcmp_u8(u8 *str1, u8 *str2, u32 len);
bool memcmp_u16(u16 *str1, u16 *str2, u32 len);
bool memcmp_u32(u32 *str1, u32 *str2, u32 len);
u32 str_len(u8 * s);
#endif
