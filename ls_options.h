// LS_OPTIONS_H used to check if the file has already been included (prevent duplication)
#ifndef LS_OPTIONS_H
#define LS_OPTIONS_H

// LsOptions struct to store the options for the ls command
typedef struct
{
    int showAll;
    int showAlmostAll;
    int useChangeTime;
    int listDirectories;
    int addTypeIndicators;
    int disableSorting;
    int useHumanReadable;
    int showInodes;
    int sizeInKilobytes;
    int useLongFormat;
    int useNumericUIDs;
    int showNonPrintable;
    int isRecursive;
    int reverseOrder;
    int sortBySize;
    int showBlockCount;
    int sortByTime;
    int useAccessTime;
    int useRawOutput;
} LsOptions;

#endif
