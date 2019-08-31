#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

/* Signal functions */

void 
*Signal(int signum, void *handler)
{
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(signum, &action, &old_action) < 0) {
        unix_error("signal error");
    }
    return (old_action.sa_handler);
}

int 
Sigprocmask(int how, const sigset_t *set, sigset_t *oldset) 
{
	int ret;
	if ((ret = sigprocmask(how, set, oldset)) < 0) {
		unix_error("sigprocmask error");
	}
	return ret;
}

int 
Sigemptyset(sigset_t *set) 
{
	int ret;
	if ((ret = sigemptyset(set)) < 0) {
		unix_error("sigemptyset error");
	}
	return ret;
}

int 
Sigfillset(sigset_t *set) 
{
	int ret;
	if ((ret = sigfillset(set)) < 0) {
		unix_error("sigfillset error");
	}
	return ret;
}

int 
Sigaddset(sigset_t *set, int signum) 
{
	int ret;
	if ((ret = sigaddset(set, signum)) < 0) {
		unix_error("sigaddset error");
	}
	return ret;
}

int 
Sigdelset(sigset_t *set, int signum) 
{
	int ret;
	if ((ret = sigdelset(set, signum)) < 0) {
		unix_error("sigdelset error");
	}
	return ret;
}

int
Sigismember(const sigset_t *set, int signum) 
{
	int ret;
	ret = sigismember(set, signum);
	if (ret < 0) {
		unix_error("sigismember error");
	}
	return ret;
}

/* Fork */

pid_t
Fork() 
{
	pid_t pid;
    if ((pid = fork()) < 0) {
        unix_error("fork error");
    }
    return pid;
}

/* Set process group id */
int 
Setpgid(pid_t pid, pid_t pgid)
{
	int status;
	if ((status = setpgid(pid, pgid)) < 0) {
		unix_error("setpgid error");
	}
	return status;
}

/* Duplicating file descriptors */

int
Dup2(int oldfd, int newfd) 
{
	int status;
	if ((status = dup2(oldfd, newfd)) < 0) {
		unix_error("dup2 error");
	}
	return status;
}

int
Pipe(int pipe_array[])
{
	int status;
	if ((status = pipe(pipe_array))) {
		unix_error("pipe error");
	}
	return status;
}
