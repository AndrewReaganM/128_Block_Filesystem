
#include "oufs_lib.h"

#define debug 0

/**
 * Read the ZPWD and ZDISK environment variables & copy their values into cwd and disk_name.
 * If these environment variables are not set, then reasonable defaults are given.
 *
 * @param cwd String buffer in which to place the OUFS current working directory.
 * @param disk_name String buffer containing the file name of the virtual disk.
 */
void oufs_get_environment(char *cwd, char *disk_name) {
    // Current working directory for the OUFS
    char *str = getenv("ZPWD");
    if (str == NULL) {
        // Provide default
        strcpy(cwd, "/");
    } else {
        // Exists
        strncpy(cwd, str, MAX_PATH_LENGTH - 1);
    }

    // Virtual disk location
    str = getenv("ZDISK");
    if (str == NULL) {
        // Default
        strcpy(disk_name, "vdisk1");
    } else {
        // Exists: copy
        strncpy(disk_name, str, MAX_PATH_LENGTH - 1);
    }

}

/**
 * Configure a directory entry so that it has no name and no inode
 *
 * @param entry The directory entry to be cleaned
 */
void oufs_clean_directory_entry(DIRECTORY_ENTRY *entry) {
    entry->name[0] = 0;  // No name
    entry->inode_reference = UNALLOCATED_INODE;
}

/**
 * Initialize a directory block as an empty directory
 *
 * @param self Inode reference index for this directory
 * @param self Inode reference index for the parent directory
 * @param block The block containing the directory contents
 *
 */

void oufs_clean_directory_block(INODE_REFERENCE self, INODE_REFERENCE parent, BLOCK *block) {
    // Debugging output
    if (debug)
        fprintf(stderr, "New clean directory: self=%d, parent=%d\n", self, parent);

    // Create an empty directory entry
    DIRECTORY_ENTRY entry;
    oufs_clean_directory_entry(&entry);

    // Copy empty directory entries across the entire directory list
    for (int i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i) {
        block->directory.entry[i] = entry;
    }

    // Now we will set up the two fixed directory entries

    // Self
    strncpy(entry.name, ".", 2);
    entry.inode_reference = self;
    block->directory.entry[0] = entry;

    // Parent (same as self
    strncpy(entry.name, "..", 3);
    entry.inode_reference = parent;
    block->directory.entry[1] = entry;

}

/**
 * Allocate a new data block
 *
 * If one is found, then the corresponding bit in the block allocation table is set
 *
 * @return The index of the allocated data block.  If no blocks are available,
 * then UNALLOCATED_BLOCK is returned
 *
 */
BLOCK_REFERENCE oufs_allocate_new_block() {
    BLOCK block;
    // Read the master block
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &block);

    // Scan for an available block
    int block_byte;
    int flag;

    // Loop over each byte in the allocation table.
    for (block_byte = 0, flag = 1; flag && block_byte < N_BLOCKS_IN_DISK / 8; ++block_byte) {
        if (block.master.block_allocated_flag[block_byte] != 0xff) {
            // Found a byte that has an opening: stop scanning
            flag = 0;
            break;
        };
    };
    // Did we find a candidate byte in the table?
    if (flag == 1) {
        // No
        if (debug)
            fprintf(stderr, "No blocks\n");
        return (UNALLOCATED_BLOCK);
    }

    // Found an available data block

    // Set the block allocated bit
    // Find the FIRST bit in the byte that is 0 (we scan in bit order: 0 ... 7)
    int block_bit = oufs_find_open_bit(&block.master.block_allocated_flag[block_byte]);

    // Now set the bit in the allocation table
    block.master.block_allocated_flag[block_byte] |= (1 << block_bit);

    // Write out the updated master block
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &block);

    if (debug)
        fprintf(stderr, "Allocating block=%d (%d)\n", block_byte, block_bit);

    // Compute the block index
    BLOCK_REFERENCE block_reference = (block_byte << 3) + block_bit;

    if (debug)
        fprintf(stderr, "Allocating block=%d\n", block_reference);

    // Done
    return (block_reference);
}


/**
 *  Given an inode reference, read the inode from the virtual disk.
 *
 *  @param i Inode reference (index into the inode list)
 *  @param inode Pointer to an inode memory structure.  This structure will be
 *                filled in before return)
 *  @return 0 = successfully loaded the inode
 *         -1 = an error has occurred
 *
 */
int oufs_read_inode_by_reference(INODE_REFERENCE i, INODE *inode) {
    if (debug)
        fprintf(stderr, "Fetching inode %d\n", i);

    // Find the address of the inode block and the inode within the block
    BLOCK_REFERENCE block = i / INODES_PER_BLOCK + 1;
    int element = (i % INODES_PER_BLOCK);

    BLOCK b;
    if (vdisk_read_block(block, &b) == 0) {
        // Successfully loaded the block: copy just this inode
        *inode = b.inodes.inode[element];
        return (0);
    }
    // Error case
    return (-1);
}

int oufs_write_inode_by_reference(INODE_REFERENCE i, INODE *inode) {
    if (debug)
        fprintf(stderr, "Fetching inode %d\n", i);

    // Find the address of the inode block and the inode within the block
    BLOCK_REFERENCE block = i / INODES_PER_BLOCK + 1;
    int element = (i % INODES_PER_BLOCK);

    BLOCK b;
    if (vdisk_read_block(block, &b) == 0) {
        // Successfully loaded the block: copy just this inode
        b.inodes.inode[element] = *inode;
        if(vdisk_write_block(block, &b) != 0)
            return EXIT_FAILURE;
        return (0);
    }
    // Error case
    return (-1);
}
/**
 * Function to compare two items with qsort.
 *
 * Code recommended from qsort() man pages.
 *
 * @param p1 value one
 * @param p2 value two
 */
int cmpstringp(const void *p1, const void *p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */

    return strcmp(*(char *const*)p1, *(char *const*)p2);
}
