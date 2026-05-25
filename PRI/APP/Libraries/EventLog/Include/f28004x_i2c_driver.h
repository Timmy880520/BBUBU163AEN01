/**************************************************************************************************
File Name: f28004x_i2c_driver.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of f28004x_i2c_driver.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    09/15/2020 Fred Huang       1. support Ver. 1.0.
    09/10/2021 S.Y Lee          1. Support transmit and receive function and add reset function. Test OK.
**************************************************************************************************/

#ifndef _F28004X_I2C_DRIVER_H_
#define _F28004X_I2C_DRIVER_H_

/* Includes */
#include "F28x_Project.h"

/* Macro definitions */
// Statement Replacement
#define I2C_IPSC_VALUE(Clock)           (unsigned long)((CLK_100MHz / Clock) - 1)
#define I2C_PERIOD_VALUE(Clock, Baud)   (unsigned long)(Clock / Baud)

#ifndef CLK_100MHz
#define CLK_100MHz                      100000000UL
#endif

#ifndef CLK_20MHz
#define CLK_20MHz                       20000000UL
#endif

// Group of declarations
#define Create_I2C_Object(ObjName, \
                          RegName, \
                          Clock, \
                          Baud)         mI2cObj ObjName = \
                                        {\
                                            {\
                                                &RegName, \
                                                I2C_IPSC_VALUE(Clock), \
                                                I2C_PERIOD_VALUE(Clock, Baud), \
                                                0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 \
                                            }, \
                                            {\
                                                I2C_Master_Init, \
                                                I2C_Master_Process_Transmit, \
                                                I2C_Master_Process_Receive, \
                                                I2C_SW_Reset \
                                            }\
                                        }

/* Type definitions */
// Enumeration
typedef enum
{
    I2C_7bit_ADDRESS        = 0,
    I2C_10bit_ADDRESS       = 1
} I2cAddressMode;

typedef enum
{
    I2C_RECEIVER_MODE       = 0,
    I2C_TRANSMITTER_MODE    = 1
} I2cTransmitMode;

// Struct & Union
typedef struct I2cStr mI2cObj;

struct I2cStr
{
    struct
    {
        volatile struct I2C_REGS *reg;
        unsigned long ipscValue;
        unsigned long periodValue;
        unsigned short slaveAddress;    // I2C slave address
        unsigned short txSize;          // Number of bytes to be transmitted
        unsigned short rxSize;          // Number of bytes to be received
        unsigned short occupied;        // Set if the transfer is not complete
        unsigned short txStep;          // Step count of transmitter
        unsigned short txSubStep;       // Sub-step count of transmitter
        unsigned short rxStep;          // Step count of receiver
        unsigned short transmitting;    // Processing transmit function
        unsigned short receiving;       // Processing receive function
        unsigned short *txdPtr;         // Data pointer of transmitter
        unsigned short *rxdPtr;         // Data pointer of receiver
    }member;

    struct
    {
        void (*init)(mI2cObj*, I2cAddressMode);
        unsigned char (*transmit)(void*, unsigned char, void*, unsigned char,void*, unsigned char);
        unsigned char (*receive)(void*, unsigned char, void*, unsigned char,void*, unsigned char);
        void (*resetModule)(mI2cObj*);        // Pointer of reset I2C function
    }func;
};

/* Global function prototypes */
void I2C_Master_Init(mI2cObj *obj, I2cAddressMode addressMode);
unsigned char I2C_Master_Process_Transmit(void *obj, unsigned char id,
                                          void *aPtr, unsigned char aSize,
                                          void *dPtr, unsigned char dSize);
unsigned char I2C_Master_Process_Receive(void *obj, unsigned char id,
                                         void *aPtr, unsigned char aSize,
                                         void *dPtr, unsigned char dSize);
unsigned char I2C_Master_Write_FIFO(void *obj, unsigned short *buffer, unsigned short dataCount);
unsigned char I2C_Master_Read_FIFO(void *obj, unsigned short *buffer, unsigned short dataCount);
void I2C_SW_Reset(mI2cObj *obj);

/* Global data declarations */

//------------------------------------------------------------------------------
#endif /* _F28004X_I2C_DRIVER_H_ */
