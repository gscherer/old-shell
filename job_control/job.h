#ifndef SHELL_JOB_H
#define SHELL_JOB_H

#include <termios.h>

#include "process.h"

struct type_Job 
{
	long id;
	char *cmd;
	pid_t pgid;
	char notified;
	struct termios tmodes;
	int stdin, stdout, stderr;
	Collection processes;
};

enum enum_Job_Status
{
	JOB_RUNNING, 
	JOB_COMPLETED, 
	JOB_STOPPED
};

int
job_is_stopped(Job /* job */);

int
job_is_completed(Job /* job */);

void
job_mark_as_running(Job /* job */);

void
job_wait_block(Job /* job */);

Job_Status
job_status(Job /* job */);

void
job_print_status(Job /* job */);

Item_Interface
item_interface_job();

Job
item_job(int /* terminal */, int /* id */, char * /* cmd */);

#endif