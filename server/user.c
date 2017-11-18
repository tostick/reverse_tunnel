#include "utils.h"
#include "hash.h"
#include "user.h"
#include "libevent.h"

#include <time.h>
#include <unistd.h>


extern tHash    **g_hash_tab;

void rand_password(char *passwd, int len)
{
    assert(passwd);
    
    int index = 0;
    int i = 0;
    char sequence[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!@#$%^&*.";

    srand(time(NULL));
    for (i = 0; i < len; i++)
    {
        index = rand()%72;
        passwd[i] = sequence[index];
    }
}


UINT1  init_user()
{

    init_hash(MAX_USER_NUM);

    return 0;
}

UINT1 add_user(int fd, struct bufferevent* bev, char *passwd)
{
    assert (bev && passwd);
    
    tHash user;

    memset(&user, 0, sizeof(user));
    user.flag = 1;
    user.socket_fd = fd;
    user.session_id = fd;
    user.bev        = bev;

    /* 随机产生密码 */
    rand_password(passwd, 6);
    
    memcpy(user.passwd, passwd, 6);
    
    add_hash_node(user, MAX_USER_NUM);

    return 0;
}

UINT1 del_user(int fd)
{
    tHash *phash = NULL;

    phash = find_hash_node(fd, MAX_USER_NUM);
    if (NULL == phash)
    {
        return 1;
    }
    
    if (phash->connect_bev)
    {
        send_msg(phash->connect_bev, "connection close");
        phash = find_hash_node(phash->connect_fd, MAX_USER_NUM);
        if (NULL != phash)
        {
            phash->connect_bev = NULL;
            phash->connect_fd = 0;
        }
    }

    delete_hash_node_by_id(fd, MAX_USER_NUM);

    return 0;
}

UINT1 send_order_request(int fd, char *msg)
{
    tHash *phash = NULL;

    phash = find_hash_node(fd, MAX_USER_NUM);

    if (NULL == phash)
    {
        ERROR("not find node(id:%d)\n", fd);

        return 1;
    }
    
    send_msg(phash->bev, "hello cc");

    return 0;
}

void command_parse()
{

}

UINT1 process_client_info(int fd, char *msg)
{
    assert(NULL != msg);

    tHash *phash = NULL;
    char info[256] = {0}; 

    phash = find_hash_node(fd, MAX_USER_NUM);
    if (NULL == phash)
    {
        ERROR("not find node(id:%d)\n", fd);

        return FAILURE;
    }
    
    /* 判断请求来自client还是teminal */
    if (0 == memcmp(msg, "connect", 7))
    {
        tHash *t_phash = NULL;

        if (0 != phash->connect_fd)
        {
            sprintf(info, "connect exist already(id:%d)", phash->connect_fd);
            ERROR("%s\n", info);
            send_msg(phash->bev, info);

            return FAILURE;
        }
        
        /* 获取id号 */
        char *p_id = NULL;
        char *p_passwd = NULL;
        int connect_id = 0;

        strtok(msg, " ");
        p_id = strtok(NULL," ");
        p_passwd = strtok(NULL," ");

        if (NULL == p_id)
        {
           sprintf(info, "connect id is NULL");
           goto end;
        }
        connect_id = atoi(p_id);
        
        if (NULL == p_passwd)
        {
           sprintf(info, "connect password is NULL");
           goto end;
        }

        t_phash = find_hash_node(connect_id, MAX_USER_NUM);
        if (NULL == t_phash)
        {
           sprintf(info, "not find node(id:%d)", connect_id);
           goto end;
        }
        if (0 != strcmp(p_passwd, t_phash->passwd))
        {
           sprintf(info, "password error(id:%d)\n", connect_id);
           goto end;
        }

        t_phash->connect_fd = fd;
        t_phash->connect_bev = phash->bev;
            
        phash->connect_fd = connect_id;
        phash->connect_bev = t_phash->bev;
        phash->flag = 1;
        send_msg(phash->bev, "connect success!!!");

        return SUCCESS;

end:
       ERROR("%s\n", info);
       send_msg(phash->bev, info);

       return FAILURE;
    }

    if (NULL != phash->connect_bev)
    {
        if (1 == phash->flag)
        {
            // command_parse(msg);
            send_msg(phash->connect_bev, msg);
        }
        else
        {
            send_msg(phash->connect_bev, msg);
        }
    }
    else
    {
        send_msg(phash->bev, "please build connect first");
    }
    
    return SUCCESS;
}

