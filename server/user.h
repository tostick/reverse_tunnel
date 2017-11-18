#ifndef _USER_H_
#define _USER_H_


UINT1  init_user();

UINT1  add_user(int fd, struct bufferevent* bev, char *passwd);
UINT1  del_user(int fd);

UINT1 send_order_request(int fd, char *msg);

UINT1 process_client_info(int fd, char *msg);


#endif
