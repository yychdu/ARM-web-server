#pragma once
#ifndef __SOCK
#define __SOCK

#include "sys/types.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"

typedef struct sockaddr SA;
typedef struct sockaddr_in SA4;
int socket_b(int port);


#endif // !1
