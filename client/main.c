#include<stdio.h>  
#include<string.h>  
#include<stdlib.h>  

#include<errno.h>  
#include<unistd.h>  

#include "libevent.h"

extern struct bufferevent* g_bev;

int main(int argc, char** argv)  
{  
    if( argc < 3 )  
    {  
        printf("please input 2 parameter\n");  
        return -1;  
    }  
  
  
    //�������������Ƿ������˵�IP��ַ���˿ں�  
    int sockfd = tcp_connect_server(argv[1], atoi(argv[2]));  
    if( sockfd == -1)  
    {  
        perror("tcp_connect error ");  
        return -1;  
    }  
  
    printf("connect to server successful\n");  
  
    struct event_base* base = event_base_new();  
  
    struct bufferevent* bev = bufferevent_socket_new(base, sockfd,  
                                                     BEV_OPT_CLOSE_ON_FREE);  
    //�����ն������¼�  
    struct event* ev_cmd = event_new(base, STDIN_FILENO,  
                                      EV_READ | EV_PERSIST, cmd_msg_cb,  
                                      (void*)bev);  
    event_add(ev_cmd, NULL);  

    //��socket�ر�ʱ���õ��ص�����  
    bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void*)ev_cmd);  
    
    bufferevent_enable(bev, EV_READ | EV_PERSIST);  
  
    g_bev = bev;
    event_base_dispatch(base);

    
  
    printf("finished \n");  
    return 0;  
}  
