/**************************************************************************************************
File name: slave_pmbus.c
External Data:
    Name                                    Source
    --------------------------------------- ------------------------------------------------------

External Functions:
    None.
Description:
    1pu sine wave with offset generation function.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- ------------------------------------------------------------------
    07/22/2021 SHIHYAO.LEE       1. version 1.0.

**************************************************************************************************/
//#include <string.h>
//#include "Slave_pmbus.h"

//PmbusStatusReg  pmStatus;
//PmbusShaReg     pmSha;
//unsigned int    numberCnt;
/***********************************************************************************************************************
Function Name:
    void Init_PMBus_Slave(unsigned char pmbusAddress)
Input:
    unsigned char pmbusAddress : PMBus 7 bit address
    unsigned char pmbusMask : PMBus address mask. The bit need mask set to 0.
Output:

Comment:
    Initial PMBus module
***********************************************************************************************************************/
/*
void Init_PMBus_Slave(unsigned char pmbusAddress, unsigned char pmbusMask)
{
    EALLOW;
    PmbusaRegs.PMBCTRL.bit.CLKDIV = 9;          //(SYSCLK)/(CLKDIV+1) should <= 10MHz
    PmbusaRegs.PMBCTRL.bit.SLAVE_EN = 1;
    PmbusaRegs.PMBSC.bit.SLAVE_ADDR = pmbusAddress;
    PmbusaRegs.PMBSC.bit.SLAVE_MASK = pmbusMask;
    PmbusaRegs.PMBSC.bit.MAN_SLAVE_ACK = 1;     //0 = Slave automatically acknowledges device address, 1 = Enables the Manual Slave Address Acknowledgement
    PmbusaRegs.PMBSC.bit.PEC_ENA = 1;           //1 = Slave PEC processing enabled; SY:No use this function. This PEC sometimes not work.
                                                //And if master read more after set TX_PEC bit, bus will hang to low.
    PmbusaRegs.PMBSC.bit.MAN_CMD = 1;           //0 = Slave automatically acknowledges received command code, 1 = Data Request flag generated after receipt of command code
    PmbusaRegs.PMBSC.bit.RX_BYTE_ACK_CNT = 3;   //4 bytes received by slave. Hardware automatically acknowledges the first 3 received

    //Interrupt
    PmbusaRegs.PMBINTM.bit.SLAVE_ADDR_READY = 0;
    PmbusaRegs.PMBINTM.bit.EOM = 0;
    PmbusaRegs.PMBINTM.bit.DATA_REQUEST = 0;
    PmbusaRegs.PMBINTM.bit.DATA_READY = 0;
    EDIS;
}
*/
/***********************************************************************************************************************
Function Name:
    void Interrupt_Sevice_Slave_PMBus(void)
Input:
    unsigned char pmbusAddress : PMBus 7 bit address
    unsigned char pmbusMask : PMBus address mask. The bit need mask set to 0.
Output:

Comment:
    Initial PMBus module
***********************************************************************************************************************/
/*
void Interrupt_Sevice_Slave_PMBus(I2cRegObj *trx)
{
    pmStatus.all = PmbusaRegs.PMBSTS.all;
    pmSha.all = PmbusaRegs.PMBHSA.all;

    LED_TOGGLE();
    if(pmStatus.bit.SLAVE_ADDR_READY)
    {
        if(pmStatus.bit.RPT_START == 1)                                         //Read address
        {
            if (trx->enumSmbusState == SMBUS_DATA_RECEIVING_STATUS)
           {
                trx->enumSmbusState = SMBUS_PREPARE_DATA_STATUS;
           }
           else
           {
               trx->i2cRxIndex = 0;                   //For Tx Target length
               trx->enumSmbusState = SMBUS_DATA_SENTS_STATUS;
           }
        }
        else if(pmSha.bit.SLAVE_RW == 0)                 //write address
        {
            memset(trx->i2cRxBuffer, 0, sizeof(trx->i2cRxBuffer));
            memset(trx->i2cTxBuffer, 0, sizeof(trx->i2cTxBuffer));
            trx->i2cTxIndex = 0;
            trx->i2cRxIndex = 0;
            trx->enumSmbusState = SMBUS_CMD_RECEIVING;
        }

    }

    if(pmStatus.bit.DATA_READY)
    {
        if (trx->enumSmbusState == SMBUS_DATA_RECEIVING_STATUS)
        {
            for(int count = 0; count < pmStatus.bit.RD_BYTE_COUNT; count++)
            {
                unsigned long dataLoc = 0x000000FF;
                dataLoc = dataLoc << (count * 8);
                trx->i2cRxBuffer[trx->i2cRxIndex++] = (PmbusaRegs.PMBRXBUF & dataLoc) >> (count * 8);
            }
        }
        if (trx->enumSmbusState == SMBUS_CMD_RECEIVING)
        {
            trx->receiveCommand = PmbusaRegs.PMBRXBUF & 0x00FF;
            trx->enumSmbusState = SMBUS_DATA_RECEIVING_STATUS;
        }
    }
    if(pmStatus.bit.EOM)
    {
        //if(pmStatus.bit.PEC_VALID == 1)   //1 is valid
        if (trx->enumSmbusState == SMBUS_DATA_RECEIVING_STATUS)
        {
            trx->enumSmbusState = SMBUS_STORE_DATA_STATUS;
        }
        if (trx->enumSmbusState == SMBUS_DATA_SENTS_STATUS)
        {
            trx->enumSmbusState = SMBUS_STANDBY;
        }
    }


    if (trx->SMBus_Handler != NULL)
    {
        trx->SMBus_Handler(trx);
    }
    //Test code
    if(trx->enumSmbusState == SMBUS_PREPARE_DATA_STATUS)
    {
        trx->i2cTxBuffer[0] = 0x30;
        trx->i2cTxBuffer[1] = 0x31;
        trx->i2cTxBuffer[2] = 0x32;
        trx->i2cTxBuffer[3] = 0x33;
        trx->i2cTxBuffer[4] = 0x34;
        trx->i2cTxBuffer[5] = 0x35;
        trx->i2cTxBuffer[6] = 0x36;
        trx->i2cTxBuffer[7] = 0x37;
        trx->i2cRxIndex = 6;
        trx->enumSmbusState = SMBUS_DATA_SENTS_STATUS;
    }

    if(pmStatus.bit.RPT_START == 1 || pmStatus.bit.DATA_REQUEST)
    {
        if(trx->enumSmbusState == SMBUS_DATA_SENTS_STATUS)
        {
//            LED_TOGGLE();
//            numberCnt = trx->i2cRxIndex - trx->i2cTxIndex;
//            if(numberCnt > 4) numberCnt = 4;
            trx->pmTxData = 0;
            for(unsigned int count = 0; count < 4; count++)
            {

                if ((trx->i2cRxIndex != 0) && (trx->i2cRxIndex > trx->i2cTxIndex))
                {
                    trx->pmTxData |= (unsigned long)(trx->i2cTxBuffer[trx->i2cTxIndex++] & 0xFF) << (count * 8);
                }
                else
                {
                    trx->pmTxData |= (unsigned long)0xFF << (count * 8);
                }

            }
            EALLOW;
//            if(numberCnt < 4 || (trx->i2cRxIndex == trx->i2cTxIndex))
//                PmbusaRegs.PMBSC.bit.TX_PEC = 1;
//            PmbusaRegs.PMBSC.bit.TX_COUNT = numberCnt;
            PmbusaRegs.PMBSC.bit.TX_COUNT = 4;
            EDIS;
            PmbusaRegs.PMBTXBUF = trx->pmTxData;
        }
    }
    PmbusaRegs.PMBACK.bit.ACK = 1;                      //Acknowledge received data
}
*/
