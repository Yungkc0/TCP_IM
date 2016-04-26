#ifndef SERVCLI_H
#	define SERVCLI_H

/* write n bytes to a descriptor */
ssize_t writen(int fd, const char *vptr, size_t n);

/* read n bytes to a descriptor */
ssize_t readn(int fd, char *vptr, size_t n);

/* wrapped functions */
void imread(int fd, char *vptr, size_t n);

void imwrite(int fd, const char *vptr, size_t n);

void imselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

/* TCP_listen function */
int tcp_listen(const char *port);

#endif
