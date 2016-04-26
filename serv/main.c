#include "im.h"

int
main(int argc, char **argv)
{
	int listenfd, connfd;
	void *doit(void *);
	pthread_t tid;
	socklen_t addrlen;
	struct sockaddr_in cliaddr;

	srand(time(0));

	if (argc == 2)
		listenfd = tcp_listen(argv[1]);
	else if (argc == 1)
		listenfd = tcp_listen(SERVPORT);
	else
		err_quit("usage: serv [port]");

	for ( ; ; ) {
		if ((connfd = accept(listenfd, (SA *) &cliaddr, &addrlen)) < 0)
			err_sys("accept error");
		if (pthread_create(&tid, NULL, &doit, (void *) &connfd) != 0)
			err_sys("pthread_create error");
	}
}

void *
doit(void *arg)
{
	int i, maxfdp1, fd = *(int *) arg;
	fd_set rset;
	struct timeval tvl = {0, 200 * 1000};
	uint8_t aeskey[32];
	char cmd = 0, pwd[PWDSIZE], buf[MAXLINE];
	aes256_context ctx;

	memset(pwd, 0, PWDSIZE);
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	maxfdp1 = fd + 1;
	mkaeskey(aeskey);
	aes256_init(&ctx, aeskey);
	DUMP("key: ", i, aeskey, sizeof(aeskey));

	readn(fd, buf, MAXLINE);
	printf(buf);
	imselect(maxfdp1, &rset, NULL, NULL, &tvl);
	if (FD_ISSET(fd, &rset)) {
		imread(fd, buf, MAXLINE);
		if (buf[0] != LO_REQ) {
			err_msg("invalid LO_REQ packet");
			pthread_exit(0);
		}
		FD_ZERO(&rset);
	} else {
		err_msg("Login request timeout");
		pthread_exit(0);
	}
	err_msg("abc");

	imwrite(fd, (const char *)aeskey, 32);

	imselect(maxfdp1, &rset, NULL, NULL, &tvl);
	if (FD_ISSET(fd, &rset)) {
		imread(fd, pwd, PWDSIZE);
		FD_ZERO(&rset);
	} else {
		err_msg("pwd timeout");
		pthread_exit(0);
	}

	DUMP("pwd: ", i, pwd, PWDSIZE);
	deAES256(aeskey, (uint8_t *) pwd);
	DUMP("dep: ", i, pwd, sizeof(pwd));

	imwrite(fd, pwd, PWDSIZE);
	printf("%s\n", pwd);
	printf("\n");
	aes256_done(&ctx);

	if (close(fd) == -1)
		err_msg("close error");

	return (NULL);
}
