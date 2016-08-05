#ifndef _S_LIST_H_
#define _S_LIST_H_


typedef struct t_s_list{
    struct t_s_list *next;
}s_list;

#define s_list_entry(ptr, type, member)     container_of(ptr, type, member)  
static INLINE void s_list_init_head(s_list *list)
{
    list->next = NULL;
}

static INLINE void s_list_add(s_list *list_head, s_list *new)
{
    new->next = list_head->next;
    list_head->next = new;
}

static INLINE s_list *s_list_first_entry(s_list *list_head)
{
    return list_head->next;
}

static INLINE s_list *s_list_get_entry(s_list *list_head)
{
    s_list *node = list_head->next;
    if(node)
        list_head->next = list_head->next->next;
    return node;
}
#endif
