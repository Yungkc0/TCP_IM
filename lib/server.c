#include "im.h"

/* send data to dst from socket fd */
int
imSend(int fd, uint16_t dst, char *data)
{
}

/* recieve data from fd and save in buff[] */
int
imRecv(int fd, char *buff, int n)
{
}

/* TCP_listen function from Unix Network Programming [W.Richard Stevens] */
int
tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	return 1;
}
