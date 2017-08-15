#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define MAX_HAND_NUM	32
#define UNUSED			(-1)

struct _st_hand_ary {
	int cmd;
	(*handle)(char*, size_t);
} glb_hand_ary[MAX_HAND_NUM];

void init_hand_ary(int def_cmd, (*def_handle)(char*, size_t))
{
	int i = 0;
	for(i=0; i < MAX_HAND_NUM; i++) {
		glb_hand_ary[i].cmd = def_cmd;
		glb_hand_ary[i].handle = def_handle;
	}
}

void tty_handle(int cmd, char *buf, size_t length)
{
	int i = 0;
	for(i=0; i < MAX_HAND_NUM; i++) {
		if(glb_hand_ary[i].cmd == cmd) {
			glb_hand_ary[i].handle(buf, length);
			break;
		}
	}
}

void network_handle(int cmd, char *buf, size_t length)
{
	int i = 0;
	for(i=0; i < MAX_HAND_NUM; i++) {
		if(glb_hand_ary[i].cmd == cmd) {
			glb_hand_ary[i].handle(buf, length);
			break;
		}
	}
}

void register_func(int cmd, (*handle)(char*, size_t))
{
	int i = 0;
	for(i=0; i < MAX_HAND_NUM; i++) {
		if(glb_hand_ary[i].cmd == UNUSED) {
			glb_hand_ary[i].cmd = cmd;
			glb_hand_ary[i].handle = handle;
			break;
		}
	}
}

void unregister_func(int cmd, (*handle)(char*, size_t))
{
	int i = 0;
	for(i=0; i < MAX_HAND_NUM; i++) {
		if(glb_hand_ary[i].cmd == cmd) {
			glb_hand_ary[i].cmd = UNUSED;
			glb_hand_ary[i].handle = default_handle;
			break;
		}
	}
}
