#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
/* for wait() */
#include <sys/wait.h>
#include <getopt.h>
#include <sys/types.h> 
#include <sys/time.h> 
#include <sys/stat.h>

#include "tt_debug.h"
#include "tt_config.h"
#include "tt_thread.h"
#include "tt_callback.h"


/**@internal
 * @brief Handles SIGCHLD signals to avoid zombie processes
 *
 * When a child process exits, it causes a SIGCHLD to be sent to the
 * parent process. This handler catches it and reaps the child process so it
 * can exit. Otherwise we'd get zombie processes.
 */
static void sigchld_handler(int s)
{
	int	status;
	pid_t rc;

	debug(LOG_DEBUG, "SIGCHLD handler: Trying to reap a child");

	rc = waitpid(-1, &status, WNOHANG | WUNTRACED);

	if(rc == -1) {
		if(errno == ECHILD) {
			debug(LOG_DEBUG, "SIGCHLD handler: waitpid(): No child exists now.");
		} else {
			debug(LOG_ERR, "SIGCHLD handler: Error reaping child (waitpid() returned -1): %s", strerror(errno));
		}
		return;
	}

	if(WIFEXITED(status)) {
		debug(LOG_DEBUG, "SIGCHLD handler: Process PID %d exited normally, status %d", (int)rc, WEXITSTATUS(status));
		return;
	}

	if(WIFSIGNALED(status)) {
		debug(LOG_DEBUG, "SIGCHLD handler: Process PID %d exited due to signal %d", (int)rc, WTERMSIG(status));
		return;
	}

	debug(LOG_DEBUG, "SIGCHLD handler: Process PID %d changed state, status %d not exited, ignoring", (int)rc, status);
}

static void termination_handler(int s)
{
	debug(LOG_NOTICE, "Handler for termination caught signal %d", s);

	debug(LOG_NOTICE, "Exiting...");
	exit(s == 0 ? 1 : 0);
}

static void timer_handler(int sig)
{
	if(sig == SIGALRM) {
		debug(LOG_NOTICE, "Get signal Alarm\n");
	}
}

static void usrsig_handler(int sig)
{
	if(sig == SIGUSR1) {
		debug(LOG_NOTICE, "Get signal USR1\n");
	} else if(sig == SIGUSR2) {
		debug(LOG_NOTICE, "Get signal USR2\n");
	}
}

/** @internal
 * Registers all the signal handlers
 */
static void init_signals(void)
{
	struct sigaction sa;

	debug(LOG_DEBUG, "Setting SIGCHLD handler to sigchld_handler()");
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	debug(LOG_DEBUG, "Register signal ALARM\n");
	sa.sa_handler = timer_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	debug(LOG_DEBUG, "Register signal USR1 and USR2\n");
	sa.sa_handler = usrsig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}
	if (sigaction(SIGUSR2, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}
	
	/* Trap SIGPIPE */
	debug(LOG_DEBUG, "Setting SIGPIPE  handler to SIG_IGN");
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	debug(LOG_DEBUG, "Setting SIGTERM,SIGQUIT,SIGINT  handlers to termination_handler()");
	sa.sa_handler = termination_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	/* Trap SIGTERM */
	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	/* Trap SIGQUIT */
	if (sigaction(SIGQUIT, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}

	/* Trap SIGINT */
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		debug(LOG_ERR, "sigaction(): %s", strerror(errno));
		exit(1);
	}
}

int main(int argc, char **argv)
{
	init_config(argv[1]); //config file
	
	init_signals();	
	//alarm(WT_SDCARD_PRIOD);

	register_def_all();

	pthread_t tty_id;
	pthread_create(&tty_id, NULL, pthread_handle_tty, NULL);
	
	pthread_t net_id;
	pthread_create(&net_id, NULL, pthread_handle_network, NULL);
	
	pthread_join(&tty_id);
	pthread_join(&net_id);
	
	debug(LOG_NOTICE,"<=== Main Process exit!\n");
	return 0;
}