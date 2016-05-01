#include "im.h"

int Nusers = 2;

/* get client's password by ID */
char *
getpwd(uint16_t id)
{
	int i;
	for (i = 2; i < Nusers; ++i) {
		if (UserList[i].id == id)
			return UserList[i].pwd;
	}
	return NULL;
}

/* add a user to list and return an ID */
uint16_t
adduser(int fd, const char *pwd, const char *name)
{
	UserList[Nusers].fd = fd;
	strncpy(UserList[Nusers].pwd, pwd, PWDSIZE);
	strncpy(UserList[Nusers].name, name, NAMESIZE);
	UserList[Nusers].id = Nusers + 100;
	return UserList[Nusers++].id;
}

/* delete a user */
void
deluser(uint16_t id)
{
	int i;
	for (i = 2; i < Nusers; ++i)
		if (UserList[i].id == id)
			memcpy(&UserList[i], &UserList[Nusers - 1], (Nusers - i) * sizeof(struct clientinfo));
	--Nusers;
}

/* cnt = 0 */
void
cntzero(uint16_t id)
{
	int i;
	for (i = 2; i < Nusers; ++i)
		if (UserList[i].id == id)
			UserList[i].cnt = 0;
}
