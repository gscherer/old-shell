#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#define SHELL_ARG_MAX 1000
#define SHELL_PROMPT "> "

void
unix_error(const char *msg);

int 
tokenize(char *tokens[], const int max_tokens, char *line, const char delim);

void
parse_input(char *input, char *input_argv[], int *input_argc);

#endif
