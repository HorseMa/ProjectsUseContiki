#include "global.h"


st_ModuleCfg stModuleCfg;
en_RadioState enRadioState;
pst_Packet pstUartTxBuf;
pst_Packet pstUartRxBuf;

process_event_t ev_uartSendOver;
process_event_t ev_uartRecvPkg;

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
}