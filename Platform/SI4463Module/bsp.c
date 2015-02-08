#include "stm8s_conf.h"
#include "stm8s.h"
#include "bsp.h"
#include "global.h"
#include <string.h>


void eepromInit(void)
{
  FLASH_DeInit();
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}

void eepromWriteBytes(unsigned char *eepromAddr,unsigned char *bytes,unsigned char cnt)
{
  unsigned char u8Loop;
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  for(u8Loop = 0;u8Loop < cnt;u8Loop ++)
  {
    FLASH_ProgramByte((unsigned long)eepromAddr + (unsigned long)u8Loop, bytes[u8Loop]);
  }
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void eepromReadBytes(unsigned char *eepromAddr,unsigned char *bytes,unsigned char cnt)
{
  unsigned char u8Loop;
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  for(u8Loop = 0;u8Loop < cnt;u8Loop ++)
  {
    bytes[u8Loop] = FLASH_ReadByte((unsigned long)eepromAddr + (unsigned long)u8Loop);
  }
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void clockInit(void)
{
  /* Turn on internal high speed clock and use it */
  CLK_HSICmd(ENABLE);
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, 
                        CLK_SOURCE_HSI, 
                        ENABLE, 
                        CLK_CURRENTCLOCKSTATE_ENABLE);
  CLK_LSICmd(DISABLE);
}

void gpioInit(void)
{
  
  /* Light both of LED */
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
  
  /* Config GPIO */
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); // CONFIG pin
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); // SLEEP pin
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); // test pin
  
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT); // RF_GPIO
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT); // RF_GPIO
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST); // SDN pin
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_IT); // NIRQ pin
  
  EXTI_DeInit();
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC,EXTI_SENSITIVITY_FALL_ONLY);
}

void timerInit(void)
{
  /* TIM2 initial */
  TIM2_TimeBaseInit(TIM2_PRESCALER_16,7812);
  TIM2_ITConfig( TIM2_IT_UPDATE , ENABLE);
  TIM2_SetCounter(0x0000);
  /*计时器打开*/
  TIM2_Cmd(ENABLE);
  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
}

void spiInit(void)
{
  /* SPI initial*/
  //SPI_DeInit();
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST); // SPI_CS output high
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);  // SPI_SCLK output
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST); // SPI_MOSI output
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT); // SPI_MISO input
  //SPI_NSSInternalSoftwareCmd(ENABLE); // Use NSS pin
  
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER
  , SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, 
  SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT,0x07);
  SPI_Cmd(ENABLE);
}

void uartInit(void)
{
  /* UART1 initial*/
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST); // SPI_CS output high
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);  // SPI_SCLK output
  //UART1_DeInit();
  /* Configure the UART1 */
  UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
  UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  /* Enable UART1 Transmit interrupt*/
  //UART1_ITConfig(UART1_IT_TXE, ENABLE);
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  //UART1_ITConfig(UART1_IT_TXE, ENABLE);
  UART1_Cmd(ENABLE);
}

void watchdog_start(void)
{
  
}

void watchdog_periodic(void)
{
  
}

void uart_tx_start(pst_Packet pstPacket)
{
  //pstUartTxBuf = pstPacket;
  UART1_SendData8(pstPacket->data[0]);
  pstPacket->offset += 1;
  UART1_ITConfig(UART1_IT_TXE, ENABLE);
}

void uart_tx(pst_Packet pstPacket)
{
  if(pstPacket != NULL);
  {
    uartAddPktToTxList(pstPacket);
  }
  if(pstUartTxBuf == NULL)
  {
    process_post(&uartSend_process,ev_uartSendOver,NULL);
  }
}

void debug_info_print(char const * str)
{
  pst_Packet pstPacket = pktbuf_alloc();
  if(pstPacket != NULL)
  {
    pstPacket->offset = 0;
    pstPacket->len = strlen(str);
    memcpy(&pstPacket->data,str,pstPacket->len);
    uart_tx(pstPacket);
  }
}

