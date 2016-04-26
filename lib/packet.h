#ifndef PACKET_H
#	define PACKET_H

#define LO_REQ 0x02
#define LO_KEY 0x03
#define LO_OK  0x04
#define LO_PWD 0x06
#define LO_ERR 0x0a
#define IM_LIST 0x05
#define IM_LKEY 0x08
#define IM_QUIT 0x07
#define IM_SENDP 0x01
#define IM_SENDL 0x09
#define IM_HEART 0x0b

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
