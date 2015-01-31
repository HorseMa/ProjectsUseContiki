#include "contiki.h"
#include "tasks.h"
#include "bsp.h"
#include "list.h"
#include "radio.h"
#include "si446x_defs.h"
#include "packetsbuf.h"
#include <string.h>

extern pst_Packet radio_pkg_tx,radio_pkg_rx;
LIST(ls_radio_tx);
LIST(ls_radio_rx);
//extern struct mmem mmem_pkt_rx;

process_event_t ev_checkradio;
process_event_t ev_radio_rcv;

PROCESS(blink_process,"blink");
PROCESS(checkradio_process,"checkradio");
PROCESS(radio_rcv_process,"radioreceive");

AUTOSTART_PROCESSES(&blink_process, &checkradio_process,&radio_rcv_process);

PROCESS_THREAD(blink_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();
  vRadio_StartRX(0,0);
  while(1) {
    etimer_set(&et_blink, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    if(!radio_pkg_tx)
    {
      radio_pkg_tx = pktbuf_alloc();
      for(unsigned char i = 0;i < 20;i ++)
        memcpy(&radio_pkg_tx->data[i * 5],"hello",5);
      radio_pkg_tx->len = 5 * 20;
      vRadio_StartTx_Variable_Packet(0, radio_pkg_tx->data, radio_pkg_tx->len);
      
    }
  }

  PROCESS_END();

} 


PROCESS_THREAD(checkradio_process, ev, data)
{ 
  static unsigned char radio_chk_return;
  PROCESS_BEGIN();
  ev_checkradio = process_alloc_event();
  list_init(ls_radio_tx);
  list_init(ls_radio_rx);
  radio_pkg_tx = NULL;
  radio_pkg_rx = NULL;
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_checkradio);

    radio_chk_return = bRadio_Check_Tx_RX();
    if(radio_chk_return == SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
    {
      pktbuf_free(radio_pkg_tx);
      while((radio_pkg_tx = list_pop(ls_radio_tx)) != NULL)
      {
        vRadio_StartTx_Variable_Packet(0, radio_pkg_tx->data, radio_pkg_tx->len);
      }
    }
    if(radio_chk_return == SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
    {
      list_add(ls_radio_rx,radio_pkg_rx);
      radio_pkg_rx = NULL;
      process_post(&radio_rcv_process,ev_radio_rcv,NULL);
    }
    else
    {
      
    }
  }

  PROCESS_END();

} 

PROCESS_THREAD(radio_rcv_process, ev, data)
{ 
  static pst_Packet pstPacket;
  unsigned char i;
  PROCESS_BEGIN();
  ev_radio_rcv = process_alloc_event();
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_radio_rcv);
    while((pstPacket = list_pop(ls_radio_rx)) != NULL)
    {
      /*if(memcmp(pstPacket->data,"hello",5))
        ledToggle();*/
      pktbuf_free(pstPacket);
    }
  }

  PROCESS_END();

} 