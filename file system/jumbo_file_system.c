#include "jumbo_file_system.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// To identify empty slots in a directory OR empty data blocks in a file,
// 0 is used as a sentinel value for block num

// C does not have a bool type, so I created one that you can use
typedef char bool_t;
#define TRUE 1
#define FALSE 0


static block_num_t current_dir;


// optional helper function you can implement to tell you if a block is a dir node or an inode
static bool_t is_dir(block_num_t block_num) {
    struct block curr;
    read_block(block_num, &curr);
    if (curr.is_dir == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// helper func; finds the requested file/dir in the current directory and 
// returns its index in the entries array; If not found, -1 is returned.
// Input: name of the file/directory
// Output: index of the file/dir in the entries array; -1 if file/dir doesnt exit 
static int find_target(const char* target_name) {
    struct block curr;
    read_block(current_dir, &curr);

    for(size_t i = 0; i < MAX_DIR_ENTRIES; i++) {
        block_num_t sub_block_num = curr.contents.dirnode.entries[i].block_num;
        if (sub_block_num > 0) {
            char* sub_name = curr.contents.dirnode.entries[i].name;

            if (strcmp(target_name, sub_name) == 0) {
                return i;
            }
        }
    }

    return -1;
}

/* jfs_mount
 *   prepares the DISK file on the _real_ file system to have file system
 *   blocks read and written to it.  The application _must_ call this function
 *   exactly once before calling any other jfs_* functions.  If your code
 *   requires any additional one-time initialization before any other jfs_*
 *   functions are called, you can add it here.
 * filename - the name of the DISK file on the _real_ file system
 * returns 0 on success or -1 on error; errors should only occur due to
 *   errors in the underlying disk syscalls.
 */
int jfs_mount(const char* filename) {
    int ret = bfs_mount(filename);
    current_dir = 1;
    return ret;
}


/* jfs_mkdir
 *   creates a new subdirectory in the current directory
 * directory_name - name of the new subdirectory
 * returns 0 on success or one of the following error codes on failure:
 *   E_EXISTS, E_MAX_NAME_LENGTH, E_MAX_DIR_ENTRIES, E_DISK_FULL
 */
int jfs_mkdir(const char* directory_name) {
    if (strlen(directory_name) > MAX_NAME_LENGTH) {
        return E_MAX_NAME_LENGTH;
    }

    block_num_t new_block_num = allocate_block();
    if (new_block_num == 0) {
        return E_DISK_FULL; 
    }
    // get current directory 
    struct block curr_dir_block;
    read_block(current_dir, &curr_dir_block);

    if (curr_dir_block.contents.dirnode.num_entries >= MAX_DIR_ENTRIES) {
        return E_MAX_DIR_ENTRIES;
    }
    // entry represent the free spot in directory
    int entry;
    for(size_t i = 0; i < MAX_DIR_ENTRIES; i++) {
        if (curr_dir_block.contents.dirnode.entries[i].block_num != 0) {
            char* mem_name = curr_dir_block.contents.dirnode.entries[i].name;
            if (strcmp(directory_name, mem_name) == 0) {
                return E_EXISTS;
            }
        } else {
            // identify free spot 
            entry = i;
        }
    }

    curr_dir_block.contents.dirnode.num_entries++;
    curr_dir_block.contents.dirnode.entries[entry].block_num = new_block_num;
    strcpy(curr_dir_block.contents.dirnode.entries[entry].name, directory_name);
    // update current dir
    write_block(current_dir, &curr_dir_block);
    // write to the new directory block with correct data
    struct block new_dir;
    memset(&new_dir, 0, BLOCK_SIZE);
    write_block(new_block_num, &new_dir);
    return 0;
}


/* jfs_chdir
 *   changes the current directory to the specified subdirectory, or changes
 *   the current directory to the root directory if the directory_name is NULL
 * directory_name - name of the subdirectory to make the current
 *   directory; if directory_name is NULL then the current directory
 *   should be made the root directory instead
 * returns 0 on success or one of the following error codes on failure:
 *   E_NOT_EXISTS, E_NOT_DIR
 */
int jfs_chdir(const char* directory_name) {
    if (directory_name == NULL) {
        current_dir = 1;
        return 0;
    }

    struct block curr;
    read_block(current_dir, &curr);

    int t_index = find_target(directory_name);
    if (t_index == -1) {
        return E_NOT_EXISTS;
    }

    block_num_t e_block_num = curr.contents.dirnode.entries[t_index].block_num;

    if(is_dir(e_block_num) == TRUE) {
        current_dir = e_block_num;
        return 0;
    } else {
        return E_NOT_DIR;
    }
}


/* jfs_ls
 *   finds the names of all the files and directories in the current directory
 *   and writes the directory names to the directories argument and the file
 *   names to the files argument
 * directories - array of strings; the function will set the strings in the
 *   array, followed by a NULL pointer after the last valid string; the strings
 *   should be malloced and the caller will free them
 * file - array of strings; the function will set the strings in the
 *   array, followed by a NULL pointer after the last valid string; the strings
 *   should be malloced and the caller will free them
 * returns 0 on success or one of the following error codes on failure:
 *   (this function should always succeed)
 */
int jfs_ls(char* directories[MAX_DIR_ENTRIES+1], char* files[MAX_DIR_ENTRIES+1]) {
    int dir_index = 0;
    int file_index = 0;

    struct block curr;
    read_block(current_dir, &curr);

    for (size_t i = 0; i < MAX_DIR_ENTRIES; i++) {
        block_num_t dorf_block_num = curr.contents.dirnode.entries[i].block_num; 
        /* if an entry exists */
        if (dorf_block_num > 0) {
            char* dorf_name = curr.contents.dirnode.entries[i].name;
            int len = strlen(dorf_name);
            char* m_name = malloc((len + 1) * sizeof(char));
            strcpy(m_name, dorf_name);

            if (is_dir(dorf_block_num) == TRUE) {
                directories[dir_index] = m_name;
                dir_index++;
            } else {
                files[file_index] = m_name;
                file_index++;
            }
        }
    }
    // populate free slots with NULL
    for (size_t i = dir_index; i < (MAX_DIR_ENTRIES+1); i++) {
        directories[i] = NULL;
    }

    for (size_t i = file_index; i < (MAX_DIR_ENTRIES+1); i++) {
        files[i] = NULL;
    }

    return 0;
}   


/* jfs_rmdir
 *   removes the specified subdirectory of the current directory
 * directory_name - name of the subdirectory to remove
 * returns 0 on success or one of the following error codes on failure:
 *   E_NOT_EXISTS, E_NOT_DIR, E_NOT_EMPTY
 */
int jfs_rmdir(const char* directory_name) {
    struct block curr;
    read_block(current_dir, &curr);

    int t_index = find_target(directory_name);
    if (t_index == -1) {
        return E_NOT_EXISTS;
    }

    block_num_t sub_block_num = curr.contents.dirnode.entries[t_index].block_num;

    if(is_dir(sub_block_num) == TRUE) {
        struct block sub;
        read_block(sub_block_num, &sub);

        if (sub.contents.dirnode.num_entries > 0) {
            return E_NOT_EMPTY;
        } else {
            //update current directory
            curr.contents.dirnode.entries[t_index].block_num = 0;
            curr.contents.dirnode.num_entries--;
            write_block(current_dir, &curr);
            // release the deleted dir block 
            release_block(sub_block_num);
            return 0;
        }
    } else {
        return E_NOT_DIR;
    }

}

/* jfs_creat
 *   creates a new, empty file with the specified name
 * file_name - name to give the new file
 * returns 0 on success or one of the following error codes on failure:
 *   E_EXISTS, E_MAX_NAME_LENGTH, E_MAX_DIR_ENTRIES, E_DISK_FULL
 */
int jfs_creat(const char* file_name) {
    if (strlen(file_name) > MAX_NAME_LENGTH) {
        return E_MAX_NAME_LENGTH;
    }

    block_num_t new_block_num = allocate_block();
    if (new_block_num == 0) {
        return E_DISK_FULL;
    }

    struct block curr_dir_block;
    read_block(current_dir, &curr_dir_block);

    if (curr_dir_block.contents.dirnode.num_entries >= MAX_DIR_ENTRIES) {
        return E_MAX_DIR_ENTRIES;
    }
    // find the free spot in the directory
    int entry;
    for(size_t i = 0; i < MAX_DIR_ENTRIES; i++) {
        if (curr_dir_block.contents.dirnode.entries[i].block_num != 0) {
            char* mem_name = curr_dir_block.contents.dirnode.entries[i].name;
            if (strcmp(file_name, mem_name) == 0) {
                return E_EXISTS;
            }
        } else {
            entry = i;
        }
    }  
    // update the info of the curr directory 
    curr_dir_block.contents.dirnode.num_entries++;
    curr_dir_block.contents.dirnode.entries[entry].block_num = new_block_num;
    strcpy(curr_dir_block.contents.dirnode.entries[entry].name, file_name);

    write_block(current_dir, &curr_dir_block);
    // create a new file from allocated block
    struct block new_file;
    memset(&new_file, 0, BLOCK_SIZE);
    new_file.is_dir = 1;
    write_block(new_block_num, &new_file);

    return 0;
}


/* jfs_remove
 *   deletes the specified file and all its data (note that this cannot delete
 *   directories; use rmdir instead to remove directories)
 * file_name - name of the file to remove
 * returns 0 on success or one of the following error codes on failure:
 *   E_NOT_EXISTS, E_IS_DIR
 */
int jfs_remove(const char* file_name) {
    struct block curr;
    read_block(current_dir, &curr);

    int t_index = find_target(file_name);
    if (t_index == -1) {
        return E_NOT_EXISTS;
    }

    block_num_t subf_block_num = curr.contents.dirnode.entries[t_index].block_num;

    if(is_dir(subf_block_num) == FALSE) {
        //update curr directory 
        curr.contents.dirnode.entries[t_index].block_num = 0;
        curr.contents.dirnode.num_entries--;
        write_block(current_dir, &curr);

        struct block file;
        // get the file to be removed 
        read_block(subf_block_num, &file);
        // traverse the data_blocks array of the file and release any allocated
        //blocks
        int i = 0;
        block_num_t data_num = file.contents.inode.data_blocks[i];
        while (data_num != 0) {
            release_block(data_num);
            i++;
            data_num = file.contents.inode.data_blocks[i];
        }
        // realse inode of file
        release_block(subf_block_num);
        return 0;
                    
    } else {
        return E_IS_DIR;
    }      
}


/* jfs_stat
 *   returns the file or directory stats (see struct stat for details)
 * name - name of the file or directory to inspect
 * buf  - pointer to a struct stat (already allocated by the caller) where the
 *   stats will be written
 * returns 0 on success or one of the following error codes on failure:
 *   E_NOT_EXISTS
 */
int jfs_stat(const char* name, struct stats* buf) {
    struct block curr;
    read_block(current_dir, &curr);

    int t_index = find_target(name);
    if (t_index == -1) {
        return E_NOT_EXISTS;
    }
    block_num_t sub_block_num = curr.contents.dirnode.entries[t_index].block_num;
    char* sub_name = curr.contents.dirnode.entries[t_index].name;

    if (is_dir(sub_block_num) == TRUE) {
        buf->is_dir = 0;
    } else {
        buf->is_dir  = 1;
        struct block file;
        read_block(sub_block_num, &file);
        buf->file_size = file.contents.inode.file_size;
        //get num blocks here 
        int index = 0;
        while (file.contents.inode.data_blocks[index] != 0) {
            index++;
        }
        buf->num_data_blocks = index;
    }
        buf->block_num = sub_block_num;
        strcpy(buf->name, sub_name);

    return 0;
}

/// Helper for write///

// free allocated
// takes in an array that contains block nums and an index
// free all the blocks inside the array up until index 
static void free_allocated(int* array, int index) {
    for (int i = 0; i < index; i++) {
        release_block(array[i]);
    }
}

// sum offset
// takes in an array that contains bytes to write and an index
// sum the bytes written up unitl index to get the offset 
static int sum_offset(int* array, int index) {
    int sum = 0;
    for (int i = 0; i < index; i++) {
        sum = sum + array[i];
    }
    return sum;
}


/* jfs_write
 *   appends the data in the buffer to the end of the specified file
 * file_name - name of the file to append data to
 * buf - buffer containing the data to be written (note that the data could be
 *   binary, not text, and even if it is text should not be assumed to be null
 *   terminated)
 * count - number of bytes in buf (write exactly this many)
 * returns 0 on success or one of the following error codes on failure:
 *   E_NOT_EXISTS, E_IS_DIR, E_MAX_FILE_SIZE, E_DISK_FULL
 */
int jfs_write(const char* file_name, const void* buf, unsigned short count) {
    struct block curr;
    read_block(current_dir, &curr);
    int t_index = find_target(file_name);
    if (t_index == -1) {
        return E_NOT_EXISTS;
    }

    block_num_t sub_block_num = curr.contents.dirnode.entries[t_index].block_num;

    if (is_dir(sub_block_num) == TRUE) {
        return E_IS_DIR;
    }
        struct block file;
        read_block(sub_block_num, &file);

        unsigned int file_size = file.contents.inode.file_size;
        unsigned int total_size = count + file_size;
        if (total_size > MAX_FILE_SIZE) {
            return E_MAX_FILE_SIZE;
        }
        
        // number of filled blocks of the existing file
        int filled_blocks = file_size / BLOCK_SIZE;
        // number of bytes left over in the last file
        int num_bytes_l = file_size % BLOCK_SIZE;
        // number of blocks that has data inside 
        int populated_blocks = filled_blocks;
        
        bool_t partial_page = FALSE;
        
        if (num_bytes_l > 0) {
            partial_page = TRUE;
            populated_blocks++;
        }

        // data array that stores the number of bytes to write 
        int data[(MAX_DATA_BLOCKS + 2)] = {0}; // increased by 2 to accomodate max file size
        // array that stores the block num of newly allocated blocks 
        int new_blocks[MAX_DATA_BLOCKS] = {0};

        if (partial_page == TRUE) {
            int free_space = BLOCK_SIZE - num_bytes_l;
            int iterator = 0;

            if (count <= free_space){ // the current partial page is enough 
                data[iterator] = count;
                iterator++;
                data[iterator] = 0; // sentinel 
            } else { // needs more blocks after the partial page
                data[iterator] = free_space;
                iterator++;

                int write_left = count - free_space; // data left after filling partial page
            
                while (write_left >= BLOCK_SIZE) {
                    data[iterator] = BLOCK_SIZE;
                    write_left = write_left - BLOCK_SIZE;
                    iterator++;
                }

                data[iterator] = write_left; // takes care of the case where write left is a multiple of BS
                iterator++;
                data[iterator] = 0; // put in sentinel value 

            }

        } else { // no partial page 
            int write_left = count; 
            int iterator = 0;

            while (write_left > BLOCK_SIZE) {
                data[iterator] = BLOCK_SIZE;
                write_left = write_left - BLOCK_SIZE;
                iterator++;
            }

            data[iterator] = write_left;
            iterator++;
            data[iterator] = 0;
        }

        int data_index = 0;
        int new_blocks_index = 0;

        if (partial_page == TRUE) {
            data_index++;
        }

        while(data[data_index] != 0) {
            int b_num = allocate_block();
            if (b_num == 0) {
                // free all previously allocated blocks 
                free_allocated(new_blocks, new_blocks_index);
                return E_DISK_FULL;
            }
            new_blocks[new_blocks_index] = b_num;
            new_blocks_index++;
            data_index++;
        }
        // reset the iterators 
        data_index = 0;
        new_blocks_index = 0;
        int lb_index = populated_blocks - 1; // the last block's index in the inode's data_blocks

        if (partial_page == TRUE) {
            void* last_page_data = malloc(BLOCK_SIZE);
            int last_num = file.contents.inode.data_blocks[lb_index];
            read_block(last_num, last_page_data);
            // write the data into the last page with offset 
            memcpy((char*)last_page_data + num_bytes_l, buf, data[data_index]);
            write_block(last_num, last_page_data);
            free(last_page_data);
            data_index++;
        }

        while (data[data_index] != 0) { // write the rest of the pages
            lb_index++;
            file.contents.inode.data_blocks[lb_index] = new_blocks[new_blocks_index];

            void* dest = malloc(BLOCK_SIZE);
            memset(dest, 0, BLOCK_SIZE);
            int offset = sum_offset(data, data_index);
            memcpy(dest, (char*)buf + offset, data[data_index]);
            write_block(new_blocks[new_blocks_index], dest);
            free(dest);

            data_index++;
            new_blocks_index++;
        }

        file.contents.inode.file_size = total_size;
        write_block(sub_block_num, &file);

    return 0;
}


/* jfs_read
 *   reads the specified file and copies its contents into the buffer, up to a
 *   maximum of *ptr_count bytes copied (but obviously no more than the file
 *   size, either)
 * file_name - name of the file to read
 * buf - buffer where the file data should be written
 * ptr_count - pointer to a count variable (allocated by the caller) that
 *   contains the size of buf when it's passed in, and will be modified to
 *   contain the number of bytes actually written to buf (e.g., if the file is
 *   smaller than the buffer) if this function is successful
 * returns 0 on success or one of the following error codes on failure:
 *   E_NOT_EXISTS, E_IS_DIR
 */
int jfs_read(const char* file_name, void* buf, unsigned short* ptr_count) {
    struct block curr;
    read_block(current_dir, &curr);

    int t_index = find_target(file_name);
    if (t_index == -1) {
        return E_NOT_EXISTS;
    }

    block_num_t sub_block_num = curr.contents.dirnode.entries[t_index].block_num;

    if (is_dir(sub_block_num) == TRUE) {
        return E_IS_DIR;
    }

    struct block file;
    read_block(sub_block_num, &file);

    unsigned int file_size = file.contents.inode.file_size;
    int num_blocks = 0;
    
    int filled_blocks = file_size / BLOCK_SIZE;
    int lblock_bytes = file_size % BLOCK_SIZE;

    num_blocks = filled_blocks;  // get the number of blocks in the file
    if (lblock_bytes > 0) {
        num_blocks++;
    }

    int bytes_written = 0;
    int will_write = 0;

    for (int i = 0; i < num_blocks; i++) {
        void* dest = malloc(BLOCK_SIZE * sizeof(char));
        read_block(file.contents.inode.data_blocks[i], dest);

        if (i == (num_blocks - 1) && lblock_bytes > 0) {
            will_write = bytes_written + lblock_bytes;
        } else {
            will_write = bytes_written + BLOCK_SIZE;
        }

        if (will_write > *ptr_count) { // case where file is bigger than buffer
            int diff = *ptr_count - bytes_written; // amt of space available
            memcpy((char*)buf + (i*BLOCK_SIZE), dest, diff);
            bytes_written = bytes_written + diff; 
            free(dest);
            break;
        } else {
            int diff = will_write - bytes_written; 
            memcpy((char*)buf + (i*BLOCK_SIZE), dest, diff);
            bytes_written = will_write;
        }

        free(dest);
    }

    *ptr_count = bytes_written;
    return 0; 
}


/* jfs_unmount
 *   makes the file system no longer accessible (unless it is mounted again).
 *   This should be called exactly once after all other jfs_* operations are
 *   complete; it is invalid to call any other jfs_* function (except
 *   jfs_mount) after this function complete.  Basically, this closes the DISK
 *   file on the _real_ file system.  If your code requires any clean up after
 *   all other jfs_* functions are done, you may add it here.
 * returns 0 on success or -1 on error; errors should only occur due to
 *   errors in the underlying disk syscalls.
 */
int jfs_unmount() {
  int ret = bfs_unmount();
  return ret;
}
