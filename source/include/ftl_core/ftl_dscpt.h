#ifndef _FTL_DSCPT_H_
#define _FTL_DSCPT_H_

enum
{
    mREAD_SEND_CMD = 0,
    mREAD_RANDON_CMD,
    mREAD_START_DMA,
    mREAD_WAIT_DMA,

    mREAD_MUL_PLANE,


    mERASE_CMD,
    mERASE_MUL_CMD,

    mWRITE_CMD,
    mWRITE_START_DMA,
    mWRITE_WAIT_DMA,
    mWRITE_MUL_CMD,
    mWRITE_CMD_DONE
};


#endif
