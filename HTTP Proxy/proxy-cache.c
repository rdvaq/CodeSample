#include "proxy-cache.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define max_name_length 100
#define max_file_length 10 * 1024 * 1024
#define nstos 1000000000.0
#define TRUE 1
#define FALSE 0

file_Node *head;
file_Node *tail;

int cache_capacity;
int cache_items;

double get_curr_time() {
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    double time = ts.tv_sec;
    double ntos = ts.tv_nsec / nstos;
    time += ntos;

    return time;
}

int is_stale(file_Node* node) {
    double c_time = get_curr_time();
    if (node->expire == 0) return TRUE;
    int diff = c_time - (node->istorage_time + node->expire);
    if (diff > 0) return TRUE;

    return FALSE;
}

int get_age(file_Node* node) {
    double c_time = get_curr_time();
    int age = c_time - node->istorage_time;
    return age;
}

void initialize_cache(int capacity) {
    cache_capacity = capacity;
    cache_items = 0;
    head = NULL;
}

void free_node(file_Node* node) {
    free(node->name);
    free(node->header);
    free(node->content);
    free(node);
}

int match_name(char* name, file_Node* node) {
    char* file_name = node->name;
    if (strcmp(file_name, name) == 0) {
        return TRUE;
    }
    return FALSE;
}

file_Node* find_file(char* name) {
    file_Node* curr = head;
    while (curr != NULL) {
        if (match_name(name, curr) == TRUE) {
            return curr;
        } 
        curr = curr->next;
    }
    return NULL;
}

char* make_new_header(int age, file_Node* node, int* hlen) {
    char num_str[500];
    sprintf(num_str, "%d", age);

    char age_header[5] = "Age: ";

    int age_len = strlen(num_str);

    int a_f_len = age_len + 5;
    char* age_field = malloc(a_f_len);
    memcpy(age_field, age_header, 5);
    memcpy(age_field + 5, num_str, age_len);

    char* header = node->header;
    int header_len = node->header_len;

    int n_h_len = header_len + a_f_len + 2;
    *hlen = n_h_len;
    char* n_header = malloc(n_h_len);
    bzero(n_header, n_h_len);

    memcpy(n_header, header, header_len - 2);
    memcpy(n_header + header_len - 2, age_field, a_f_len);
    memcpy(n_header + header_len - 2 + a_f_len, "\r\n\r\n", 4);

    return n_header;
}

void write_node_client(file_Node* node, int sockfd) {
    int age = get_age(node);
    int hlen;
    char* header = make_new_header(age, node, &hlen);
    write(sockfd, header, hlen);
    write(sockfd, node->content, node->content_len);    
}

file_Node* remove_node_from_list(file_Node* node) {
    file_Node* prev = node->prev;
    file_Node* next = node->next;

    if (node == head) {
        head = next;
        if (next == NULL) { // only 1 node in the list 
            tail = next;
        } else { // > 1 node in the list 
            next->prev = prev;
        }         
    } else if (node == tail) { //has to have > 1 node in the list
        tail = prev;
        prev->next = next;
    } else { // in the middle of the list
        prev->next = next;
        next->prev = prev;
    } 

    return node;
}

file_Node* find_stale() {
    file_Node* curr = head;

    while (curr != NULL) {
        if (is_stale(curr)) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}


void evict() {
    file_Node* evict = find_stale();
    if (evict != NULL) { // first remove oldest stale
        evict = remove_node_from_list(evict);
    } else {
        evict = remove_node_from_list(head);
    } 
    // printf("evicting: %s\n", evict->name);
    free_node(evict);
    cache_items--;
}


void put_file(char* name, int expire, char* header, int hlen, char* file, int flen) {
    file_Node* node = find_file(name);

    if (node != NULL) { // file already exists
        free(node->header);
        free(node->content);
        free(node->name);
        remove_node_from_list(node);

    } else {
        //check if cache is full, if full evict 
        if (cache_items >= cache_capacity) {
            //evict
            evict();
        }

        cache_items++;
        node = malloc(sizeof(file_Node));
    }
    node->name = name;
    node->header = header;
    node->content = file;
    node->header_len = hlen;
    node->content_len = flen;
    //update max_age
    node->expire = expire;
    //update initial storage time
    node->istorage_time = get_curr_time();

    if (tail != NULL) {
        node->prev = tail;
        tail->next = node;
        tail = node;
    } else { // LIST is empty 
        head = node;
        tail = node;
        node->prev = NULL;
    }
    node->next = NULL;

}

// node is garunteed to be in the list and is not stale 
void get_file(file_Node* node, int sockfd) {
    remove_node_from_list(node);
    if (tail != NULL) {
        node->prev = tail;
        tail->next = node;
        tail = node;
    } else { // LIST is empty 
        head = node;
        tail = node;
        node->prev = NULL;
    }
    node->next = NULL;

    write_node_client(node, sockfd);

}

void clean_cache() {
    file_Node* curr = head;
    while (curr != NULL) {
        file_Node* next = curr->next;
        free_node(curr);
        curr = next;
    }
}


// void print() {

//     file_Node* curr = head;
//     printf("\nLIST:\n");
//     while (curr != NULL) {
//         char* curr_name = curr->name;
//         printf("%s\n", curr_name);
//         // printf("storage time: %.10lf\n", curr->istorage_time);
//         printf("max_age: %d\n", curr->expire);
//         printf("file length: %d\n", curr->content_len);
//         printf("\n");
//         curr = curr->next;
//     }
// }