/**
List the files in the OU File System.

CS3113

*/

#include <stdio.h>
#include <string.h>

#include "oufs_lib.h"

int main(int argc, char **argv) {
    // Fetch the key environment vars
    char cwd[MAX_PATH_LENGTH];
    char disk_name[MAX_PATH_LENGTH];
    oufs_get_environment(cwd, disk_name);

    // Check arguments
    if (argc == 1) {
        // Open the virtual disk
        vdisk_disk_open(disk_name);

        char currentDir[MAX_PATH_LENGTH] = "./";

        // Make the specified directory
        oufs_list(cwd, currentDir);

        // Clean up
        vdisk_disk_close();

    }else if (argc == 2) {
        // Open the virtual disk
        vdisk_disk_open(disk_name);

        // Make the specified directory
        oufs_list(cwd, argv[1]);

        // Clean up
        vdisk_disk_close();

    }else {
        // Wrong number of parameters
        fprintf(stderr, "Usage: zfilez <dirname> or zfilez for CWD\n");
    }

}


