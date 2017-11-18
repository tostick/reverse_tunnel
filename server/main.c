#include <pthread.h>

#include "utils.h"
#include "user.h"
#include "telnet.h"
#include "libevent.h"

static pthread_t agent_cli_pid;


int main(int argc, char **argv)
{
    int listener = 0;
    struct event_base* base = NULL;
    struct event* ev_listen = NULL;

    if (2 != argc)
    {
        printf("para error(%s port)\n", argv[0]);

        return 1;
    }
    
    if (pthread_create(&agent_cli_pid, NULL, (void *)thread_agent_cli, NULL))
    {
        printf("CLI thread ERR\n");
        return 1;
    } 

    // add libevent
    listener = tcp_server_init(atoi(argv[1]), MAX_USER_NUM);  
    if( listener == -1 )  
    {  
        perror(" tcp_server_init error ");  
        return -1;  
    }

    init_user();
  
    base = event_base_new();  
  
    /* 添加监听客户端请求连接事件 */    
    ev_listen = event_new(base, listener, EV_READ | EV_PERSIST,  
                                        accept_cb, base);  
    event_add(ev_listen, NULL);  
  
  
    event_base_dispatch(base);  
    event_base_free(base);  
  

    return 0;
}
