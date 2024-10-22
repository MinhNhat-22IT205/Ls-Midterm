#include "sort.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <time.h>

void sortEntries(char **fileNames, int count, const LsOptions *options, const char *dirPath)
{
    // struct to store the options and directory path in order to pass them to the compareEntries function
    struct
    {
        const LsOptions *options;
        const char *dirPath;
    } sortArg = {options, dirPath};

    // sorts fileNames array using a comparison function
    qsort_r(fileNames, count, sizeof(char *), compareEntries, &sortArg);
}

int compareEntries(const void *a, const void *b, void *arg)
{
    // declare a struct for arg
    struct
    {
        const LsOptions *options;
        const char *dirPath;
    } *sortArg = arg;

    // DESTRUCTURE THE ARG

    // assign the options and directory path to the variables
    const LsOptions *options = sortArg->options;
    const char *dirPath = sortArg->dirPath;

    // assign the file names to the a and b (it's the comparision units)
    const char *name1 = *(const char **)a;
    const char *name2 = *(const char **)b;
    struct stat st1, st2;                          // struct to store the file status
    char fullpath1[PATH_MAX], fullpath2[PATH_MAX]; // buffers to store the full paths of the files
    // assign fullpath1 and fullpath2 by formating a string that joins the directory path and the file name and stores it in the fullpath buffer
    snprintf(fullpath1, PATH_MAX, "%s/%s", dirPath, name1);
    snprintf(fullpath2, PATH_MAX, "%s/%s", dirPath, name2);

    // lstat() to assign file status to st1 and st2
    if (lstat(fullpath1, &st1) == -1 || lstat(fullpath2, &st2) == -1)
    {
        return strcmp(name1, name2); // if the file status cannot be retrieved, compare the file names
    }

    int result;

    // option -S: sort by size
    if (options->sortBySize)
    {
        result = st2.st_size - st1.st_size; // sort by size
    }
    // option -t: sort by time
    else if (options->sortByTime)
    {
        // assign time1 and time2 by using access time if -u option is provided, change time if -c option is provided, otherwise use modification time
        time_t time1 = options->useAccessTime ? st1.st_atime : options->useChangeTime ? st1.st_ctime
                                                                                      : st1.st_mtime;
        time_t time2 = options->useAccessTime ? st2.st_atime : options->useChangeTime ? st2.st_ctime
                                                                                      : st2.st_mtime;
        result = time2 - time1; // sort by time
    }
    else
    {
        result = strcmp(name1, name2); // sort by name
    }

    // option -r: reverse sort
    return options->reverseOrder ? -result : result; // if the reverse option is provided, return the negative of the result, otherwise return the result
}
