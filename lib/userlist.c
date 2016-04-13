struct clientinfo{
	uint16_t ID;
	int addr;
	char pwd[PWDSIZE];
} UserList[100];

int Nusers = 0;

/* get client's password by ID */
char *
getpwd(uint16_t ID)
{
	return UserList[ID - 100].pwd;
}

/* add a user to list and return an ID */
uint16_t
adduser(int addr, char *pwd)
{
	UserList[Nusers].ID = Nusers + 100;
	UserList[Nusers].addr = addr;
	strncpy(UserList[Nuserf].pwd, pwd, strlen(pwd));
}
