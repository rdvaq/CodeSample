// #include <stdlib.h>
// #include <stdio.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <netdb.h>
// #include <assert.h>
// #include <string.h>


// int main() {


//     double one = 10.012310304235905345934;
//     double two = 11.23446423948892023984;
//     int diff = 100;
//     // printf("%d\n", diff);
//     char num[10];
//     sprintf(num, "%d", diff);

//     // itoa(diff, num, 10);
//     printf("%s\n", num);
//     printf("%d\n", strlen(num));
//     // // char s[22] = "Host: www.cs.tufts.edu= ";
//     // char* s = malloc(40);
//     // strcpy(s, "Host: 1 www.cs.tufts.edu=       ");
//     // // printf("%s\n", s);

//     // // char* extract2 = strtok_r(s, ":", &s);
//     // char* extract2 = strtok_r(s, "=", &s);
//     // printf("%s\n", s);

//     // extract2 = strtok_r(s, " ", &s);

//     // if (extract2 == NULL) printf("null\n");
//     // printf("%d\n", strlen(extract2));
//     // printf("%s\n", extract2);

//     // printf("%s\n", extract2);
//     // // printf("%d\n", strlen(extract2));
//     // printf("%s\n", s);
//     // char* extract3 = strtok_r(s, " ", &s);
//     // printf("%s\n", extract3);

//     //     char* extract4 = strtok_r(s, " ", &s);
//     // printf("%s\n", extract4);
//     // printf("%d\n", strlen(extract2));

//     // extract2 = strtok_r(s, " ", &s);
//     // printf("%s\n", extract2);

//     // char needle[4] = "ABCD";
//     //char needle[4] = "Host:";
//     // // char hay[152] = "GET http://www.cs.tufts.edu/comp/112/index.html HTTP/1.1\nHost: www.cs.tufts.edu\nUser-Agent: curl/7.61.1\nAccept: */*\nProxy-Connection: Keep-Alive\n\n";
//     // // char hay[40] = "asdadasdasdadasdadasdaABCDnasdasdasda";

//     // char* res = strstr(hay, needle);

//     // if (res == NULL) {
//     //     printf("null\n");
//     // } else {
//     //     printf("%s\n %d\n", res, hay - res);
//     // }
    


//     // printf("%d\n", '\r');

//     return 0;
// }


//     // printf("host is: %s, len is: %d\n", header_info[1], strlen(header_info[1]));
//     // printf("%s\n", header_info[1]);
//     // printf("len is: %d\n", strlen(header_info[1]));

//     // printf("char:%c", header_info[1][16]);
//     // for (int i = 0; i < strlen(header_info[1]); i++) {
//     //     if (header_info[1][i] == '\0') printf("\nnull found\n");
//     //     if (header_info[1][i] == ' ') printf("\nspace\n");
//     //     if (header_info[1][i] == '\n') printf("\nnew line\n");
//     //     if (header_info[1][i] == '\r') printf("\nnew line\n");
//     //     printf("%c", header_info[1][i]);
//     // }
//     // printf("\n");



// char* make_header(char** header_info, int h_len) {
//     int total_len;

//     // create get request
//     int get_len = strlen(header_info[0]); 
//     char get[get_len + 1]; //add 1 for \n
//     strncat(get, header_info[0], get_len);
//     get[get_len] = '\n';

//     // printf("header get: %s\n", get);


//     int host_len = strlen(header_info[1]) + 7; // 6 for "Host: ", 1 for \n 
//     if (h_len > 2) {
//         int port_len = strlen(header_info[2] + 1); // add 1 for :
//         total_len = get_len + host_len + port_len + 1; // add 1 for final \n
//     } else {
//         total_len = get_len + host_len + 1;
//     }

//     char* header = malloc(total_len);

//     return header;
// }