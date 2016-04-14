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

/* TCP_listen function */
int
tcp_listen(const char *port)
{
	int listenfd;
	const int on = 1;
	struct sockaddr_in servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
		err_sys("socket error");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(port));

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		err_sys("setsockopt error");
	if (bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("bind error");
	
	if (listen(listenfd, LISTENQ) < 0)
		err_sys("listen error");

	return listenfd;
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
