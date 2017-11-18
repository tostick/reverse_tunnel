#include <pthread.h>


#include "utils.h"
#include "hash.h"


/* hash��ͷ */
tHash     **g_hash_tab        = NULL;  
pthread_mutex_t   *g_hash_tab_mutex = NULL; 



/********************************************************************
 * ��������init_hash
 * ������  ��ʼ��hash�ṹ
 * ��Σ�  tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ��HASH_SUCCESS - �ɹ� HASH_FAILURE - ʧ��
 ********************************************************************/
int init_hash(UINT4 tab_size)
{
	UINT1 ret = 0;
	UINT4 i   = 0;
    tHash  **pphash = NULL;


    /* ����hash�� */
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
 * ��������add_hash_node
 * ������  ����hash�ṹ
 * ��Σ�  session_id session��id�� tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ��HASH_SUCCESS - �ɹ� HASH_FAILURE - ʧ��
 ********************************************************************/
UINT1 add_hash_node(tHash info, UINT4 tab_size)
{
    UINT4   index = 0;
    tHash *phash = NULL;
    tHash *pexist_hash = NULL;

    UINT4 session_id = info.session_id;

    /* ����session_id��hash */
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
            /* ��hash�ڵ��Ƿ��ͻ */
            while (pexist_hash)
            {
                /* Ѱ���Ƿ���ͬһĿ���ַ�Ľڵ� */
                if (pexist_hash->session_id == phash->session_id)
                {
                    break;
                }
                pexist_hash = pexist_hash->next;
            }

            if (pexist_hash)
            {
                /* �ҵ���ͬһĿ���ַ�Ľڵ㣬��ʹ���µ������滻�ɵ����� */
                phash->next = pexist_hash->next;
                memcpy(pexist_hash, phash, sizeof(struct st_hash));
                free(phash);
                phash = pexist_hash;
            }
            else
            {
                /* ���ڵ���뵽��ͻ�ڵ��ͷ�� */
                phash->next	   = g_hash_tab[index];
                g_hash_tab[index]	   = phash;
            }
        }
        else
        {
            /* hash�ڵ㲻��ͻ����ֱ�Ӹ�ֵ */
            phash->next	  = NULL;
            g_hash_tab[index] = phash;
        }

        pthread_mutex_unlock(&g_hash_tab_mutex[index]);
    }

    return HASH_SUCCESS;

}

/********************************************************************
 * ��������delete_hash_node_by_id
 * ������  ����session_idɾ��hash�ڵ�
 * ��Σ�  session_id session��id�� tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ��HASH_SUCCESS - �ɹ� HASH_FAILURE - ʧ��
 ********************************************************************/
UINT1 delete_hash_node_by_id(UINT4 session_id, UINT4 tab_size)
{
	UINT4		index = 0;
	tHash *phash = NULL;
	tHash *prev_hash = NULL;

	/* ����session_id��hash */
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
 * ��������find_hash_node
 * ������  ����hash�ڵ�
 * ��Σ�  session_id session��id�� tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ��HASH_SUCCESS - �ɹ� HASH_FAILURE - ʧ��
 ********************************************************************/
tHash * find_hash_node(UINT4 session_id, UINT4 tab_size)
{
	UINT4		index = 0;
	tHash *phash = NULL;

	/* ����session_id��hash */
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
 * ��������free_hash_link_table
 * ������  �ͷ�̽��hash ����ṹ
 * ��Σ�  phash_tab       hash����ڵ�
 * ���Σ�  ��
 * ����ֵ����
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
 * ��������free_hash_table
 * ������  �ͷ�̽��hash��
 * ��Σ�  tab_size    hash��Ĵ�С
 * ���Σ�  ��
 * ����ֵ����
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


