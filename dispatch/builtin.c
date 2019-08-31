#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <assert.h>

#include "builtin.h"

void
builtin_jobs(Shell shell, char *argv[], int argc)
{
	Iterator job_iter;
	Job job;

	job_iter = collection_iterator(shell->jobs);

	while (iterator_has_next(job_iter)) {
		job = iterator_next(job_iter);
		job_print_status(job);
	}

	iterator_deconstruct(job_iter);
}

static void
builtin_fg_or_bg(Shell shell, char *argv[], int argc, int fg)
{
	long id;
	Job job;
	int cont;

	assert(shell != NULL && argv != NULL);

	id = strtol(argv[1], NULL, 10);
	
	if (id == LONG_MAX) {
		unix_error("Job id caused an overflow :(");
	}

	job = collection_find(shell->jobs, &id);
	cont = 0;

	if (job != NULL) {
		if (job_is_stopped(job)) {
			cont = 1;
		}

		if (fg) {
			shell_place_job_fg(shell, job, cont);			
		} else {
			shell_place_job_bg(shell, job, cont);
			job_print_status(job);
		}
	} else {
		printf("[%ld] Job not found!!1!\n", id);
	}
}

void
builtin_fg(Shell shell, char *argv[], int argc)
{
	builtin_fg_or_bg(shell, argv, argc, 1);
}

void
builtin_bg(Shell shell, char *argv[], int argc)
{
	builtin_fg_or_bg(shell, argv, argc, 0);
}