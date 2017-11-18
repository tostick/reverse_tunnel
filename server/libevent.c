#include<errno.h>  

#include "utils.h"
#include "user.h"
#include "libevent.h"


void send_msg(struct bufferevent* bev, char *msg)
{
    assert(NULL != bev);

    bufferevent_write(bev, msg, strlen(msg));  
}

void accept_cb(int fd, short events, void* arg)  
{  
    char passwd[7] = {0};
    
    evutil_socket_t sockfd;  
  
    struct sockaddr_in client;  
    socklen_t len = sizeof(client);  
  
    sockfd = accept(fd, (struct sockaddr*)&client, &len );  
    evutil_make_socket_nonblocking(sockfd);  
  
    printf("accept a client %d\n", sockfd);  

    struct event_base* base = (struct event_base*)arg;  
  
    struct bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);  
    bufferevent_setcb(bev, socket_read_cb, NULL, event_cb, arg);  
  
    bufferevent_enable(bev, EV_READ | EV_PERSIST);  

    add_user((int)sockfd, bev, passwd);

    char reply_msg[64] = {0};

    sprintf(reply_msg, "user id:%d, secret:%s", (int)sockfd, passwd);  
    bufferevent_write(bev, reply_msg, strlen(reply_msg));  
}  
  
  
  
void socket_read_cb(struct bufferevent* bev, void* arg)  
{  
    int  fd = 0;
    char msg[1024+1];  

    do
    {
        memset(msg, 0, sizeof(msg));
        bufferevent_read(bev, msg, 1024);  

        fd = event_get_signal(&bev->ev_read);
        process_client_info(fd, msg);
        printf("%s\n", msg);  
    } while (0 != msg[0]);
}  
  
  
  
void event_cb(struct bufferevent *bev, short event, void *arg)  
{  

    int fd;

    fd = event_get_signal(&bev->ev_read);
    
    if (event & BEV_EVENT_EOF)  
        printf("connection closed(fd:%d)\n", fd);  
    else if (event & BEV_EVENT_ERROR)  
        printf("some other error(fd:%d)\n", fd);  

    del_user(fd);
    
    //这将自动close套接字和free读写缓冲区  
    bufferevent_free(bev);  
}  
  
  
typedef struct sockaddr SA;  
int tcp_server_init(int port, int listen_num)  
{  
    int errno_save;  
    evutil_socket_t listener;  
  
    listener = socket(AF_INET, SOCK_STREAM, 0);  
    if( listener == -1 )  
        return -1;  
  
    //允许多次绑定同一个地址。要用在socket和bind之间  
    evutil_make_listen_socket_reuseable(listener);  
  
    struct sockaddr_in sin;  
    sin.sin_family = AF_INET;  
    sin.sin_addr.s_addr = 0;  
    sin.sin_port = htons(port);  
  
    if( bind(listener, (SA*)&sin, sizeof(sin)) < 0 )  
        goto error;  
  
    if( listen(listener, listen_num) < 0)  
        goto error;  
  
  
    //跨平台统一接口，将套接字设置为非阻塞状态  
    evutil_make_socket_nonblocking(listener);  
  
    return listener;  
  
    error:  
        errno_save = errno;  
        evutil_closesocket(listener);  
        errno = errno_save;  
  
        return -1;  
} 
