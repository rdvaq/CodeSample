#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <assert.h>
#include <string.h>

#include "proxy.h"
#include "proxy-cache.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: ./a.out [port number]\n");
        exit(1);
    }

    // initialize cache
    initialize_cache(10);

    //// first set up the server ////
    int p_sockfd = server_up(argv[1]);

    listen(p_sockfd, 5);

    while(1) {
        int c_sockfd = accept_client(p_sockfd);
        // read in header
        int gh_len;
        char* get_header = read_get_header(c_sockfd, &gh_len); 

        //////// FINISHED getting hearder, check if in cache/////
        int gh_arg;
        char** parsed_get_header = parse_get_header(get_header, gh_len, &gh_arg);

        // use name field from parsed_get_header to check if it is in cache
        char* req_name = parsed_get_header[0];
        file_Node* node = find_file(req_name);

        // determine if file is in cache 

        if (node != NULL && (!is_stale(node))) { //case: file in cache
            // update lru 
            get_file(node, c_sockfd);   
        } else {
            // case: file not in cache or file is stale
            /////// connect to server, act as client //////
            int server_sockfd = connect_server(parsed_get_header, gh_arg);

            int wb = write(server_sockfd, get_header, gh_len);
            if (wb < 0) error("ERROR writing to socket");

            char* new_file;
            int age;
            int rh_len;
            int f_len;
            char* read_header = read_from_server(server_sockfd, &rh_len,    
                                                &new_file, &f_len, &age);

            int w;
            w = write(c_sockfd, read_header, rh_len);
            if (w < 0) error("ERROR writing to socket");
            w = write(c_sockfd, new_file, f_len);
            if (w < 0) error("ERROR writing to socket");


            //cache file
            put_file(req_name, age, read_header, rh_len, new_file, f_len);

            close(server_sockfd);
        }

        free(parsed_get_header[1]);
        free(parsed_get_header[2]);
        free(parsed_get_header);
        free(get_header);
        close(c_sockfd);
    }

    clean_cache();
    return 0;
}



