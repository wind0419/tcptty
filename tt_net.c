
#include "tt_handle.h"
#include "tt_proto.h"

int sock_loop_handle_ary(int fd, (*hand_ary[])(char*, size_t))
{
	while(1) {
		read(one_package);
		check_valid(cmd);
		
		for(hand_ary[i].cmd == cmd)
			hand_ary[i].handle(one_package, length);
	}
}

int socket_recv_tt()
{
	select(max,&rds,0,tv);
	//timeout
	// loop read length
	//handle signal
}

int switch_net2host()
{
	
}

/*only read and check valid
* Do NOT logic handle
*/
int sock_loop_handle(int fd, (*handle)(int, char*, size_t))
{
	/* malloc buffer for socket */
	unsigned char outhexbuf[ONE_TCP_MAX_LEN] = {0};
	PRO *pro = (PRO*)malloc(sizeof(PRO));
	memset(pro, 0, sizeof(PRO));
	pro->payload = (unsigned char*)malloc(ONLINE_MAX_LEN);
	memset(pro->payload, 0, (ONLINE_MAX_LEN));
	
	while(1) {
		memset(&pro->hd, 0, sizeof(PRO_HD));
		memset(pro->payload, 0, (ONLINE_MAX_LEN));
		ret = socket_recv_tt(socket, pro, tt_ms);
		if(ret == 0) {
			// timeout
			handle(LOCAL, pro, ret);
		} else if(ret < 0) {
			// socket error
			goto EXIT_ERR;
		} else if( (ret != sizeof(PRO_HD)) && 
				   (ret != sizeof(PRO_HD)+pro->hd.len) ) {
			debug(LOG_NOTICE, "Recv Length %d Error!\n",ret);
			continue;
		} else {
			handle(pro->hd.cmd, one_package, length);
		}
	}
EXIT_ERR:
	debug(LOG_NOTICE, "<--- Socket recv out!\n");
	if(pro && pro->payload) {
		free(pro->payload);
		free(pro);
	}
	return -1;
}
