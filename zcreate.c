/**
Open a file for writing in the OU File System.

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

    char mode[2] = "w";

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
        //fprintf(stderr, "%i\n", getpid());
        int i = 0;
        while((inputBuffer[i] = getchar()) != EOF)
        {
            i++;
            if(i >= (BLOCK_SIZE*BLOCKS_PER_INODE))
                break;
        }
        oufs_fwrite(fileDesc, &inputBuffer, i);

        // Clean up
        oufs_fclose(fileDesc);
        vdisk_disk_close();

    } else {
        // Wrong number of parameters
        fprintf(stderr, "Usage: zcreate <filename>\n");
    }

}
