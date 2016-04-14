#include "im.h"

/* packet n bytes data */
void
pack(Packet *packet, char *buff, const char *pwd)
{
	uint32_t k[4];
	packet->rand = buff + 7;
	packet->data = buff + 27;
	sscanf(buff + 1, "%hu", &packet->n);

	mkrand(packet->rand);
	mkpvtkey(packet->rand, pwd, k);
	printf("k1: %08x%08x%08x%08x\n", k[0], k[1], k[2], k[3]);
	if (!encrypt(packet->data, packet->n, k))
		err_quit("A packet error!!! %d", packet->n);
}

/* unpack n bytes data */
void
unpack(char *buff, Packet *packet, const char *pwd)
{
	uint32_t k[4];
	packet->rand = buff + 7;
	packet->data = buff + 27;
	sscanf(buff + 1, "%hu", &packet->n);

	mkpvtkey(packet->rand, pwd, k);
	printf("k2: %08x%08x%08x%08x\n", k[0], k[1], k[2], k[3]);
	if (!decrypt(packet->data, packet->n, k))
		err_quit("A packet error!");
}
