#include <stdio.h>
#include <string.h>

#include "oufs_lib.h"

int main(int argc, char** argv) {
	// Get the key environment variables
	char cwd[MAX_PATH_LENGTH];
	char disk_name[MAX_PATH_LENGTH];
	oufs_get_environment(cwd, disk_name);

	if(vdisk_disk_open(disk_name) != 0) {
		return(-1);
	}

	if(argc == 2){
		if(strncmp(argv[1], "-master", 8) == 0) {
			// Master record
			BLOCK block;
			if(vdisk_read_block(0, &block) != 0) {
				fprintf(stderr, "Error reading master block\n");
			}else{
				// Block read: report state
				printf("Inode table:\n");
				for(int i = 0; i < INODES_PER_BLOCK *  N_INODE_BLOCKS / 8; ++i) {
					printf("%02x\n", block.master.inode_allocated_flag[i]);
				}
				printf("Block table:\n");
				for(int i = 0; i < N_BLOCKS_IN_DISK / 8; ++i) {
					printf("%02x\n", block.master.block_allocated_flag[i]);
				}
			}

		}else{
			fprintf(stderr, "Unknown argument (%s)\n", argv[1]);
		}

	}else if(argc == 3) {
		if(strncmp(argv[1], "-inode", 7) == 0) {
			// Inode query
			int index;
			if(sscanf(argv[2], "%d", &index) == 1){
				if(index < 0 || index >= N_INODES) {
					fprintf(stderr, "Inode index out of range (%s)\n", argv[2]);
				}else{
					INODE inode;
					oufs_read_inode_by_reference(index, &inode);

					printf("Inode: %d\n", index);
					printf("Type: %c\n", inode.type);
					for(int i = 0; i < BLOCKS_PER_INODE; ++i) {
						printf("Block %d: %d\n", i, inode.data[i]);
					}
					printf("Size: %d\n", inode.size);

				}
			}else{
				fprintf(stderr, "Unknown argument (-inode %s)\n", argv[2]);
			}
		}else if(strncmp(argv[1], "-inodee", 8) == 0) {
			// Extended Inode query
			int index;
			if(sscanf(argv[2], "%d", &index) == 1){
				if(index < 0 || index >= N_INODES) {
					fprintf(stderr, "Inode index out of range (%s)\n", argv[2]);
				}else{
					INODE inode;
					oufs_read_inode_by_reference(index, &inode);

					printf("Inode: %d\n", index);
					printf("Type: %c\n", inode.type);
					printf("N references: %d\n", inode.n_references);
					for(int i = 0; i < BLOCKS_PER_INODE; ++i) {
						printf("Block %d: %d\n", i, inode.data[i]);
					}
					printf("Size: %d\n", inode.size);

				}
			}else{
				fprintf(stderr, "Unknown argument (-inode %s)\n", argv[2]);
			}
		}else if(strncmp(argv[1], "-dblock", 8) == 0) {
			// Inspect directory block
			int index;
			if(sscanf(argv[2], "%d", &index) == 1){
				if(index < 0 || index >= N_BLOCKS_IN_DISK) {
					fprintf(stderr, "Block index out of range (%s)\n", argv[2]);
				}else{
					BLOCK block;
					vdisk_read_block(index, &block);
					printf("Directory at block %d:\n", index);
					for(int i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i) {
						if(block.directory.entry[i].inode_reference != UNALLOCATED_INODE) {
							printf("Entry %d: name=\"%s\", inode=%d\n", i, block.directory.entry[i].name,
								   block.directory.entry[i].inode_reference);
						}
					}
				}
			}
		}else if(strncmp(argv[1], "-raw", 4) == 0) {
			// Inspect raw block
			int index;
			if(sscanf(argv[2], "%d", &index) == 1){
				if(index < 0 || index >= N_BLOCKS_IN_DISK) {
					fprintf(stderr, "Block index out of range (%s)\n", argv[2]);
				}else{
					BLOCK block;
					vdisk_read_block(index, &block);
					printf("Raw data at block %d:\n", index);
					for(int i = 0; i < BLOCK_SIZE; ++i) {
						if(block.data.data[i] >= ' ' && block.data.data[i] <= '~')
							printf("%3d: %02x %c\n", i, block.data.data[i], block.data.data[i]);
						else
							printf("%3d: %02x\n", i, block.data.data[i]);
					}
				}
			}
		}

	}

	vdisk_disk_close();
}
