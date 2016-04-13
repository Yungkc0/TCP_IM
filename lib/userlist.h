#ifndef USERLIST_H
#	define USERLIST_H

/* get client's password by ID */
char *getpwd(uint16_t ID);

/* add a user to list and return an ID */
uint16_t adduser(int addr, char pwd);

#endif
