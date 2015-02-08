#ifndef __PACKETS_BUF_H__
#define __PACKETS_BUF_H__


#define MAX_PACKET_NUM    8
#define BUF_SIZE          70

struct st_Packet
{
  struct st_Packet* next;
  unsigned char len;
  unsigned char offset;
  unsigned char used;
  unsigned char data[BUF_SIZE];
};
typedef struct st_Packet* pst_Packet;

void pktbuf_init(void);
pst_Packet pktbuf_alloc(void);
void pktbuf_free(pst_Packet pkt);

#endif