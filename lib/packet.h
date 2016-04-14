#ifndef PACKET_H
#	define PACKET_H

typedef struct impacket{
	uint8_t cmd;
	uint16_t n;
	uint16_t fromID;
	uint16_t toID;
	char *rand;
	char *data;
} Packet;

/* packet n bytes data */
void pack(Packet *packet, char *buff, const char *pwd);

/* unpack n bytes data */
void unpack(char *buff, Packet *packet, const char *pwd);

#endif
