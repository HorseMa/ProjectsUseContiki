#include "packetsbuf.h"
#include "stm8s.h"
#include <string.h>

struct st_Packet stPacket[MAX_PACKET_NUM];

void pktbuf_init(void)
{
  for(unsigned char i = 0;i < MAX_PACKET_NUM;i ++)
  {
    stPacket[i].used = FALSE;
    stPacket[i].len = 0;
    stPacket[i].offset = 0;
  }
}

pst_Packet pktbuf_alloc(void)
{
  for(unsigned char i = 0;i < MAX_PACKET_NUM;i ++)
  {
    if(stPacket[i].used == FALSE)
    {
      stPacket[i].used = TRUE;
      stPacket[i].offset = 0;
      stPacket[i].len = 0;
      memset(stPacket[i].data,0,BUF_SIZE);
      return &stPacket[i];
    }
  }
  return NULL;
}

void pktbuf_free(pst_Packet pkt)
{
  if(!pkt)
    return;
  pkt->used = FALSE;
  pkt->len = 0;
  pkt->offset = 0;
}