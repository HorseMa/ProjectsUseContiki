#ifndef __PACKETS_BUF_H__
#define __PACKETS_BUF_H__


#define MAX_PACKET_NUM    4
#define BUF_SIZE          128

typedef struct
{
  unsigned char len;
  unsigned char offset;
  unsigned char used;
  unsigned char data[BUF_SIZE];
}st_Packet,*pst_Packet;


void pktbuf_init(void);
pst_Packet pktbuf_alloc(void);
void pktbuf_free(pst_Packet pkt);

#endif