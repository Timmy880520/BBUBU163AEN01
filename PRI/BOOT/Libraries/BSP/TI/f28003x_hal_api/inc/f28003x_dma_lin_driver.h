/**************************************************************************************************
File Name: f28003x_dma_lin_Driver.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of f28003x_dma_lin_Driver.c.

===================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. support version 1.x.

**************************************************************************************************/

#ifndef __F28003X_DMA_LIN_DRIVER_H__
#define __F28003X_DMA_LIN_DRIVER_H__

/* Includes */
#include "F28x_Project.h"

/* Macro definitions */
#define DMA_CHANNEL_BASE                    0x00001020UL
#define LINA_Register_BASE                  0x00006A00UL
#define DMA_LINA_RX_TRIGGER                 118UL
#define DMA_LINA_TX_TRIGGER                 117UL
#define DMA_LINB_RX_TRIGGER                 120UL
#define DMA_LINB_TX_TRIGGER                 119UL

// Statement Replacement
#ifndef Check_Power_2
#define Check_Power_2(Number)               (((((unsigned)Number - 1) ^ Number) == (Number << 1) - 1) ? Number : -1)
#endif

// Group of declarations (DMA accessing memory MUST be placed in GS0 ~ GS3 RAM = 0x0000C000 ~ 0x0000FFFF)
#define Create_LIN_RX_DMA_Object(ObjName, \
                                 Size, \
                                 RegName)   unsigned short ObjName##Mem[Check_Power_2(Size)];\
                                            unsigned short ObjName##Buffer[Check_Power_2(Size)];\
                                            DmaLinObj ObjName = \
                                            {\
                                                {\
                                                    &RegName,\
                                                    ObjName##Mem, Size,\
                                                    ObjName##Buffer, Size,\
                                                    0, 0\
                                                },\
                                                {\
                                                    DMA_LIN_SCI_Mode_RX_Init,\
                                                    Read_Circular_DMA_Data\
                                                }\
                                            }

#define Create_LIN_TX_DMA_Object(ObjName, \
                                 Size, \
                                 RegName)   unsigned short ObjName##Mem[Size];\
                                            DmaLinObj ObjName = \
                                            {\
                                                {\
                                                    &RegName,\
                                                    ObjName##Mem, Size,\
                                                    0, 0, 0, 0\
                                                },\
                                                {\
                                                    DMA_LIN_SCI_Mode_TX_Init,\
                                                    Write_DMA_Data\
                                                }\
                                            }

/* Type definitions */
// Struct & Union
typedef struct
{
    unsigned short *ptr;
    unsigned short size;
}DmaDataStr;

typedef struct DmaLinStr DmaLinObj;

struct DmaLinStr
{
    struct
    {
        volatile struct CH_REGS *reg;
        DmaDataStr dma;
        DmaDataStr buffer;
        unsigned short index;
        unsigned short dataAmount;
    }member;

    struct
    {
        void (*init)(DmaLinObj*, unsigned long);
        unsigned char (*accessDmaData)(DmaLinObj*);
    }func;
};

/* Global function prototypes */
void DMA_LIN_SCI_Mode_RX_Init(DmaLinObj *obj, unsigned long dataRegAddress);
void DMA_LIN_SCI_9_Bit_Mode_RX_Init(DmaLinObj *obj, unsigned long dataRegAddress);
void DMA_LIN_SCI_Mode_TX_Init(DmaLinObj *obj, unsigned long dataRegAddress);
void DMA_LIN_SCI_9_Bit_Mode_TX_Init(DmaLinObj *obj, unsigned long dataRegAddress);
unsigned char Write_DMA_Data(DmaLinObj *obj);
unsigned char Write_DMA_LIN_SCI_9_Bit_Mode_Data(DmaLinObj *obj);
unsigned char Read_Circular_DMA_Data(DmaLinObj *obj);
unsigned char Read_Circular_DMA_LIN_SCI_9_Bit_Mode_Data(DmaLinObj *obj);

/* Global data declarations */

#endif
