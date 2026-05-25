/**************************************************************************************************
File Name: f28003x_lin_driver.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of f28003x_lin_driver.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. support version 1.x.

**************************************************************************************************/

#ifndef __F28003X_LIN_DRIVER_H__
#define __F28003X_LIN_DRIVER_H__

/* Includes */
#include "F28x_Project.h"
#include "f28003x_dma_lin_driver.h"

/* Macro definitions */
// Statement Replacement
#define LIN_P_VALUE(Clock, Baud)        (unsigned long)(Clock / Baud / 16 - 1)
#define LIN_M_VALUE(Clock, Baud)        (unsigned long)((Clock - (LIN_P_VALUE(Clock, Baud) + 1) * 16 * Baud) / Baud)

#ifndef CLK_120MHz
#define CLK_120MHz                      120000000UL
#endif

// Group of declarations
#define Create_LIN_Object(ObjName, \
                          RegName, \
                          Clock, \
                          Baud)         LinObj ObjName = \
                                        {\
                                            {\
                                                &RegName, \
                                                LIN_P_VALUE(Clock, Baud), \
                                                LIN_M_VALUE(Clock, Baud), \
                                                0, 0\
                                            }, \
                                            {\
                                                LIN_SCI_Mode_Init, \
                                                LIN_SCI_Mode_DMA_Transmit, \
                                                LIN_SCI_Mode_DMA_Receive, \
                                                LIN_SCI_Mode_Clear_State_Register\
                                            }\
                                        }

/* Type definitions */
// Enumeration
typedef enum
{
    // Bit[1:0] for DMA
    DMA_LIN_DISABLE     = 0,
    DMA_LINRX_ENABLE    = 1,
    DMA_LINTX_ENABLE    = 2,
    DMA_LIN_ENABLE      = 3,

    // Bit[3:2] for data bits
    DATA_8_BIT          = 0,
    DATA_9_BIT          = 4
}LinFeatureEnum;

// Struct & Union
typedef struct LinStr LinObj;

struct LinStr
{
    struct
    {
        volatile struct LIN_REGS *reg;
        unsigned long pValue;
        unsigned long mValue;
        unsigned short txDataCount;
        unsigned short rxErrorData;
    }member;

    struct
    {
        void (*init)(LinObj*, unsigned long);
        unsigned char (*transmit)(void*, void*, unsigned char);
        unsigned char (*receive)(void*, void**, unsigned char*);
        void (*clearState)(LinObj*);
    }func;
};

/* Global function prototypes */
void LIN_SCI_Mode_Init(LinObj *obj, unsigned long feature);
unsigned char LIN_SCI_Mode_Transmit(void *obj, void *buffer, unsigned char size);
unsigned char LIN_SCI_Mode_Receive(void *obj, void **buffer, unsigned char *size);
unsigned char LIN_SCI_Mode_DMA_Transmit(void *obj, void *buffer, unsigned char size);
unsigned char LIN_SCI_Mode_DMA_Receive(void *obj, void **buffer, unsigned char *size);
void LIN_SCI_Mode_Clear_State_Register(LinObj *obj);

/* Global data declarations */

#endif
