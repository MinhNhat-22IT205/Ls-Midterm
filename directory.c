#include "directory.h"
#include "file_info.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>

void listDirectory(const char *path, const LsOptions *options)
{
    DIR *dirStream;          // dirStream is a pointer to a directory stream
    struct dirent *entry;    // dirent is a structure that contains the information about a directory entry
    struct stat fileStat;    // fileStat is a structure that contains the information about a file
    char fullPath[PATH_MAX]; // fullPath is a char array that stores the full path of the file
    // the PATH_MAX is a constant defined in limits.h that specifies the maximum length of a pathname

    // Assign status to fileStat
    if (lstat(path, &fileStat) == -1)
    {
        perror(path); // print error message
        return;
    }

    // option -d: list directory entries instead of contents
    if (options->listDirectories)
    {
        printFileInfo(".", path, &fileStat, options);
        return;
    }

    // Open directory
    if ((dirStream = opendir(path)) == NULL)
    {
        perror(path); // print error message
        return;
    }

    // Read directory entries and put into fileNames array
    char **fileNames = NULL;
    int fileCount = 0;
    while ((entry = readdir(dirStream)) != NULL)
    {
        // Skip . and .. for -A option
        int isDotOrDotDot = !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..");
        if (options->showAlmostAll && isDotOrDotDot)
            continue;
        // Skip hidden files if -a or -A not provided
        if (!options->showAll && !options->showAlmostAll && entry->d_name[0] == '.')
            continue;

        // Allocate memory for the file names
        size_t newSize = (fileCount + 1) * sizeof(char *); // calculates the new size needed to store an additional file name in the array.
        fileNames = realloc(fileNames, newSize);           // resize array size to a new size
        fileNames[fileCount] = strdup(entry->d_name);      // duplicates the string (file name) and stores it in the newly allocated slot in fileNames
        fileCount++;
    }

    // Sort entries if -f option is not provided
    if (!options->disableSorting)
    {
        sortEntries(fileNames, fileCount, options, path);
    }

    // Print entries
    for (int i = 0; i < fileCount; i++)
    {
        // assign value to fullPath
        snprintf(fullPath, PATH_MAX, "%s/%s", path, fileNames[i]); // formats a string that joins the directory path and the file name fileNames[i] with a / in between and stores it in the fullPath buffer
        if (lstat(fullPath, &fileStat) == -1)                      // assign file status to fileStat
            continue;

        printFileInfo(path, fileNames[i], &fileStat, options);

        // Handle recursive option
        int isDirectory = S_ISDIR(fileStat.st_mode);
        int isNotDotOrDotDot = strcmp(fileNames[i], ".") != 0 && strcmp(fileNames[i], "..") != 0;
        if (options->isRecursive && isDirectory && isNotDotOrDotDot)
        {
            printf("\n%s:\n", fullPath);
            listDirectory(fullPath, options);
        }

        free(fileNames[i]);
    }
    free(fileNames);     // free the memory allocated for fileNames
    closedir(dirStream); // close the directory stream
}