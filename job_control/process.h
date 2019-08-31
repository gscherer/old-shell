#ifndef SHELL_PROCESS_H
#define SHELL_PROCESS_H

#include "job_control.h"

struct type_Process
{
	char **argv;
	int argc;
	pid_t pid;
	char completed;
	char stopped;
	int status;
};

int
process_mark_status(Process /* process */, pid_t /* pid */, int /* status */);

Item_Interface
item_interface_process();

Process
item_process(char ** /* argv */, int /* argc */);

#endif