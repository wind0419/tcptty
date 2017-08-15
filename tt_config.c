#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "tt_config.h"

static struct _st_tt tt_config;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_config(const char *cfgfile)
{
	// parse cfgfile
	// tt_config.xxx = val;
	memset(&tt_config, 0, sizeof(struct _st_tt));
	// init tt_config.mutex;
}

struct _st_tt* get_config(void)
{
	return &tt_config;
}

