#include "im.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
uint32_t lkey[4];

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

	cnt_timer();   /* start timer for cnt */
	signal(SIGALRM, cnt_signal_handler);

	for ( ; ; ) {
		if ((connfd = accept(listenfd, (SA *) &cliaddr, &addrlen)) < 0)
			err_sys("accept error");
		if (pthread_create(&tid, NULL, &doit, (void *) &connfd) != 0)
			err_sys("pthread_create error");
		pthread_mutex_lock(&mutex);
		UserList[Nusers].tid = tid;
		pthread_mutex_unlock(&mutex);
	}
}

void *
doit(void *arg)
{
	int i, maxfdp1, fd = *(int *) arg;
	uint16_t id;
	fd_set rset;
	Packet pkt;
	struct timeval tvl = {0, 200 * 1000};
	uint8_t aeskey[32];
	uint32_t key[4];
	char pwd[PWDSIZE], buf[MAXLINE];
	aes256_context ctx;
	struct clientinfo *dstusr;

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

	/* send aes256 key(LO_KEY) to encrypt password */
	mkpkt(&pkt, LO_KEY, 27 + 32, 0x00, 0x00, NULL, (char *) aeskey);
	pack(&pkt, buf);
	strncpy(buf + 27, pkt.data, 32);
	imwrite(fd, buf, pkt.n);

	/* recieve password and username(LO_PWD) */
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
	deAES256(aeskey, (uint8_t *) pkt.data + PWDSIZE);
	strncpy(pkt.data, pwd, PWDSIZE);
	if (!isvp(&pkt, LO_PWD)) { /* if some non-7 bit ASCII character in password */
		mkpkt(&pkt, LO_ERR, 27, 0, 0, NULL, NULL);
		pack(&pkt, buf);
		imwrite(fd, buf, pkt.n);
		close(fd);
		pthread_exit(0);
	}

	/* send LO_OK and a ID number */
	if (pthread_mutex_lock(&mutex) != 0)
		err_sys("pthread_mutex_lock error");
	if ((id = adduser(fd, pkt.data, pkt.data + PWDSIZE)) == 0) {
		mkpkt(&pkt, LO_ERR, 27, 0, 0, NULL, NULL);
		pack(&pkt, buf);
	printf("ok\n");
		imwrite(fd, buf, pkt.n);
		close(fd);
		pthread_exit(0);
	}
	pthread_mutex_unlock(&mutex);
	sprintf(buf + 27, "%hu", id);
	mkpkt(&pkt, LO_OK, 27 + strlen(buf + 27), 0, id, buf + 7, buf + 27);
	mkrand(pkt.rand);
	mkpvtkey(pkt.rand, pwd, key);
	pack(&pkt, buf);
	if (!encrypt(pkt.data, pkt.n - 27, key))
		err_quit("fatal error");
	imwrite(fd, buf, pkt.n);
	printf(">>>id %hu: login ok<<<\n", id);
	/* login process end */

	/* loop */
	for ( ; ; ) {
		memset(buf, 0, pkt.n);
		imread(fd, buf, MAXLINE);
		unpack(buf, &pkt);

		switch (pkt.cmd) {
			case IM_HEART:
				cntzero(id);
				pkt.toID = pkt.fromID;
				pkt.fromID = 0x00;
				pack(&pkt, buf);
				imwrite(fd, buf, pkt.n);
				break;
			case IM_QUIT:
				if (close(fd) == -1)
					err_sys("close error");
				if (pthread_mutex_lock(&mutex) != 0)
					err_sys("pthread_mutex_lock error");
				deluser(id);
				pthread_mutex_unlock(&mutex);
				printf(">>>id %hu: client quit<<<\n", id);
				pthread_exit(0);
				break;
			case IM_SENDP:
				mkpvtkey(pkt.rand, pwd, key);
				decrypt(pkt.data, pkt.n - 27, key);
				pthread_mutex_lock(&mutex);     /* lock to protect UserList */
				if ((dstusr = getuser(pkt.toID)) == NULL) {
					pthread_mutex_unlock(&mutex);
					break;
				}
				mkpvtkey(pkt.rand, dstusr->pwd, key);
				encrypt(pkt.data, pkt.n - 27, key);
				imwrite(dstusr->fd, buf, pkt.n);
				pthread_mutex_unlock(&mutex);
				printf("id %hu ---> id %hu (%d bytes)\n", pkt.fromID, pkt.toID, pkt.n);
				break;
			case IM_SENDL:
				printf(">>>id %hu ---> chat room<<<\n", id);
				pack(&pkt, buf);
				for (i = 2; i < Nusers; ++i)
					imwrite(UserList[i].fd, buf, pkt.n);
				break;
			case IM_GETLIST:
				printf(">>>id %hu: request for list<<<\n", id);
				mkpkt(&pkt, IM_LIST, 27 + (NAMESIZE + 2) * (Nusers - 2) + 1, 0, id, buf + 7, buf + 27);
				splist(pkt.data);
				mkrand(pkt.rand);
				mkpvtkey(pkt.rand, pwd, key);
				pack(&pkt, buf);
				encrypt(pkt.data, pkt.n - 27, key);
				imwrite(fd, buf, pkt.n);
				break;
			case IM_GETLKEY:
				mkpkt(&pkt, IM_LKEY, 27, 0, id, buf + 7, buf + 27);
				mkrand(pkt.rand);
				mkpvtkey(pkt.rand, pwd, key);
				if (Nusers == 3)       /* update or create a key for chat room */
					mkpvtkey(pkt.rand, pwd, lkey);
				sprintf(pkt.data, "%d %d %d %d", lkey[0], lkey[1], lkey[2], lkey[3]);
				pkt.n += strlen(pkt.data);
				encrypt(pkt.data, pkt.n - 27, key);
				pack(&pkt, buf);
				imwrite(fd, buf, pkt.n);
				break;
			default:
				;
		}
	}
}
