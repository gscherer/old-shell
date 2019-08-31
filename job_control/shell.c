#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#include "../collections/array/array.h"
#include "../wrappers.h"
#include "shell.h"

Shell
shell_construct()
{
    Shell shell;

    shell = (Shell)calloc(1, sizeof(struct type_Shell));
    shell->terminal = STDIN_FILENO;
    shell->is_interactive = isatty(shell->terminal);
    shell->jobs = collection_construct(array, job);

    if (shell->is_interactive) {
        /* Loop until we are in the foreground, sending SIGTTIN to our process group
         * until we are. */
        while (tcgetpgrp(shell->terminal) != (shell->pgid = getpgrp())) {
            kill(- shell->pgid, SIGTTIN); 
        }

        /* Ignore job control signals */
        Signal(SIGINT, SIG_IGN);
        Signal(SIGQUIT, SIG_IGN);
        Signal(SIGTSTP, SIG_IGN);
        Signal(SIGTTIN, SIG_IGN);
        Signal(SIGTTOU, SIG_IGN);
        //Signal(SIGCHLD, SIG_IGN);

        /* Put the shell in its own process group */
        shell->pgid = getpid();
        Setpgid(shell->pgid, shell->pgid);

        /* Be the controlling process for the terminal */
        tcsetpgrp(shell->terminal, shell->pgid);

        /* Save default terminal attributes for the shell */
        tcgetattr(shell->terminal, &shell->tmodes);
    }

    return shell;
}

void
shell_deconstruct(Shell shell)
{
	assert(shell != NULL);
    
	collection_deconstruct(shell->jobs);
	free(shell);
}

void
shell_launch_process(Shell shell, Process process, pid_t pgid,
	int infile, int outfile, int errfile, int fg)
{
    pid_t pid;

    assert(shell != NULL && process != NULL);

    if (shell->is_interactive) {
        /* Put the process into the process group and give the process group the
         * terminal, if appropriate. This must be done by both the shell and the
         * individual child processes because of potential race conditions.
         */
        pid = getpid();

        if (pgid == 0) {
            pgid = pid;
        }
        
        Setpgid(pid, pgid);

        if (fg) {
            tcsetpgrp(shell->terminal, pgid);
        }

        /* Set job handling signals back to the default */
        Signal(SIGINT, SIG_DFL);
        Signal(SIGQUIT, SIG_DFL);
        Signal(SIGTSTP, SIG_DFL);
        Signal(SIGTTIN, SIG_DFL);
        Signal(SIGTTOU, SIG_DFL);
        Signal(SIGCHLD, SIG_DFL);
    }

    /* Set the standard input/output channels of the new process */
    if (infile != STDIN_FILENO) {
        Dup2(infile, STDIN_FILENO);
        close(infile);
    }
    if (outfile != STDOUT_FILENO) {
        Dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }
    if (errfile != STDERR_FILENO) {
        Dup2(errfile, STDERR_FILENO);
        close(errfile);
    }

    /* Exec the new process */
    if (execvp(process->argv[0], process->argv) < 0) {
        printf("%s: Command not found\n", process->argv[0]);
        exit(1);
    }
}

void
shell_launch_job(Shell shell, Job job, int fg)
{
	Iterator p_iter;
	Process curr_process;
    pid_t pid;
    int process_pipe[2], infile, outfile;

    assert(shell != NULL && job != NULL);

    infile = job->stdin;
    p_iter = collection_iterator(job->processes);

    while (iterator_has_next(p_iter)) {
        curr_process = iterator_next(p_iter);

    	if (iterator_has_next(p_iter)) {
            Pipe(process_pipe);
    		outfile = process_pipe[1];
    	} else {
    		outfile = job->stdout;
    	}

    	pid = Fork();

    	if (pid == 0) {
    		shell_launch_process(shell, curr_process, job->pgid, infile, 
                outfile, job->stderr, fg);
    	} else if (pid) {
    		curr_process->pid = pid;

    		if (shell->is_interactive) {
    			if (!job->pgid) {
    				job->pgid = pid;
    			}
    			Setpgid(pid, job->pgid);
    		}
    	}

        /* Clean up after pipes */

        if (infile != job->stdin) {
            close(infile);
        }

        if (outfile != job->stdout) {
            close(outfile);
        }

        infile = process_pipe[0];    	
    }

    iterator_deconstruct(p_iter);

    if (!shell->is_interactive) {
        job_wait_block(job);
    } else if (fg) {
        shell_place_job_fg(shell, job, 0);
    } else {
        shell_place_job_bg(shell, job, 0);
    }
}

void
shell_continue_job(Shell shell, Job job, int fg)
{
	assert(shell != NULL && job != NULL);

    job_mark_as_running(job);
    
    if (fg) {
    	shell_place_job_fg(shell, job, 1);
    } else {
    	shell_place_job_bg(shell, job, 1);
    }
}

void
shell_place_job_fg(Shell shell, Job job, int cont)
{
	assert(shell != NULL && job != NULL);

    /* Put the job in the foreground */
    tcsetpgrp(shell->terminal, job->pgid);

    /* Send the job a continue signal if necessary */
    if (cont) {
        tcsetattr(shell->terminal, TCSADRAIN, &job->tmodes);
        job_mark_as_running(job);
        if (kill(- job->pgid, SIGCONT) < 0) {
            unix_error("kill (SIGCONT)");
        }
    }

    /* Wait for it to report */
    job_wait_block(job);

    /* Put the shell back in the foreground */
    tcsetpgrp(shell->terminal, shell->pgid);

    /* Restore the terminal's modes */
    tcgetattr(shell->terminal, &job->tmodes);
    tcsetattr(shell->terminal, TCSADRAIN, &shell->tmodes);
}

void 
shell_place_job_bg(Shell shell, Job job, int cont)
{
    if (cont) {
        job_mark_as_running(job);
        if (kill(- job->pgid, SIGCONT) < 0) {
            unix_error("kill (SIGCONT)");
        }
    }
}

void
shell_wait_jobs_async(Collection jobs)
{
	int status;
	pid_t pid;
	Job curr_job;
	Process curr_process;
	Iterator jobs_iter;

	jobs_iter = collection_iterator(jobs);

	do {
		curr_process = NULL;
		pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);

		if (pid > 0) {
			while (iterator_has_next(jobs_iter)) {
                curr_job = iterator_next(jobs_iter);

				/* Assumes a Process uses it's pid as a key */
				curr_process = collection_find(curr_job->processes, &pid);

				if (curr_process != NULL) {
					break;
				}
			}
            
            iterator_reset(jobs_iter);

		} else if (pid == 0 || errno == ECHILD) {

		} else {
			unix_error("waitpid");
		}

	} while (!process_mark_status(curr_process, pid, status));
    
    iterator_deconstruct(jobs_iter);
}

void
shell_notify_user(Collection jobs)
{
	Iterator job_iter;
	Job curr_job;

	assert(jobs != NULL);

	shell_wait_jobs_async(jobs);
	job_iter = collection_iterator(jobs);

	while (iterator_has_next(job_iter)) {
        curr_job = iterator_next(job_iter);

		if (job_is_completed(curr_job)) {
			job_print_status(curr_job);
            iterator_remove(job_iter);
		} else if (job_is_stopped(curr_job) && !curr_job->notified) {
			job_print_status(curr_job);
			curr_job->notified = 1;
		}
	}

	iterator_deconstruct(job_iter);
}
