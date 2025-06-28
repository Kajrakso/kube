/* shamelessly copied from https://github.com/sebastianotronto/nissy-classic/blob/master/src/env.h */
#ifndef ENV_H
#define ENV_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

extern char* tabledir;

void init_env(void);
bool file_exists(const char* filename);

#endif
