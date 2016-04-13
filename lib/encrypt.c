#include "im.h"

/* make a random number list */
void
mkrand(char *rand)
{
	int i;
	srand((int)time(0));
	for (i = 0; i < RANDSIZE; ++i)
		rand[i] = 0 + random() % 255;
}

/* TEA encrypt for 64 bit data */
void
enTEA(uint32_t v[], uint32_t k[])
{
	uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
	uint32_t delta = 0x9E3779B9;
	uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
	for (i=0; i < 32; ++i) {
		sum += delta;
		v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
	}
	v[0] = v0;
	v[1] = v1;
}

/* TEA decrypt for 64 bit data */
void
deTEA(uint32_t v[], uint32_t k[])
{
	uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;
	uint32_t delta = 0x9E3779B9;
	uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
	for (i = 0; i < 32; ++i) {
		v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
		v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		sum -= delta;
	}
	v[0] = v0;
	v[1] = v1;
}

/* calculate md5 sum of data and save in digest[] */
void
md5sum(const md5_byte_t *data, int nbytes, md5_byte_t *digest)
{
	md5_state_t state;

	md5_init(&state);
	md5_append(&state, data, nbytes);
	md5_finish(&state, digest);
}


/* make private key */
void
mkpvtkey(const char *rand, const char *pwd, uint32_t *key)
{
	int i;
	md5_byte_t digest[16], buff[PWDSIZE + RANDSIZE + 1];
	memset(buff, 0, PWDSIZE + RANDSIZE + 1);

	md5sum((const md5_byte_t *)pwd, strlen(pwd), digest);
	strncpy((char *)buff, rand, 20);
	strncat((char *)buff, (const char *)digest, 16);
	md5sum(buff, sizeof(buff), digest);
	for (i = 0; i < 16; i += 4)
		key[i / 4] = digest[i] * digest[i + 1] * digest[i + 2] * digest[i + 3];
}

/* encrypt n bytes data */
int
encrypt(char *data, int nbytes, uint32_t key[])
{
	if (nbytes <= 0 || nbytes % 8 != 0)
		return 0;

	int i = 0, j;
	uint32_t v[2] = {0, 0};
	while (i < nbytes) {
		j = i;
		for (j = i; j < i + 4; ++j) { /* read 8 bytes from v[] */
			v[0] <<= 8;
			v[0] |= data[j] & 0xff;
			v[1] <<= 8;
			v[1] |= data[j + 4] & 0xff;
		}
		enTEA(v, key);
		memset(data + i, 0, 8);
		for (j = i + 3; j >= i; --j) { /* write encrypted 8 bytes to data[] */
			data[j] |= v[0] & 0xff;
			v[0] >>= 8;
			data[j + 4] |= v[1] & 0xff;
			v[1] >>= 8;
		}
		i += 8;
	}
	return 1;
}

/* decrypt n bytes data */
int
decrypt(char *data, int nbytes, uint32_t key[])
{
	if (nbytes <= 0 || nbytes % 8 != 0)
		return 0;

	int i = 0, j;
	uint32_t v[2] = {0, 0};
	while (i < nbytes) {
		for (j = i; j < i + 4; ++j) { /* read encrypted 8 bytes from v[] */
			v[0] <<= 8;
			v[0] |= data[j] & 0xff;
			v[1] <<= 8;
			v[1] |= data[j + 4] & 0xff;
		}
		deTEA(v, key);
		memset(data + i, 0, 8);
		for (j = i + 3; j >= i; --j) { /* write decrypted 8 bytes to data[] */
			data[j] |= v[0] & 0xff;
			v[0] >>= 8;
			data[j + 4] |= v[1] & 0xff;
			v[1] >>= 8;
		}
		i += 8;
	}
	return 1;
}
