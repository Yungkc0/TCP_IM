#ifndef SERVCLI_H
#	define SERVCLI_H

/* send data to dst from socket fd */
int imSend(int fd, uint16_t dst, char *buff, int nbytes);

/* recieve data from fd and save in buff[] */
int imRecv(int fd, char *buff, const char *pwd);

/* TCP_listen function from Unix Network Programming [W.Richard Stevens] */
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);

#endif
