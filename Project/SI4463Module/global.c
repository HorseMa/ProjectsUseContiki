#include "global.h"


st_ModuleCfg stModuleCfg;
en_RadioState enRadioState;
pst_Packet pstUartTxBuf;
pst_Packet pstUartRxBuf;

process_event_t ev_uartSendOver;
process_event_t ev_uartRecvPkg;

LIST(ls_radio_tx);
LIST(ls_radio_rx);

LIST(ls_uart_tx);
LIST(ls_uart_rx);

void globalInit(void)
{
  stModuleCfg.uartbaudrate = 9600;
  stModuleCfg.uartdatabit = UART_DATA_BIT_8;
  stModuleCfg.uartparitybit = UART_PARITY_NO;
  stModuleCfg.uartstopbit = UART_STOPBITS_1;
  
  stModuleCfg.airbaudrate = 10000;
  stModuleCfg.airchannel = 0;
  stModuleCfg.airpower = 0x7f;
  
  enRadioState = RADIO_IN_IDLE;
  
  list_init(ls_radio_tx);
  list_init(ls_radio_rx);
  list_init(ls_uart_tx);
  list_init(ls_uart_rx);
}

pst_Packet radioGetPktFromTxList(void)
{
  return list_pop(ls_radio_tx);
}

void radioAddPktToTxList(pst_Packet pstPacket)
{
  list_add(ls_radio_tx,pstPacket);
}

pst_Packet radioGetPktFromRxList(void)
{
  return list_pop(ls_radio_rx);
}

void radioAddPktToRxList(pst_Packet pstPacket)
{
  list_add(ls_radio_rx,pstPacket);
}

pst_Packet uartGetPktFromTxList(void)
{
  return list_pop(ls_uart_tx);
}

void uartAddPktToTxList(pst_Packet pstPacket)
{
  list_add(ls_uart_tx,pstPacket);
}

pst_Packet uartGetPktFromRxList(void)
{
  return list_pop(ls_uart_rx);
}

void uartAddPktToRxList(pst_Packet pstPacket)
{
  list_add(ls_uart_rx,pstPacket);
}