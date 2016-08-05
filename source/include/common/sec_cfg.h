/*******************************************************************************
*Filename:<SEC_CFG.H>
*
*Brief   :Configuation ARM code and data section
*
*Owner   :vito first created at:2016-06-29
*
*Revision:V1.0
*******************************************************************************/
#ifndef __SEC_CFG_H__
#define __SEC_CFG_H__

#pragma anon_unions            /* ARM CC feature ,can compile anonymous union */

#define SECTION_CFG mENABLE

#if(mENABLE == SECTION_CFG)
#define ALIGN(x)    __attribute__( (aligned(x)))

#define BTALIGN ALIGN(1)    /* byte  aligned */
#define WRALIGN ALIGN(2)    /* word  aligned */
#define DWALIGN ALIGN(4)    /* dword aligned */
#define STCHK  __attribute__((section("STCHK"),zero_init))   /* ROM   code */

#define RCODE  __attribute__((section("RCODE")))   /* ROM   code */
#define ICODE  __attribute__((section("ICODE")))   /* ISRAM code */
#define SCODE  __attribute__((section("SCODE")))   /* SRAM  code */

#define RDATA  __attribute__((section("RDATA")))   /* ROM   data */
#define IDATA  __attribute__((section("IDATA")))   /* ISRAM data */
#define SDATA  __attribute__((section("SDATA")))   /* SRAM  data */

#define RZERO  __attribute__((section("RZERO"), zero_init)) /* ROM   zero data*/
#define IZERO  __attribute__((section("IZERO"), zero_init)) /* ISRAM zero data*/
#define SZERO  __attribute__((section("SZERO"), zero_init)) /* SRAM  zero data*/

#pragma import (__use_no_heap) /* not use memory heap */
#else   /* (mENABLE == SECTION_CFG) */
#define ALIGN(x)
#define BTALIGN
#define WRALIGN
#define DWALIGN

#define RCODE
#define ICODE
#define SCODE

#define RDATA
#define IDATA
#define SDATA

#define RZERO
#define IZERO
#define SZERO

#endif  /* (mENABLE == SECTION_CFG) */

#endif  /* #ifndef __SEC_CFG_H__*/

