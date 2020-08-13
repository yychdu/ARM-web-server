#include <stdio.h>
#include "trans.h"
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include <sys/socket.h>
//extern char *web_dir = "~/web_full/html";
extern char* dir;

void trans(int fd)
{
	request_t req;
	response_t res;
	printf("response fd:%d\n", fd);
	int Clientfd = fd;//获取客户的请求，解析请求首行，获取请求信息
	get_request(fd, &req);//获取RealPath
	res.request_info = &req;
	do_request(&res);//根据请求信息，处理用户的请求，形成响应信息
	//使用响应信息，响应客户端
	response_b(Clientfd, &res);
}

static void get_request(int fd, request_t* req)
{
	char buf[1024];
	int r = recv(fd, buf, sizeof(buf), 0);
	if (r == -1)
	{
		sys_err("recv");
		return;
	}
	else
	{
		printf("\n%s\n", buf);//打印信息
		sscanf(buf, "%s %s %s\r\n", req->method, req->path, req->protocol);
		if (strcmp(req->method, "GET") == 0)//仅get
			parse_get(req);
		else
		{
			printf("Method Cannot Parse\n!");
		}
	}

}


static void parse_get(request_t* req)
{
	char RealPath[128] = { 0 };
	if (strchr(req->path, '?') == NULL)
		req->Isstatic = true;
	else
	{
		req->Isstatic = false;
		char path[128];
		strcpy(path, req->path);
		char* p = strtok(path, "?");
		strcpy(req->path, p);
		p = strtok(NULL, "?");
		strcpy(req->param, p);
	}
	//将请求路径转换为服务器的发布路径
	strcpy(RealPath, dir);//eg: rpath = ~/web_full/html
	if (strcmp(req->path, "/") == 0)
		strcpy(req->path, "/index.html");
	strcat(RealPath, req->path);//rpath = ~/web_full/html/alter.html
	strcpy(req->path, RealPath);
}


//完善响应信息
/*
常见状态码：
200 OK                        //客户端请求成功
400 Bad Request               //客户端请求有语法错误，不能被服务器所理解
401 Unauthorized              //请求未经授权，这个状态代码必须和WWW-Authenticate报头域一起使用
403 Forbidden                 //服务器收到请求，但是拒绝提供服务
404 Not Found                 //请求资源不存在，eg：输入了错误的URL
500 Internal Server Error     //服务器发生不可预期的错误
503 Server Unavailable        //服务器当前不能处理客户端的请求，一段时间后可能恢复正常
*/
static void do_request(response_t* res)
{
	printf("response PATH : %s\n", res->request_info->path);
	if (res->request_info->Isstatic == true)
		res->code = access(res->request_info->path, R_OK) ? 404 : 200;//是否可以读取
	else
		res->code = access(res->request_info->path, X_OK) ? 404 : 200;//是否可以执行
	if (res->code == 404)
	{
		memset(res->request_info->path, 0, sizeof(res->request_info->path));
		strcpy(res->request_info->path, dir);
		strcat(res->request_info->path, "/error.html");
	}
	res->file_type = get_ftype(res->request_info->path);
}

static char* get_ftype(const char* path)
{
	char* file_type = strrchr(path, '.');
	if (strcmp(file_type, ".jpg") == 0)
		return "image/jpg";
	else if (strcmp(file_type, ".png") == 0)
		return "image/png";
	return "text/html";
}

static void response_b(int fd, response_t* res)
{
	char first_line[128] = { 0 };//首行信息
	char content[128] = { 0 };
	sprintf(first_line, "%s %d\r\n", res->request_info->protocol, res->code);
	write(fd, first_line, strlen(first_line));
	if (res->code == 404 || res->request_info->Isstatic == true)
	{
		//sprintf(first_line, "%s %d\r\n", res->request_info->protocol, res->code);
		sprintf(content, "Content-Type:%s\r\n\r\n", res->file_type);
		write(fd, content, strlen(content));
		dup2(fd, 1);
		execlp("cat", "cat", res->request_info->path, NULL);
	}
	else
	{
		dup2(fd, 1);
		setenv("QUERY_STRING", res->request_info->param, 1);
		execl(res->request_info->path, "path", NULL);
	}
}