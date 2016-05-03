#include "im.h"

/* convert a Packet to buf */
void
pack(Packet *p, char *buf)
{
	buf[0] = p->cmd;
	if (p->n > 27 && (p->n - 27) % 8 != 0)
		p->n = (p->n - 27) - (p->n - 27) % 8 + 8 + 27;
	buf[2] = p->n, buf[1] = p->n >> 8;
	buf[4] = p->fromID, buf[3] = p->fromID >> 8;
	buf[6] = p->toID, buf[5] = p->toID >> 8;
}

/* convert buf to a Packet */
void
unpack(char *buf, Packet *p)
{
	if (buf[0] == IM_HEART || buf[7] == '\0')
		p->rand = NULL;
	else
		p->rand = buf + 7;
	if (buf[0] == IM_GETLIST || buf[0] == IM_HEART)
		p->data = NULL;
	else
		p->data = buf + 27;
	p->cmd = buf[0];
	p->n = buf[1], p->n <<= 8, p->n = buf[2];
	if (p->n > 27 && (p->n - 27) % 8 != 0)
		p->n = (p->n - 27) - (p->n - 27) % 8 + 8 + 27;
	p->fromID = buf[3], p->fromID <<= 8, p->fromID = buf[4];
	p->toID = buf[5], p->toID <<= 8, p->toID = buf[6];
}

/* make packet */
void
mkpkt(Packet *p, char cmd, uint16_t n, uint16_t fromID, uint16_t toID, char *rand, char *data)
{
	p->cmd = cmd;
	p->n = n;
	p->fromID = fromID;
	p->toID = toID;
	p->rand = rand;
	p->data = data;
}

/* is a valid packet? */
int
isvp(const Packet *p, uint8_t cmd)
{
	int i, sign = 1;
	if (p->cmd != cmd)
		return 0;
	
	switch (cmd) {
		case LO_PWD:
			for (i = 0; i < PWDSIZE; ++i)
				if ((p->data[i] & 0xff) > 0x7f)
					sign = 0;
			break;
		default:
			;
	}
	return sign;
}
