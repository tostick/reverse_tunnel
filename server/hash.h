#ifndef _HASH_H_
#define _HASH_H_

#define  HASH_SUCCESS  0
#define  HASH_FAILURE  1



#define Find_Hash_Node(key_id, hash_size) (key_id % (hash_size))


typedef struct st_hash  tHash;
struct st_hash
{
    int socket_fd;
    int session_id;
    int connect_fd;
    char passwd[8];

    int flag;

    struct bufferevent* bev;
    struct bufferevent* connect_bev;
    
    tHash   *next;
};

/********************************************************************
 * ��������init_hash
 * ������  ��ʼ��hash�ṹ
 * ��Σ�  tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ��HASH_SUCCESS - �ɹ� HASH_FAILURE - ʧ��
 ********************************************************************/
int init_hash(UINT4 tab_size);

/********************************************************************
 * ��������add_hash_node
 * ������  ����hash�ṹ
 * ��Σ�  session_id session��id�� tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ��HASH_SUCCESS - �ɹ� HASH_FAILURE - ʧ��
 ********************************************************************/
UINT1 add_hash_node(tHash info, UINT4 tab_size);


/********************************************************************
 * ��������delete_hash_node_by_id
 * ������  ����session_idɾ��hash�ڵ�
 * ��Σ�  session_id session��id�� tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ��HASH_SUCCESS - �ɹ� HASH_FAILURE - ʧ��
 ********************************************************************/
UINT1 delete_hash_node_by_id(UINT4 session_id, UINT4 tab_size);


/********************************************************************
 * ��������find_hash_node
 * ������  ����hash�ڵ�
 * ��Σ�  session_id session��id�� tab_size hash�ṹ�Ĵ�С
 * ��Σ�  ��
 * ����ֵ����NULL - �ɹ� NULL - ʧ��
 ********************************************************************/
tHash * find_hash_node(UINT4 session_id, UINT4 tab_size);


/********************************************************************
 * ��������free_hash_table
 * ������  �ͷ�̽��hash��
 * ��Σ�  tab_size    hash��Ĵ�С
 * ���Σ�  ��
 * ����ֵ����
 ********************************************************************/
void  free_hash_table(UINT4 tab_size);





#endif
