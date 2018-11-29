#ifndef OUFS_LIB
#define OUFS_LIB

#include "oufs.h"

#define MAX_PATH_LENGTH 200
#define TOKEN_DELIMITERS "/"

// Bit handling code based on James Aspnes course website. Accessed 11/04/2018.
// http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)BitExtraction.html
#define BITS_IN_BYTE 8

//Retrieve the nth bit of x
#define GET_BIT(var, bitINDEX) ((((var)[(bitINDEX)/BITS_IN_BYTE]) & (0x1 << ((bitINDEX) % BITS_IN_BYTE))) != 0)

//Use to set the nth bit of x
#define SET_BIT(var, bitINDEX) ((var)[(bitINDEX)/BITS_IN_BYTE]) |= (0x1 << ((bitINDEX) % BITS_IN_BYTE))

//Used to reset the nth bit of x
#define RESET_BIT(var, bitINDEX) ((var)[(bitINDEX) / BITS_IN_BYTE]) &= ~(0x1 << ((bitINDEX) % BITS_IN_BYTE))

// PROVIDED
void oufs_get_environment(char *cwd, char *disk_name);

// PROJECT 3
int oufs_format_disk(char *virtual_disk_name);

int oufs_read_inode_by_reference(INODE_REFERENCE i, INODE *inode);

int oufs_write_inode_by_reference(INODE_REFERENCE i, INODE *inode);

int oufs_find_file(char *cwd, char *path, INODE_REFERENCE *parent, INODE_REFERENCE *child, char *local_name);

int oufs_mkdir(char *cwd, char *path);

int oufs_list(char *cwd, char *path);

int oufs_rmdir(char *cwd, char *path);

void oufs_inode_reset(INODE *inode);

void oufs_clear_dblock(BLOCK *block);

// Helper functions in oufs_lib_support.c
void oufs_clean_directory_block(INODE_REFERENCE self, INODE_REFERENCE parent, BLOCK *block);

void oufs_clean_directory_entry(DIRECTORY_ENTRY *entry);

BLOCK_REFERENCE oufs_allocate_new_block();

// Helper functions to be provided
int oufs_find_open_bit(unsigned char *value);

char **oufs_path_parse(char *input, int *numberOfTokens);

int cmpstringp(const void *p1, const void *p2);

OUFILE *oufs_fopen(char *cwd, char *path, char *mode);

void oufs_fclose(OUFILE *fp);

int oufs_fwrite(OUFILE *fp, unsigned char *buf, int len);

int oufs_fread(OUFILE *fp, unsigned char *buf, int *len);

int oufs_remove(char *cwd, char *path);

int oufs_link(char *cwd, char *path_src, char *path_dst);

#endif
