#ifndef TT_PROTO_H
#define TT_PROTO_H

#include "tt_type.h"

#define DEV_ID_LEN			(32)
#define DEV_NAME_LEN		(32)
#define DEV_DATA_MAX_LEN	(1024)

enum _tty_list {
	T_CMD_INIT = 0,
	T_CMD1,
	T_CMD2,
	
	T_CMD_MAX,
};

enum _svr_cmd_list {
	S_CMD_INIT = 0,
	S_CMD1,
	S_CMD2,
	
	S_CMD_MAX,
};

#define PACK(n) __attribute__((aligned(n),packed))

#define IN
#define OUT
#define MAX_PAYLEN	(65536)
#define ONLINE_MAX_LEN	(1024)
#define ONE_TCP_MAX_LEN	(1400)

typedef struct _st_pro_hd {
	ushort header;
	ushort len;
	uchar ver;
	ushort cmd;
	uchar stat;
	uint seq;
	uint reserve;
}PACK(1) PRO_HD;

typedef struct _st_pro {
	PRO_HD hd;
	uchar *payload;
	//ushort crc;
}PACK(1) PRO;

void pro_net2host(PRO_HD *phd);

/*
 * switch PRO_HD struct to hexbuf;
 * Then switch to Net-Endian (Big Endian)
 * You can do send(socket,hex,len) directly.
 * length of hex = sizeof(PRO_HD)
*/
void pro_hd2hexbuf(IN PRO_HD *phd, OUT uchar *hex);

/*
 * switch PRO struct to hexbuf;
 * Then switch to Net-Endian (Big Endian)
 * You can do send(socket,hex,len) directly.
 * @len: length of all hexbuf
*/
void pro_pro2hexbuf(IN PRO *pro, OUT uchar *hex, OUT int *len);

/*
 * switch hexbuf to PRO_HD struct;
 * Then switch to Host-Endian (Mips-7688 is Little Endian)
 * You can access the value by phd->xx directly.
*/
void pro_hex2hd(IN uchar *hex, OUT PRO_HD *phd);

/*
 * switch hexbuf to PRO struct;
 * Then switch to Host-Endian (Mips-7688 is Little Endian)
 * You can access the value by pro->xx directly.
 * NOTE: this will malloc for pro->payload which len is pro->hd.len
 * After all you must do free(pro->payload)
 * Before this you must malloc for pro at function outside
*/
void pro_hex2pro(IN uchar *hex, OUT PRO *pro);

void dump_phd(PRO_HD *phd);
void dump_pro(PRO *pro);
#endif
