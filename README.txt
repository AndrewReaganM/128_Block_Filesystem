Andrew Massey
Project 3 - CS3113
andrewreaganm@ou.edu

Program Outline:
Project 3 is a virtual file system that has only directories (as of this release). The file system features 128 blocks for directories and files, which will be added in a future release.

How To Use:
Download the files and run "make" in the directory to make the executables. After the executables are created, run "./zformat" to create and prepare the disk. The disk can now be manipulated through the available functions listed below.

The available functions are:
    - zformat: formats a file to represent the file system.
    - zmkdir <dirPath>: creates a directory, given a path.
    - zrmdir <dirPath>: removes a directory, given a path.
    - zfilez <optional: dirName or fileName>: lists all of the files in the CWD, or in the given path.
    - ztouch <filePath>: creates an empty file with a specified name.
    - zcreate <filePath>: creates a file using data from stdin. The end of the data should be a newline and EOF key. If the file already exists, it is truncated to zero and then written to.
    - zappend <filePath>: appends to or creates a file using data from stdin. The end of the data should be a newline and EOF key.
    - zmore <filePath>: copies a specified file from OUFS to stdout.
    - zremove <filePath>: removes a specified file from its parent directory. Note: if the file is linked elsewhere, the file may not actually be removed.
    - zlink <srcFilePath dstFilePath>: links an existing file to another directory entry with a provided name. Note: this does not copy the data. Throws an error if the src file does not exist, or destination parent does not exist.

To set the current working directory or the vdisk location, simply run the following in your shell:
    - To set the CWD: ' export ZPWD="<absolute_path>" '
    - To set the disk location: ' export ZDISK="<path_to_disk>" '

Assumptions:
   - There is enough space for the vdisk in the specified disk location.

Other Information:
  - File data is not removed from the disk, it is simply ignored.
  - zremove does not delete the file if other links exist.
  - zlink does not copy data - it simply links a new file name to the existing file.
  - The vdisk will always be 32768 bytes long.


Sources Cited
How to use qsort:
    https://linux.die.net/man/3/qsort
    https://bewuethr.github.io/sorting-strings-in-c-with-qsort/

strncpy() information:
    https://www.tutorialspoint.com/c_standard_library/c_function_strncpy.htm

Bit modification and reading from James Aspnes course website:
    http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)BitExtraction.html

Info about write() function:
    http://codewiki.wikidot.com/c:system-calls:write

Info about free() function:
    http://www.cs.ecu.edu/karl/3300/spr14/Notes/C/Memory/heap.html
