#ifndef __BSP_H__
#define __BSP_H__

#include "stm8s_conf.h"
#include "stm8s.h"
#include "packetsbuf.h"

#define ledInit()             GPIO_WriteHigh(GPIOD, GPIO_PIN_2)
#define ledTurnOn()           GPIO_WriteLow(GPIOD, GPIO_PIN_2)
#define ledTurnOff()          GPIO_WriteHigh(GPIOD, GPIO_PIN_2)
#define ledToggle()           (GPIO_ReadOutputData(GPIOD) & 0x04)?GPIO_WriteLow(GPIOD, GPIO_PIN_2):GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
//#define ledBlink(tLedId id, unsigned int duty, unsigned char percentage);

void eepromInit(void);
void eepromWriteBytes(unsigned char *eepromAddr,unsigned char *bytes,unsigned char cnt);
void eepromReadBytes(unsigned char *eepromAddr,unsigned char *bytes,unsigned char cnt);
void clockInit(void);
void gpioInit(void);
void spiInit(void);
void timerInit(void);
void uartInit(void);
void watchdog_start(void);
void watchdog_periodic(void);
void uart_tx_start(pst_Packet);

#define DEBUG_INFO_PRINT(s) debug_info_print(s)
  
#endif