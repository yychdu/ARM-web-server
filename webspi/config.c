#include "config.h"

int log_fd = -1;

//init errorlog
int init_logfile(const char* filename)
{
	log_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
	if (log_fd == -1)
	{
		perror("init log file");
		return -1;
	}
	else
	{
		return log_fd;
	}
}

void sys_err(const char* str)
{
	if (log_fd == -1)
	{
		printf("errorlog init ERR\r\n!");
		return;
	}
	char msg[256];
	write(log_fd, str, strlen(str));
	sprintf(msg, "\nfile %s \t lint: %d \n reason :%s\n", __FILE__, __LINE__, strerror(errno));
	write(log_fd, msg, strlen(msg));
}

int get_conf(const char* file)
{
	int result = -1;
	char buf[512];
	char name[128], value[128];
	FILE* fp = fopen(file, "r");
	if (fp != NULL)
	{
		do
		{
			fgets(buf, 512, fp);
			if (buf[0] == '#')
				continue;
			else
			{
				sscanf(buf, "%[^=]=%s", name, value);
				//printf("%s  =   %s   !\n", name, value);
				setenv(name, value, 1);
			}
		} while (!feof(fp));
		fclose(fp);
		result = 0;
	}
	else
		printf("err open log file");
	return result;
}
