#ifndef encrypt_H
#	define encrypt_H

/* make a random number list */
void mkrand(char *rand);

/* TEA encrypt for 64 bit data */
void enTEA(uint32_t v[], uint32_t k[]);

/* TEA decrypt for 64 bit data */
void deTEA(uint32_t v[], uint32_t k[]);

/* encrypt n bytes data */
int encrypt(char *data, int nbytes, uint32_t key[]);

/* decrypt n bytes data */
int decrypt(char *data, int nbytes, uint32_t key[]);

/* calculate md5 sum of data and save in digest */
void md5sum(const md5_byte_t *data, int nbytes, md5_byte_t *digest);

/* make private key */
void mkpvtkey(const char *rand, const char *pwd, uint32_t *key);

#endif /* encrypt_H */
