#ifndef _LIBEVENT_H_
#define _LIBEVENT_H_

#include<event.h>  
#include<event2/bufferevent.h>  
#include<event2/buffer.h>  
#include<event2/util.h>  


void send_content(char *content);

int tcp_connect_server(const char* server_ip, int port);  
  
  
void cmd_msg_cb(int fd, short events, void* arg);  
void server_msg_cb(struct bufferevent* bev, void* arg);  
void event_cb(struct bufferevent *bev, short event, void *arg);  


#endif
