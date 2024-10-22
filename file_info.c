#include "file_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>

void printFileInfo(const char *path, const char *name, const struct stat *fileStat, const LsOptions *options)
{
    // option -i: show inode number
    if (options->showInodes)
        printf("%lu ", (unsigned long)fileStat->st_ino);

    // option -s: show block count
    if (options->showBlockCount)
    {
        // Prints the size of the file in a human-readable format if -h option is provided
        // otherwise prints the size in kilobytes if -k option is provided
        // otherwise prints the size in blocks
        if (options->useHumanReadable)
            printf("%s ", formatHumanReadableSize(fileStat->st_size)); // "4.0K"
        else if (options->sizeInKilobytes)
            printf("%6lld ", (long long)fileStat->st_size / 1024); // "4"
        else
            printf("%lld ", (long long)(fileStat->st_blocks / 2));
    }

    // option -l: long listing
    if (options->useLongFormat)
    {
        char *permissions = formatPermissions(fileStat->st_mode); // return sth like "-rwxr-xr-x"
        printf("%s ", permissions);
        free(permissions); // free the memory allocated for permissions

        printf("%3lu ", (unsigned long)fileStat->st_nlink); // number of hard links to the file

        // option -n: numeric user ID (UID) and group ID (GID)
        if (options->useNumericUIDs)
        {
            printf("%d %d ", fileStat->st_uid, fileStat->st_gid); // numeric user ID (UID) and group ID (GID)
        }
        // else: show user name and group name
        else
        {
            struct passwd *pw = getpwuid(fileStat->st_uid); // get user name from UID
            struct group *gr = getgrgid(fileStat->st_gid);  // get group name from GID
            // print user name and group name
            printf("%-8s %-8s ",
                   pw ? pw->pw_name : "unknown",
                   gr ? gr->gr_name : "unknown");
        }

        // Prints the size of the file in a human-readable format if -h option is provided, otherwise prints the size in kilobytes if -k option is provided, otherwise prints the size in bytes
        if (options->useHumanReadable)
            printf("%s ", formatHumanReadableSize(fileStat->st_size)); // "4.0K"
        else if (options->sizeInKilobytes)
            printf("%6lld ", (long long)fileStat->st_size / 1024); // "4"
        else
            printf("%8lld ", (long long)fileStat->st_size); // "4096"

        // assign fileTime by using access time if -u option is provided, change time if -c option is provided, otherwise use modification time
        time_t fileTime = options->useAccessTime ? fileStat->st_atime : options->useChangeTime ? fileStat->st_ctime
                                                                                               : fileStat->st_mtime;
        char timeStr[26];                                                        // initialize a string that will store the formatted time
        strftime(timeStr, sizeof(timeStr), "%b %d %H:%M", localtime(&fileTime)); // formats the time as "Apr 01 12:00"
        printf("%s ", timeStr);
    }

    // option -w: display the file name exactly as it is or display non-printable characters as ?
    if (options->useRawOutput)
    {
        printf("%s", name); // displays the file name exactly as it is
    }
    // else: replace any non-printable characters in the file name with ? to avoid rendering issues
    else
    {
        // loop through each character in the file name
        for (const char *p = name; *p; p++)
        {
            // option -q: display non-printable characters as ?
            if (options->showNonPrintable && !isprint(*p))
            {
                putchar('?');
            }
            else
            {
                putchar(*p);
            }
        }
    }

    // option -F: add type indicators
    if (options->addTypeIndicators)
        printf("%c", getFileTypeIndicator(fileStat->st_mode));

    printf("\n");
}

char *formatPermissions(mode_t mode)
{
    char *permissions = malloc(11);
    if (!permissions)
        return NULL;

    // Determine file type and set the first character of permissions accordingly
    if (S_ISREG(mode))
    {
        permissions[0] = '-'; // Regular file
    }
    else if (S_ISDIR(mode))
    {
        permissions[0] = 'd'; // Directory
    }
    else if (S_ISLNK(mode))
    {
        permissions[0] = 'l'; // Symbolic link
    }
    else if (S_ISBLK(mode))
    {
        permissions[0] = 'b'; // Block special file
    }
    else if (S_ISCHR(mode))
    {
        permissions[0] = 'c'; // Character special file
    }
    else if (S_ISFIFO(mode))
    {
        permissions[0] = 'p'; // FIFO (named pipe)
    }
    else if (S_ISSOCK(mode))
    {
        permissions[0] = 's'; // Socket
    }
    else
    {
        permissions[0] = '?'; // Unknown
    }

    // User permissions - next three characters
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_ISUID) ? ((mode & S_IXUSR) ? 's' : 'S') : ((mode & S_IXUSR) ? 'x' : '-');

    // Group permissions - next three characters
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_ISGID) ? ((mode & S_IXGRP) ? 's' : 'S') : ((mode & S_IXGRP) ? 'x' : '-');

    // Others permissions - next three characters
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & 01000) ? ((mode & S_IXOTH) ? 't' : 'T') : ((mode & S_IXOTH) ? 'x' : '-');

    permissions[10] = '\0'; // Null-terminate the string to make it a valid C string
    return permissions;
}

char getFileTypeIndicator(mode_t mode)
{
    // return the file type indicator based on the file type
    if (S_ISDIR(mode))
        return '/'; // directory
    if (S_ISLNK(mode))
        return '@'; // symbolic link
    if (S_ISFIFO(mode))
        return '|'; // FIFO
    if (S_ISSOCK(mode))
        return '='; // socket
    if (mode & S_IXUSR)
        return '*'; // executable
    return ' ';
}
char *formatHumanReadableSize(off_t size)
{
    static char buffer[20];

    // decide the unit of size char
    const char *units[] = {"B", "K", "M", "G", "T", "P", "E", "Z", "Y"};
    int i = 0;
    double dsize = (double)size;

    // find the appropriate unit for the size (like i KB, MB, GB, etc.)
    while (dsize >= 1024 && i < 8)
    {
        dsize /= 1024;
        i++;
    }

    snprintf(buffer, sizeof(buffer), "%.1f%s", dsize, units[i]); // print the size with one decimal place and the appropriate unit
    return buffer;
}
