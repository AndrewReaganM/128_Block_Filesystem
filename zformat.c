#include <stdio.h>
#include <string.h>
#include "zformat.h"
#include "oufs_lib.h"

/**
 * Formats a file to be used by the OUFS file ssytem
 *
 * @param argc number of arguments
 * @param argv cstring array of arguments
 * @return int representing the success of the program.
 */
int main(int argc, char **argv) {

    char currentDir[PATH_MAX];
    char currentName[NAME_MAX];
    getcwd(currentDir, PATH_MAX);
    oufs_get_environment(currentDir, currentName);

    oufs_format_disk(currentName);

    return EXIT_SUCCESS;
}