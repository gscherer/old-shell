#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

#include "job.h"

struct type_Shell {
    pid_t pgid;
    struct termios tmodes;
    int terminal;
    int is_interactive;
 	Collection jobs;
};

Shell
shell_construct();

void
shell_deconstruct(Shell /* shell */);

void
shell_launch_process(Shell /* shell */, Process /* process */, pid_t /* pgid */,
	int /* infile */, int /* outfile */, int /* errfile */, int /* fg */);

void
shell_launch_job(Shell /* shell */, Job /* job */, int /* fg */);

void
shell_continue_job(Shell /* shell */, Job /* job */, int /* fg */);

void
shell_place_job_fg(Shell /* shell */, Job /* job */, int /* cont */);

void 
shell_place_job_bg(Shell /* shell */, Job /* job */, int /* cont */);

void
shell_wait_jobs_async(Collection /* jobs */);

void
shell_notify_user(Collection /* jobs */);

#endif