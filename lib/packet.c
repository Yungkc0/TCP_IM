#include "im.h"

/* packet n bytes data */
void
pack(Packet *packet, char *buff, const char *pwd)
{
	uint32_t k[4];

	mkrand(packet.rand);
	mkpvtkey(packet->rand, pwd, k);
	if (!encrypt(packet->data, packet->n, k))
		err_quit("A packet error!!!");
}

/* unpack n bytes data */
void
unpack(char *buff, Packet *packet, const char *pwd)
{
	uint32_t k[4];
	packet->rand = buff + 7;
	packet->data = buff + 27;

	mkpvtkey(rand, pwd, k);
	if (!decrypt(packet->data, packet->n, k))
		err_quit("A packet error!");
}
