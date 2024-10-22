// DIRECTORY_H used to check if the file has already been included (prevent duplication)
#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "ls_options.h"

void listDirectory(const char *path, const LsOptions *options);

#endif
