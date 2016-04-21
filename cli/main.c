#include "im.h"

int
main(int argc, char **argv)
{
	int sockfd, i;
	struct sockaddr_in serv;
	char buf[MAXLINE];
	uint8_t enkey[32];

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");
	memset(buf, 0, MAXLINE);
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv.sin_port = htons(32111);
	if (argc == 2)
		serv.sin_addr.s_addr = inet_addr(argv[1]);
	else if (argc == 3){
		serv.sin_addr.s_addr = inet_addr(argv[1]);
		serv.sin_port = htons(atoi(argv[2]));
	} else if (argc == 1)
		;
	else
		err_quit("usage: cli [IP address of server] [port]");

	if (connect(sockfd, (SA *) &serv, sizeof(serv)) < 0)
		err_sys("connect error");

	read(sockfd, enkey, 32);
	strcpy(buf, "abcdefghijk");
	enAES256(enkey, (uint8_t *) buf);
	DUMP("pwd: ", i, buf, strlen(buf));
	DUMP("key: ", i, enkey, sizeof(enkey));
	if (write(sockfd, buf, strlen(buf)) != strlen(buf))
		err_sys("write error");

	read(sockfd, buf, MAXLINE);
	printf("%s\n", buf);

	return 0;
}
