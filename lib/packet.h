#ifndef PACKET_H
#	define PACKET_H

struct packetHeader{
	uint8_t cmd;
	int n;
	uint16_t fromID;
	uint16_t toID;
	char rand[20];
	char *data;
}

void
pack()
{
}

void
unpack()
{
}

#endif
