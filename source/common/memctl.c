
#include <global_def.h>
#include <memctl.h>

typedef struct
{
    struct list_head   node;
    u32         sram_addr;      //
    u32         len;            //unit is sector
}t_mem_ctl;

#define MAX_MEM_ALLOC_CNT   (100)
#define TOTAL_MEM_SIZE      (128) //unit is sector
#define SRAM_START_ADDR     (0x20000000)

#define SRAM_ADDR           (0x02)
#define DRAM_ADDR           (0x04)
#define HOST_ADDR           (0x08)

#define MEM_ADDR_BIT        (4)
#define MEM_ADDR_OFST       (28)
#define MEM_ADDR_MSK        ((1 << MEM_ADDR_OFST) - 1)
static t_mem_ctl mem_mng[MAX_MEM_ALLOC_CNT];
struct list_head mem_src_list;
struct list_head mem_free_list;
struct list_head mem_alloced_list;

static u32     src_node_cnt;
static u32     free_node_cnt;
static u32     alloc_node_cnt;
#if 1
#define mem_add_src_node(node, src_list)        list_add_tail((node) , (src_list));\
                                                src_node_cnt++;
#define mem_add_free_node(node, free_list)       list_add_tail((node) , (free_list));\
                                                free_node_cnt++;
#define mem_add_alloc_node(node, alloc_list)      list_add_tail((node) , (alloc_list));\
                                                alloc_node_cnt++;

/*******************************************************************************
*function name:mem_init
*
*description  :init memctl module
*
*parameter    :none
*
*return value :none
*
*revise       :Vito created at:2016-06-24
*******************************************************************************/
void mem_init(void)
{
    u32 idx;
    t_mem_ctl *mem_node = &mem_mng[0];
    //Link all node
    INIT_LIST_HEAD(&mem_src_list);
    INIT_LIST_HEAD(&mem_free_list);
    INIT_LIST_HEAD(&mem_alloced_list);
    src_node_cnt    = 0;
    free_node_cnt   = 0;
    alloc_node_cnt  = 0;
    mem_node->sram_addr     = SRAM_START_ADDR;
    mem_node->len           = TOTAL_MEM_SIZE * LBA_BYTE_CNT;

    mem_add_src_node(&mem_node->node , &mem_src_list);
    
    for(idx = 1; idx < MAX_MEM_ALLOC_CNT; idx++)
    {
        mem_add_free_node(&(mem_mng[idx].node), &mem_free_list);
    }
}

/*******************************************************************************
*function name:malloc
*
*description  :alloc memory for other module
*
*parameter    :@size , the size of memory that other module need
*
*return value :void *p,if alloc success,return the point of buffer 
*              if alloc fail,return NULL.
*              User who alloc memory should check the point value of malloc function
*
*revise       :Vito created at:2016-06-24
*******************************************************************************/
void *malloc(u32 size, bool f_force_alloc_sram) //unit is byte
{
    t_mem_ctl *list_node;
    t_mem_ctl *free_node;
    void *p = NULL;
    if(size == 0)
    {
        DBG_MESSAGE("Para Err\n");
        ASSERT_LVL(MEM,ASSERT_LVL0,0);
        return p;
    }
    if(src_node_cnt == 0)
    {
        DBG_MESSAGE("No src Node\n");
        ASSERT_LVL(MEM,ASSERT_LVL0,0);
        return p;
    }
    //check src list 
    if((size < 0x1000) || (TRUE == f_force_alloc_sram))
    {
        list_for_each_entry_reverse(t_mem_ctl, list_node, &mem_src_list, node)
        {
            if(list_node->len >= size)//sram enough
            {
                if(free_node_cnt > 0)
                {
                    free_node = list_first_entry(&mem_free_list,t_mem_ctl,node);
                    list_del(&free_node->node);
                    free_node_cnt--;
                    free_node->len          = size;
                    free_node->sram_addr    = list_node->sram_addr + list_node->len - size;
                    mem_add_alloc_node(&free_node->node, &mem_alloced_list);
                        
                    list_node->len          -= size;
                    //list_node->sram_addr    += size; 
                    if(0 == list_node->len)
                    {
                        list_del(&list_node->node);
                        src_node_cnt--;
                        mem_add_free_node(&list_node->node, &mem_free_list);
                    }
                    p =  (u32 *)free_node->sram_addr;
                }
                break;
            }
        }
    }
    else
    {
        list_for_each_entry(t_mem_ctl, list_node, &mem_src_list, node)
        {
            if(list_node->len >= size)//sram enough
            {
                if(free_node_cnt > 0)
                {
                    free_node = list_first_entry(&mem_free_list,t_mem_ctl,node);
                    list_del(&free_node->node);
                    free_node_cnt--;
                    free_node->len          = size;
                    free_node->sram_addr    = list_node->sram_addr;
                    mem_add_alloc_node(&free_node->node, &mem_alloced_list);
                        
                    list_node->len          -= size;
                    list_node->sram_addr    += size; 
                    if(0 == list_node->len)
                    {
                        list_del(&list_node->node);
                        src_node_cnt--;
                        mem_add_free_node(&list_node->node, &mem_free_list);
                    }
                    p =  (u32 *)free_node->sram_addr;
                }
                break;
            }
        }
    }
    if(p != NULL)
    {
        DBG_MESSAGE("Alloc Success:%x Len:%x\n", (u32)p, size);
    }
    else
    {
        DBG_MESSAGE("Alloc Fail\n");
        ASSERT_LVL(MEM,ASSERT_LVL0,0);
    }
    return p;
}

/*******************************************************************************
*function name:free
*
*description  :free the memory that had beed alloced by malloc function
*
*parameter    :@*p: the start address of memory which is going to be freed.
*
*return value : mFREE_SUCCESS,       free success
*               mPOINT_NOT_EXIST,    point not exist
*               mPARA_ERR            point is invalid
*
*revise       :Vito created at:2016-06-24
*******************************************************************************/
e_free_result free(void *p)
{
    //check p
    t_mem_ctl *alloc_node;
    t_mem_ctl *src_node;
    t_mem_ctl *pre_node;
    t_mem_ctl *nxt_node;
    DBG_MESSAGE("free point:%8x\n", (u32)p);
    if(NULL == p)
    {
        DBG_MESSAGE("Free Para Err\n");
        ASSERT_LVL(MEM,ASSERT_LVL0,0);
        return mPARA_ERR;
    }

    if(alloc_node_cnt == 0)
    {
        DBG_MESSAGE("No Alloc Node\n");
        ASSERT_LVL(MEM,ASSERT_LVL0,0);
        return mPOINT_NOT_EXIST;
    }
    list_for_each_entry_reverse(t_mem_ctl, alloc_node, &mem_alloced_list, node)
    {
        if(alloc_node->sram_addr == (u32)p) //matched
        {
            list_del(&alloc_node->node);
            alloc_node_cnt--;
            list_for_each_entry(t_mem_ctl, src_node, &mem_src_list, node)
            {
                if((src_node->sram_addr+src_node->len) == (u32)p)
                {
                    src_node->len += alloc_node->len;
                    mem_add_free_node(&alloc_node->node,&mem_free_list);
                    DBG_MESSAGE("Free Success0:%x Len:%x\n", src_node->sram_addr, src_node->len);
                    nxt_node = list_first_entry(&src_node->node,t_mem_ctl,node);
                    if((src_node->sram_addr+src_node->len) == (nxt_node->sram_addr))
                    {
                        src_node->len += nxt_node->len;
                        list_del(&nxt_node->node);
                        src_node_cnt--;
                        mem_add_free_node(&nxt_node->node, &mem_free_list);
                        DBG_MESSAGE("Continue:%x len:%x\n", src_node->sram_addr, src_node->len);
                    }
                    return mFREE_SUCCESS;
                }
                else if((alloc_node->sram_addr+alloc_node->len) == src_node->sram_addr)
                {
                    src_node->len       += alloc_node->len;
                    src_node->sram_addr =  alloc_node->sram_addr;
                    mem_add_free_node(&alloc_node->node,&mem_free_list);
                    DBG_MESSAGE("Free Success1:%x Len:%x\n", src_node->sram_addr, src_node->len);
                    
                    return mFREE_SUCCESS;
                }
                else if(alloc_node->sram_addr < src_node->sram_addr)
                {//insert node
                    pre_node = list_last_entry(&src_node->node,t_mem_ctl,node);
                    list_add(&alloc_node->node,&pre_node->node);
                    src_node_cnt++;
                    DBG_MESSAGE("Free Success2:%x Len:%x\n", alloc_node->sram_addr, alloc_node->len);
                    return mFREE_SUCCESS;
                }
            }
            DBG_MESSAGE("Free Success3:%x Len:%x\n", alloc_node->sram_addr, alloc_node->len);
            mem_add_src_node(&alloc_node->node,&mem_src_list);
            return mFREE_SUCCESS;
        }
    }
    DBG_MESSAGE("Point %x Not Alloced\n", p);
    ASSERT_LVL(MEM,ASSERT_LVL0,0);
    return mPOINT_NOT_EXIST;
    //UART_MESSAGE(0);
}

u32 mem_get_dwrd(u32 d_ram_start_addr, u16 w_ofst)
{
    u32 *p_addr = (u32 *)d_ram_start_addr;
    
    switch(d_ram_start_addr >> MEM_ADDR_OFST)
    {
        case SRAM_ADDR:
            return p_addr[w_ofst];
        case DRAM_ADDR: //reserved for DRAM case
            break;
        case HOST_ADDR: //reserved for HMB case
            break;
        default:
            while(1);
    }
		while(1);
}

void mem_set_dwrd(u32 d_ram_start_addr, u16 w_ofst, u32 d_val)
{
    u32 *p_addr = (u32 *)d_ram_start_addr;
    
    switch(d_ram_start_addr >> MEM_ADDR_OFST)
    {
        case SRAM_ADDR:
            p_addr[w_ofst] = d_val;
        case DRAM_ADDR:
            break;
        case HOST_ADDR:
            break;
        default:
            while(1);
    }
}

//unit test code to test module MemCtl.c
void test_mem_ctl_func(void)
{
    u32 i, j, k;
    void *p[50];
    mem_init();
    j = 0;
    for(i = 1; i < 100; i+=i)
    {
        p[j] = malloc(i << LBA_BYTE_EXP, FALSE);
        //DBG_MESSAGE("alloc buff:%x , size:%d\n", (u32)p[j], i);
        j++;
    }

    for(k = 0; k <= (j >> 1); k++)
    {
        free(p[k]);
        //DBG_MESSAGE("free buff:%x , size:%d\n", (u32)p[j], i);
    }

    for(i = 1; i < 100; i+=i)
    {
        p[j] = malloc(i << LBA_BYTE_EXP, TRUE);
        //DBG_MESSAGE("alloc buff:%x , size:%d\n", (u32)p[j], i);
        j++;
    }

    for(k = 0; k <= j; k++)
    {
        free(p[k]);
        //DBG_MESSAGE("free buff:%x , size:%d\n", (u32)p[j], i);
    }
    
    
    
    #if 0
    for(i = 1; i < 100; i+=i)
    {
        p[j] = malloc(i << LBA_BYTE_EXP);
        //DBG_MESSAGE("alloc buff:%x , size:%d\n", (u32)p[j], i);
        j++;
    }
    k = j - 1;
    for(j = 0; j < k; j++)
    {
        free(p[j]);
        //DBG_MESSAGE("free buff:%x , size:%d\n", (u32)p[j], i);
    }

    j = 0;
    for(i = 1; i < 100; i+=i)
    {
        p[j] = malloc(i << LBA_BYTE_EXP);
        //DBG_MESSAGE("alloc buff:%x , size:%d\n", (u32)p[j], i);
        j++;
    }

    for(; j > 0; j--)
    {
        free(p[j-1]);
        //DBG_MESSAGE("free buff:%x , size:%d\n", (u32)p[j], i);
    }

    j = 0;
    for(i = 1; i < 100; i+=i)
    {
        p[j] = malloc(i << LBA_BYTE_EXP);
        //DBG_MESSAGE("alloc buff:%x , size:%d\n", (u32)p[j], i);
        j++;
    }

    k = j;
    //j = k >> 1;
    for(; j >= (k >> 1); j--)
    {
        free(p[j-1]);
        //DBG_MESSAGE("free buff:%x , size:%d\n", (u32)p[j], i);
    }

    k = j;
    j = k >> 1;
    for(j = 0; j <= (k >> 1); j++)
    {
        free(p[j]);
        //DBG_MESSAGE("free buff:%x , size:%d\n", (u32)p[j], i);
    }
    #endif
}
#endif
