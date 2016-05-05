#include "im.h"

int Nusers = 2;

/* find a user by id */
struct clientinfo *
getuser(uint16_t id)
{
	int i;
	
	for (i = 2; i < Nusers; ++i)
		if (UserList[i].id == id)
			return &UserList[i];
	return NULL;
}

/* get user's password by ID */
char *
getpwd(uint16_t id)
{
	int i;

	for (i = 2; i < Nusers; ++i)
		if (UserList[i].id == id)
			return UserList[i].pwd;
	return NULL;
}

/* add a user to list and return an ID */
uint16_t
adduser(int fd, const char *pwd, const char *name)
{
	if (Nusers == 2)
		UserList[Nusers].id = 2;
	else if (Nusers < 100)
		UserList[Nusers].id = UserList[Nusers - 1].id + 1;
	else
		return 0;
	UserList[Nusers].fd = fd;
	UserList[Nusers].cnt = 0;
	strncpy(UserList[Nusers].pwd, pwd, PWDSIZE);
	strncpy(UserList[Nusers].name, name, NAMESIZE);
	return UserList[Nusers++].id;
}

/* delete a user */
void
deluser(uint16_t id)
{
	int i;

	for (i = 2; i < Nusers; ++i)
		if (UserList[i].id == id)
			memcpy(&UserList[i], &UserList[i + 1], (Nusers - i - 1) * sizeof(struct clientinfo));
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

/* print the list of users to string */
void
splist(char *buf)
{
	int i, j;

	j = 0;
	for (i = 2; i < Nusers; ++i) {
		buf[j + 1] = UserList[i].id, buf [j] = UserList[i].id >> 8;
		strncpy(buf + j + 2, UserList[i].name, NAMESIZE);
		j += 2 + NAMESIZE;
	}
	buf[j] = '\n';
}
