#ifndef _FTL_META_H_
#define _FTL_META_H_

//for 4k flash,     omly support multiplane binding
//8K flash,         only support 2 data ap point,AP_HOST and AP_GC
//16k flash,        support 3 data ap point, AP_HOST, AP_GC, AP_WL
//           in AP_HOST,C0 record AP_GC,    C1 record AP_WL
//           in AP_GC  ,C0 record AP_HOST,  C1 record AP_WL
//           in AP_WL  ,C0 record AP_HOST,  C1 record AP_GC
typedef union
{
    struct
    {
        u32  d_lpa0;    //28bit     //d0
        u32  d_lpa1;    //28bit     //d1
        u32  d_lpa2;    //28bit     //d2 
        u32  d_lpa3;    //28bit     //d3
        u32  d_ptu_C0;  //32bit     //d4
        u32  d_ver_C0;  //28bit     //d5
        u32  d_ptu_C1;  //32bit   //only valid in 16k logic page flash  //d6
        u32  d_ver_C1;  //28bit   //only valid in 16k logic page falsh  //d7
        u32  d_pre_p2l; //32bit     //d8
        u32  d_signature;
    }t_data;

    struct
    {
        u32 d_lpa0;
        u32 d_lpa1;
        u32 d_lpa2;
        u32 d_lpa3;
        u32 d_type0;
        u32 d_type1;
        u32 d_type2;
        u32 d_type3;
        u32 d_pre_srp;
        u32 d_signature;
    }t_ftl;
}t_meta_type;

#define LPA0_BASE_OFST      0
#define LPA1_BASE_OFST      1
#define LPA2_BASE_OFST      2
#define LPA3_BASE_OFST      3
#define PTUC0_BASE_OFST     4
#define VERC0_BASE_OFST     5
#define PTUC1_BASE_OFST     6
#define VERC1_BASE_OFST     7
#define PRE_P2L_BASE_OFST   8
#define PRE_SRP_BASE_OFST   8
#define SING_BASE_OFST      9
#endif
