#ifndef USERLIST_H
#	define USERLIST_H

struct clientinfo{
	uint16_t id;
	int fd;
	uint8_t cnt;
	pthread_t tid;
	char pwd[20];
	char name[20];
} UserList[100];

extern int Nusers;

/* find a user by id */
struct clientinfo *getuser(uint16_t id);

/* get client's password by ID */
char *getpwd(uint16_t id);

/* add a user to list and return an ID */
uint16_t adduser(int fd, const char *pwd, const char *name);

/* delete a user */
void deluser(uint16_t id);

/* cnt = 0 */
void cntzero(uint16_t id);

/* print the list of users to string */
void splist(char *buf);

#endif
