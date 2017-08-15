#ifndef TT_NET_H
#define TT_NET_H
#include <stddef.h>

int sock_connect(char *ipaddr, int port, int nonblk);
int sock_close(int fd);
int sock_send(int fd, char *buffer, size_t length);
int sock_loop_read(int fd, char *outbuf, size_t *outlen);
//int sock_loop_handle_ary(int fd, (*hand_ary[])(char*, size_t));
int sock_loop_handle(int fd, (*handle)(int, char*, size_t));

#endif
