#include "im.h"

int
main(int argc, char **argv)
{
	int i;
	char buff[MAXLINE];
	char pwd[PWDSIZE] = "test";
	Packet packet;
	snprintf(buff + 1, 3, "%hu", 16);
	strcpy(buff + 27, "abcdefg");

	packet.data = buff + 27;
	printf("%s\n", packet.data);
	pack(&packet, buff, pwd);
	for (i = 0; i < strlen(packet.data); ++i) 
		printf("%02x", buff[i] & 0xff);
	putchar('\n');
	printf("pa: %s\n", buff + 27);
	unpack(buff, &packet, pwd);
	printf("un: %s\n", buff + 27);

	int listenfd, connfd;
	void sig_int(int);
	void *doit(void *);
	pthread_t tid;
	socklen_t clilen, addrlen;
	struct sockaddr *cliaddr;

	if (argc == 2)
		listenfd = tcp_listen(argv[1]);
	else if (argc == 1)
		listenfd = tcp_listen(SERVPORT);
	else
		err_quit("usage: serv [ port ]");

	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error");

	for ( ; ; ) {
		clilen = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &clilen)) < 0)
			err_sys("accpet error");

		if (pthread_create(&tid, NULL, &doit, (void *) &connfd) != 0)
			err_sys("pthread_cetate error");
	}
}

void *
doit(void *arg)
{
	if (close((int) arg) == -1)
		err_sys("close error");

	return (NULL);
}
