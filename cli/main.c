#include "im.h"

Packet p;
int sockfd, cnt = 0;
char c[7];
char buf[MAXLINE], pwd[PWDSIZE] = "";
uint32_t lkey[4];
uint32_t key[4];
void
int_handler(int m)
{
	c[0] = IM_QUIT;
	imwrite(sockfd, c, 27);
	close(sockfd);
	exit(0);
}
void
cnt_handler(int m)
{
	if (++cnt == 5)
		close(sockfd);
	mkpkt(&p, IM_HEART, 0x07, 0x01, 0, NULL, NULL);
	pack(&p, c);
	imwrite(sockfd, c, 7);
}
int
main(int argc, char **argv)
{
	int i;
	struct sockaddr_in serv;
	uint8_t enkey[32];

	srand((int)time(0));

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");
	memset(buf, 0, MAXLINE);
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv.sin_port = htons(atoi(SERVPORT));
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

	buf[0] = LO_REQ;
	imwrite(sockfd, buf, strlen(buf));
	imread(sockfd, buf, MAXLINE);
	strncpy((char *) enkey, buf + 27, 32);
	strcpy(pwd, "abcdefghi");
	strncpy(buf + 27, pwd, PWDSIZE);
	strncpy(buf + 27 + PWDSIZE, "yungkc", NAMESIZE);
	enAES256(enkey, (uint8_t *) buf + 27);
	enAES256(enkey, (uint8_t *) buf + 27 + PWDSIZE);
	buf[0] = LO_PWD;
	imwrite(sockfd, buf, PWDSIZE + NAMESIZE + 27);

	imread(sockfd, buf, MAXLINE);
	if (buf[0] == LO_ERR)
		exit(0);
	mkpvtkey(buf + 7, pwd, key);
	decrypt(buf + 27, 8, key);
	err_msg("%s", buf + 27);
	uint16_t id = atoi(buf + 27);

	cnt_timer();
	signal(SIGALRM, cnt_handler);
	signal(SIGINT, int_handler);

	pthread_t tid;
	void *doit(void *);
	if (pthread_create(&tid, NULL, &doit, (void *) NULL) != 0)
		err_sys("pthread_create error");
	
	Packet pq;
	mkpkt(&pq, IM_GETLKEY, 27, id, 0, NULL, NULL);
	pack(&pq, buf);
	imwrite(sockfd, buf, pq.n);
	for ( ; ; ) {
		fgets(buf + 27, 20, stdin);
		if (!strcmp(buf + 27, "gl\n")) {
			mkpkt(&pq, IM_GETLIST, 27, id, 0, buf + 7, buf + 27);
			pack(&pq, buf);
			imwrite(sockfd, buf, pq.n);
		} else if (buf[27] == 'l' && buf[28] == ' ') {
			mkpkt(&pq, IM_SENDL, 20 + 27, id, 1, buf + 7, buf + 27);
			pack(&pq, buf);
			encrypt(pq.data, pq.n - 27, lkey);
			imwrite(sockfd, buf, pq.n);
		} else {
			mkpkt(&pq, IM_SENDP, 20 + 27, id, 3, buf + 7, buf + 27);
			pack(&pq, buf);
			mkrand(pq.rand);
			mkpvtkey(pq.rand, pwd, key);
			if (!encrypt(pq.data, pq.n - 27, key))
				err_quit("fatal error");
			imwrite(sockfd, buf, pq.n);
		}
	}
	exit(0);
}

void *
doit(void *arg)
{
	int i;
	Packet pt;
	char rbuf[MAXLINE];
	
	pt.n = MAXLINE;
	for (;;) {
		memset(rbuf, 0, MAXLINE);
		imread(sockfd, rbuf, MAXLINE);
		unpack(rbuf, &pt);

		switch (pt.cmd) {
			case IM_HEART:
				cnt = 0;
				break;
			case IM_LIST:
				mkpvtkey(pt.rand, pwd, key);
				if (!decrypt(pt.data, pt.n - 27, key))
					err_quit("fatal error");
				for (i = 0; pt.data[i] != '\n'; i += 18)
					printf("%d-%s", pt.data[i + 1], pt.data + i + 2);
				putchar('\n');
				break;
			case IM_LKEY:
				mkpvtkey(pt.rand, pwd, key);
				printf("lkey: %s\n", pt.data);
				sscanf(pt.data, "%d%d%d%d", &lkey[0], &lkey[1], &lkey[2], &lkey[3]);
				printf("%08x %08x %08x %08x\n", lkey[0], lkey[1], lkey[2], lkey[3]);
				break;
			case IM_SENDP:
				mkpvtkey(pt.rand, pwd, key);
				if (!decrypt(pt.data, pt.n - 27, key))
					err_quit("fatal error");
				printf("%s", pt.data);
				break;
			case IM_SENDL:
				decrypt(pt.data, pt.n - 27, lkey);
				printf("%s", pt.data);
				break;
			default:
				;
		}
	}
}
