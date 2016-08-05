/*******************************************************************************
*Filename:<ftl_dscpt.c>
*
*Brief   :ftl descripter used to communicate ftl core and nf core
*
*Owner   :Vito first created at:2016-07-19
*
*Revision:V1.0
*******************************************************************************/
#include <global_def.h>

#define MAX_DESCPT_CNT      16
typedef struct
{
    u32 meta[4];
}t_nand_meta_type;

typedef struct
{
    struct list_head   node;
    u32                 d_ptu;
    u32                 *p_buff;
    u32                 d_bit_map;
    t_nand_meta_type    meta;
}t_ftl_dscrpt_type;

struct list_head read_cmd_list;
struct list_head write_cmd_list;
struct list_head erase_cmd_list;
struct list_head other_cmd_list;
//t_ftl_dscrpt_type t_ftl_dscrpt[mUSER_CNT][MAX_DESCPT_CNT];


void ftl_dscrpt_init(void)
{
    INIT_LIST_HEAD(&read_cmd_list);
    INIT_LIST_HEAD(&write_cmd_list);
    INIT_LIST_HEAD(&erase_cmd_list);
    INIT_LIST_HEAD(&other_cmd_list);
}

void ftl_set_descrpt(u8 b_type)
{
    
}
#if 1
void ftl_set_descrpt_write(u32 d_ptu, u32 *p_buff)
{
    t_ftl_dscrpt_type *p_ftl_dscrpt = (t_ftl_dscrpt_type *)malloc(sizeof(t_ftl_dscrpt_type), FALSE);

    p_ftl_dscrpt->d_ptu     = d_ptu;
    p_ftl_dscrpt->p_buff    = p_buff;

    list_add_tail(&p_ftl_dscrpt->node, &write_cmd_list);
    if(PLANE_ALIGNED(d_ptu))
    {
        //set wrtie cmd handle flag
    }
}

void ftl_set_descrpt_read(u32 d_ptu, u32 *p_buff)
{
    t_ftl_dscrpt_type *p_ftl_dscrpt = (t_ftl_dscrpt_type *)malloc(sizeof(t_ftl_dscrpt_type), FALSE);

    p_ftl_dscrpt->d_ptu     = d_ptu;
    p_ftl_dscrpt->p_buff    = p_buff;
    
    list_add_tail(&p_ftl_dscrpt->node, &read_cmd_list);
}

void ftl_set_descrpt_erase(u32 d_ptu)
{
    t_ftl_dscrpt_type *p_ftl_dscrpt = (t_ftl_dscrpt_type *)malloc(sizeof(t_ftl_dscrpt_type), FALSE);

    p_ftl_dscrpt->d_ptu     = d_ptu;
    
    list_add_tail(&p_ftl_dscrpt->node, &erase_cmd_list);
}

void ftl_set_descrpt_other(u32 d_ptu, u8 cmd_type)
{
    t_ftl_dscrpt_type *p_ftl_dscrpt = (t_ftl_dscrpt_type *)malloc(sizeof(t_ftl_dscrpt_type), FALSE);

    p_ftl_dscrpt->d_ptu     = d_ptu;
    p_ftl_dscrpt->p_buff    = (u32 *)cmd_type;
    list_add_tail(&p_ftl_dscrpt->node, &other_cmd_list);
}
#endif

//void nfc_cmd_handle(u8 b_state, )

