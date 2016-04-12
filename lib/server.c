#include "im.h"

/* send n bytes data to dst from socket fd */
int
imSend(int fd, uint8_t cmd, uint16_t dst, const char *pwd, const char *data, int nbytes)
{
	int n;
	char packet[n = nbytes - nbytes % 8 + 8];

	snprintf(packet, 1, "%c", cmd);
}

/* recieve n bytes data from fd and save in buff[] */
int
imRecv(int fd, const char *pwd, char *buff, int nbytes)
{
}

/* TCP_listen function from Unix Network Programming [W.Richard Stevens] */
int
tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	return 1;
}

/* packet n bytes data */
void
pack(char *packet, const char *pwd, const char *data, int n)
{
	char rand[RANDSIZE];
	uint32_t k[4];

	mkrand(rand);
	mkpvtkey(rand, pwd, k);
	strncpy(packet + 7, rand, RANDSIZE);
	strncpy(packet + 27, data, n);
	if (!encrypt(packet + 27, n, k))
		err_quit("A packet error!!!");
}

/* unpack n bytes data */
void
unpack()
{
}
