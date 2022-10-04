#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>


void error(char *msg) {
    perror(msg);
    exit(0);
}

// create and bind parent socket, returns parent socket fd
int server_up(char* arg_portno) {
    int psockfd;
    int portno;
    struct sockaddr_in serv_addr;

    psockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (psockfd < 0) error("ERROR opening socket\n");

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(arg_portno);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(psockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding\n");
    }

    return psockfd;
}

int accept_client(int parent_socket) {
    int csockfd;
    struct sockaddr_in cli_addr;
    int clilen;

    clilen = sizeof(cli_addr);
    csockfd = accept(parent_socket, (struct sockaddr*) &cli_addr, &clilen);

    if (csockfd < 0) error("ERROR on accept\n");

    return csockfd;
}

// returns the length of header; returns -1 if not finsihed reading header
int check_valid_header(char* buf, int bytes_read) {
    char* hay_stack = malloc(bytes_read + 1);
    bzero(hay_stack, bytes_read + 1);
    memcpy(hay_stack, buf, bytes_read);

    char* res = strstr(hay_stack, "\r\n\r\n");

    int header_len = -1;

    if (res != NULL) {
        header_len = (res + 4) - hay_stack;
    }

    free(hay_stack);
    return header_len;
}

char* copy_string(char* string) {
    char* copy = malloc(strlen(string) + 1);
    strcpy(copy, string);
    return copy;
}

char* read_get_header(int sockfd, int* h_len) {
    int n;
    int bytes_read = 0;
    int buf_size = 1024;
    int read_size = 512;
    char* buf = malloc(buf_size);
    bzero(buf, buf_size);

    n = read(sockfd, buf, read_size);
    if (n < 0) error("Error reading from socket");
    bytes_read += n;

    while (check_valid_header(buf, bytes_read) < 0) {
        if (buf_size - bytes_read < read_size) {
            char* buf2 = realloc(buf, buf_size * 2);
            if (buf2 != NULL) {
                buf = buf2;
            } else {
                error("ERROR realloc");
            }

            bzero(buf + buf_size, buf_size);
            buf_size = buf_size * 2;
        }

        n = read(sockfd, buf + bytes_read, read_size);
        if (n < 0) error("Error reading from socket");
        bytes_read += n;
    }

    *h_len = bytes_read;

    return buf;   
}

int find_host(char* string, char** extracted_info, int* index) {

    char* extract = strtok_r(string, ":", &string);

    if (strcmp(extract, "host") == 0) {
        char* host = strtok_r(string, ":", &string); // port check
        host = strtok_r(host, " ", &host);  //ignore white space (including before and after host name)
        char* host_copy = copy_string(host);
        extracted_info[*index] = host_copy;
        (*index)++;
        
        if (strlen(string) > 0) { // portnum exists
            char* string_copy = copy_string(string);
            extracted_info[*index] = string_copy;
            (*index)++;
        }
        return 0;
    } else {
        return -1;
    }
}

char** parse_get_header(char* header, int header_len, int* arg_length) 
{   
    //not acutally a string
    char* pstring = malloc(header_len);
    memcpy(pstring, header, header_len);

    int i;
    for (i = 0; i < header_len; i++) {
        pstring[i] = tolower(pstring[i]);
    }
    
    int index = 0;
    int found_host;
    char **extracted_info = malloc(3 * sizeof(char*));
    extracted_info[2] = NULL;

    char *extract = strtok(pstring, "\r\n");
    char *extract_copy = copy_string(extract);

    extracted_info[index] = extract_copy;
    index++;

    extract = strtok(NULL, "\r\n");

    while (extract != NULL) {
        found_host = find_host(extract, extracted_info, &index);
        if (found_host == 0) break;
        extract = strtok(NULL, "\r\n");
    }

    *arg_length = index;
    free(pstring);
    return extracted_info;
}

int connect_server(char** header_info, int num_hargs) {
    int s_sockfd, s_portno;
    struct sockaddr_in s_serv_addr;
    struct hostent *s_server;

    if(num_hargs > 2) {
        s_portno = atoi(header_info[2]);
    } else {
        s_portno = 80;
    }

    s_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sockfd < 0) error("ERROR opening socket\n");

    s_server = gethostbyname(header_info[1]);
    if (s_server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &s_serv_addr, sizeof(s_serv_addr));
    s_serv_addr.sin_family = AF_INET;
    bcopy((char *)s_server->h_addr, (char *)&s_serv_addr.sin_addr.s_addr,
                                                        s_server->h_length);
    s_serv_addr.sin_port = htons(s_portno);

    if (connect(s_sockfd,(struct sockaddr *) &s_serv_addr,
                                                    sizeof(s_serv_addr)) < 0) { 
        error("ERROR connecting");
    }
    return s_sockfd;
}

int find_len(char* string, char** extracted_info, int index) {

    char* extract = strtok_r(string, ":", &string);

    if (strcmp(extract, "content-length") == 0) {
        char* len = strtok_r(string, " ", &string);  //ignore white space (including before and after host name)
        char* len_copy = copy_string(len);
        extracted_info[index] = len_copy;
        return 0;
    } else {
        return -1;
    }
}

int find_age(char* string, char** extracted_info, int index) {
    char* extract = strtok_r(string, ":", &string);

    if (strcmp(extract, "cache-control") == 0) {
        strtok_r(string, "=", &string); 
        char* age = strtok_r(string, " ", &string);  //ignore white space (including before and after host name)
        if (age == NULL) {
            extracted_info[index] = "3600"; // an hour
        } else {
            char* age_copy = copy_string(age);
            extracted_info[index] = age_copy;
        }

        return 0;
    } else {
        return -1;
    }
}

char** parse_read_header(char* header, int header_len) 
{   
    //not acutally a string
    char* pstring = malloc(header_len + 1);
    bzero(pstring, header_len + 1);
    memcpy(pstring, header, header_len);

    int i;
    for (i = 0; i < header_len; i++) {
        pstring[i] = tolower(pstring[i]);
    }

    int found_len = -1;
    int found_age = -1;
    
    //1. request name 2. length 3. age
    char** extracted_info = malloc(3 * sizeof(char*));

    char* extract = strtok(pstring, "\r\n");
    char* extract_copy = copy_string(extract);
    extracted_info[0] = extract_copy;

    extract = strtok(NULL, "\r\n");

    while (extract != NULL) {
        char* copy = copy_string(extract);
        if (found_len < 0) found_len = find_len(extract, extracted_info, 1);
        if (found_age < 0) found_age = find_age(copy, extracted_info, 2);
        free(copy);
        extract = strtok(NULL, "\r\n");
    }

    if (found_age < 0) {
        extracted_info[2] = "3600";
    }
    free(pstring);
    return extracted_info;
}

char* read_from_server(int sockfd, int* rh_len, char** file, int* f_len, int* age) {
    int n;
    int bytes_read = 0;
    int buf_size = 2048;
    int read_size = 2048;
    char* buf = malloc(buf_size);
    bzero(buf, buf_size);

    n = read(sockfd, buf, read_size);
    if (n < 0) error("Error reading from socket");
    bytes_read += n;
    int header_len = check_valid_header(buf, bytes_read);

    while (header_len < 0) {
        if (buf_size - bytes_read < read_size) {
            char* buf2 = realloc(buf, buf_size * 2);
            if (buf2 != NULL) {
                buf = buf2;
            } else {
                error("ERROR realloc");
            }

            bzero(buf + buf_size, buf_size);
            buf_size = buf_size * 2;
        }

        n = read(sockfd, buf + bytes_read, read_size);
        if (n < 0) error("Error reading from socket");
        bytes_read += n;
        header_len = check_valid_header(buf, bytes_read);
    }

    char* header = malloc(header_len);
    bzero(header, header_len);
    memcpy(header, buf, header_len);
    *rh_len = header_len;

    char** read_info = parse_read_header(header, header_len);

    int file_len = atoi(read_info[1]);
    int file_age = atoi(read_info[2]);

    *age = file_age;
    *f_len = file_len;

    free(read_info[0]);
    free(read_info[1]);
    if (file_age != 3600) {
        free(read_info[2]);
    }
    free(read_info);

    char* content = malloc(file_len);
    bzero(content, file_len);
    int content_read = bytes_read - header_len;
    memcpy(content, buf + header_len, content_read);
    free(buf);

    while(content_read < file_len) {
        int r = read(sockfd, content + content_read, file_len - content_read);
        if (n < 0) error("Error reading from socket");
        content_read += r;
    }

    *file = content;

    return header;
}


