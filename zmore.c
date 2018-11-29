/**
Print a file in the OU File System.

CS3113

*/

#include <stdio.h>
#include <string.h>

#include "oufs_lib.h"

int main(int argc, char **argv) {
    // Fetch key environment vars
    char cwd[MAX_PATH_LENGTH];
    char disk_name[MAX_PATH_LENGTH];
    oufs_get_environment(cwd, disk_name);
    OUFILE *fileDesc;
    int c;
    char inputBuffer[(BLOCK_SIZE*BLOCKS_PER_INODE) + 1];
    int length = 0;
    char mode[2] = "r";

    // Check arguments
    if (argc == 2) {
        // Open the virtual disk
        vdisk_disk_open(disk_name);

        // Make or open the specified file
        if((fileDesc = oufs_fopen(cwd, argv[1], &mode)) == NULL)
        {
            fprintf(stderr, "Unable to open file.\n");
            return EXIT_FAILURE;
        }
        oufs_fread(fileDesc, inputBuffer, &length);

        write(1, inputBuffer, length);

        // Clean up
        vdisk_disk_close();
        return EXIT_SUCCESS;

    } else {
        // Wrong number of parameters
        fprintf(stderr, "Usage: zmore <filename>\n");
    }

}
