#ifndef PACKET_H
#	define PACKET_H

#define IM_SENDP   0x01
#define LO_REQ     0x02
#define LO_KEY     0x03
#define LO_OK      0x04
#define IM_LIST    0x05
#define LO_PWD     0x06
#define IM_GETLIST 0x07
#define IM_LKEY    0x08
#define IM_SENDL   0x09
#define LO_ERR     0x0a
#define IM_HEART   0x0b
#define IM_QUIT    0x0c

typedef struct impacket{
	uint8_t cmd;
	uint16_t n;
	uint16_t fromID;
	uint16_t toID;
	char *rand;
	char *data;
} Packet;

/* convert a Packet to buf */
void pack(Packet *p, char *buf);

/* convert buf to a Packet */
void unpack(char *buf, Packet *p);

/* make packet */
void mkpkt(Packet *p, char cmd, uint16_t n, uint16_t fromID, uint16_t toID, char *rand, char *data);

/* is a valid packet? */
int isvp(const Packet *p, uint8_t cmd);

#endif
