#include "im.h"

int sockfd;
char c[27] = {IM_HEART};
void
cnt_handler(int m)
{
	imwrite(sockfd, c, 27);
}
int
main(int argc, char **argv)
{
	int i;
	struct sockaddr_in serv;
	char buf[MAXLINE], pwd[PWDSIZE] = "";
	uint32_t key[4];
	uint8_t enkey[32];

	srand((int)time(0));

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");
	memset(buf, 0, MAXLINE);
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv.sin_port = htons(atoi(SERVPORT));
	if (argc == 2)
		serv.sin_addr.s_addr = inet_addr(argv[1]);
	else if (argc == 3){
		serv.sin_addr.s_addr = inet_addr(argv[1]);
		serv.sin_port = htons(atoi(argv[2]));
	} else if (argc == 1)
		;
	else
		err_quit("usage: cli [IP address of server] [port]");

	if (connect(sockfd, (SA *) &serv, sizeof(serv)) < 0)
		err_sys("connect error");

	buf[0] = LO_REQ;
	imwrite(sockfd, buf, strlen(buf));
	imread(sockfd, buf, MAXLINE);
	strncpy((char *) enkey, buf + 27, 32);
	strcpy(pwd, "abcdefghi");
	strncpy(buf + 27, pwd, PWDSIZE);
	enAES256(enkey, (uint8_t *) buf + 27);
	buf[0] = LO_PWD;
	imwrite(sockfd, buf, strlen(buf + 27) + 27);

	imread(sockfd, buf, MAXLINE);
	mkpvtkey(buf + 7, pwd, key);
	decrypt(buf + 27, 8, key);
	err_msg("%s", buf + 27);

	//cnt_timer();
	//signal(SIGALRM, cnt_handler);

	for (;;)
		read(sockfd, buf, MAXLINE);
	exit(0);
}
