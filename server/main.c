#include "../im.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static uint32_t lkey[4];
static char public_key[MAXLINE];
static int pubkey_len;
static char private_key[MAXLINE];
static char *port;

int main(int argc, char **argv)
{
	int listenfd = -1, connfd;
	void *doit(void *);
    	void config(int, char **);
	pthread_t tid;
	socklen_t addrlen;
	struct sockaddr_in cliaddr;

    	config(argc, argv);
	srand(time(0));

    	listenfd = tcp_listen(port);

	cnt_timer();		/* start timer for cnt */
	signal(SIGALRM, cnt_signal_handler);

	for (;;) {
		if ((connfd = accept(listenfd, (SA *) & cliaddr, &addrlen)) < 0)
			err_sys("accept error");
        if (pthread_create(&tid, NULL, &doit, (void *)&connfd) != 0)
		    err_sys("pthread_create error");
        pthread_mutex_lock(&mutex);
        UserList[Nusers].tid = tid;
        pthread_mutex_unlock(&mutex);
    }
}

void config(int argc, char *argv[])
{
    int c;
    FILE *fp;

    char *pubkey_path = PUBKEY_PATH;
    char *prikey_path = PRIKEY_PATH;
    port = SERVPORT;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"private", required_argument, 0, 'r' },
            {"public",  required_argument, 0, 'u' },
            {"port",    required_argument, 0, 'o' },
            {0,         0,                 0, 0 }
        };

        c = getopt_long(argc, argv, "", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'p':
                prikey_path = optarg;
                break;
            case 'u':
                pubkey_path = optarg;
                break;
            case 'o':
                port = optarg;
                break;
            default:
                ;
        }
    }

    if ((fp = fopen(prikey_path, "r+")) == NULL)
        err_sys(prikey_path);
    while (fgets(private_key, MAXLINE, fp) != NULL)
        ;
    fclose(fp);
    if ((fp = fopen(pubkey_path, "r+")) == NULL)
        err_sys(pubkey_path);
    while (fgets(public_key, MAXLINE, fp) != NULL)
        ;
    fclose(fp);
    pubkey_len = strlen(public_key);
}

void *doit(void *arg)
{
	int i, maxfdp1, fd = *(int *)arg;
	uint16_t id;
	fd_set rset;
	Packet pkt;
	struct timeval tvl = { 5, 0 };
	uint32_t key[4];
	char pwd[PWDSIZE], buf[MAXLINE];
	char rsa_decrypted[MAXLINE];
	struct clientinfo *dstusr;

	memset(pwd, 0, PWDSIZE);
	memset(buf, 0, MAXLINE);
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	maxfdp1 = fd + 1;

	/* login in */
	/* recieve LO_REQ and check it */
	imselect(maxfdp1, &rset, NULL, NULL, &tvl);
	if (FD_ISSET(fd, &rset)) {
		imread(fd, buf, MAXLINE);
		if (buf[0] != LO_REQ) {
			err_msg
			    ("%d: invalid LO_REQ packet, connection dropped!",
			     fd);
			close(fd);
			pthread_exit(0);
		}
		FD_ZERO(&rset);
	} else {
		err_msg("%d: LO_REQ timeout, connection dropped!", fd);
		close(fd);
		pthread_exit(0);	/* timeout */
	}

	/* send RSA publickey(LO_KEY) to encrypt password */
	mkpkt(&pkt, LO_KEY, 27 + pubkey_len, 0x00, 0x00, NULL, (char *) public_key);
	pack(&pkt, buf);
	strncpy(buf + 27, pkt.data, pubkey_len);
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
		pthread_exit(0);	/* timeout */
	}

	unpack(buf, &pkt);	/* check password */
	strncpy(pwd, pkt.data, PWDSIZE);
	rsa_private_decrypt((unsigned char*) pkt.data, pkt.n, (unsigned char*) private_key, (unsigned char*) rsa_decrypted);
	strncpy(pkt.data, pwd, PWDSIZE);
	if (!isvp(&pkt, LO_PWD)) {	/* if some non-7bits ASCII character in password */
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
        pthread_mutex_unlock(&mutex);
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
	for (;;) {
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
			pthread_mutex_lock(&mutex);	/* lock to protect UserList */
			if ((dstusr = getuser(pkt.toID)) == NULL) {
				pthread_mutex_unlock(&mutex);
				break;
			}
			mkpvtkey(pkt.rand, dstusr->pwd, key);
			encrypt(pkt.data, pkt.n - 27, key);
			imwrite(dstusr->fd, buf, pkt.n);
			pthread_mutex_unlock(&mutex);
			printf("id %hu ---> id %hu (%d bytes)\n", pkt.fromID,
			       pkt.toID, pkt.n);
			break;
		case IM_SENDL:
			printf(">>>id %hu ---> chat room<<<\n", id);
			pack(&pkt, buf);
			for (i = 2; i < Nusers; ++i)
				imwrite(UserList[i].fd, buf, pkt.n);
			break;
		case IM_GETLIST:
			printf(">>>id %hu: request for list<<<\n", id);
			mkpkt(&pkt, IM_LIST,
			      27 + (NAMESIZE + 2) * (Nusers - 2) + 1, 0, id,
			      buf + 7, buf + 27);
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
			if (Nusers == 3)	/* update or create a key for chat room */
				mkpvtkey(pkt.rand, pwd, lkey);
			sprintf(pkt.data, "%d %d %d %d", lkey[0], lkey[1],
				lkey[2], lkey[3]);
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
