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
	int Clientfd = fd;//��ȡ�ͻ������󣬽����������У���ȡ������Ϣ
	get_request(fd, &req);//��ȡRealPath
	res.request_info = &req;
	do_request(&res);//����������Ϣ�������û��������γ���Ӧ��Ϣ
	//ʹ����Ӧ��Ϣ����Ӧ�ͻ���
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
		printf("\n%s\n", buf);//��ӡ��Ϣ
		sscanf(buf, "%s %s %s\r\n", req->method, req->path, req->protocol);
		if (strcmp(req->method, "GET") == 0)//��get
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
	//������·��ת��Ϊ�������ķ���·��
	strcpy(RealPath, dir);//eg: rpath = ~/web_full/html
	if (strcmp(req->path, "/") == 0)
		strcpy(req->path, "/index.html");
	strcat(RealPath, req->path);//rpath = ~/web_full/html/alter.html
	strcpy(req->path, RealPath);
}


//������Ӧ��Ϣ
/*
����״̬�룺
200 OK                        //�ͻ�������ɹ�
400 Bad Request               //�ͻ����������﷨���󣬲��ܱ������������
401 Unauthorized              //����δ����Ȩ�����״̬��������WWW-Authenticate��ͷ��һ��ʹ��
403 Forbidden                 //�������յ����󣬵��Ǿܾ��ṩ����
404 Not Found                 //������Դ�����ڣ�eg�������˴����URL
500 Internal Server Error     //��������������Ԥ�ڵĴ���
503 Server Unavailable        //��������ǰ���ܴ���ͻ��˵�����һ��ʱ�����ָܻ�����
*/
static void do_request(response_t* res)
{
	printf("response PATH : %s\n", res->request_info->path);
	if (res->request_info->Isstatic == true)
		res->code = access(res->request_info->path, R_OK) ? 404 : 200;//�Ƿ���Զ�ȡ
	else
		res->code = access(res->request_info->path, X_OK) ? 404 : 200;//�Ƿ����ִ��
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
	char first_line[128] = { 0 };//������Ϣ
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