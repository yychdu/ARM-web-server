#include "stdio.h"
#include "socket.h"
#include "config.h"

//create socket  AND  bind socket
int socket_b(int port)
{
	int result = -1;
	//struct sockaddr serv;//通用地址类型
	SA4 serv;//ipv4 address
	int Serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (Serverfd != -1)
	{
		int opt = 1;
		int r = setsockopt(Serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if (r == -1)
			sys_err("setsockopt");
		else
		{
			serv.sin_family = AF_INET;
			serv.sin_port = htons(port);
			serv.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY 代表本机所有的ip地址  宏
			//将Serverfd和本地的ip地址和端口绑定
			r = bind(Serverfd, (SA*)&serv, sizeof(serv));
			if (r == -1)
				sys_err("bind");
			else
			{
				printf("bind Success!\n");
				result = Serverfd;
			}
		}
	}	
	else
		sys_err("socket");
	return result;
}

