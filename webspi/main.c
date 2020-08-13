#include <stdio.h>
#include "socket.h"
#include "config.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "trans.h"
#include "string.h"

char* dir;
int main()
{
	//传感器数据-----
	int fd;
	int ret;
	char filename[] = "/dev/icm20608";
	signed int databuf[7];
	unsigned char data[14];
	signed int gyro_x_adc, gyro_y_adc, gyro_z_adc;
	signed int accel_x_adc, accel_y_adc, accel_z_adc;
	signed int temp_adc;

	float gyro_x_act, gyro_y_act, gyro_z_act;
	float accel_x_act, accel_y_act, accel_z_act;
	float temp_act;
	//----
	fd = open("/dev/icm20608", O_RDWR);
	FILE* fp = fopen("data.log", "a+");
	if (fp == NULL)
	{
		printf("init data log err!\n");
		return -1;
	}
	if (fd < 0) 
	{
		printf("can't open file %s\r\n", filename);
		return -1;
	}
	int result = 0;
	SA4 client;
	int Clientfd;
	char web_dir[128];
	char IP[128];
	int port;
	log_fd = init_logfile("error.log");
	if (log_fd == -1)
	{
		printf("init log ERR!");
		result = -1;
	}
	else
	{
		//-------------------------------------通过读写环境变量 读取 port 和 web_dir
		int r = get_conf("web.conf");
		if (r == -1)
		{
			sys_err("config");
			result = -1;
		}
		else
		{
			dir = getenv("web_dir");
			char* p = getenv("web_port");
			if (p == NULL || dir == NULL)
			{
				result = -1;
				sys_err("env val");
			}
			else
				port = atoi(p);
		}
		//------------------------
		int Serverfd = socket_b(port);
		if (Serverfd == -1)
		{
			result = -1;
			sys_err("socket");
		}
		else
		{
			r = listen(Serverfd, 5);
			if (r == -1)
			{
				result = -1;
				sys_err("listen");
			}
			else
			{
				int i = 0;
				socklen_t _Size = sizeof(client);
				while (1)
				{
					Clientfd = accept(Serverfd, (SA*)&client, &_Size);
					printf("C fd:%d\n", Clientfd);
					if (Clientfd == -1)
					{
						result = -1;
						sys_err("accept");
					}
					else
					{
						printf("new link ---------%s\n", inet_ntop(AF_INET, &client.sin_addr, IP, 128));
						ret = read(fd, databuf, sizeof(databuf));
						if (ret == 0) 
						{ 			/* 数据读取成功 */
							gyro_x_adc = databuf[0];
							gyro_y_adc = databuf[1];
							gyro_z_adc = databuf[2];
							accel_x_adc = databuf[3];
							accel_y_adc = databuf[4];
							accel_z_adc = databuf[5];
							temp_adc = databuf[6];

							/* 计算实际值 */
							gyro_x_act = (float)(gyro_x_adc) / 16.4;
							gyro_y_act = (float)(gyro_y_adc) / 16.4;
							gyro_z_act = (float)(gyro_z_adc) / 16.4;
							accel_x_act = (float)(accel_x_adc) / 2048;
							accel_y_act = (float)(accel_y_adc) / 2048;
							accel_z_act = (float)(accel_z_adc) / 2048;
							temp_act = ((float)(temp_adc)-25) / 326.8 + 25;
						}


						char recvdata[1024] = "";
						recv(Clientfd, recvdata, 1024, 0);
						printf("%s\trecv%dbyte data\n", recvdata, strlen(recvdata));
						char senddata[1024] = "HTTP/1.1 200\r\nContent-Type:text/html\r\n\r\n";
						send(Clientfd, senddata, strlen(senddata), 0);
						char string[1024] = "";
						char* ptr = string;
						sprintf(ptr, "<html>\r\n<body>\r\n<p>原始值:</p>\r\n<p>gx = %d, gy = %d, gz = %d</p>\r\n", gyro_x_adc, gyro_y_adc, gyro_z_adc);
						ptr += strlen(ptr);
						sprintf(ptr, "<p>ax = %d, ay = %d, az = %d</p>\r\n", accel_x_adc, accel_y_adc, accel_z_adc);
						ptr += strlen(ptr);
						sprintf(ptr, "<p>temp = %d</p>\r\n<p>实际值:</p>", temp_adc);
						ptr += strlen(ptr);
						sprintf(ptr, "<p>act gx = %.2f°/S, act gy = %.2f°/S, act gz = %.2f°/S</p>\r\n", gyro_x_act, gyro_y_act, gyro_z_act);
						ptr += strlen(ptr);
						sprintf(ptr, "<p>act ax = %.2fg, act ay = %.2fg, act az = %.2fg</p>\r\n", accel_x_act, accel_y_act, accel_z_act);
						ptr += strlen(ptr);
						sprintf(ptr, "<p>act temp = %.2f°C</p>\r\n", temp_act);

						strcat(string, "</body>\r\n<html>");
						fputs(string, fp);
						send(Clientfd, string, strlen(string), 0);
						close(Clientfd);
					}
				}
				close(Serverfd);
			}

		}
	}
	fclose(fp);
	close(fd);
	return result;
}