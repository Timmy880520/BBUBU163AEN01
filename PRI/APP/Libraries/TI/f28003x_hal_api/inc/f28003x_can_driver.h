/**************************************************************************************************
File Name: f28003x_can_driver.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of f28003x_can_driver.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/23/2023 Watch Lee        1. support version 1.x.

**************************************************************************************************/

#ifndef __F28003X_CAN_DRIVER_H__
#define __F28003X_CAN_DRIVER_H__

/* Includes */
#include "F28x_Project.h"
#include "f28003x_dma_can_driver.h"

/* Macro definitions */
// Statement Replacement
/*
    CAN_BTR calculation:
    Bit time     = SYNC_PSEG + PROP_SEG + PSEG1 + PSEG2
    Sample point = (SYNC_PSEG + PROP_SEG + PSEG1) / Bit time
    Prescaler    = CAN_CLK / Buad rate / Bit time (Prescaler Must be an integer)

    Example:
    Bit rate = 125kbps, Sample point = 75%, SYNC_PSEG = 1, PROP_SEG = 0
    Bit time     = 1 + PSEG1 + PSEG2
    Sample point = (1 + PSEG1) / (1 + PSEG1 + PSEG2)
        -> PSEG1 = 3 * PSEG2 - 1
    Prescaler    = 120M / 125k / (1 + PSEG1 + PSEG2)
                 = 960 / (1 + PSEG1 + PSEG2)
                 = 240 / PSEG2
    Select PSEG2 = 5, PSEG1 = 3 * 5 - 1 = 14, Prescaler = 240 / 5 = 48
    SJW = PSEG2 / 2 - 1 = 1
    TSEG1 = PSEG1 - 1 = 13
    TSEG2 = PSEG2 - 1 = 4
    BRP = Mod64(Prescaler - 1) = 47
    BRPE = (Prescaler - 1) / 64 = 0
    CAN_BTR = ((BRPE & 15) << 16) | ((TSEG2 & 7) << 12) | ((TSEG1 & 15) << 8) | ((SJW & 3) << 6) | (BRP & 63)
            = 0x00004D6F
*/
#define CAN_BTR_125kHz                  0x00004D6FUL
#define CAN_BTR_250kHz                  0x00001C1DUL
#define CAN_BTR_500kHz                  0x00004D4BUL
#define CAN_BTR_1MHz                    0x00004D45UL

// Group of declarations
#define Create_CAN_Object(ObjName, \
                          RegName, \
                          CanBTR)       CanObj ObjName = \
                                        {\
                                            {\
                                                &RegName, \
                                                CanBTR\
                                            }, \
                                            {\
                                                CAN_Init, \
                                                CAN_Set_Rx_Mailbox, \
                                                CAN_Get_Rx_ID_Filter, \
                                                CAN_Transmit, \
                                                CAN_DMA_Receive, \
                                                CAN_SW_Reset, \
                                                CAN_Get_Error_Status\
                                            }\
                                        }

/* Type definitions */
// Enumeration
typedef enum
{
    // Bit[1:0] for DMA
    DMA_CAN_DISABLE     = 0,
    DMA_CANRX_ENABLE    = 1
}CanFeatureEnum;

typedef enum
{
    CAN_RX_MAILBOX_1    = 1,
    CAN_RX_MAILBOX_2    = 2,
    CAN_RX_MAILBOX_3    = 3,
    CAN_RX_MAILBOX_4    = 4,
    CAN_RX_MAILBOX_5    = 5,
    CAN_RX_MAILBOX_6    = 6,
    CAN_RX_MAILBOX_7    = 7,
    CAN_RX_MAILBOX_8    = 8,
    CAN_RX_MAILBOX_9    = 9,
    CAN_RX_MAILBOX_10   = 10,
    CAN_RX_MAILBOX_11   = 11,
    CAN_RX_MAILBOX_12   = 12,
    CAN_RX_MAILBOX_13   = 13,
    CAN_RX_MAILBOX_14   = 14,
    CAN_RX_MAILBOX_15   = 15,
    CAN_RX_MAILBOX_16   = 16,
    CAN_RX_MAILBOX_17   = 17,
    CAN_RX_MAILBOX_18   = 18,
    CAN_RX_MAILBOX_19   = 19,
    CAN_RX_MAILBOX_20   = 20,
    CAN_RX_MAILBOX_21   = 21,
    CAN_RX_MAILBOX_22   = 22,
    CAN_RX_MAILBOX_23   = 23,
    CAN_RX_MAILBOX_24   = 24,
    CAN_RX_MAILBOX_25   = 25,
    CAN_RX_MAILBOX_26   = 26,
    CAN_RX_MAILBOX_27   = 27,
    CAN_RX_MAILBOX_28   = 28,
    CAN_RX_MAILBOX_29   = 29,
    CAN_RX_MAILBOX_30   = 30,
    CAN_RX_MAILBOX_31   = 31,
//    CAN_TX_MAILBOX_32   = 32
}CanMailBoxEnum;

// Struct & Union
typedef union
{
    unsigned long all;

    struct
    {
        unsigned long msk       :29;        // Identifier mask
        unsigned long rsvd1     :1;         // Reserved
        unsigned long mDir      :1;         // Mask message direction
        unsigned long mXtd      :1;         // Mask extended identifier
    }bit;
}CanMaskType;

typedef union
{
    unsigned long all;

    struct
    {
        unsigned long id        :29;        // Identifier
        unsigned long dir       :1;         // Message direction
        unsigned long xtd       :1;         // Extended identifier
        unsigned long msgVal    :1;         // Message valid
    }bit;
}CanArbType;

typedef union
{
    unsigned long all;

    struct
    {
        unsigned long dlc       :4;         // Data length code
        unsigned long rsvd1     :3;         // Reserved
        unsigned long eob       :1;         // End of block

        unsigned long txRqst    :1;         // Transmit request
        unsigned long rmtEn     :1;         // Remote enable
        unsigned long rxIE      :1;         // Receive interrupt enable
        unsigned long txIE      :1;         // Transmit interrupt enable
        unsigned long uMask     :1;         // Use acceptance mask
        unsigned long intPnd    :1;         // Interrupt pending
        unsigned long msgLst    :1;         // Message lost
        unsigned long newDat    :1;         // New data

        unsigned long rsvd2     :16;        // Reserved
    }bit;
}CanMctrlType;

typedef union
{
    unsigned long all;

    struct
    {
        unsigned long Data0     :8;         // Data byte 0
        unsigned long Data1     :8;         // Data byte 1
        unsigned long Data2     :8;         // Data byte 2
        unsigned long Data3     :8;         // Data byte 3
    }bit;
}CanDatAType;

typedef union
{
    unsigned long all;

    struct
    {
        unsigned long Data4     :8;         // Data byte 4
        unsigned long Data5     :8;         // Data byte 5
        unsigned long Data6     :8;         // Data byte 6
        unsigned long Data7     :8;         // Data byte 7
    }bit;
}CanDatBType;

typedef struct
{
    CanArbType arb;
    CanMctrlType mctrl;
    CanDatAType datA;
    CanDatBType datB;
}CanPacketType;

typedef struct CanStr CanObj;

struct CanStr
{
    struct
    {
        volatile struct CAN_REGS *reg;
        unsigned long bitTimingValue;
    }member;

    struct
    {
        void (*init)(CanObj*, unsigned long);
        unsigned char (*setRxMailbox)(CanObj*, unsigned char, unsigned long, unsigned long);
        unsigned char (*getRxIdFiler)(CanObj*, unsigned char, unsigned long*, unsigned long*);
        unsigned char (*transmit)(void*, void*, unsigned char);
        unsigned char (*receive)(void*, void**, unsigned char*);
        void (*reset)(CanObj*);
        unsigned char (*getError)(CanObj*);
    }func;
};

/* Global function prototypes */
void CAN_Init(CanObj *obj, unsigned long feature);
unsigned char CAN_Set_Rx_Mailbox(CanObj *obj, unsigned char number, unsigned long id, unsigned long mask);
unsigned char CAN_Get_Rx_ID_Filter(CanObj *obj, unsigned char number, unsigned long *id, unsigned long *mask);
unsigned char CAN_Transmit(void *obj, void *packet, unsigned char size);
unsigned char CAN_Receive(void *obj, void **packet, unsigned char *size);
unsigned char CAN_DMA_Receive(void *obj, void **packet, unsigned char *size);
void CAN_SW_Reset(CanObj *obj);
unsigned char CAN_Get_Error_Status(CanObj *obj);

/* Global data declarations */

#endif
