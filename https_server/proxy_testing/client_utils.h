#ifndef _CLIENT_UTILS_H
#define _CLIENT_UTILS_H

#include "../http.h"

HTTP_Response_T block_until_response(int socket);

#endif 