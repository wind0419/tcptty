
#include "tt_proto.h" //LOGIN_BAK
#include "tt_net.h" //sock_send
#include "tt_handle.h" //register_func
#include "tt_type.h"

//glb_flags;
//glb_configs;
//glb_data;

void handle_local_timeout(char *buf, size_t len)
{
	config = get_config();
	if(config->is_login) {
		config->ping_t++;
		//send heartbeat
		sock_send(fd, "heartbeat", sequence++);
	} else {
		//will continue to login
	}
}

void handle_net_login_bak(char *buf, size_t len)
{
	config = get_config();
	config->is_login = true;
	if(config->status != E_DEV_TAKEUP) {
		config->status = E_DEV_ONLINE;
	}
}

void handle_net_heart_bak(char *buf, size_t len)
{
	config = get_config();
	config->ping_t = 0;
}

void handle_net_get_devstat(char *buf, size_t len)
{
	sock_send(fd, "device status", 1024);
}

void handle_net_takeup_dev(char *buf, size_t len)
{
	pro = (PRO_HD*)buf;
	if(0 == memcmp(config->taskid, pro->payload+32, 32)) {
		;
	} else {
		if(config->taskid == NULL) {
			memcpy(config->taskid, pro->payload+32, 32);
		} else {
			// device had takeuped
			sock_send(fd, "device had takeuped", 1024);
		}
	}
}

void handle_net_do_start(char *buf, size_t len)
{
	int sock_fd = config->sock_fd;
	int tty_fd = config->tty_fd;
	
	if(check_valid_taskid()) {	
		// send to uart
		ret = tty_send(tty_fd, payload, len);
	} else {
		// device is not correct;
		sock_send(sock_fd, "device current status", 1024);
	}
	//or
	push_msg("response",1024); // push to queue then will be send by tt_net.c
}

void handle_net_test_response(char *buf, size_t len)
{
	int sock_fd = config->sock_fd;
	pro = (PRO*)buf;
	pro->hd.stat = RRO_RSP;
	ret = sock_send(sock_fd, "device response", 1024);
}

void handle_net_cmd_stop(char *buf, size_t len)
{
	// send to uart
	tty_send(fd, buf, len);
}

/* another pthread */
void handle_tty_get_devid(char *buf, size_t len)
{
	config = get_config();
	// need to lock config ???
	pthread_mutex_lock(&config->mutex);
	config->is_getid = true;
	pthread_mutex_unlock(&config->mutex);
}

void handle_tty_status(char *buf, size_t len)
{
	// send to net
	sock_send(fd, "dev status", sequence++);
}


void default_handle(char *buf, size_t len)
{
	;
}

void register_def_all()
{
	init_hand_ary(UNUSED, default_handle);
}

void register_net_all()
{
	register_func(LOGIN_BAK, handle_net_login_bak);
	register_func(LOCAL_TIMEOUT, handle_local_timeout);
}

void register_tty_all()
{
	register_func(DEVID, handle_tty_get_devid);
	register_func(DEVNAME, handle_local_timeout);
}

