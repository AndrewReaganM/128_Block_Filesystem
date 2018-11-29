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
    if(parentINODE.size >= DIRECTORY_ENTRIES_PER_BLOCK)
    {
        fprintf(stderr, "The specified parent is already full.\n");
        return EXIT_FAILURE;
    }

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
    *child = UNALLOCATED_INODE;

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
                        *parent = currentBlock.directory.entry[j].inode_reference;
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
    for(int i=0; i < pathNumTok-1; ++i) //For each part of the path
    {
        if(status == 1)
            status = 0;
        //fprintf(stderr, "Working on finding '%s'\n", tokenizedPath[i]);
        for(int j=0; j < DIRECTORY_ENTRIES_PER_BLOCK; ++j) //Look through all the entries in the dirblock
        {
            if(strncmp(tokenizedPath[i], currentBlock.directory.entry[j].name, FILE_NAME_SIZE) == 0) {
                oufs_read_inode_by_reference(currentBlock.directory.entry[j].inode_reference, &currentINODE);

                *parent = currentBlock.directory.entry[j].inode_reference;
                if(currentINODE.type == IT_DIRECTORY) {
                    //fprintf(stderr, "Reading block from current INODE reference.\n");
                    vdisk_read_block(currentINODE.data[0], &currentBlock);
                    status = 1;
                }
            }
            if(status == 1)
                break;
        }
        if(status == 0) {
            fprintf(stderr, "Unable to locate directory or file '%s'.\n", tokenizedPath[i]);
            *parent = UNALLOCATED_INODE;
            return EXIT_FAILURE;
        }
    }

    strncpy(local_name, tokenizedPath[pathNumTok-1], FILE_NAME_SIZE-1);
    for(int i=0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i) {
        if (strncmp(currentBlock.directory.entry[i].name, local_name, FILE_NAME_SIZE) == 0) {
            (*child) = currentBlock.directory.entry[i].inode_reference;
            break;
        }
    }

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

    //Read parent inode and block.
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
    char* typeList[DIRECTORY_ENTRIES_PER_BLOCK];
    size_t listInc = 0;
    INODE checkingINODE;

    for(int i=0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++) {
        if(strnlen(childBLOCK.directory.entry[i].name, FILE_NAME_SIZE) > 0) {
            itemList[listInc] = childBLOCK.directory.entry[i].name;

            listInc++;
        }
    }

    qsort(itemList, listInc, sizeof(char*), cmpstringp);

    for(int i=0; i < listInc; ++i) {
        for(int j=0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
            if(strncmp(childBLOCK.directory.entry[j].name, itemList[i], FILE_NAME_SIZE) == 0) {
                oufs_read_inode_by_reference(childBLOCK.directory.entry[j].inode_reference, &childINODE);

                if(childINODE.type == IT_FILE) {
                    printf("%s\n", itemList[i]);
                }
                else {
                    printf("%s/\n", itemList[i]);
                }
                break;
            }
        }
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
/**
 * Function to reset a given inode.
 * @param inode the address of an inode to be reset.
 */

/**
 *
 * @param cwd
 * @param path
 * @param mode
 * @return
 */
OUFILE *oufs_fopen(char *cwd, char *path, char *mode)
{
    char local_name[FILE_NAME_SIZE];
    INODE_REFERENCE parentINODE_REF, childINODE_REF;
    INODE childINODE, parentINODE;
    oufs_find_file(cwd, path, &parentINODE_REF, &childINODE_REF, &local_name);

    OUFILE *fp = malloc(sizeof(OUFILE));

    switch(*mode) {
        case 'r' : //File reading case
            if(childINODE_REF == UNALLOCATED_INODE) {
                fprintf(stderr, "oufs_fopen: file does not exist. Exiting...\n");
                return NULL;
            }
            //Initialize oufile_s
            fp->inode_reference = childINODE_REF;
            fp->mode = *mode;
            fp->offset = 0;
            return(fp);
        case 'w' : //File writing case
            if(parentINODE_REF == UNALLOCATED_INODE)
            {
                fprintf(stderr, "oufs_fopen: parent does not exist. Exiting...\n");
                return NULL;
            }
            if(childINODE_REF == UNALLOCATED_INODE)
            {
                //Find an empty place in the block.
                oufs_read_inode_by_reference(parentINODE_REF, &parentINODE);
                BLOCK parentBLOCK;
                vdisk_read_block(parentINODE.data[0],&parentBLOCK);
                int availableEntry = -1;
                for(int i=0; i < BLOCKS_PER_INODE; i++) {
                    if(parentBLOCK.directory.entry[i].inode_reference == UNALLOCATED_INODE) {
                        availableEntry = i;
                        break;
                    }
                }
                if(availableEntry < 0)
                {
                    fprintf(stderr, "oufs_fopen: parent directory is full. Exiting...\n");
                    return NULL;
                }

                //Allocate a new inode for the file.
                BLOCK masterBLOCK;
                vdisk_read_block(MASTER_BLOCK_REFERENCE, &masterBLOCK);
                int newINODE_REFERENCE = oufs_find_open_bit(masterBLOCK.master.inode_allocated_flag);
                if(newINODE_REFERENCE < 1) //Error if no available inodes.
                {
                    fprintf(stderr, "oufs_fopen: no available inodes. Exiting...\n");
                    return NULL;
                }
                childINODE_REF = (INODE_REFERENCE) newINODE_REFERENCE;
                SET_BIT(masterBLOCK.master.inode_allocated_flag, childINODE_REF); //Set the bit.
                childINODE.size = 0;
                for(int i = 0; i < BLOCKS_PER_INODE; i++)
                {
                    childINODE.data[i] = UNALLOCATED_BLOCK; //Set all blocks to unallocated.
                }
                childINODE.n_references = 1;
                childINODE.type = IT_FILE;

                parentINODE.size++;

                strncpy(parentBLOCK.directory.entry[availableEntry].name, local_name, FILE_NAME_SIZE-1);
                parentBLOCK.directory.entry[availableEntry].name[FILE_NAME_SIZE-1] = 0; //Ensure null termination.
                parentBLOCK.directory.entry[availableEntry].inode_reference = childINODE_REF;
                vdisk_write_block(parentINODE.data[0], &parentBLOCK);
                vdisk_write_block(MASTER_BLOCK_REFERENCE, &masterBLOCK);
                oufs_write_inode_by_reference(parentINODE_REF, &parentINODE);
                oufs_write_inode_by_reference(childINODE_REF, &childINODE);
            }
            else
            {
                oufs_read_inode_by_reference(childINODE_REF, &childINODE);
            }

            //OUFILE *fp declared above.
            fp->inode_reference = childINODE_REF;
            fp->mode = *mode;
            fp->offset = 0;
            return(fp);
        case 'a' : //File appending case.
            if(parentINODE_REF == UNALLOCATED_INODE)
            {
                fprintf(stderr, "oufs_fopen: parent does not exist. Exiting...\n");
                return NULL;
            }
            if(childINODE_REF == UNALLOCATED_INODE)
            {
                //Find an empty place in the block.
                oufs_read_inode_by_reference(parentINODE_REF, &parentINODE);
                BLOCK parentBLOCK;
                vdisk_read_block(parentINODE.data[0], &parentBLOCK);
                int availableEntry = -1;
                for(int i=0; i < BLOCKS_PER_INODE; i++) {
                    if(parentBLOCK.directory.entry[i].inode_reference == UNALLOCATED_INODE) {
                        availableEntry = i;
                        break;
                    }
                }
                if(availableEntry < 0)
                {
                    fprintf(stderr, "oufs_fopen: parent directory is full. Exiting...\n");
                    return NULL;
                }

                //Allocate a new inode for the file.
                BLOCK masterBLOCK;
                vdisk_read_block(MASTER_BLOCK_REFERENCE, &masterBLOCK);
                int newINODE_REFERENCE = oufs_find_open_bit(masterBLOCK.master.inode_allocated_flag);
                if(newINODE_REFERENCE < 1) //Error if no available inodes.
                {
                    fprintf(stderr, "oufs_fopen: no available inodes. Exiting...\n");
                    return NULL;
                }
                childINODE_REF = (INODE_REFERENCE) newINODE_REFERENCE;
                SET_BIT(masterBLOCK.master.inode_allocated_flag, childINODE_REF); //Set the bit.
                childINODE.size = 0;
                for(int i = 0; i < BLOCKS_PER_INODE; i++)
                {
                    childINODE.data[i] = UNALLOCATED_BLOCK; //Set all blocks to unallocated.
                }
                childINODE.n_references = 1;
                childINODE.type = IT_FILE;

                parentINODE.size++;

                strncpy(parentBLOCK.directory.entry[availableEntry].name, local_name, FILE_NAME_SIZE-1);
                parentBLOCK.directory.entry[availableEntry].name[FILE_NAME_SIZE-1] = 0; //Ensure null termination.
                parentBLOCK.directory.entry[availableEntry].inode_reference = childINODE_REF;
                vdisk_write_block(parentINODE.data[0], &parentBLOCK);
                vdisk_write_block(MASTER_BLOCK_REFERENCE, &masterBLOCK);
                oufs_write_inode_by_reference(parentINODE_REF, &parentINODE);
                oufs_write_inode_by_reference(childINODE_REF, &childINODE);
            }
            else
            {
                oufs_read_inode_by_reference(childINODE_REF, &childINODE);
            }

            //OUFILE *fp declared above.
            fp->inode_reference = childINODE_REF;
            fp->mode = *mode;
            fp->offset = childINODE.size;
            return(fp);
        default:
            fprintf(stderr, "oufs_fopen: Invalid mode(%s). Exiting...\n", mode);
            return NULL;

    }
}
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
/**
 * Writes to or appends given data to a file.
 * @param fp the OUFILE object representing the file opened previously.
 * @param buf buffer to be written to the file.
 * @param len the length of the buffer.
 * @return
 */
int oufs_fwrite(OUFILE *fp, unsigned char *buf, int len)
{
    INODE inode;
    oufs_read_inode_by_reference((*fp).inode_reference, &inode);

    int bufLocation = 0;
    int offsetInBlock;
    int currentBlock;
    int blkInMem = -1;
    BLOCK blockMem, masterBlock;
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &masterBlock);

    switch((*fp).mode){
        case 'w' :
            if((*fp).offset > 0)
            {
                for(int i=0; i< BLOCKS_PER_INODE; i++)
                {
                    if(inode.data[i] == UNALLOCATED_BLOCK) //Break loop if the block reference is already unallocated.
                        break;

                    RESET_BIT(masterBlock.master.block_allocated_flag, inode.data[i]); //Free the block
                    inode.data[i] = UNALLOCATED_BLOCK; //Set data to unallocated.
                }
            }
            inode.size = 0;

            while(bufLocation < len) //While there is still data to write.
            {
                if(bufLocation >= (BLOCK_SIZE*BLOCKS_PER_INODE))
                {
                    break;
                }
                offsetInBlock = (*fp).offset % 256; //Calculate the current position in block.
                currentBlock = ((*fp).offset - offsetInBlock) / 256; //Calculate the current block.

                if(blkInMem != currentBlock) //Read in current block if necessary.
                {
                    if(inode.data[currentBlock] == UNALLOCATED_BLOCK) //Setup new block
                    {
                        int allocNewBlock;
                        if((allocNewBlock = oufs_find_open_bit(masterBlock.master.block_allocated_flag)) < 0)
                        {
                            fprintf(stderr, "No more blocks available.\n");
                            return EXIT_FAILURE;
                        }
                        SET_BIT(masterBlock.master.block_allocated_flag, allocNewBlock);
                        inode.data[currentBlock] = (BLOCK_REFERENCE) allocNewBlock;
                    }
                    vdisk_read_block(inode.data[currentBlock], &blockMem);
                    blkInMem = currentBlock;
                }

                blockMem.data.data[offsetInBlock] = buf[bufLocation];

                inode.size++;
                (*fp).offset++;
                bufLocation++;

                //If block full, buf empty, or file full.
                if((offsetInBlock >= (BLOCK_SIZE - 1)) || (bufLocation >= (len-1)) || (bufLocation >= (BLOCK_SIZE*BLOCKS_PER_INODE)))
                {
                    vdisk_write_block(inode.data[blkInMem], &blockMem); //Write the block.
                }
            }

            vdisk_write_block(MASTER_BLOCK_REFERENCE, &masterBlock); //Write the master block.
            oufs_write_inode_by_reference((*fp).inode_reference, &inode); //Write the inode.
            return EXIT_SUCCESS;
        case 'a' :
            //Write the characters to the file.
            while(bufLocation < len) //While there is still data to write.
            {
                if(bufLocation >= (BLOCK_SIZE*BLOCKS_PER_INODE))
                {
                    break;
                }
                offsetInBlock = (*fp).offset % 256; //Calculate the current position in block.
                currentBlock = ((*fp).offset - offsetInBlock) / 256; //Calculate the current block.

                if(blkInMem != currentBlock) //Read in current block if necessary.
                {
                    if(inode.data[currentBlock] == UNALLOCATED_BLOCK) //Setup new block
                    {
                        int allocNewBlock;
                        if((allocNewBlock = oufs_find_open_bit(masterBlock.master.block_allocated_flag)) < 0)
                        {
                            fprintf(stderr, "No more blocks available.\n");
                            return EXIT_FAILURE;
                        }
                        SET_BIT(masterBlock.master.block_allocated_flag, allocNewBlock);
                        inode.data[currentBlock] = (BLOCK_REFERENCE) allocNewBlock;
                    }
                    vdisk_read_block(inode.data[currentBlock], &blockMem);
                    blkInMem = currentBlock;
                }

                blockMem.data.data[offsetInBlock] = buf[bufLocation];

                inode.size++;
                (*fp).offset++;
                bufLocation++;

                //If block full, buf empty, or file full.
                if((offsetInBlock >= (BLOCK_SIZE - 1)) || (bufLocation >= (len-1)) || (bufLocation >= (BLOCK_SIZE*BLOCKS_PER_INODE)))
                {
                    vdisk_write_block(inode.data[blkInMem], &blockMem); //Write the block.
                }
            }

            vdisk_write_block(MASTER_BLOCK_REFERENCE, &masterBlock); //Write the master block.
            oufs_write_inode_by_reference((*fp).inode_reference, &inode); //Write the inode.
            return EXIT_SUCCESS;
        case 'r' :
            fprintf(stderr, "File in read only mode - cannot write.\n");
            return EXIT_FAILURE;
        default:
            return EXIT_FAILURE;
    }
}
/**
 * This function reads a file in the OU File System and saves it to a provided buffer.
 * @param fp the OUFILE object representing the file opened previously.
 * @param buf the buffer for the file to be read into.
 * @param len the length of the file to be saved.
 * @return system defined success value.
 */
int oufs_fread(OUFILE *fp, unsigned char *buf, int *len) {

    int bufLocation = 0;
    int offsetInBlock;
    int currentBlock;
    int blkInMem = -1;
    BLOCK blockMem;
    INODE fileINODE;

    if((*fp).mode != 'r')
    {
        fprintf(stderr, "File cannot be read - opened in '%c' mode.\n", (*fp).mode);
        return EXIT_FAILURE;
    }

    oufs_read_inode_by_reference((*fp).inode_reference, &fileINODE);

    while (bufLocation < fileINODE.size-1) //While there is still data to write.
    {
        offsetInBlock = bufLocation % 256; //Calculate the current position in block.
        currentBlock = (bufLocation - offsetInBlock) / 256; //Calculate the current block.

        if(blkInMem != currentBlock) //Read in current block if necessary.
        {

            vdisk_read_block(fileINODE.data[currentBlock], &blockMem);
            blkInMem = currentBlock;
        }

        buf[bufLocation] = blockMem.data.data[offsetInBlock];

        bufLocation++;

    }
    buf[bufLocation] = 0;
    *len = bufLocation;
    return EXIT_SUCCESS;
}
/**
 * This function removes a file reference or file from the OUFS file system.
 *
 * Note: the file data is still on the disk until it is overwritten.
 * @param cwd the current working directory of the file system.
 * @param path the user provided path.
 * @return system defined success value.
 */
int oufs_remove(char *cwd, char *path)
{
    char local_name[FILE_NAME_SIZE];
    INODE_REFERENCE parentINODE_REF, childINODE_REF;
    INODE parentINODE, childINODE;
    BLOCK parentBLOCK;
    oufs_find_file(cwd, path, &parentINODE_REF, &childINODE_REF, &local_name);

    //Check if child exists
    if(childINODE_REF == UNALLOCATED_INODE)
    {
        fprintf(stderr, "File specified does not exist.\n");
        return EXIT_FAILURE;
    }

    //Read the inodes.
    oufs_read_inode_by_reference(parentINODE_REF, &parentINODE);
    oufs_read_inode_by_reference(childINODE_REF, &childINODE);

    //Read the parent block.
    vdisk_read_block(parentINODE.data[0], &parentBLOCK);

    //Decrement the child inode number of references.
    childINODE.n_references--;

    //Remove the file entry from parent.
    for(int i=0; i< BLOCKS_PER_INODE; i++)
    {
        if(parentBLOCK.directory.entry[i].inode_reference == childINODE_REF)
        {
            parentBLOCK.directory.entry[i].inode_reference = UNALLOCATED_INODE; //Set entry reference to unalloc.
            memset(parentBLOCK.directory.entry[i].name, 0, FILE_NAME_SIZE); //Set entry name to nulls.
            break;
        }

    }
    parentINODE.size--;
    oufs_write_inode_by_reference(parentINODE_REF, &parentINODE);
    vdisk_write_block(parentINODE.data[0], &parentBLOCK);

    //Check if file is ready for deletion
    if(childINODE.n_references < 1)
    {
        BLOCK masterBLOCK;
        vdisk_read_block(MASTER_BLOCK_REFERENCE, &masterBLOCK);
        //Remove all references
        for(int i=0; i < BLOCKS_PER_INODE; i++)
        {
            if(childINODE.data[i] == UNALLOCATED_BLOCK) {
                break;
            }
            RESET_BIT(masterBLOCK.master.block_allocated_flag, childINODE.data[i]); //Deallocate block
            childINODE.data[i] = UNALLOCATED_BLOCK;
        }
        childINODE.size = 0;
        childINODE.type = IT_NONE;
        oufs_write_inode_by_reference(childINODE_REF, &childINODE); //Write clean inode to inode block.

        RESET_BIT(masterBLOCK.master.inode_allocated_flag, childINODE_REF); //Deallocate inode.

        vdisk_write_block(MASTER_BLOCK_REFERENCE, &masterBLOCK); //Write master block.
    }
    return EXIT_SUCCESS;
}

int oufs_link(char *cwd, char *path_src, char *path_dst)
{
    INODE_REFERENCE srcChildINODE_REF, srcParentINODE_REF, dstChildINODE_REF, dstParentINODE_REF;
    INODE srcChildINODE, dstParentINODE;
    char srcLocalName[FILE_NAME_SIZE];
    char dstLocalName[FILE_NAME_SIZE];
    BLOCK dstParentBLOCK;

    //Discover the parent and destination locations
    oufs_find_file(cwd, path_src, &srcParentINODE_REF, &srcChildINODE_REF, srcLocalName);
    oufs_find_file(cwd, path_dst, &dstParentINODE_REF, &dstChildINODE_REF, dstLocalName);

    //Ensure destination does not exist.
    if(dstChildINODE_REF != UNALLOCATED_INODE)
    {
        fprintf(stderr, "Destination file already exists.\n");
        return EXIT_FAILURE;
    }

    //Ensure source child does exist.
    if(srcChildINODE_REF == UNALLOCATED_INODE)
    {
        fprintf(stderr, "Source file does not exist.\n");
        return EXIT_FAILURE;
    }

    //Ensure source child is a file.
    oufs_read_inode_by_reference(srcChildINODE_REF, &srcChildINODE);
    if(srcChildINODE.type != IT_FILE)
    {
        fprintf(stderr, "Source is not a file.\n");
        return EXIT_FAILURE;
    }

    //Ensure the destination parent exists.
    if(dstParentINODE_REF == UNALLOCATED_INODE)
    {
        fprintf(stderr, "Source parent does not exist.\n");
        return EXIT_FAILURE;
    }
    //Check if the destination parent has room.
    oufs_read_inode_by_reference(dstParentINODE_REF, &dstParentINODE);
    if(dstParentINODE.size >= INODES_PER_BLOCK)
    {
        fprintf(stderr, "Source parent is full.\n");
        return EXIT_FAILURE;
    }

    //Read dest parent inode reference and block
    oufs_read_inode_by_reference(dstParentINODE_REF, &dstParentINODE);
    vdisk_read_block(dstParentINODE.data[0], &dstParentBLOCK);

    //Find empty entry in destination parent directory block.
    for(int i=0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        if(dstParentBLOCK.directory.entry[i].inode_reference == UNALLOCATED_INODE)
        {
            dstParentBLOCK.directory.entry[i].inode_reference = srcChildINODE_REF; //Set the available entry to src
            strncpy(dstParentBLOCK.directory.entry[i].name, dstLocalName, FILE_NAME_SIZE-1); //Copy dst name.
            dstParentBLOCK.directory.entry[i].name[FILE_NAME_SIZE-1] = 0; //Null terminate if full
            dstParentINODE.size++; //Increment num directories in parent inode.
            break;
        }
    }

    //Increment number of references on src child inode.
    srcChildINODE.n_references++;

    //Write changes to disk.
    vdisk_write_block(dstParentINODE.data[0], &dstParentBLOCK);
    oufs_write_inode_by_reference(dstParentINODE_REF, &dstParentINODE);
    oufs_write_inode_by_reference(srcChildINODE_REF, &srcChildINODE);
    return EXIT_SUCCESS;
}