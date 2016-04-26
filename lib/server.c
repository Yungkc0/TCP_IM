#include "im.h"

/* write n bytes to a descriptor */
ssize_t
writen(int fd, const char *vptr, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	const char *ptr = vptr;

	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

/* read n bytes from a descriptor */
ssize_t
readn(int fd, char *vptr, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *ptr = vptr;
	
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);
}

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

/* wrapped functions */
void
imwrite(int fd, const char *vptr, size_t n)
{
	if (writen(fd, vptr, n) < 0)
		err_msg("write error");
}

void
imread(int fd, char *vptr, size_t n)
{
	if (readn(fd, vptr, n) < 0)
		err_msg("write error");
}

void
imselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	if (select(nfds, readfds, writefds, exceptfds, timeout) < 0)
		err_msg("select error");
}
