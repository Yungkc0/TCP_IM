#include "../im.h"

/* TCP_connect function */
int tcp_connect(const char *host, const char *port)
{
	int fd, n;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, port, &hints, &res)) != 0)
		err_quit("tcp_connect error for %s, %s: %s", host, port, gai_strerror(n));
	ressave = res;

	do {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0)
			continue;
		if (connect(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;         /* Success */
		if (close(fd) < 0)
			err_sys("close error");
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		err_quit("tcp_connect error for %s, %s", host, port);

	freeaddrinfo(ressave);

	return fd;
}

/* TCP_listen function */
int tcp_listen(const char *port)
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
	if (bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))
	    < 0)
		err_sys("bind error");

	if (listen(listenfd, LISTENQ) < 0)
		err_sys("listen error");

	return listenfd;
}

/* timer for cnt */
void cnt_timer()
{
	struct itimerval itv;
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, NULL);
}

void cnt_signal_handler(int signo)
{
	int i;

    UNUSED(signo);
	for (i = 2; i < Nusers; ++i) {
		if (++UserList[i].cnt >= 5) {
			pthread_cancel(UserList[i].tid);
			if (close(UserList[i].fd) == -1)
				err_sys("close error");
			err_msg
			    (">>>%hu (thread %ld): client crashed, thread exit.<<<",
			     UserList[i].id, UserList[i].tid);
			deluser(UserList[i].id);
		}
	}
}

/* wrapped functions */
void imwrite(int fd, const char *buf, size_t n)
{
	if (write(fd, buf, n) < 0)
		err_msg("write error");
}

void imread(int fd, char *buf, size_t n)
{
	if (read(fd, buf, n) < 0)
		err_msg("read error");
}

void
imselect(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
	 struct timeval *timeout)
{
	if (select(nfds, readfds, writefds, exceptfds, timeout) < 0)
		err_sys("select error");
}
