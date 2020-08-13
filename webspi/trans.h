#pragma once
#include "unistd.h"
#include "ctype.h"
#include "stdbool.h"

typedef struct
{
	char method[16];//GET POST
	char path[512];
	char protocol[16];//HTTP/1.1
	char param[128];
	bool Isstatic;
}request_t;//保存请求信息的结构体

typedef struct
{
	request_t* request_info;
	int code;// 200 404
	char* file_type;
}response_t;//响应信息头

void trans(int fd);
static void parse_get(request_t* req);
static void do_request(response_t* res);
static void response_b(int fd, response_t* res);
static char* get_ftype(const char* path);
static void get_request(int fd, request_t* req);