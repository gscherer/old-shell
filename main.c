#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>  /* provides pid_t */
#include <unistd.h>     /* provides _exit, fork */
#include <signal.h>     /* provides SIGCHLD and other signals */
#include <sys/wait.h>   /* provides wait_pid constants */
#include <termios.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "utils.h"
#include "wrappers.h"
#include "job_control/shell.h"
#include "dispatch/builtin.h"

int 
main(int argc, char *argv[], char *envp[]) 
{
    Shell shell;
    Job job;
    Process process;
    size_t id;
    char *input, *input_argv[SHELL_ARG_MAX];
    int input_argc, fg;

    id = input_argc = 0;
    shell = shell_construct();

    while(!feof(stdin)) {
        fg = 1;
        if (!collection_is_empty(shell->jobs)) {
            shell_notify_user(shell->jobs);            
        }
        input = readline("> ");
        if (input == NULL) {
            free(input);
            break;
        }
        parse_input(input, input_argv, &input_argc);
        if (input_argc == 0) {
            continue;
        }
        if (strcmp("&", input_argv[input_argc - 1]) == 0) {                    
            fg = 0;
            free(input_argv[input_argc - 1]);
            input_argv[input_argc - 1] = NULL;
            input_argc--;
        }
        if (input_argc == 0) {
            continue;
        }
        if (strcmp("jobs", input_argv[0]) == 0) {
            builtin_jobs(shell, input_argv, input_argc);
        } else if (strcmp("fg", input_argv[0]) == 0)  {
            builtin_fg(shell, input_argv, input_argc);
        } else if (strcmp("bg", input_argv[0]) == 0) {
            builtin_bg(shell, input_argv, input_argc);
        } else {
            job = item_job(shell->terminal, id++, input_argv[0]);
            process = item_process(input_argv, input_argc);
            collection_add(shell->jobs, job);
            collection_add(job->processes, process);
            shell_launch_job(shell, job, fg);            
        }
    }
    shell_deconstruct(shell);
    return 0;
}