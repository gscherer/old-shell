#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>    
#include <signal.h> 
#include <limits.h> 
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "../collections/array/array.h"
#include "job.h"

static void *
job_key(Item item)
{
	assert(item != NULL);

	return &(((Job)item)->id);
}

static int
job_compare(void *key, void *other_key)
{
	long t_key, t_other_key;

	assert(key != NULL && other_key != NULL);

	t_key = *(long *)key;
	t_other_key = *(long *)other_key;

	return (t_key > t_other_key) ? 1 : (t_key < t_other_key) ? -1 : 0;
}

static char *
job_to_string(Item item)
{
	assert(item != NULL);

	return ((Job)item)->cmd;
}

Job_Status
job_status(Job job)
{
	if (job_is_completed(job)) {
		return JOB_COMPLETED;
	} else if (job_is_stopped(job)) {
		return JOB_STOPPED;
	}

	return JOB_RUNNING;
}

void
job_print_status(Job job)
{
	char *fmt;
	Job_Status status;

	fmt = "[%d] %s\t%s\n";
	status = job_status(job);

	switch (status) {
		case JOB_COMPLETED:
			printf(fmt, job->id, "Completed", job_to_string(job));
			break;
		case JOB_STOPPED:
			printf(fmt, job->id, "Stopped", job_to_string(job));
			break;
		case JOB_RUNNING:
			printf(fmt, job->id, "Running", job_to_string(job));		
			break;
	}
}

static void
job_deconstruct(Item item)
{
	Job job;

	assert(item != NULL);

	job = item;

	collection_deconstruct(job->processes);
	free(job->cmd);	
	free(job);
}

int
job_is_stopped(Job job)
{
	Iterator p_iter;
	Process p;
	int ret;

	assert(job != NULL);

	ret = 1;
	p_iter = collection_iterator(job->processes);

	while (iterator_has_next(p_iter)) {
		p = iterator_next(p_iter);
		if (!p->completed && !p->stopped) {
			ret = 0;
			break;
		}
	}

	iterator_deconstruct(p_iter);

	return ret;
}

int
job_is_completed(Job job)
{
	Iterator p_iter;
	Process p;
	int ret;

	assert(job != NULL);

	ret = 1;
	p_iter = collection_iterator(job->processes);

	while (iterator_has_next(p_iter)) {
		p = iterator_next(p_iter);
		if (!p->completed) {
			ret = 0;
			break;
		}
	}

	iterator_deconstruct(p_iter);

	return ret;
}

void
job_mark_as_running(Job job)
{
	Iterator p_iter;
	Process p;

	assert(job != NULL);

	p_iter = collection_iterator(job->processes);

	while (iterator_has_next(p_iter)) {
		p = iterator_next(p_iter);
		p->stopped = 0;
	}

	iterator_deconstruct(p_iter);
	job->notified = 0;
}

/*
 * Check for processes that have status information available, blocking until
 * all process in the job have reported.
 *
 * Status is reported for any processes that stop as well as processes
 * that terminate.
 */
void
job_wait_block(Job job)
{
	int status;
	pid_t pid;
	Process curr_process;

	assert(job != NULL);

	do {
		curr_process = NULL;
		pid = waitpid(WAIT_ANY, &status, WUNTRACED);
		if (pid > 0) {
			curr_process = collection_find(job->processes, &pid);
		} else if (pid == 0 || errno == ECHILD) {

		} else {
			unix_error("waitpid");
		}
	} while (!process_mark_status(curr_process, pid, status) 
		&& !job_is_stopped(job) 
		&& !job_is_completed(job));
}

Item_Interface
item_interface_job()
{
	return item_interface(job_key, job_compare, job_to_string, job_deconstruct);
}

Job
item_job(int terminal, int id, char *cmd)
{
	Job job;

	assert(cmd != NULL);

	job = (Job)calloc(1, sizeof(struct type_Job));
	job->id = id;
	job->cmd = cmd;
	job->pgid = 0;
	job->notified = 0;
	tcgetattr(terminal, &job->tmodes);
	job->stdin = STDIN_FILENO;
	job->stdout = STDOUT_FILENO;
	job->stderr = STDERR_FILENO;
	job->processes = collection_construct(array, process);

	return job;
}
