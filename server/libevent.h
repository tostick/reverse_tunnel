#ifndef _LIBEVENT_H_
#define _LIBEVENT_H_



int tcp_server_init(int port, int listen_num);  
void accept_cb(int fd, short events, void* arg);  
void socket_read_cb(struct bufferevent* bev, void* arg);  
void event_cb(struct bufferevent *bev, short event, void *arg);  

void send_msg(struct bufferevent* bev, char *msg);


#endif
