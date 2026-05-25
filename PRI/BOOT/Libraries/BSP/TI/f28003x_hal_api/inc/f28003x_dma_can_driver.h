/**************************************************************************************************
File Name: f28003x_dma_can_driver.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of f28003x_dma_can_driver.c.

===================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/23/2023 Watch Lee        1. support version 1.x.

**************************************************************************************************/

#ifndef __F28003X_DMA_CAN_DRIVER_H__
#define __F28003X_DMA_CAN_DRIVER_H__

/* Includes */
#include "F28x_Project.h"

/* Macro definitions */
#ifndef DMA_CHANNEL_BASE
#define DMA_CHANNEL_BASE                    0x00001020U
#endif

// CAN module register base address
#define DMA_CANA_REGS_BASE                  (unsigned long)&CanaRegs

// DMA CAN trigger number
#define DMA_CANA_RX_TRIGGER                 169UL

// Statement Replacement
#ifndef Check_Power_2
#define Check_Power_2(Number)               (((((unsigned)Number - 1) ^ Number) == (Number << 1) - 1) ? Number : -1)
#endif

// Group of declarations (DMA accessing memory MUST be placed in GS0 ~ GS3 RAM = 0x0000C000 ~ 0x0000FFFF)
#define Create_CAN_RX_DMA_Object(ObjName, \
                                 Size, \
                                 RegName)   unsigned long ObjName##Mem[Check_Power_2(Size)][4];\
                                            unsigned long ObjName##Buffer[1][4];\
                                            DmaCanObj ObjName = \
                                            {\
                                                {\
                                                    &RegName,\
                                                    ObjName##Mem, Size,\
                                                    ObjName##Buffer, 1,\
                                                    0\
                                                },\
                                                {\
                                                    DMA_CAN_RX_Init,\
                                                    Read_CAN_RX_DMA_Data\
                                                }\
                                            }

/* Type definitions */
// Enumeration
// Struct & Union
typedef struct
{
    unsigned long (*ptr)[4];
    unsigned short size;
}DmaCanDataStr;

typedef struct DmaCanStr DmaCanObj;

struct DmaCanStr
{
    struct
    {
        volatile struct CH_REGS *reg;
        DmaCanDataStr dma;
        DmaCanDataStr buffer;
        unsigned short index;
    }member;

    struct
    {
        void (*init)(DmaCanObj*, unsigned long);
        unsigned char (*accessDmaData)(DmaCanObj*);
    }func;
};

/* Global function prototypes */
void DMA_CAN_RX_Init(DmaCanObj *obj, unsigned long dataRegAddress);
unsigned char Read_CAN_RX_DMA_Data(DmaCanObj *obj);

/* Global data declarations */

#endif
