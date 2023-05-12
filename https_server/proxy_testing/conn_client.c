#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
//notes: do all connect request contain host fields??

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// create a connect request header
// Params: resource, host, http version
// side effect: returns the length of the header through len
// returns: connect header
char* make_conn_request(char* resource, char* host, char* version, int* len);
char* make_get_request(char* resource, char* host, char* version, int* len);
char* read_from_server(int sockfd, int* rh_len, char** file, int* f_len, int* age);

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 5) {
       fprintf(stderr,"usage %s hostname port resource host\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");

    int ch_l;
    // char* ch = make_conn_request("www.google.com:443", "www.google.com:443",
    //                             "HTTP/1.1", &ch_l);
    // char* ch = make_conn_request("www.cs.cmu.edu/~prs/bio.html", "www.cs.cmu.edu",
    //                             "HTTP/1.1", &ch_l);  
    char* ch = make_conn_request(argv[3], argv[4], "HTTP/1.1", &ch_l);
                                                                
    // for (int i = 0; i < ch_l; i++) {
    //     putchar(ch[i]);
    // }

    n = write(sockfd, ch, ch_l);
    char buf[1024];
    n = read(sockfd, buf, 1024);
    // printf("read %d bytes\n", n);
    // for (int i = 0; i < n; i++) {
    //     putchar(buf[i]);
    // }

    int gh_l;
    // char* gh = make_get_request("www.google.com:443", "www.google.com:443",
    //                             "HTTPS/1.1", &gh_l);
    int host_len = strlen(argv[4]);
    int rec_len = strlen(argv[3]);
    // printf("%d\n", host_len);
    // printf("%d\n", rec_len);

    char* r = calloc(1, host_len);
    
    if (rec_len - host_len > 0) {
        char* rah = calloc(1, rec_len + 1);
        strcpy(rah, argv[3]);
        memcpy(r, rah + host_len, rec_len - host_len);
    } else {
        r[0] = '/';
    }

    // printf("%s\n", r);

    char* gh = make_get_request(r, argv[4], "HTTP/1.1", &gh_l);
                                 
    printf("issuing get request, req len: %d\n", gh_l);
    for (int i = 0; i < gh_l; i++) {
        putchar(gh[i]);
    }
    n = write(sockfd, gh, gh_l);


    char* file;
    int header_len;
    int file_len;
    int age;

    read_from_server(sockfd, &header_len, &file, &file_len, &age);
    int fd = open("t2_file", O_CREAT|O_RDWR, 0666);
    if (fd < 0) {
        fprintf(stderr, "file openeing error\n");
        exit(1);
    }
    write(fd, file, file_len);
    close(fd);
    free(file);
    // char* buf2 = calloc(1, 10000);
    // int bytes_read = 0; 

    // n = read(sockfd, buf2 + bytes_read, 1024);

    // while (n == 1024) {
    //     bytes_read += n;
    //     n = read(sockfd, buf2 + bytes_read, 1024);
    // }
    
    // printf("read %d bytes\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++) {
    //     putchar(buf2[i]);
    // }

    free(ch);
    free(gh);
    close(sockfd);
    return 0;
}

char* make_conn_request(char* resource, char* host, char* version, int* len) {
    // int header_len = 8 + strlen(resource) + 1 + strlen(version) + 4;
    int header_len = 8 + strlen(resource) + 1 + strlen(version) + 2 + 6 + strlen(host) + 4;
    char* h = calloc(header_len, 1);
    char conn[] = "CONNECT ";
    int index = 0;
    memcpy(h+index, conn, 8);
    index += 8;
    memcpy(h+index, resource, strlen(resource));
    index += strlen(resource);
    memcpy(h+index, " ", 1);
    index += 1;
    memcpy(h+index, version, strlen(version));
    index += strlen(version);
    memcpy(h+index, "\r\n", 2);
    index += 2;
    char ho[] = "Host: ";
    memcpy(h+index, ho, 6);
    index += 6;
    memcpy(h+index, host, strlen(host));
    index += strlen(host);
    memcpy(h+index, "\r\n\r\n", 4);
    *len = header_len;
    return h;
}


char* make_get_request(char* resource, char* host, char* version, int* len) {
    int header_len = 4 + strlen(resource) + 1 + strlen(version) + 2 + 6 + strlen(host) + 4;
    char* h = calloc(header_len, 1);
    char conn[] = "GET ";
    int index = 0;
    memcpy(h+index, conn, 4);
    index += 4;
    memcpy(h+index, resource, strlen(resource));
    index += strlen(resource);
    memcpy(h+index, " ", 1);
    index += 1;
    memcpy(h+index, version, strlen(version));
    index += strlen(version);
    memcpy(h+index, "\r\n", 2);
    index += 2;
    char ho[] = "Host: ";
    memcpy(h+index, ho, 6);
    index += 6;
    memcpy(h+index, host, strlen(host));
    index += strlen(host);
    memcpy(h+index, "\r\n\r\n", 4);
    *len = header_len;
    return h;
}

char* copy_string(char* string) {
    char* copy = malloc(strlen(string) + 1);
    strcpy(copy, string);
    return copy;
}

int check_valid_header(char* buf, int bytes_read) {
    char* hay_stack = malloc(bytes_read + 1);
    bzero(hay_stack, bytes_read + 1);
    memcpy(hay_stack, buf, bytes_read);

    char* res = strstr(hay_stack, "\r\n\r\n"); //returns the add of 'end' in haystack

    int header_len = -1;

    if (res != NULL) {
        header_len = (res + 4) - hay_stack; // pointer arith to figure out len
    }

    free(hay_stack);
    return header_len;
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