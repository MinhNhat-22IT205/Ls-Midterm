#include <stdio.h>            // Standard I/O functions
#include <stdlib.h>           // Standard library functions
#include <unistd.h>           // POSIX API (for getopt)
#include "ls_options.h"       // Custom header for ls options structure
#include "directory.h"        // Custom header for directory handling
#include <bits/getopt_core.h> // Core getopt functions

int main(int argc, char *argv[])
{
    LsOptions options = {0}; // Initialize all options to 0
    int opt;

    // Parse command line options
    while ((opt = getopt(argc, argv, "AacdefFhiklnqRrSstuw")) != -1)
    {
        switch (opt)
        {
        case 'A': // Enable 'showAlmostAll' option
            options.showAlmostAll = 1;
            break;
        case 'a': // Enable 'showAll' option
            options.showAll = 1;
            break;
        case 'c': // Use change time for sorting/display
            options.useChangeTime = 1;
            options.useAccessTime = 0;
            break;
        case 'd': // List directories instead of contents
            options.listDirectories = 1;
            break;
        case 'F': // Add type indicators (e.g., '/' for directories)
            options.addTypeIndicators = 1;
            break;
        case 'f': // Disable sorting
            options.disableSorting = 1;
            break;
        case 'h': // Use human-readable format for file sizes
            options.useHumanReadable = 1;
            options.sizeInKilobytes = 0;
            break;
        case 'i': // Show inode numbers
            options.showInodes = 1;
            break;
        case 'k': // Show file size in kilobytes
            options.sizeInKilobytes = 1;
            options.useHumanReadable = 0;
            break;
        case 'l': // Use long format (detailed listing)
            options.useLongFormat = 1;
            options.useNumericUIDs = 0;
            break;
        case 'n': // Use numeric UID and GID in the long format
            options.useNumericUIDs = 1;
            options.useLongFormat = 1;
            break;
        case 'q': // Show non-printable characters as '?'
            options.showNonPrintable = 1;
            options.useRawOutput = 0;
            break;
        case 'R': // Recursively list directories
            options.isRecursive = 1;
            break;
        case 'r': // Reverse sorting order
            options.reverseOrder = 1;
            break;
        case 'S': // Sort by file size
            options.sortBySize = 1;
            break;
        case 's': // Show block count for each file
            options.showBlockCount = 1;
            break;
        case 't': // Sort by modification time
            options.sortByTime = 1;
            break;
        case 'u': // Use access time for sorting/display
            options.useAccessTime = 1;
            options.useChangeTime = 0;
            break;
        case 'w': // Display raw file names
            options.useRawOutput = 1;
            options.showNonPrintable = 0;
            break;
        default: // Invalid option; display usage message
            fprintf(stderr, "Usage: %s [-AacdefFhiklnqRrSstuw] [file...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // If no paths specified, use current directory
    if (optind >= argc)
    {
        listDirectory(".", &options); // List current directory
    }
    else
    {
        // Process each path argument provided by the user
        for (; optind < argc; optind++)
        {
            char *path = argv[optind];
            listDirectory(path, &options);
        }
    }

    return 0; // End of program
}
