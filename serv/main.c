#include "im.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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

	if (pthread_mutex_init(&mutex, NULL) != 0)
		err_sys("pthread_mutex_init error");

	cnt_timer();   /* start timer for cnt */
	signal(SIGALRM, cnt_signal_handler);

	for ( ; ; ) {
		if ((connfd = accept(listenfd, (SA *) &cliaddr, &addrlen)) < 0)
			err_sys("accept error");
		printf("---%d(%s) connected---", connfd, inet_ntoa(cliaddr.sin_addr));
		if (pthread_create(&tid, NULL, &doit, (void *) &connfd) != 0)
			err_sys("pthread_create error");
		printf("%d\n", UserList[Nusers].tid = tid);
	}
}

void *
doit(void *arg)
{
	int maxfdp1, fd = *(int *) arg;
	uint16_t id;
	fd_set rset;
	Packet pkt;
	struct timeval tvl = {0, 200 * 1000};
	uint8_t aeskey[32];
	uint32_t key[4];
	char pwd[PWDSIZE], buf[MAXLINE];
	aes256_context ctx;

	memset(pwd, 0, PWDSIZE);
	memset(buf, 0, MAXLINE);
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	maxfdp1 = fd + 1;
	mkaeskey(aeskey);
	aes256_init(&ctx, aeskey);

	/* login in */
	/* recieve LO_REQ and check it */
	imselect(maxfdp1, &rset, NULL, NULL, &tvl);
	if (FD_ISSET(fd, &rset)) {
		imread(fd, buf, MAXLINE);
		if (buf[0] != LO_REQ) {
			err_msg("%d: invalid LO_REQ packet, connection dropped!", fd);
			close(fd);
			pthread_exit(0);
		}
		FD_ZERO(&rset);
	} else {
		err_msg("%d: LO_REQ timeout, connection dropped!", fd);
		close(fd);
		pthread_exit(0);    /* timeout */
	}

	/* send aes256 key to encrypt password */
	mkpkt(&pkt, LO_KEY, 27 + 32, 0x00, 0x00, NULL, (char *) aeskey);
	pack(&pkt, buf);
	strncpy(buf + 27, pkt.data, 32);
	imwrite(fd, buf, pkt.n);

	/* recieve password and username */
	FD_SET(fd, &rset);
	imselect(maxfdp1, &rset, NULL, NULL, &tvl);
	if (FD_ISSET(fd, &rset)) {
		imread(fd, buf, MAXLINE);
		FD_ZERO(&rset);
	} else {
		err_msg("%d: LO_PWD timeout, connection dropped!", fd);
		close(fd);
		pthread_exit(0);    /* timeout */
	}

	unpack(buf, &pkt);       /* check password */
	strncpy(pwd, pkt.data, PWDSIZE);
	deAES256(aeskey, (uint8_t *) pwd);
	strncpy(pkt.data, pwd, PWDSIZE);

	if (!isvp(&pkt, LO_PWD)) { /* if some non-7 bit ASCII character in password */
		mkpkt(&pkt, LO_ERR, 27, 0, 0, NULL, NULL);
		pack(&pkt, buf);
		err_msg("%d: LO_ERR", fd);
		imwrite(fd, buf, 27);
		close(fd);
		pthread_exit(0);
	}
	
	/* send LO_OK and a ID number */
	if (pthread_mutex_lock(&mutex) != 0)
		err_sys("pthread_mutex_lock error:");
	id = adduser(fd, pkt.data, pkt.data + PWDSIZE);
	pthread_mutex_unlock(&mutex);
	sprintf(buf + 27, "%hu", id);
	mkpkt(&pkt, LO_OK, 27 + strlen(buf + 27), 0, id, buf + 7, buf + 27);
	mkrand(pkt.rand);
	mkpvtkey(pkt.rand, pwd, key);
	pack(&pkt, buf);
	if (!encrypt(pkt.data, pkt.n - 27, key))
		err_quit("fatal error");
	imwrite(fd, buf, pkt.n);
	/* login process end */

	/* loop */
	for ( ; ; ) {
		imread(fd, buf, MAXLINE);
		unpack(buf, &pkt);

		switch (pkt.cmd) {
			case IM_HEART:
				cntzero(id);
				break;
			case IM_QUIT:
				if (close(fd) == -1)
					err_sys("close error");
				deluser(id);
				pthread_exit(0);
				break;
		}
	}

	return (NULL);
}
