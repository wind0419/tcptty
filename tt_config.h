#ifndef TT_CONFIG_H
#define TT_CONFIG_H

struct _st_net {
	char ipaddr[16];
	int port;
	int timeout;
	bool nonblk;
};

struct _st_tty {
	char device[32];
	int speed;
	int timeout;
	bool nonblk;
};

struct _st_flags {
	uint32 ping_t;
	bool is_login;
	bool is_getid;
	bool is_getname;
	bool is_getver;
	bool is_started;
	bool is_paused;
	bool is_stoped;
	bool is_running;
};

struct _st_status {
	int status;
	char devid[32];
	char devname[32];
	char devver[8];
	char taskid[32];
};

struct _st_tt {
	struct _st_net net;
	struct _st_tty tty;
	struct _st_flags flags;
	struct _st_status devst;
	int sock_fd;
	int tty_fd;
	pthread_mutex_t mutex;
};

void init_config(const char *cfgfile);
struct _st_tt* get_config(void);

#define LOCK_CONFIG() do { \
	debug(LOG_DEBUG, "Locking config"); \
	pthread_mutex_lock(&config_mutex); \
	debug(LOG_DEBUG, "Config locked"); \
} while (0)

#define UNLOCK_CONFIG() do { \
	debug(LOG_DEBUG, "Unlocking config"); \
	pthread_mutex_unlock(&config_mutex); \
	debug(LOG_DEBUG, "Config unlocked"); \
} while (0)

#endif
