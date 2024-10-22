// SORT_H used to check if the file has already been included (prevent duplication)
#ifndef SORT_H
#define SORT_H

#include "ls_options.h"

void sortEntries(char **fileNames, int count, const LsOptions *options, const char *dirPath);
int compareEntries(const void *a, const void *b, void *arg);

#endif
