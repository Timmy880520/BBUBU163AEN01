/**************************************************************************************************
 File name: Slave_pmbus.h
 Global Data:
 Name                 Type               Description
 -------------------- ------------------ -----------------------------------------------------


 ==================================================================================================
 History:
 Date       Author           Description of Change
 ---------- ---------------- ------------------------------------------------------------------
 07/22/2021 S.Y Lee        1. version 1.0.

 **************************************************************************************************/
#ifndef INCLUDE_SLAVE_PMBUS_H_
#define INCLUDE_SLAVE_PMBUS_H_

//#include "f28004x_device.h"         // f28004x Headerfile Include File
#include "F28x_Project.h"
#include "Io.h"

/* Macro definitions */

/* Type definitions */
typedef union PMBSTS_REG PmbusStatusReg;
typedef union PMBHSA_REG PmbusShaReg;

typedef enum
{
    SMBUS_STANDBY = 0,
    SMBUS_CMD_RECEIVING,
    SMBUS_DATA_RECEIVING_STATUS,
    SMBUS_STORE_DATA_STATUS,
    SMBUS_PREPARE_DATA_STATUS,
    SMBUS_DATA_SENTS_STATUS
} EnumSMBusState;

typedef struct I2cRegister I2cRegObj;

struct I2cRegister
{
    EnumSMBusState enumSmbusState;
    unsigned char receiveCommand;
    unsigned char i2cTxIndex;
    unsigned char i2cRxIndex;
    unsigned char i2cRxBuffer[100];
    unsigned char i2cTxBuffer[100];
    Uint32 pmTxData;
    void (*SMBus_Handler)(I2cRegObj*);
};

/* Global function prototypes */

extern void Init_PMBus_Slave(unsigned char pmbusAddress,
                             unsigned char pmbusMask);
extern void Interrupt_Sevice_Slave_PMBus(I2cRegObj *trx);

#endif /* INCLUDE_SLAVE_PMBUS_H_ */
