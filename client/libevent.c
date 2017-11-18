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

  
struct bufferevent* g_bev;

void send_content(char *content)
{
    assert(content);
    
    if (g_bev)
    {
        bufferevent_write(g_bev, content, strlen(content)); 
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
    char msg[1024] = {0}; 
    char buf[1024] = {0};  
  
    size_t len = bufferevent_read(bev, msg, sizeof(msg));  
    msg[len] = '\0';  
  
    printf("%s\n", msg);  

    if (!memcmp(msg, "user id:", 8) || !memcmp(msg, "connect to", 10))
    {
        return;
    }
    else if (!memcmp(msg, "recv", 4))
    {
        /* 判断文件是否存在 */
        // ??????
        /* 发送文件 */
        char *p_file = NULL;
        
        strtok(msg, " ");
        p_file = strtok(NULL," ");
        
        send_file(p_file);
        
        return;
    }
    
    FILE   *stream = NULL; 

    stream = popen( msg, "r" );
//    fread( buf, sizeof(char), sizeof(buf), stream);

    while (fgets(buf, sizeof(buf), stream) != NULL) 
    {
        bufferevent_write(bev, buf, strlen(buf));
        memset(buf, 0, sizeof(buf));
    }

    pclose( stream ); 
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
