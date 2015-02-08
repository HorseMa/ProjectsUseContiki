#include "contiki.h"
#include "tasks.h"
#include "bsp.h"
#include "list.h"
#include "radio.h"
#include "si446x_defs.h"
#include "packetsbuf.h"
#include <string.h>
#include "global.h"
#include "bsp.h"
process_event_t ev_checkradio;
process_event_t ev_radio_rcv;

PROCESS(blink_process,"blink");
PROCESS(checkradio_process,"checkradio");
PROCESS(radio_rcv_process,"radioreceive");
PROCESS(uartSend_process,"uartSend");
PROCESS(uartRecv_process,"uartRecv");

AUTOSTART_PROCESSES(&blink_process, &checkradio_process,&radio_rcv_process,&uartSend_process,&uartRecv_process);

PROCESS_THREAD(blink_process, ev, data)
{ 
  static struct etimer et_blink;
  pst_Packet pstPacket;
  PROCESS_BEGIN();
  vRadio_StartRX();
  while(1) {
    etimer_set(&et_blink, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    /*pstPacket = pktbuf_alloc();
    if(!pstPacket)
    {
      for(unsigned char i = 0;i < 13;i ++)
        memcpy(&pstPacket->data[i * 5],"hello",5);
      pstPacket->len = 5 * 13;
      vRadio_StartTx_Variable_Packet(pstPacket);
    }*/
  }

  PROCESS_END();

} 


PROCESS_THREAD(checkradio_process, ev, data)
{ 
  PROCESS_BEGIN();
  ev_checkradio = process_alloc_event();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_checkradio);

    bRadio_Check_Tx_RX();
  }

  PROCESS_END();

} 

PROCESS_THREAD(radio_rcv_process, ev, data)
{ 
  static pst_Packet pstPacket;
  PROCESS_BEGIN();
  ev_radio_rcv = process_alloc_event();
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_radio_rcv);
    while((pstPacket = radioGetPktFromRxList()) != NULL)
    {
      /*if(memcmp(pstPacket->data,"hello",5))
        ledToggle();*/
      pktbuf_free(pstPacket);
    }
  }

  PROCESS_END();

} 


PROCESS_THREAD(uartSend_process, ev, data)
{ 
  PROCESS_BEGIN();
  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_uartSendOver);
    if(pstUartTxBuf != NULL)
    {
      continue;
    }
    if((pstUartTxBuf = uartGetPktFromTxList()) != NULL)
    {
      uart_tx_start(pstUartTxBuf);
    }
  }
  
  PROCESS_END();
} 


PROCESS_THREAD(uartRecv_process, ev, data)
{ 
  static pst_Packet pstPacket;
  PROCESS_BEGIN();
  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_uartRecvPkg);
  }
  
  PROCESS_END();
} 