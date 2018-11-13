#include "oufs_lib.h"

#define debug 0
/**
 * Function that formats the virtual disk per the specification given in oufs.h
 *
 * @param virtual_disk_name the name of the virtual disk.
 * @return the success of the program, either EXIT_FAILURE or EXIT SUCCESS
 */
int oufs_format_disk(char *virtual_disk_name) {

    if (vdisk_disk_open(virtual_disk_name) != 0) {
        fprintf(stderr, "oufs_format_disk: Error opening disk or another disk already opened.\n");
        //return(EXIT_FAILURE);
    }
    /************************** Write zeroes to entire disk *****************************/
    DATA_BLOCK zero_block;
    memset(zero_block.data, 0, 256);
    for (int j = 0; j < 128; j++) {
        vdisk_write_block(j, &zero_block);
    }

    /*********************************** Block Setup ***********************************/

    //Set up master block
    BLOCK mblock;
    SET_BIT(mblock.master.block_allocated_flag, MASTER_BLOCK_REFERENCE);

    //Mark inode block???????
    SET_BIT(mblock.master.inode_allocated_flag, MASTER_BLOCK_REFERENCE);

    //Set up inode blocks
    BLOCK iblock;
    iblock.inodes.inode[0].type = IT_NONE;

    for (int i = 0; i < INODES_PER_BLOCK; i++) {
        iblock.inodes.inode[i].type = IT_NONE;
        iblock.inodes.inode[i].n_references = 0;
        iblock.inodes.inode[i].size = 0;
        for (int j = 0; j < BLOCKS_PER_INODE; j++) {
            iblock.inodes.inode[i].data[j] = UNALLOCATED_BLOCK;
        }
    }

    // Write unallocated inode blocks and set the bits in the master block to be written later.
    for (int i = 1; i <= N_INODE_BLOCKS; i++) {
        vdisk_write_block(i, &iblock);
        SET_BIT(mblock.master.block_allocated_flag, i);
    }

    iblock.inodes.inode[0].type = IT_DIRECTORY;
    iblock.inodes.inode[0].n_references = 1;
    iblock.inodes.inode[0].size = 2;
    iblock.inodes.inode[0].data[0] = 9;

    vdisk_write_block(1, &iblock);
    //Create blank directory block.
    BLOCK dirBlock;

    //Set both . and .. to the root itself.
    strncpy(dirBlock.directory.entry[0].name, ".", FILE_NAME_SIZE-1);
    dirBlock.directory.entry[0].inode_reference = 0;
    strncpy(dirBlock.directory.entry[1].name, "..", FILE_NAME_SIZE-1);
    dirBlock.directory.entry[1].inode_reference = 0;

    //Initialize the rest of the directory entries as unallocated and clear names.
    for(int i=2; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        strncpy(dirBlock.directory.entry[i].name, "", FILE_NAME_SIZE-1);
        dirBlock.directory.entry[i].inode_reference = UNALLOCATED_INODE;
    }

    //Write root directory block.
    vdisk_write_block(N_INODE_BLOCKS+1, &dirBlock);
    SET_BIT(mblock.master.block_allocated_flag, N_INODE_BLOCKS+1);

    //Write Master Block
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &mblock);

    if(debug)
        fprintf(stderr, "Disk successfully formatted.\n");

    vdisk_disk_close();

    return EXIT_SUCCESS;
}
/**
 * Function to make new directories in the OUFS file system.
 * @param cwd the current working directory of the open disk.
 * @param path the user given path of the directory to be made.
 * @return the success of the program, either EXIT_FAILURE or EXIT SUCCESS
 */
int oufs_mkdir(char *cwd, char *path) {
    INODE_REFERENCE child, parent;
    char local_name[FILE_NAME_SIZE];

    //Find where the directory should be located.
    if(oufs_find_file(cwd, path, &parent, &child, &local_name) == EXIT_FAILURE)
    {
        fprintf(stderr, "Unable to traverse CWD or provided path.\n");
        return EXIT_FAILURE;
    }

    INODE parentINODE;
    oufs_read_inode_by_reference(parent, &parentINODE); //Read parent inode

    BLOCK parentBlock;
    vdisk_read_block(parentINODE.data[0], &parentBlock); //Read parent parentBlock from inode reference

    for(int i=0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
    {
        if(strncmp(parentBlock.directory.entry[i].name, local_name, FILE_NAME_SIZE) == 0){
            //TODO: Support file and directory with same name.
            fprintf(stderr, "directory '%s' already exists\n", local_name);
            return EXIT_FAILURE;
        }
    }

    // Create the directory in the current parentBlock.

    // Find an open inode, read master parentBlock and search.
    BLOCK masterBlock;
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &masterBlock);

    int openINODE = oufs_find_open_bit(masterBlock.master.inode_allocated_flag);
    int openBLOCK = oufs_find_open_bit(masterBlock.master.block_allocated_flag);

    if((openBLOCK == -1) || (openINODE == -1))
    {
        fprintf(stderr, "Either out of open inodes or blocks.\n");
        return EXIT_FAILURE;
    }

    // Mark bits as allocated
    SET_BIT(masterBlock.master.inode_allocated_flag, openINODE);
    SET_BIT(masterBlock.master.block_allocated_flag, openBLOCK);

    for(int i=0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
    {
        if(strncmp(parentBlock.directory.entry[i].name, "", FILE_NAME_SIZE) == 0)
        {
            strncpy(parentBlock.directory.entry[i].name, local_name, FILE_NAME_SIZE-1);
            parentBlock.directory.entry[i].name[FILE_NAME_SIZE-1] = 0; //Guarantee null termination.
            parentBlock.directory.entry[i].inode_reference = (INODE_REFERENCE) openINODE;
            parentINODE.size += 1;
            break;
        }
    }

    //Read inode block of new directory.
    INODE newINODE;
    newINODE.type = IT_DIRECTORY;
    newINODE.n_references = 1;
    newINODE.data[0] = (BLOCK_REFERENCE) (openBLOCK);

    for (int j = 1; j < BLOCKS_PER_INODE; j++) {
        newINODE.data[j] = UNALLOCATED_BLOCK;
    }

    newINODE.size = 2;

    //Set up directory block.
    BLOCK newDBLOCK;
    strncpy(newDBLOCK.directory.entry[0].name, ".", FILE_NAME_SIZE-1);
    newDBLOCK.directory.entry[0].inode_reference = (INODE_REFERENCE) openINODE;
    strncpy(newDBLOCK.directory.entry[1].name, "..", FILE_NAME_SIZE-1);
    newDBLOCK.directory.entry[1].inode_reference = parent;

    //Initialize the rest of the directory entries as unallocated and clear names.
    for(int i=2; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        strncpy(newDBLOCK.directory.entry[i].name, "", FILE_NAME_SIZE-1);
        newDBLOCK.directory.entry[i].inode_reference = UNALLOCATED_INODE;
    }

    //Write back the approprite blocks and inodes.
    vdisk_write_block(parentINODE.data[0], &parentBlock);
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &masterBlock);
    oufs_write_inode_by_reference(openINODE, &newINODE);
    oufs_write_inode_by_reference(parent, &parentINODE);
    vdisk_write_block(openBLOCK, &newDBLOCK);

    return EXIT_SUCCESS;
}
/**
 * Function used to traverse the file structure one token at a time to find a given file or directoyr.
 * @param cwd input of the current working directory.
 * @param path input of the program specified path.
 * @param parent INODE_REFERENCE pointer to the inode of the parent.
 * @param child INODE_REFERENCE pointer to the inode of the child.
 * @param local_name the name of the final chunk of the given path.
 * @return the success of the program, either EXIT_FAILURE or EXIT SUCCESS
 */
int oufs_find_file(char *cwd, char *path, INODE_REFERENCE *parent, INODE_REFERENCE *child, char *local_name) {
    int numTok = 0;
    BLOCK currentBlock;
    INODE currentINODE;
    int status = 1;

    vdisk_read_block(ROOT_DIRECTORY_BLOCK, &currentBlock);
    *parent = 0;
    *child = 0;

    if(strncmp(path, "/", 1) != 0) //If the path is not absolute, navigate to CWD.
    {
        //fprintf(stderr, "Path is relative. Navigating to CWD.\n");
        char** tokenizedCWD = oufs_path_parse(cwd, &numTok); //Tokenize the CWD
        for(int i=0; i < numTok; ++i) //For each part of the path
        {
            if(status == 1)
                status = 0;
            //fprintf(stderr, "Working on finding '%s'\n", tokenizedCWD[i]);
            for(int j=0; j < DIRECTORY_ENTRIES_PER_BLOCK; ++j) //Look through all the entries in the dirblock
            {
                if(strncmp(tokenizedCWD[i], currentBlock.directory.entry[j].name, FILE_NAME_SIZE) == 0)
                {
                    oufs_read_inode_by_reference(currentBlock.directory.entry[j].inode_reference, &currentINODE);
                    if(currentINODE.type == IT_DIRECTORY)
                    {
                        //fprintf(stderr, "Reading block from current INODE reference.\n");
                        vdisk_read_block(currentINODE.data[0], &currentBlock);
                        status = 1;
                    }
                }
                if(status == 1)
                    break;
            }
            if(status == 0)
            {
                fprintf(stderr, "Unable to locate directory '%s'. CWD is likely invalid.\n", tokenizedCWD[i]);
                return EXIT_FAILURE;
            }
        }
    }

    //At this point, path is either absolute, or currentBlock is at the CWD block.
    //fprintf(stderr, "Traversing user provided path.\n");
    int pathNumTok;
    char** tokenizedPath = oufs_path_parse(path, &pathNumTok); //Tokenize the CWD
    for(int i=0; i < pathNumTok; ++i) //For each part of the path
    {
        if(status == 1)
            status = 0;
        //fprintf(stderr, "Working on finding '%s'\n", tokenizedPath[i]);
        for(int j=0; j < DIRECTORY_ENTRIES_PER_BLOCK; ++j) //Look through all the entries in the dirblock
        {
            if(strncmp(tokenizedPath[i], currentBlock.directory.entry[j].name, FILE_NAME_SIZE) == 0) {
                oufs_read_inode_by_reference(currentBlock.directory.entry[j].inode_reference, &currentINODE);


                if((currentINODE.type == IT_DIRECTORY) && (i != pathNumTok-1)) {
                    fprintf(stderr, "Reading INODE and block from current INODE reference (parent).\n");
                    *parent = currentBlock.directory.entry[j].inode_reference;
                    vdisk_read_block(currentINODE.data[0], &currentBlock);
                    status = 1;
                }
                else if((currentINODE.type == IT_DIRECTORY) && (i == pathNumTok-1))
                {
                    fprintf(stderr, "Setting child to current inode reference.\n");
                    *child = currentBlock.directory.entry[j].inode_reference;
                    status = 1;
                }

            }
            if(status == 1)
                break;
        }
        if((status == 0) && numTok > 1) {
            fprintf(stderr, "Unable to locate directory or file '%s'.\n", tokenizedPath[i]);
            return EXIT_FAILURE;
        }
    }
    strncpy(local_name, tokenizedPath[pathNumTok-1], FILE_NAME_SIZE-1);


    return EXIT_SUCCESS;
}
/**
 * Command similar to 'ls' but for OUFS. Lists files in ASCII order.
 *
 * @param cwd input of the current working directory.
 * @param path input of the program specified path.
 * @return the success of the program, either EXIT_FAILURE or EXIT SUCCESS
 */
int oufs_list(char *cwd, char *path)
{
    INODE_REFERENCE child, parent;
    INODE parentINODE;

    BLOCK parentBLOCK;

    char local_name[FILE_NAME_SIZE];

    //Find where the directory should be located.
    if(oufs_find_file(cwd, path, &parent, &child, &local_name) == EXIT_FAILURE)
    {
        fprintf(stderr, "Unable to traverse CWD or provided path.\n");
        return EXIT_FAILURE;
    }

    oufs_read_inode_by_reference(parent, &parentINODE);
    vdisk_read_block(parentINODE.data[0], &parentBLOCK);

    INODE childINODE;
    BLOCK childBLOCK;
    int childStatus = 0;
    int locationInParent;
    for(locationInParent=0; locationInParent < DIRECTORY_ENTRIES_PER_BLOCK; ++locationInParent)
    {
        if(strncmp(parentBLOCK.directory.entry[locationInParent].name, local_name, FILE_NAME_SIZE) == 0){
            //TODO: Support file and directory with same name.
            //fprintf(stderr, "Found local_name '%s' in parent block.\n", local_name);
            child = parentBLOCK.directory.entry[locationInParent].inode_reference;
            oufs_read_inode_by_reference(parentBLOCK.directory.entry[locationInParent].inode_reference, &childINODE);

            childStatus = 1;
            break;
        }

    }
    if(childStatus == 0) //Fail if not in specified parent.
    {
        fprintf(stderr, "Specified directory (%s) not found in parent. Exiting...\n", local_name);
        return EXIT_FAILURE;
    }
    vdisk_read_block(childINODE.data[0], &childBLOCK);

    char* itemList[DIRECTORY_ENTRIES_PER_BLOCK];
    size_t listInc = 0;

    for(int i=0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        if(strnlen(childBLOCK.directory.entry[i].name, FILE_NAME_SIZE) > 0)
        {
            itemList[listInc] = childBLOCK.directory.entry[i].name;
            listInc++;
        }
    }

    qsort(itemList, listInc, sizeof(char*), cmpstringp);

    for(int i=0; i < listInc; ++i)
    {
        //TODO: Support for files (no /)
        printf("%s/\n", itemList[i]);
    }
    return EXIT_SUCCESS;
}

/**
 * Finds the first open bit in a given char.
 * @param value the beginning of the char array.
 * @return the first available open bit, or -1 for failure.
 */
int oufs_find_open_bit(unsigned char *value) {
    for(int i=0; i <= (sizeof(value) * 8); ++i)
    {
        if(GET_BIT(value, i) == 0)
        {
            return i;
        }
    }
    return (-1);
}
/**
 * Function to parse a path, using delimiters specified in TOKEN_DELIMITERS.
 *
 * @param input the path to be tokenized.
 * @param numberOfTokens the number of tokens that were created.
 * @return a char** of the tokens.
 */
char **oufs_path_parse(char *input, int *numberOfTokens) {
    int bufferSize = FILE_NAME_SIZE;
    int numTok = 0;
    char **tokenizedData = calloc(bufferSize, sizeof(char *));
    char *tokenPointer;

    //Allocating memory
    if (!tokenizedData) {
        fprintf(stderr, "trsh_INPUTPARSE: memory allocation error - pointer array\n");
        exit(EXIT_FAILURE);
    }
    tokenPointer = strtok(input, TOKEN_DELIMITERS); //Get a pointer to the first token.
    while (tokenPointer != NULL) { //Checking to see if token exists.
        tokenizedData[numTok] = tokenPointer;
        if (numTok >= bufferSize) {
            bufferSize += bufferSize;
            tokenizedData = realloc(tokenizedData, bufferSize * sizeof(char *));
            if (!tokenizedData) {
                fprintf(stderr, "trsh_INPUTPARSE: error reallocating memory - pointer array\n");
                exit(EXIT_FAILURE);
            }
        }
        numTok++;
        tokenPointer = strtok(0, TOKEN_DELIMITERS); //Get next pointer
    }
    *numberOfTokens = numTok;
    return tokenizedData; //Return the array of pointers.
}
/**
 * Function to remove a directory at the end of a specified path.
 * @param cwd input of the current working directory.
 * @param path input of the program specified path.
 * @return the success of the program, either EXIT_FAILURE or EXIT SUCCESS
 */
int oufs_rmdir(char *cwd, char *path) {
    INODE_REFERENCE child, parent;
    char local_name[FILE_NAME_SIZE];

    //Find where the directory should be located.
    if(oufs_find_file(cwd, path, &parent, &child, &local_name) == EXIT_FAILURE)
    {
        fprintf(stderr, "Unable to traverse CWD or provided path.\n");
        return EXIT_FAILURE;
    }

    INODE childINODE, parentINODE;
    BLOCK childBLOCK, parentBLOCK;

    oufs_read_inode_by_reference(parent, &parentINODE);
    vdisk_read_block(parentINODE.data[0], &parentBLOCK);

    int childStatus = 0;
    int locationInParent;
    BLOCK_REFERENCE childBlockRef;
    for(locationInParent=0; locationInParent < DIRECTORY_ENTRIES_PER_BLOCK; ++locationInParent)
    {
        if(strncmp(parentBLOCK.directory.entry[locationInParent].name, local_name, FILE_NAME_SIZE) == 0){
            //TODO: Support file and directory with same name.
            //fprintf(stderr, "Found local_name '%s' in parent block.\n", local_name);
            child = parentBLOCK.directory.entry[locationInParent].inode_reference;
            oufs_read_inode_by_reference(parentBLOCK.directory.entry[locationInParent].inode_reference, &childINODE);
            vdisk_read_block(childINODE.data[0], &childBLOCK);
            childBlockRef = childINODE.data[0];
            childStatus = 1;
            break;
        }

    }
    if(childStatus == 0) //Fail if not in specified parent.
    {
        fprintf(stderr, "Specified directory (%s) not found in parent. Exiting...\n", local_name);
        return EXIT_FAILURE;
    }

    if((childINODE.type != IT_DIRECTORY)) //Fail if not directory.
    {
        fprintf(stderr, "Given path termination point (%s) is not a directory.\n", local_name);
        return EXIT_FAILURE;
    }
    if(childINODE.size > 2) //Fail if not empty.
    {
        fprintf(stderr, "Given path termination point (%s) is not empty.\n", local_name);
        return EXIT_FAILURE;
    }

    /************************************** BEGIN EDITING **************************************/

    //Edit master block
    BLOCK masterBLOCK;
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &masterBLOCK);
    RESET_BIT(masterBLOCK.master.block_allocated_flag, childINODE.data[0]);
    RESET_BIT(masterBLOCK.master.inode_allocated_flag, child);

    //Create a clean directory block to write to disk.
    BLOCK cleanDBLOCK;
    oufs_clear_dblock(&cleanDBLOCK);

    //Clean up parent
    oufs_clean_directory_entry(&parentBLOCK.directory.entry[locationInParent]); //Clean out the parent entry
    --parentINODE.size;
    oufs_inode_reset(&childINODE);

    //Write Parent INODE and BLOCK
    oufs_write_inode_by_reference(parent, &parentINODE);
    vdisk_write_block(parentINODE.data[0], &parentBLOCK);

    //WRITE MASTER BLOCK
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &masterBLOCK);

    //Write Child INODE and BLOCK
    oufs_write_inode_by_reference(child, &childINODE);
    vdisk_write_block(childBlockRef, &cleanDBLOCK);



    return EXIT_SUCCESS;
}

OUFILE *oufs_fopen(char *cwd, char *path, char *mode)
{
    INODE_REFERENCE child, parent;
    char local_name[FILE_NAME_SIZE];

    //Find where the directory should be located.
    if(oufs_find_file(cwd, path, &parent, &child, &local_name) == EXIT_FAILURE)
    {
        fprintf(stderr, "Unable to traverse CWD or provided path.\n");
        return EXIT_FAILURE;
    }

    
}
/**
 * Function to reset a given inode.
 * @param inode the address of an inode to be reset.
 */
void oufs_inode_reset(INODE *inode) {
    (*inode).size = 0;
    (*inode).data[0] = UNALLOCATED_BLOCK;
    (*inode).type = IT_NONE;
    (*inode).n_references = 0;
}
/**
 * Function to set a given block to all zeroes.
 * @param block the block to be zeroed out.
 */
void oufs_clear_dblock(BLOCK *block) {
    memset(block, 0, 256);
}