#include "tt_handle.h"
#include "tt_config.h"
#include "tt_callback.h"
	
void* pthread_handle_network()
{
	int fd = 0;
	int ret = 0;
	struct _st_tt *config = NULL;
	struct _st_flags *flags = NULL;
	
	register_net_all();
	
	while(1) {
		config = get_config();
		char *ip = config->net.ipaddr;
		int port = config->net.port;
		int nonblk = config->net.nonblk;
		int timeout = config->net.timeout;
		
		fd = sock_connect(ip, port, nonblk);
		if(fd < 0) {
			sleep(10);
			continue;
		}
		
		config->sock_fd = fd;
		flags = &config->flags;
		
		while(1) {
			if( !flags->is_login && 
				flags->is_getid  && 
				flags->is_getname && 
				flags->is_getver )
			{
				sock_send(fd, "login", 1024);
			}
			// timeout;heartbeat;cmd;response;
			ret = sock_loop_handle(fd, network_handle);
		}
		close(fd);
		config->sock_fd = -1;
	}
	
	return NULL;
}

void* pthread_handle_tty()
{
	RECONN:
	int fd = init_tty();
	struct _st_tt *config = get_config();
	struct _st_flags *flags = &config->flags;
	config->tty_fd = fd;
	
	register_tty_all();
	
	while(1) {
		if(false == flags->is_getid) {
			tty_send(fd, "get device id", sequence++);
		} else if(false == flags->is_getname) {
			tty_send(fd, "get device name", sequence++);
		} else if(false == flags->is_getver) {
			tty_send(fd, "get device version", sequence++);
		}
		tty_loop_read(fd, tty_handle);//devid;devname;devstatus;error;response;
	}
	close(fd);
	config->tty_fd = -1;
	goto RECONN;
	
	return NULL;
}

