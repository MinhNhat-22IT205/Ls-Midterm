// FILE_INFO_H used to check if the file has already been included (prevent duplication)
#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <sys/types.h>
#include <sys/stat.h>
#include "ls_options.h"

void printFileInfo(const char *path, const char *name, const struct stat *fileStat, const LsOptions *options);
char *formatPermissions(mode_t mode);
char getFileTypeIndicator(mode_t mode);
char *formatHumanReadableSize(off_t size);
#endif
