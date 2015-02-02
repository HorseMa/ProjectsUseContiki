/*! @file radio.h
 * @brief This file is contains the public radio interface functions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_H_
#define RADIO_H_
#include "compiler_defs.h"
#include "packetsbuf.h"
/*****************************************************************************
 *  Global Macros & Definitions
 *****************************************************************************/
/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH     64u

/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
typedef struct
{
    U8   *Radio_ConfigurationArray;

    U8   Radio_ChannelNumber;
    U8   Radio_PacketLength;
    U8   Radio_State_After_Power_Up;

    U16  Radio_Delay_Cnt_After_Reset;

    U8   Radio_CustomPayload[RADIO_MAX_PACKET_LENGTH];
} tRadioConfiguration;


/*****************************************************************************
 *  Global Variable Declarations
 *****************************************************************************/
extern const tRadioConfiguration *pRadioConfiguration;
extern U8 customRadioPacket[RADIO_MAX_PACKET_LENGTH];
extern U8 customRadioPacketLen;

/*! Si446x configuration array */
extern const U8 Radio_Configuration_Data_Array[];

/*****************************************************************************
 *  Global Function Declarations
 *****************************************************************************/
void  vRadio_Init(void);
U8    bRadio_Check_Tx_RX(void);
void  vRadio_StartRX(void);
U8    bRadio_Check_Ezconfig(U16);
void  vRadio_StartTx_Variable_Packet(pst_Packet);
void radioTxQueueRoutin(void);

#endif /* RADIO_H_ */
