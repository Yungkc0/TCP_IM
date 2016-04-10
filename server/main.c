#include "im.h"

int
main(int argc, char **argv)
{
	int i;
	uint32_t k[4];
	char pwd[] = "asdwsxqezc";
	char rand[RANDSIZE];
	char s[20];

	mkrand(rand);
	printf("Encrypt and decrypt:\n");
	mkpvtkey(rand, pwd, k);
	scanf("%16s", s);

	encrypt(s, 16, k);
	printf("encrypted string: ");
	for (i = 0; i < 16; ++i)
		printf("%02x", s[i] & 0xff);
	printf("\ndecrypted string: ");
	decrypt(s, 16, k);
	printf("%s\n", s);

	return 0;
}
