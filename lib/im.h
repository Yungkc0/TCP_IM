#ifndef IM_H
# define IM_H

#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "aes256.h"
#include "md5.h"
#include "encrypt.h"
#include "server.h"
#include "imerr.h"
#include "packet.h"
#include "userlist.h"

#define MAXLINE 1024
#define PWDSIZE 16
#define NAMESIZE 16
#define RANDSIZE 20
#define LISTENQ 4096
#define SERVPORT "32111"

#define SA struct sockaddr

#define DUMP(s, i, buf, sz) {printf(s);                       \
														 for (i = 0; i < (sz); ++i)       \
														   printf("%02x ", *(buf + i) & 0xff);\
	                           printf("\n");}

#endif
