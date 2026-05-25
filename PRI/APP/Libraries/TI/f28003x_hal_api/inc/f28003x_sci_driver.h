/**************************************************************************************************
File Name: f28003x_sci_driver.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of f28003x_sci_driver.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/23/2023 Watch Lee        1. support version 1.x.

**************************************************************************************************/

#ifndef __F28003X_SCI_DRIVER_H__
#define __F28003X_SCI_DRIVER_H__

/* Includes */
#include "F28x_Project.h"

/* Macro definitions */
// Statement Replacement
#define SCI_BRR_Value(Clock, Baud)      (unsigned long)(Clock / Baud / 8 - 1)

#ifndef LSPCLK_30MHz
#define LSPCLK_30MHz                    30000000UL      // Default value
#endif

#ifndef LSPCLK_120MHz
#define LSPCLK_120MHz                   120000000UL
#endif

// Group of declarations
#define Create_SCI_Object(ObjName, \
                          RegName, \
                          Clock, \
                          Baud)         SciObj ObjName = \
                                        {\
                                            {\
                                                &RegName, \
                                                SCI_BRR_Value(Clock, Baud), \
                                                0, 0\
                                            }, \
                                            {\
                                                SCI_Init, \
                                                SCI_Transmit, \
                                                SCI_Receive, \
                                                SCI_Clear_State_Register\
                                            }\
                                        }

/* Type definitions */
// Enumeration
typedef enum
{
    // Bit[0] for FIFO
    SCI_FIFO_DISABLE    = 0,
    SCI_FIFO_ENABLE     = 1,

    // Bit[1] for data bits
    SCI_8_BIT           = 0,
    SCI_9_BIT           = 2     // Avoid to use FIFO with 9bit because FIFOs only set/get address bit to the first byte.
}SciFeatureEnum;

// Struct & Union
typedef struct SciStr SciObj;

struct SciStr
{
    struct
    {
        volatile struct SCI_REGS *reg;
        unsigned long brrValue;
        unsigned short txDataCount;
        unsigned short rxErrorData;
    }member;

    struct
    {
        void (*init)(SciObj*, unsigned long);
        unsigned char (*transmit)(void*, void*, unsigned char);
        unsigned char (*receive)(void*, void**, unsigned char*);
        void (*clearState)(SciObj*);
    }func;
};

/* Global function prototypes */
void SCI_Init(SciObj *obj, unsigned long feature);
unsigned char SCI_Transmit(void *obj, void *buffer, unsigned char size);
unsigned char SCI_Receive(void *obj, void **buffer, unsigned char *size);
void SCI_Clear_State_Register(SciObj *obj);

/* Global data declarations */

#endif
