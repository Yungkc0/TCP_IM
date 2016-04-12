#ifndef SERVCLI_H
#	define SERVCLI_H

/* send data to dst from socket fd */
int imSend(int fd, uint16_t dst, const char *pwd, char *data, int nbytes);

/* recieve data from fd and save in buff[] */
int imRecv(int fd, const char *pwd, char *buff, int n);

/* TCP_listen function from Unix Network Programming [W.Richard Stevens] */
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);

/* packet n bytes data */
void packet(char *packet, const char *pwd, char *data, int nbytes);

#endif
