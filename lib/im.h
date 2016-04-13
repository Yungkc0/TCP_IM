#ifndef IM_H
# define IM_H

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include "md5.h"
#include "encrypt.h"
#include "server.h"
#include "imerr.h"
#include "packet.h"
#include "userlist.h"

#define MAXLINE 1024
#define PWDSIZE 16
#define RANDSIZE 20

#endif
