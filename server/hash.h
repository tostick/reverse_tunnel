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
 * 函数名：init_hash
 * 描述：  初始化hash结构
 * 入参：  tab_size hash结构的大小
 * 入参：  无
 * 返回值：HASH_SUCCESS - 成功 HASH_FAILURE - 失败
 ********************************************************************/
int init_hash(UINT4 tab_size);

/********************************************************************
 * 函数名：add_hash_node
 * 描述：  增加hash结构
 * 入参：  session_id session的id号 tab_size hash结构的大小
 * 入参：  无
 * 返回值：HASH_SUCCESS - 成功 HASH_FAILURE - 失败
 ********************************************************************/
UINT1 add_hash_node(tHash info, UINT4 tab_size);


/********************************************************************
 * 函数名：delete_hash_node_by_id
 * 描述：  根据session_id删除hash节点
 * 入参：  session_id session的id号 tab_size hash结构的大小
 * 入参：  无
 * 返回值：HASH_SUCCESS - 成功 HASH_FAILURE - 失败
 ********************************************************************/
UINT1 delete_hash_node_by_id(UINT4 session_id, UINT4 tab_size);


/********************************************************************
 * 函数名：find_hash_node
 * 描述：  查找hash节点
 * 入参：  session_id session的id号 tab_size hash结构的大小
 * 入参：  无
 * 返回值：非NULL - 成功 NULL - 失败
 ********************************************************************/
tHash * find_hash_node(UINT4 session_id, UINT4 tab_size);


/********************************************************************
 * 函数名：free_hash_table
 * 描述：  释放探测hash表
 * 入参：  tab_size    hash表的大小
 * 出参：  无
 * 返回值：无
 ********************************************************************/
void  free_hash_table(UINT4 tab_size);





#endif
