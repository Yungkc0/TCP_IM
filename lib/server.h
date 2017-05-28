#ifndef SERVCLI_H
#	define SERVCLI_H

/* wrapped functions */
void imread(int fd, char *buf, size_t n);

void imwrite(int fd, const char *buf, size_t n);

void imselect(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
	      struct timeval *timeout);

/* TCP_listen function */
int tcp_listen(const char *port);

/* set a timer for cnt */
void cnt_timer();
void cnt_signal_handler(int);

#endif
