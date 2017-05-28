#ifndef IM_H
#define IM_H

#include <getopt.h>
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
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/bio.h>

#include "lib/rsa.h"
#include "lib/md5.h"
#include "lib/encrypt.h"
#include "lib/server.h"
#include "lib/imerr.h"
#include "lib/packet.h"
#include "lib/userlist.h"

#define MAXLINE     4096
#define PWDSIZE     16
#define NAMESIZE    16
#define RANDSIZE    20
#define LISTENQ     4096
#define SERVPORT    "32111"
#define PUBKEY_PATH "public.key"
#define PRIKEY_PATH "private.key"

#define SA struct sockaddr

#define DUMP(s, i, buf, sz) {printf(s);\
    for (i = 0; i < (sz); ++i)       \
    printf("%02x ", *(buf + i) & 0xff);\
    printf("\n");}

#endif
