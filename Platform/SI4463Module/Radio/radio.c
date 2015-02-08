/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "compiler_defs.h"
#include "si446x_defs.h"
#include "si446x_api_lib.h"
#include "radio_hal.h"
#include "radio.h"
#include "radio_config.h"
#include "packetsbuf.h"
#include "bsp.h"
#include "global.h"
/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
const U8 Radio_Configuration_Data_Array[] = 
              RADIO_CONFIGURATION_DATA_ARRAY;

const tRadioConfiguration RadioConfiguration = 
{
  .Radio_ConfigurationArray = (U8*)Radio_Configuration_Data_Array,
  .Radio_ChannelNumber = RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,
  .Radio_PacketLength = RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,
  .Radio_State_After_Power_Up = RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,
  .Radio_Delay_Cnt_After_Reset = RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET,
  //.Radio_CustomPayload = RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD
};

const tRadioConfiguration *pRadioConfiguration = 
                        &RadioConfiguration;

static void txPoll(void);
//U8 customRadioPacket[RADIO_MAX_PACKET_LENGTH];
//U8 customRadioPacketLen;


//struct mmem mmem_pkt_rx;

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void vRadio_PowerUp(void);

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
  U16 wDelay = 0u;

  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
  U16 wDelay;
  ledTurnOn();
  /* Power Up the radio chip */
  vRadio_PowerUp();
  si446x_part_info();
  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {
    for (wDelay = 0x7FFF; wDelay--; ) ;
    /* Power Up the radio chip */
    vRadio_PowerUp();
  }

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
  ledTurnOff();
}

/*!
 *  Check if Packet sent IT flag or Packet Received IT is pending.
 *
 *  @return   SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
 *
 *  @note
 *
 */

U8 bRadio_Check_Tx_RX(void)
{
  static pst_Packet radio_pkg_tx = NULL,radio_pkg_rx = NULL;
  DEBUG_INFO_PRINT("int\r\n");
  if (radio_hal_NirqLevel() == 0)
  {
      /* Read ITs, clear pending ones */
      si446x_get_int_status(0u, 0u, 0u);

      if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CMD_ERROR_PEND_BIT)
      {
        DEBUG_INFO_PRINT("cmd err\r\n");
      	/* State change to */
      	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
	
	/* Reset FIFO */
      	si446x_fifo_info(0x03);
      
	/* State change to */
        vRadio_StartRX();
        //si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
        return 0;
      }
      if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_PEND_BIT)
      {
        DEBUG_INFO_PRINT("crc err\r\n");
      	/* Reset FIFO */
      	si446x_fifo_info(0x03);
        return 0;
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_TX_FIFO_ALMOST_EMPTY_PEND_BIT)
      {
        DEBUG_INFO_PRINT("empty\r\n");
        si446x_fifo_info(0x00);
        if(radio_pkg_tx->len - radio_pkg_tx->offset > Si446xCmd.FIFO_INFO.TX_FIFO_SPACE)
        {
          si446x_write_tx_fifo(Si446xCmd.FIFO_INFO.TX_FIFO_SPACE, &radio_pkg_tx->data[radio_pkg_tx->offset]);
          radio_pkg_tx->offset += Si446xCmd.FIFO_INFO.TX_FIFO_SPACE;
        }
        else
        {
          si446x_write_tx_fifo(radio_pkg_tx->len - radio_pkg_tx->offset, &radio_pkg_tx->data[radio_pkg_tx->offset]);
          radio_pkg_tx->offset += radio_pkg_tx->len - radio_pkg_tx->offset;
        }
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
      {
        DEBUG_INFO_PRINT("sentover\r\n");
        //radio_pkg_tx = NULL;
        reConfigPktHandler();
        //freeBuf(pstRadioTxBuf);
        //pstRadioTxBuf = 0;
        txPoll();
        //vRadio_StartRX(0,0);
        //si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
        ledTurnOff();
        return SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT;
      }
      if(Si446xCmd.GET_MODEM_STATUS.MODEM_PEND & SI446X_CMD_GET_MODEM_STATUS_REP_PREAMBLE_DETECT_PEND_BIT)
      {
        DEBUG_INFO_PRINT("prea\r\n");
        enRadioState = RADIO_IN_RX;
      }
      if(Si446xCmd.GET_MODEM_STATUS.MODEM_PEND & SI446X_CMD_GET_MODEM_STATUS_REP_INVALID_PREAMBLE_PEND_BIT)
      {
        DEBUG_INFO_PRINT("inprea\r\n");
        txPoll();
      }
      if(Si446xCmd.GET_MODEM_STATUS.MODEM_PEND & SI446X_CMD_GET_MODEM_STATUS_REP_SYNC_DETECT_PEND_BIT)
      {
        DEBUG_INFO_PRINT("sync\r\n");
        ledTurnOn();
        radio_pkg_rx = pktbuf_alloc();
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_RX_FIFO_ALMOST_FULL_PEND_BIT)
      {
        DEBUG_INFO_PRINT("full\r\n");
        /* Get payload length */
        si446x_fifo_info(0x00);
        if(radio_pkg_rx != NULL)
        {
            if(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT + radio_pkg_rx->offset > BUF_SIZE)
            {
              pktbuf_free(radio_pkg_rx);
              reConfigPktHandler();
              vRadio_StartRX();
              ledTurnOff();
              return 0;
            }
            si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &radio_pkg_rx->data[radio_pkg_rx->offset]);
            radio_pkg_rx->offset += Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;
        }
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
      {
        DEBUG_INFO_PRINT("rcv\r\n");
        /* Get payload length */
        si446x_fifo_info(0x00);
        if(radio_pkg_rx != NULL)
        {
            if(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT + radio_pkg_rx->offset > BUF_SIZE)
            {
              pktbuf_free(radio_pkg_rx);
              reConfigPktHandler();
              vRadio_StartRX();
              ledTurnOff();
              return 0;
            }
            si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &radio_pkg_rx->data[radio_pkg_rx->offset]);
            radio_pkg_rx->offset += Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;
            radio_pkg_rx->len = radio_pkg_rx->offset;
            process_post(&radio_rcv_process,ev_radio_rcv,NULL);
        }
        ledTurnOff();
        //reConfigPktHandler();
        txPoll();
        //vRadio_StartRX(0,0);
        return SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT;
      }


      

  }

  return 0;
}

/*!
 *  Set Radio to RX mode. .
 *
 *  @param channel Freq. Channel,  packetLength : 0 Packet handler fields are used , nonzero: only Field1 is used
 *
 *  @note
 *
 */
void vRadio_StartRX()
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

   // Reset the Rx Fifo
   si446x_fifo_info(0x03);

  /* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
  si446x_start_rx(stModuleCfg.airchannel, 0u, 0,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX );
}

void txPoll(void)
{
  pst_Packet pstPacket;
  enRadioState = RADIO_IN_IDLE;
  if((pstPacket = radioGetPktFromTxList()) == NULL)
  {
    vRadio_StartRX();
  }
  else
  {
    vRadio_StartTx_Variable_Packet(pstPacket);
  }
}
/*!
 *  Set Radio to TX mode, variable packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
void vRadio_StartTx_Variable_Packet(pst_Packet pstPacket)
{
  if(enRadioState != RADIO_IN_IDLE)
  {
    if(pstPacket != NULL)
    {
      radioAddPktToTxList(pstPacket);
    }
    return;
  }
  DEBUG_INFO_PRINT("radio tx\r\n");
  enRadioState = RADIO_IN_TX;
  pstPacket->offset = 0;
  ledTurnOn();
  /* Leave RX state */
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

  /* Read ITs, clear pending ones */
  si446x_get_int_status(0u, 0u, 0u);

  /* Reset the Tx Fifo */
  si446x_fifo_info(0x03);

  /* Fill the TX fifo with datas */
  //radio_pkg_tx->len += 1;
  si446x_write_tx_fifo(1, &pstPacket->len);
  pstPacket->len += 1;
  
  if(pstPacket->len <= 62)
  {
    si446x_write_tx_fifo(pstPacket->len, pstPacket->data);
    pstPacket->offset += pstPacket->len;
  }
  else
  {
    si446x_write_tx_fifo(62, pstPacket->data);
    pstPacket->offset += 62;
  }
  /* Start sending packet, channel 0, START immediately */
   si446x_start_tx(stModuleCfg.airchannel, 0x80, pstPacket->len);
 
}
