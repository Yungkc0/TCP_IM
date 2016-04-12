#include "im.h"

int
main(int argc, char **argv)
{
	int i;
	uint32_t k[4] = {0, 0, 0, 0};
	char s[20];

	char pwd[PWDSIZE] = "test";
	char rand[RANDSIZE];

	mkrand(rand);
	printf("%08x%08x%08x%08x\n", k[0], k[1], k[2], k[3]);
	mkpvtkey(rand, pwd, k);
	printf("input text to encrypt: ");
	scanf("%16s", s);

	encrypt(s, 16, k);
	printf("key: %08x%08x%08x%08x\n", k[0], k[1], k[2], k[3]);
	printf("encrypted string: ");
	for (i = 0; i < 16; ++i)
		printf("%02x", s[i] & 0xff);
	printf("\ndecrypted string: ");
	decrypt(s, 16, k);
	printf("%16s\n", s);

	return 0;
}
