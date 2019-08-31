#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>  
#include <signal.h>
#include <assert.h>

#include "process.h"

static void *
process_key(Item item)
{
	Process process;

	assert(item != NULL);

	process = item;

	return &process->pid;
}

static int
process_compare(void *key, void *other_key)
{
	pid_t t_key, t_other_key;

	assert(key != NULL && other_key != NULL);

	t_key = *(size_t *)key;
	t_other_key = *(size_t *)other_key;

	return (t_key > t_other_key) ? 1 : (t_key < t_other_key) ? -1 : 0;
}

static char *
process_toString(Item item)
{
	/* @todo */
	return "process";
}

static void
process_deconstruct(Item item)
{
	Process process;

	assert(item != NULL);

	process = item;
	int i;
	for (i = 1; i < process->argc; i++) {
		free(process->argv[i]);
	}
	free(process);
}

int
process_mark_status(Process process, pid_t pid, int status)
{
	if (pid > 0) {
		if (process == NULL) {
			fprintf(stderr, "No child process %d.\n", pid);
			return -1;
		}

		process->status = status;

		if (WIFSTOPPED(status)) {
			process->stopped = 1;
		} else {
			process->completed = 1;
			if (WIFSIGNALED(status)) {
				fprintf(stderr, "%d: Terminated by signal %d.\n", (int)pid, WTERMSIG(status));
			}
		}

		return 0;		
	}

	return -1;
}

Item_Interface
item_interface_process()
{
	return item_interface(process_key, process_compare, process_toString, 
		process_deconstruct);
}

Process
item_process(char *argv[], int argc)
{
	Process process;

	assert(argv != NULL);

	process = (Process)calloc(1, sizeof(struct type_Process));
	process->argv = argv;
	process->argc = argc;
	process->completed = 0;
	process->stopped = 0;

	return process;
}