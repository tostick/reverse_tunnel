#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>  
#include<errno.h>  
#include<unistd.h>  
  
#include<stdio.h>  
#include<string.h>  
#include<stdlib.h>  
#include <assert.h>
#include "libevent.h"
#include "file.h"

struct bufferevent* g_bev = NULL;

extern int g_file_flag;

void send_msg(char *action, char *msg)
{
    assert(action && msg);
    
    if (g_bev)
    {
        char order[1024] = {0};

        sprintf(order, "%s %s", action, msg);
        bufferevent_write(g_bev, order, strlen(order)); 
    }
    else
    {
        printf("disconnect!\n");
    }
}


void cmd_msg_cb(int fd, short events, void* arg)  
{  
#if 0
    char msg[1024];  
  
    int ret = read(fd, msg, sizeof(msg));  
    if( ret < 0 )  
    {  
        perror("read fail ");  
        exit(1);  
    }  
  
    struct bufferevent* bev = (struct bufferevent*)arg;  
  
    //把终端的消息发送给服务器端  
    bufferevent_write(bev, msg, ret);
#endif    
}  
  
  
void server_msg_cb(struct bufferevent* bev, void* arg)  
{  
    char msg[1024+1];  

    do
    {
        memset(msg, 0, sizeof(msg));
        bufferevent_read(bev, msg, 1024);

        if (1 == g_file_flag)
        {
            process_recv_file(msg);
        }
        else
        {
            printf("%s\n", msg);
        }
        
    } while (0 != msg[0]);
}  
  
  
void event_cb(struct bufferevent *bev, short event, void *arg)  
{  
  
    if (event & BEV_EVENT_EOF)  
        printf("connection closed\n");  
    else if (event & BEV_EVENT_ERROR)  
        printf("some other error\n");  
  
    //这将自动close套接字和free读写缓冲区  
    bufferevent_free(bev);  
  
    struct event *ev = (struct event*)arg;  
    //因为socket已经没有，所以这个event也没有存在的必要了  
    event_free(ev);  
}  
  
  
typedef struct sockaddr SA;  
int tcp_connect_server(const char* server_ip, int port)  
{  
    int sockfd, status, save_errno;  
    struct sockaddr_in server_addr;  
  
    memset(&server_addr, 0, sizeof(server_addr) );  
  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(port);  
    status = inet_aton(server_ip, &server_addr.sin_addr);  
  
    if( status == 0 ) //the server_ip is not valid value  
    {  
        errno = EINVAL;  
        return -1;  
    }  
  
    sockfd = socket(PF_INET, SOCK_STREAM, 0);  
    if( sockfd == -1 )  
        return sockfd;  
  
  
    status = connect(sockfd, (SA*)&server_addr, sizeof(server_addr) );  
  
    if( status == -1 )  
    {  
        save_errno = errno;  
        close(sockfd);  
        errno = save_errno; //the close may be error  
        return -1;  
    }  
  
    evutil_make_socket_nonblocking(sockfd);  
  
    return sockfd;  
}
