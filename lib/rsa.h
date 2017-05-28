#ifndef RSA_H
# define RSA_H

RSA *createRSA(unsigned char *key, int public);

int rsa_public_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted);

int rsa_private_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted);

int rsa_private_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted);

int rsa_public_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted);

#endif /* RSA_H */
