#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct file_Node{
    char* name;
    char* header;
    void* content;

    int header_len;
    int content_len;
    
    double istorage_time;
    long expire;

    struct file_Node* next;
    struct file_Node* prev;

}file_Node;

double get_curr_time();

void initialize_cache(int capacity);

file_Node* find_file(char* name);

int is_stale(file_Node* node);

void write_node_client(file_Node* node, int sock_fd);

void put_file(char* name, int expire, char* header, int hlen, char* file, int flen);

void get_file(file_Node* node, int sockfd);

void clean_cache();


// void print();





