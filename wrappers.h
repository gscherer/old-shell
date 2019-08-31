#ifndef SHELL_WRAPPERS_H
#define SHELL_WRAPPERS_H

/* Signal functions */
void
*Signal(int signum, void *handler);

void
*Sigaction(int signum, void *handler);

int 
Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

int 
Sigemptyset(sigset_t *set);

int 
Sigfillset(sigset_t *set);

int 
Sigaddset(sigset_t *set, int signum);

int 
Sigdelset(sigset_t *set, int signum);

int
Sigismember(const sigset_t *set, int signum);

int 
Setpgid(pid_t pid, pid_t pgid);

/* Fork */

pid_t 
Fork();

/* Duplicating file descriptors */

int
Dup2();

int
Pipe(int *);

#endif
