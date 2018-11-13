Andrew Massey
Project 3 - CS3113
andrewreaganm@ou.edu

Program Outline:
Project 3 is a virtual file system that has only directories (as of this release). The file system features 128 blocks for directories and files, which will be added in a future release.

The available functions are:
    - zformat: formats a file to represent the file system.
    - zmkdir: creates a directory, given a path.
    - zrmdir: removes a directory, given a path.
    - zfilez: lists all of the files in the CWD, or in the given path.

To set the current working directory or the vdisk location, simply run the following in your shell:
    - To set the CWD: ' export ZPWD="<absolute_path>" '
    - To set the disk location: ' export ZDISK="<path_to_disk>" '

Sources Cited
How to use qsort:
    https://linux.die.net/man/3/qsort
    https://bewuethr.github.io/sorting-strings-in-c-with-qsort/

strncpy() information:
    https://www.tutorialspoint.com/c_standard_library/c_function_strncpy.htm

Bit modification and reading from James Aspnes course website:
    http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)BitExtraction.html