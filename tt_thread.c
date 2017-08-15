#include "tt_handle.h"
#include "tt_config.h"

void* pthread_handle_network()
{
	int fd = init_socket();
	int ret = 0;
	
	RECONN:
	fd = init_socket();
	sock_connect();
	struct _st_tt *config = get_config();
	config->sock_fd = fd;
	struct _st_flags *flags = &config->flags;
	
	while(1) {
		if( !flags->is_login && 
			flags->is_getid  && 
			flags->is_getname && 
			flags->is_getver )
		{
			sock_send(fd, "login", sequence++);
		}
		ret = sock_loop_handle(fd, network_handle);// timeout;heartbeat;cmd;response;
	}
	close(fd);
	config->sock_fd = -1;
	goto RECONN;
}

void* pthread_handle_tty()
{
	RECONN:
	int fd = init_tty();
	struct _st_tt *config = get_config();
	struct _st_flags *flags = &config->flags;
	config->tty_fd = fd;
	
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
}

