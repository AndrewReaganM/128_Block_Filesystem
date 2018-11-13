/*******
 * Low-level file system definitions
 *
 * DO NOT CHANGE THIS DATA STRUCTURE
 *
 * CS 3113
 *
 * 
 */

// Only evaluate these definitions once, even if included multiple times
#ifndef FILE_STRUCTS_H
#define FILE_STRUCTS_H

#include <string.h>
#include <limits.h>
#include "vdisk.h"

// Implementation of min operator
#define MIN(a, b) (((a) > (b)) ? (b) : (a))

/**********************************************************************/
/*
File system layout onto disk blocks:

Block 0: Master block
Blocks 1 ... N_INODE_BLOCKS: inodes
Blocks N_INODE_BLOCKS+1 ... N_BLOCKS_ON_DISK-1: data for files and directories
   (Block N_BLOCKS+1 is allocated for the root directory)
*/

/**********************************************************************/
// Basic types and sizes
// Chosen carefully so that all block types pack nicely into a full block

// An index that refers to an inode
typedef unsigned short INODE_REFERENCE;
// Value used as an index when it does not refer to an inode
#define UNALLOCATED_INODE (USHRT_MAX-1)

// Value used as an index when it does not refer to a block
#define UNALLOCATED_BLOCK USHRT_MAX
#define UNALLOCATED_BLOCK USHRT_MAX

// Number of inode blocks on the virtual disk
#define N_INODE_BLOCKS 8

// The block on the virtual disk containing the root directory
#define ROOT_DIRECTORY_BLOCK (N_INODE_BLOCKS + 1)

// Size of file/directory name
#define FILE_NAME_SIZE (16 - sizeof(INODE_REFERENCE))

// Number of data block references in an inode.  Just big enough to fit a reasonable
//  number of inodes into a single block
#define BLOCKS_PER_INODE (16-1)

/**********************************************************************/
// Data block: storage for file contents (project 4!)
typedef struct data_block_s {
    unsigned char data[BLOCK_SIZE];
} DATA_BLOCK;


/**********************************************************************/
// Inode Types
#define IT_NONE 'N'
#define IT_DIRECTORY 'D'
#define IT_FILE 'F'

// Single inode
typedef struct inode_s {
    // IT_NONE, IT_DIRECTORY, IT_FILE
    char type;

    // Number of directories references to this inode
    unsigned char n_references;

    // Contents.  UNALLOCATED_BLOCK means that this entry is not used
    BLOCK_REFERENCE data[BLOCKS_PER_INODE];

    // File: size in bytes; Directory: number of directory entries (including . and ..)
    unsigned int size;
} INODE;

// Number of inodes stored in each block
#define INODES_PER_BLOCK (BLOCK_SIZE/sizeof(INODE))

// Total number of inodes in the file system
#define N_INODES (INODES_PER_BLOCK * N_INODE_BLOCKS)

// Block of inodes
typedef struct inode_block_s {
    INODE inode[INODES_PER_BLOCK];
} INODE_BLOCK;


/**********************************************************************/
// Block 0
#define MASTER_BLOCK_REFERENCE 0

typedef struct master_block_s {
    // 8 inodes per byte: One inode per bit: 1 = allocated, 0 = free
    // The first inode is byte 0, bit 0
    unsigned char inode_allocated_flag[N_INODES >> 3];

    // 8 data blocks per byte: One block per bit: 1 = allocated, 0 = free
    // Block 0 (the master block) is byte 0, bit 0
    unsigned char block_allocated_flag[N_BLOCKS_IN_DISK >> 3];
} MASTER_BLOCK;

/**********************************************************************/
// Single directory element
typedef struct directory_entry_s {
    // Name of file/directory
    char name[FILE_NAME_SIZE];

    // UNALLOCATED_INODE if this directory entry is non-existent
    INODE_REFERENCE inode_reference;

} DIRECTORY_ENTRY;

// Number of directory entries stored in one data block
#define DIRECTORY_ENTRIES_PER_BLOCK (BLOCK_SIZE / sizeof(DIRECTORY_ENTRY))

// Directory block
typedef struct directory_block_s {
    DIRECTORY_ENTRY entry[DIRECTORY_ENTRIES_PER_BLOCK];
} DIRECTORY_BLOCK;

/**********************************************************************/
// All-encompassing structure for a disk block
// The union says that all 4 of these elements occupy overlapping bytes in 
//  memory (hence, a block will only be one of these 4 at any given time)
typedef union block_u {
    DATA_BLOCK data;
    MASTER_BLOCK master;
    INODE_BLOCK inodes;
    DIRECTORY_BLOCK directory;
} BLOCK;


/**********************************************************************/
// Representing files (project 4!)

typedef struct oufile_s {
    INODE_REFERENCE inode_reference;
    char mode;
    int offset;
} OUFILE;


#endif
