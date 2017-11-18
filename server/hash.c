#include <pthread.h>


#include "utils.h"
#include "hash.h"


/* hash表头 */
tHash     **g_hash_tab        = NULL;  
pthread_mutex_t   *g_hash_tab_mutex = NULL; 



/********************************************************************
 * 函数名：init_hash
 * 描述：  初始化hash结构
 * 入参：  tab_size hash结构的大小
 * 入参：  无
 * 返回值：HASH_SUCCESS - 成功 HASH_FAILURE - 失败
 ********************************************************************/
int init_hash(UINT4 tab_size)
{
	UINT1 ret = 0;
	UINT4 i   = 0;
    tHash  **pphash = NULL;


    /* 创建hash表 */
    pphash = (tHash **)malloc(tab_size * sizeof(tHash *));

    if (!pphash)
    {
        ERROR("malloc tab_size failed!\n");
        return HASH_FAILURE;
    }
    else
    {
        memset(pphash, 0, tab_size * sizeof(tHash *));
    }
	g_hash_tab = pphash;  

	g_hash_tab_mutex = (pthread_mutex_t *)malloc(tab_size * sizeof(pthread_mutex_t));

	for (i = 0; i < tab_size; i++)
	{
		ret = pthread_mutex_init(&g_hash_tab_mutex[i] , NULL);
	    if (HASH_SUCCESS != ret)
	    {
	        free_hash_table(tab_size);
	        ERROR("init init_hash failed\n");
			
	        return HASH_FAILURE;
	    }
	}  

    return HASH_SUCCESS;
}


/********************************************************************
 * 函数名：add_hash_node
 * 描述：  增加hash结构
 * 入参：  session_id session的id号 tab_size hash结构的大小
 * 入参：  无
 * 返回值：HASH_SUCCESS - 成功 HASH_FAILURE - 失败
 ********************************************************************/
UINT1 add_hash_node(tHash info, UINT4 tab_size)
{
    UINT4   index = 0;
    tHash *phash = NULL;
    tHash *pexist_hash = NULL;

    UINT4 session_id = info.session_id;

    /* 根据session_id来hash */
    index = Find_Hash_Node(session_id, tab_size);

    phash = (tHash *)malloc(sizeof(tHash));
    if (NULL == phash)
    {
        ERROR("malloc phash error\n");
        return HASH_FAILURE;
    }

    memcpy(phash, &info, sizeof(tHash));

    /* avoid point to inaccessible locations */
    phash->next = NULL;  
    
    if (phash->session_id > 0)
    {
        pthread_mutex_lock(&g_hash_tab_mutex[index]);
        pexist_hash = g_hash_tab[index];

        if (pexist_hash)
        {
            /* 该hash节点是否冲突 */
            while (pexist_hash)
            {
                /* 寻找是否有同一目标地址的节点 */
                if (pexist_hash->session_id == phash->session_id)
                {
                    break;
                }
                pexist_hash = pexist_hash->next;
            }

            if (pexist_hash)
            {
                /* 找到有同一目标地址的节点，则使用新的数据替换旧的数据 */
                phash->next = pexist_hash->next;
                memcpy(pexist_hash, phash, sizeof(struct st_hash));
                free(phash);
                phash = pexist_hash;
            }
            else
            {
                /* 将节点插入到冲突节点的头部 */
                phash->next	   = g_hash_tab[index];
                g_hash_tab[index]	   = phash;
            }
        }
        else
        {
            /* hash节点不冲突，则直接赋值 */
            phash->next	  = NULL;
            g_hash_tab[index] = phash;
        }

        pthread_mutex_unlock(&g_hash_tab_mutex[index]);
    }

    return HASH_SUCCESS;

}

/********************************************************************
 * 函数名：delete_hash_node_by_id
 * 描述：  根据session_id删除hash节点
 * 入参：  session_id session的id号 tab_size hash结构的大小
 * 入参：  无
 * 返回值：HASH_SUCCESS - 成功 HASH_FAILURE - 失败
 ********************************************************************/
UINT1 delete_hash_node_by_id(UINT4 session_id, UINT4 tab_size)
{
	UINT4		index = 0;
	tHash *phash = NULL;
	tHash *prev_hash = NULL;

	/* 根据session_id来hash */
	index = Find_Hash_Node(session_id, tab_size);

	if (NULL == g_hash_tab)
	{
		return HASH_FAILURE;
	}
	
	phash = g_hash_tab[index];
	if (phash->session_id == session_id)
	{
		prev_hash = phash;
		phash = phash->next;
		free(prev_hash);
		g_hash_tab[index] = phash;

		return HASH_SUCCESS;
	}

	prev_hash = phash;
	phash = phash->next;
	while (phash)
    {
        if (phash->session_id == session_id)
        {
        	prev_hash = phash->next;
			free(phash);
            return HASH_SUCCESS;
        }

		prev_hash = phash;
        phash = phash->next;
    }

	return HASH_FAILURE;

}

/********************************************************************
 * 函数名：find_hash_node
 * 描述：  查找hash节点
 * 入参：  session_id session的id号 tab_size hash结构的大小
 * 入参：  无
 * 返回值：HASH_SUCCESS - 成功 HASH_FAILURE - 失败
 ********************************************************************/
tHash * find_hash_node(UINT4 session_id, UINT4 tab_size)
{
	UINT4		index = 0;
	tHash *phash = NULL;

	/* 根据session_id来hash */
	index = Find_Hash_Node(session_id, tab_size);

	if (NULL == g_hash_tab)
	{
		return NULL;
	}
	
	phash = g_hash_tab[index];
	while (phash)
    {
        if (phash->session_id == session_id)
        {
            return phash;
        }
 
        phash = phash->next;
    }

	return NULL;

}


/********************************************************************
 * 函数名：free_hash_link_table
 * 描述：  释放探测hash 链表结构
 * 入参：  phash_tab       hash链表节点
 * 出参：  无
 * 返回值：无
 ********************************************************************/
static void free_hash_link_table(tHash *phash_tab)
{
    tHash *phash = NULL;
    tHash *phash_before = NULL;

	if (NULL == phash_tab)
	{
		return;
	}
	
    phash = phash_tab;
    while (phash->next)                
    {           
        phash_before = phash;
        phash = phash->next;

        free(phash_before);
    }

    free(phash);
}

/********************************************************************
 * 函数名：free_hash_table
 * 描述：  释放探测hash表
 * 入参：  tab_size    hash表的大小
 * 出参：  无
 * 返回值：无
 ********************************************************************/
void  free_hash_table(UINT4 tab_size)
{
	UINT4 i = 0;
	
	if (NULL != g_hash_tab)
    {
        for (i = 0; i < tab_size; i++)
        {
            free_hash_link_table(g_hash_tab[i]);
            g_hash_tab[i] = NULL;
        }
        
     
        if (g_hash_tab)
        {
            free(g_hash_tab);
            g_hash_tab = NULL;
        }
    }

    for (i = 0; i < tab_size; i++)
    {
        pthread_mutex_destroy(&g_hash_tab_mutex[i]);
    }
}


