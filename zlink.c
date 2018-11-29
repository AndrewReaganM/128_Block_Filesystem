/**
Create a file reference in the OU File System.

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
    int c;
    char inputBuffer[(BLOCK_SIZE*BLOCKS_PER_INODE) + 1];

    // Check arguments
    if (argc == 3) {
        // Open the virtual disk
        vdisk_disk_open(disk_name);

        //Remove file entry
        oufs_link(cwd, argv[1], argv[2]);

        // Clean up
        vdisk_disk_close();

    } else {
        // Wrong number of parameters
        fprintf(stderr, "Usage: zremove <existing> <new_name>\n");
    }

}
