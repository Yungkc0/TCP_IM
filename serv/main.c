#include "im.h"

int
main(int argc, char **argv)
{
	int listenfd, connfd;
	void sig_int(int);
	void *doit(void *);
	pthread_t tid;
	socklen_t addrlen;
	struct sockaddr_in cliaddr;

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
	int i, fd = *(int *) arg;
	uint8_t aeskey[32];
	char pwd[PWDSIZE];
	aes256_context ctx;
	memset(pwd, 0, PWDSIZE);

	mkaeskey(aeskey);
	aes256_init(&ctx, aeskey);
	DUMP("key: ", i, aeskey, sizeof(aeskey));
	/*enAES256(ctx.enckey, (uint8_t *) pwd);
	DUMP("enp: ", i, pwd, sizeof(pwd));*/
	write(fd, aeskey, 32);
	read(fd, pwd, PWDSIZE);
	DUMP("pwd: ", i, pwd, PWDSIZE);
	deAES256(aeskey, (uint8_t *) pwd);
	DUMP("dep: ", i, pwd, sizeof(pwd));
	write(fd, pwd, PWDSIZE);
	printf("%s\n", pwd);
	printf("\n");
	aes256_done(&ctx);

	if (close(fd) == -1)
		err_sys("close error");

	return NULL;
}
