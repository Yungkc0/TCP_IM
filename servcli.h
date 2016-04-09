#ifndef SERVCLI_H
#	define SERVCLI_H

/* send data to dst from socket fd */
int imSend(int fd, uint16_t dst, char *data);

/* recieve data from fd and save in buff[] */
int imRecv(int fd, char *buff, int n);

#endif
