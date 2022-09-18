#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "jumbo_file_system.h"


#define DISK_FILENAME "DISK"


void print_error(int err, const char* name) {
    switch (err) {
    case E_SUCCESS:
      break; // do nothing
    case E_NOT_EXISTS:
      printf("directory not found: %s\n", name);
      break;
    case E_EXISTS:
      printf("file already exists: %s\n", name);
      break;
    case E_NOT_DIR:
      printf("%s is not a directory\n", name);
      break;
    case E_IS_DIR:
      printf("%s is a directory\n", name);
      break;
    case E_NOT_EMPTY:
      printf("directory %s is not empty\n", name);
      break;
    case E_MAX_NAME_LENGTH:
      printf("%s exceeds the maximum file name length\n", name);
      break;
    case E_MAX_DIR_ENTRIES:
      printf("directory is full (max entries reached)\n");
      break;
    case E_MAX_FILE_SIZE:
      printf("file is full (max file size reached)\n");
      break;
    case E_DISK_FULL:
      printf("disk is full");
      break;
    case E_UNKNOWN:
      printf("an unknown error occurred\n");
      break;
    default:
      printf("an unrecognized error (%d) occurred\n", err);
    }
}

int main() {

    jfs_mount(DISK_FILENAME);

    // print_error(jfs_mkdir("dir1"), "dir1");
    // print_error(jfs_mkdir("dir2"), "dir2");
    // print_error(jfs_creat("test"), "test");
    // char buf[10] = "1234567890";
    // print_error(jfs_write("test", buf, 10), "test");
    // char got[6];
    // got[5] = '\0';
    // unsigned short l = 5;
    // print_error(jfs_read("test", got, &l), "test");
    // printf("got: %s\n", got);

    // char* max = malloc(MAX_FILE_SIZE);
    // char* maxread = malloc(MAX_FILE_SIZE);
    // memset(max, 'a', MAX_FILE_SIZE);
    // print_error(jfs_creat("max"), "max");
    // print_error(jfs_write("max", max, MAX_FILE_SIZE), "max");
    // unsigned short m = MAX_FILE_SIZE;
    // print_error(jfs_read("max", maxread, &m), "max");


    print_error(jfs_creat("1"), "1");
    
    // char w[16] ="xxxxxxxxxxxxxxx0";
    // print_error(jfs_write("1", w, 16), "1");
    // print_error(jfs_write("1", w, 16), "1");
    // print_error(jfs_write("1", w, 16), "1");

    // char r[65];
    // r[65] = '\0';

    // unsigned short m = MAX_FILE_SIZE;
    // print_error(jfs_read("1", r, &m), "1");
    // printf("got: %s\n", r);
 
    char w[1] ="a";
    print_error(jfs_write("1", w, 1), "1");
    void* m = malloc(1792);
    unsigned short r = 1792;
    // void* m = malloc(2);
    // unsigned short r = 1;
    print_error(jfs_read("1", m, &r), "1");
    ((char*)m)[1] = '\0';
    printf("got: %s\n", (char*)m);

    free(m);

    // struct block test;
    // read_block(1, &test);
    // printf("num entries in root dir: %d\n", test.contents.dirnode.num_entries);

    // for (size_t i = 0; i < MAX_DIR_ENTRIES; i++) {
    //     if(test.contents.dirnode.entries[i].block_num != 0) {
    //         printf("name: %s\n", test.contents.dirnode.entries[i].name);
    //         int b_num = test.contents.dirnode.entries[i].block_num;
    //         struct block new_dir;
    //         read_block(b_num, &new_dir);
    //         if (new_dir.is_dir == 0) {
    //             printf("correct directory creation\n");
    //             printf("num entries: %d\n", new_dir.contents.dirnode.num_entries);
    //             for (size_t i = 0; i < MAX_DIR_ENTRIES; i++) {
    //                 printf("block_num: %d\n", new_dir.contents.dirnode.entries[i].block_num);
    //             }
    //         } else {
    //             printf("oops\n");
    //         }
    //     }
    //     printf("\n");
    // }


    // for (int i = 0; i < MAX_FILE_SIZE; i++) {
    //     printf("a");
    // }


    return 0;
}