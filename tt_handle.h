#ifndef TT_HANDLE_H
#define TT_HANDLE_H
#include <stddef.h>

void init_hand_ary(int def_cmd, (*def_handle)(char*, size_t));

void register_func(int cmd, (*handle)(char*, size_t));
void unregister_func(int cmd, (*handle)(char*, size_t));

void network_handle(int cmd, char *buf, size_t length);
void tty_handle(int cmd, char *buf, size_t length);

#endif
