#ifndef SHELL_BUILTIN_H
#define SHELL_BUILTIN_H

#include "../job_control/shell.h"

void
builtin_jobs(Shell /* shell */, char ** /* argv */, int /* argc */);

void
builtin_fg(Shell /* shell */, char ** /* argv */, int /* argc */);

void
builtin_bg(Shell /* shell */, char ** /* argv */, int /* argc */);

#endif
