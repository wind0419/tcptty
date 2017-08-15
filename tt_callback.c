#include "tt_proto.h" //LOGIN_BAK
#include "tt_net.h" //sock_send
#include "tt_handle.h" //register_func
#include "tt_type.h"
#include "tt_config.h"

/* network select timeout */
static void handle_local_timeout(char *buf, size_t len)
{
	struct _st_tt *config = get_config();
	if(config->is_login) {
		config->ping_t++;
		//send heartbeat
		sock_send(fd, "heartbeat", sequence++);
	} else {
		//will continue to login
	}
}

static void handle_net_cmd1(char *buf, size_t len)
{
	// handle at local
}

static void handle_net_cmd2(char *buf, size_t len)
{
	//send to uart
	struct _st_tt *config = get_config();
	int fd = config->tty_fd;
	tty_send(fd, "tcp cmd to uart", 1024);
}

static void handle_net_needresp(char *buf, size_t len)
{
	struct _st_tt *config = get_config();
	int fd = config->sock_fd;
	sock_send(fd, "response", 1024);
}

/* another pthread tty */
static void handle_tty_get_devid(char *buf, size_t len)
{
	struct _st_tt * config = get_config();
	// need to lock config ???
	LOCK_CONFIG();
	config->is_getid = true;
	UNLOCK_CONFIG();
}

static void handle_tty_status(char *buf, size_t len)
{
	struct _st_tt *config = get_config();
	int fd = config->sock_fd;
	// send to net
	sock_send(fd, "dev status", 1024);
}


static void default_handle(char *buf, size_t len)
{
	;
}

void register_def_all()
{
	init_hand_ary(UNUSED, default_handle);
}

void register_net_all()
{
	register_func(LOCAL_TIMEOUT, handle_local_timeout);
	register_func(S_CMD1, handle_net_cmd1);
	register_func(S_CMD2, handle_net_cmd2);
	//...
}

void register_tty_all()
{
	register_func(T_CMD1, handle_tty_get_devid);
	register_func(T_CMD2, handle_tty_status);
	//...
}

