#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
extern int log_fd;

int init_logfile(const char*);
int get_conf(const char*);
void sys_err(const char*);
#endif
