#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "utils.h"

void 
unix_error(const char *msg) 
{
    fprintf(stderr, "Error: %s (%s)\n", msg, strerror(errno));
    exit(1);
}

int 
tokenize(char *tokens[], const int max_tokens, char *line, char delim) 
{
	int i;
	size_t token_len, line_len;
	char *line_ptr, *save_ptr;
	enum states { DELIM, TOKEN, COPY } state;

  assert(tokens != NULL);
  assert(max_tokens > 0);

	i = 0;
	token_len = 0;
	line_len = strlen(line);
	line_ptr = line;
	state = DELIM;

	while (state == COPY || (line_len && i < max_tokens)) {
        switch (state) {
            case DELIM:
               if (*line_ptr == delim) {
                   line_ptr++;
                   line_len--;
               } else {
                   save_ptr = line_ptr;
                   state = TOKEN;
               }
               break;
            case TOKEN:
               if (*line_ptr != delim) {
                   line_ptr++;
                   line_len--;
                   token_len++;
                   if (!line_len) {
                       state = COPY;
                   }
               } else {
                   state = COPY;
               }
               break;
            case COPY:
                tokens[i] = (char *)malloc(sizeof(char *) * (token_len + 1));
                memcpy(tokens[i], save_ptr, sizeof(char *) * token_len);
                tokens[i][token_len] = '\0';
                token_len = 0;
                state = DELIM;
                i++;
        }
    }

    tokens[i] = NULL;

    return i;
}

void
parse_input(char *input, char *input_argv[], int *input_argc)
{
    assert(input != NULL);
    assert(input_argv != NULL);
    assert(input_argc != NULL);

    int argc = tokenize(input_argv, SHELL_ARG_MAX - 1, input, ' ');
    free(input);
    *input_argc = argc;
}
