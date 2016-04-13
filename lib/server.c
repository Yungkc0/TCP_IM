#include "im.h"

/* send n bytes data to dst from socket fd */
int
imSend(int fd, uint16_t dst, char *buff, int nbytes)
{
	return 1;
}

/* recieve n bytes data from fd and save in buff[] */
int
imRecv(int fd, char *buff, const char *pwd)
{
	return 1;
}

/* TCP_listen function from Unix Network Programming [W.Richard Stevens] */
int
tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	return 1;
}

/* forward packet */
int
forward(int fd, const char *pwd, uint16_t ID)
{
	Packet packet;
	char buff[MAXLINE];
	uint16_t fromID = 0, toID = 0;

	imRecv(fd, buff, getpwd(fromID));
	sscanf(buff + 3, "%hu%hu", &fromID, &toID);
	if (ID != fromID)
		return 0;

	unpack(buff, &packet, pwd);
	pack(&packet, buff, getpwd(toID));
	imSend(fd, toID, buff, packet.n);

	return 1;
}
