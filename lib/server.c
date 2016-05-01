#include "im.h"

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

/* timer for cnt */
void
cnt_timer()
{
	struct itimerval itv;
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, NULL);
}

void
cnt_signal_handler(int m)
{
	int i;

	for (i = 2; i < Nusers; ++i) {
		if (++UserList[i].cnt >= 5) {
			if (close(UserList[i].fd) == -1)
				err_sys("close error");
			err_msg(">>>%d (thread %ld): client crashed, thread exit.<<<", UserList[i].fd, UserList[i].tid);
			pthread_cancel(UserList[i].tid);
			deluser(UserList[i].id);
		}
	}
}

/* wrapped functions */
void
imwrite(int fd, const char *buf, size_t n)
{
	if (write(fd, buf, n) < 0)
		err_sys("write error");
}

void
imread(int fd, char *buf, size_t n)
{
	if (read(fd, buf, n) < 0)
		err_sys("read error");
}

void
imselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	if (select(nfds, readfds, writefds, exceptfds, timeout) < 0)
		err_sys("select error");
}
