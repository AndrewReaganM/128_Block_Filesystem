#ifndef VDISK_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned short BLOCK_REFERENCE;

// Size of block in bytes
#define BLOCK_SIZE 256

// Total number of blocks on the virtual disk
#define N_BLOCKS_IN_DISK 128

int vdisk_disk_open(char *virtual_disk_name);

int vdisk_disk_close();

int vdisk_read_block(BLOCK_REFERENCE block_ref, void *block);

int vdisk_write_block(BLOCK_REFERENCE block_ref, void *block);

#endif
