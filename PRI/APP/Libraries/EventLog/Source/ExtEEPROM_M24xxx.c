/**************************************************************************************************
File Name: ExtEEPROM_M24xxx.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:          
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    Call_Timer                              Univ_Lib.c 
Description: 
    M24xxx series universal external EEPROM HAL program, "storageTimeBase" shall plug in a 
cycling ISR to enable the timeout function. 

Example: 
    void TIM_ISR()
    {
        ...
        ..
        .
        Call_Timer(storageTimeBase);
    }

===================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    03/12/2018 Watch Lee        1. version 1.0. 
    08/15/2018 Watch Lee        1. version 2.0. 
                                2. Add Pull_Low_Write_Control function.
                                3. Add Pull_High_Write_Control function.
                                4. Create the EEPROM object.
                                5. Remove direct peripheral module access.
    09/17/2021 S.Y Lee          1. version 2.1
                                2. Support TI unsigned char length is 16 bits problem.
                                   Modify all data put in high byte of short variable.
**************************************************************************************************/

/* Includes */
#include "../../EventLog/Include/ExtEEPROM_M24xxx.h"

/* Local function prototypes */
void One_Address_Byte_Read_Process(EepromObj *obj, EepromPackType data);
void Two_Address_Byte_Read_Process(EepromObj *obj, EepromPackType data);
void One_Address_Byte_Write_Process(EepromObj *obj, EepromPackType data);
void Two_Address_Byte_Write_Process(EepromObj *obj, EepromPackType data);
unsigned char Current_Read_Operation(EepromObj *obj, EepromPackType data);    
unsigned char Page_Write_Operation(EepromObj *obj, EepromPackType data);
unsigned char Check_Storage_Timeout(EepromObj *obj, TmrPkg *timeBase, TmrPkg *timer);

/* Data definitions */
Create_Timer(storageTimeBase, Storage_Base);
Create_Timer(storageTimeout, Storage_Count);

/**************************************************************************************************
Function Name:
    unsigned char Read_External_EEPROM(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.		
Output:
    return      - Result. 0x01 = completed, 0x00 = not complete, and 0x80 = failed.
Comment:
    External EEPROM read function. The function has the ability to automatic adjust the memory 
addressing, it means that the Device Select Code will automatic adjust when the read operation 
is not in the same page.
**************************************************************************************************/
unsigned char Read_External_EEPROM(EepromObj *obj, EepromPackType data)
{		
    if (Check_Storage_Timeout(obj, &storageTimeBase, &storageTimeout) || obj->member.result == 0x80)
    {	
        obj->func.resetParameter(obj);
        obj->member.result = 0x80;
        return obj->member.result;
    }
    
    if (obj->member.addressBytes == ONE_ADDRESS_BYTE)
    {
        One_Address_Byte_Read_Process(obj, data);         
    }
    else if (obj->member.addressBytes == TWO_ADDRESS_BYTE)
    {
        Two_Address_Byte_Read_Process(obj, data);          
    }
	
    if (obj->member.result == 0x01)
    {	
        Timer_Reset(storageTimeout);
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    void One_Address_Byte_Read_Process(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.			
Output:
    None.
Comment:
    External EEPROM read process of one address byte.
**************************************************************************************************/
void One_Address_Byte_Read_Process(EepromObj *obj, EepromPackType data)
{
    if (!(((data.offset + data.size - 1) ^ data.offset) & 0xFF00))
    {	
        if (!obj->member.result)
            obj->member.result = Current_Read_Operation(obj, data);
    }
    else
    {
        EepromPackType buffer;
        unsigned short readBytes = 0x100 - (data.offset & 0xFF);
        
        if (!obj->member.result)
        {
            buffer.offset = data.offset;
            buffer.size = readBytes;
            buffer.ptr = data.ptr;
            obj->member.result = Current_Read_Operation(obj, buffer) << 1;
        }
        else if (obj->member.result == 2)
        {
            buffer.offset = (data.offset & 0xFF00) + 0x100;
            buffer.size = data.size - readBytes;
            buffer.ptr = (char*)data.ptr + readBytes;
            obj->member.result -= Current_Read_Operation(obj, buffer);
        }
    }
}
/**************************************************************************************************
Function Name:
    void Two_Address_Byte_Read_Process(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.		
Output:
    None.
Comment:
    External EEPROM read process of two address byte.
**************************************************************************************************/
void Two_Address_Byte_Read_Process(EepromObj *obj, EepromPackType data)
{
    if (!obj->member.result)
        obj->member.result = Current_Read_Operation(obj, data); 
}
/**************************************************************************************************
Function Name:
    unsigned char Write_External_EEPROM(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.	
Output:
    return      - Result. 0x01 = completed, 0x00 = not complete, and 0x80 = failed.
Comment:
    External EEPROM write function. The function has the ability to automatic adjust the memory 
addressing, it means that the Device Select Code will automatic adjust when the write operation 
is not in the same page.
**************************************************************************************************/
unsigned char Write_External_EEPROM(EepromObj *obj, EepromPackType data)
{
    obj->method.enableWrite(obj);
                
    if (Check_Storage_Timeout(obj, &storageTimeBase, &storageTimeout) || obj->member.result == 0x80)
    {	
        obj->func.resetParameter(obj);
        obj->member.result = 0x80;
        return obj->member.result;
    }	

    if (obj->member.addressBytes == ONE_ADDRESS_BYTE)
    {
        One_Address_Byte_Write_Process(obj, data);         
    }
    else if (obj->member.addressBytes == TWO_ADDRESS_BYTE)
    {
        Two_Address_Byte_Write_Process(obj, data);
    }
	
    if (obj->member.result == 0x01)
    {	
        Timer_Reset(storageTimeout);
        obj->method.disableWrite(obj);
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    void One_Address_Byte_Write_Process(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.			
Output:
    None.
Comment:
    External EEPROM write process of one address byte.
**************************************************************************************************/
void One_Address_Byte_Write_Process(EepromObj *obj, EepromPackType data)
{
    unsigned char pseudoAddress = 0, deviceCode = 0;

    if (!data.size || !(((data.offset + data.size - 1) ^ data.offset) & 0xFF00))
    {
        if (!obj->member.result)
        {	        
            deviceCode = obj->member.deviceId + ((data.offset >> 7) & I2C_Device_An(obj->member.deviceId));
            
            if (obj->method.driverRead)
                obj->member.result = obj->method.driverRead(obj->member.driverObj, deviceCode, 
                                                            &pseudoAddress, 0, &pseudoAddress, 0) << 1;
            
            obj->member.busy = !obj->member.result;
        }
        else if (obj->member.result == 2)
            obj->member.result -= Page_Write_Operation(obj, data);
    }
    else
    {
        unsigned short writeBytes = 0x100 - (data.offset & 0xFF);
             
        if (!(obj->member.result & 0x03))
        {	
            deviceCode = obj->member.deviceId + ((data.offset >> 7) & I2C_Device_An(obj->member.deviceId));
            
            if (!obj->member.result) 
            {
                 if (obj->method.driverRead && obj->method.driverRead(obj->member.driverObj, deviceCode, 
                                                                      &pseudoAddress, 0, &pseudoAddress, 0))
                     obj->member.result = 0x0A;	
            }
            else if (obj->member.result == 0x08 || obj->member.result == 0x04)
            {	
                 if (obj->method.driverRead && obj->method.driverRead(obj->member.driverObj, deviceCode, 
                                                                      &pseudoAddress, 0, &pseudoAddress, 0))
                     obj->member.result = 0x02;
            }
            
            obj->member.busy = !obj->member.result;
        }
        else
        {	
            EepromPackType buffer;
            
            if ((obj->member.result & 0x08))
            {
                buffer.offset = data.offset;
                buffer.size = writeBytes;
                buffer.ptr = data.ptr;
                obj->member.result -= Page_Write_Operation(obj, buffer) << 1;
                
                if (obj->member.result == 0x06) 
                    obj->member.result = 0x0;					
            }
            else if (obj->member.result == 0x02)
            {
                buffer.offset = (data.offset & 0xFF00) + 0x100;
                buffer.size = data.size - writeBytes;
                buffer.ptr = (char*)data.ptr + writeBytes;
                obj->member.result -= Page_Write_Operation(obj, buffer);
                
                if (!obj->member.result) 
                    obj->member.result = 0x04;				
            }
        }
    }
}
/**************************************************************************************************
Function Name:
    void Two_Address_Byte_Write_Process(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.		
Output:
    None.
Comment:
    External EEPROM write process of two address byte.
**************************************************************************************************/
void Two_Address_Byte_Write_Process(EepromObj *obj, EepromPackType data)
{
    if (!obj->member.result)
    {	
        unsigned char pseudoAddress = 0;
        
        if (obj->method.driverRead)
            obj->member.result = obj->method.driverRead(obj->member.driverObj, obj->member.deviceId, 
                                                        &pseudoAddress, 0, &pseudoAddress, 0) << 1;
        
        obj->member.busy = !obj->member.result;
    }
    else if (obj->member.result == 2)
        obj->member.result -= Page_Write_Operation(obj, data);
}
/**************************************************************************************************
Function Name:
    unsigned char Current_Read_Operation(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.		
Output:
    return      - Result. 0x01 = completed, 0x00 = not complete.
Comment:
    External EEPROM current read function. The function does not support automatic adjustment of 
the memory addressing.
**************************************************************************************************/
unsigned char Current_Read_Operation(EepromObj *obj, EepromPackType data)
{
    unsigned long byteAddr = 0, deviceCode = 0;
     
    if (obj->member.addressBytes == ONE_ADDRESS_BYTE)
    { 
        byteAddr = data.offset & 0xFF;
        deviceCode = obj->member.deviceId + ((data.offset >> 7) & I2C_Device_An(obj->member.deviceId));        
    }
    else if (obj->member.addressBytes == TWO_ADDRESS_BYTE)
    { 
        byteAddr = (((unsigned long)data.offset & 0xFF) << 16) + ((data.offset & 0xFF00) >> 8);
        deviceCode = obj->member.deviceId;  
    }

    if (obj->method.driverRead)    
        obj->member.busy = !obj->method.driverRead(obj->member.driverObj, deviceCode, 
                                                   &byteAddr, obj->member.addressBytes, data.ptr, data.size);

    return !obj->member.busy;
}
/**************************************************************************************************
Function Name:
    unsigned char Page_Write_Operation(EepromObj *obj, EepromPackType data)
Input:
    *obj        - Object pointer of EEPROM. 
    data        - Structure of EEPROM data pack.	
Output:
    return      - Result. 0x01 = completed, 0x02 = separate page write, 0x00 = not complete.
Comment:
    External EEPROM page write function. This function has the ability to automatic separate the 
transferring data to correspond with the number of bytes in a single write cycle.
**************************************************************************************************/
unsigned char Page_Write_Operation(EepromObj *obj, EepromPackType data)
{
    unsigned long addr, writeSize, deviceCode = 0;
    unsigned short writeShift = obj->member.txPageCount * obj->member.pageWriteBytes;

    if (obj->member.addressBytes == ONE_ADDRESS_BYTE)
    { 
        deviceCode = obj->member.deviceId + ((data.offset >> 7) & I2C_Device_An(obj->member.deviceId));
        addr = (data.offset & 0xFF) + writeShift;
    }
    else if (obj->member.addressBytes == TWO_ADDRESS_BYTE)
    {
        deviceCode = obj->member.deviceId;
        addr = ((((unsigned long)data.offset + writeShift) & 0xFF) << 16) + (((data.offset + writeShift) & 0xFF00) >> 8);
    }      
    
    if (writeShift < data.size)
    {			
        if (data.size > obj->member.pageWriteBytes)
        {	
            if (((obj->member.txPageCount + 1) * obj->member.pageWriteBytes) < data.size)
                writeSize = obj->member.pageWriteBytes;
            else
                writeSize = data.size - writeShift;
        }
        else 
            writeSize = data.size; 
        
        unsigned char *buffer = ((unsigned char*)data.ptr + writeShift);
   
        if (obj->method.driverWrite)
        {    
            if (obj->method.driverWrite(obj->member.driverObj, deviceCode, 
                                        &addr, obj->member.addressBytes, buffer, writeSize))
            {
                obj->member.txPageCount++;
                obj->member.busy = 0;
                return 2;
            }
            else
                obj->member.busy = 1;
        }
    }
    else
    {
        obj->member.txPageCount = 0;
        obj->member.busy = 0;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Check_Storage_Timeout(EepromObj *obj, TmrPkg *timeBase, TmrPkg *timer)
Input:
    *obj      - Object pointer of EEPROM. 
    *timeBase - The pointer points to a variable of timer structure which shall be counted up 
                by an exception of fixed cycles. The timer structure(TmrPkg) is declared 
                in Univ_Lib.h. 
    *timer    - The pointer points to a variable of timer structure which is counted up by this 
                function to detect a timeout condition. The timer structure(TmrPkg) is declared 
                in Univ_Lib.h.
Output:
    return    - Result. 1 = a timeout condition is detected, 0 = no timeout condition.
Comment:
    External EEPROM check timeout function.
**************************************************************************************************/
unsigned char Check_Storage_Timeout(EepromObj *obj, TmrPkg *timeBase, TmrPkg *timer)
{
    unsigned char checkFlag = 0;
     
    if (timeBase->reg.bits.flg)
    {			    
        timeBase->reg.val = 0;
        checkFlag = 1;
    }
    
    if (obj->member.busy)
    {				
        if (checkFlag)
        {			    		
            if (Call_Timer(*timer))
            {		
                timer->reg.val = 0;
                return 1;
            }
        }
    }
    else	
        timer->reg.val = 0;
    return 0;
}
/**************************************************************************************************
Function Name:
    void Reset_EEPROM_Object_Parameter(EepromObj *obj)
Input:
    *obj      - Object pointer of EEPROM. 
Output:
    None.
Comment:
    Reset EEPROM object parameter function.
**************************************************************************************************/
void Reset_EEPROM_Object_Parameter(EepromObj *obj)
{
    obj->member.busy = 0;
    obj->member.txPageCount = 0;
    obj->member.result = 0;
}
/**************************************************************************************************
Function Name:
    void Pull_Low_Write_Control(EepromObj *obj)
Input:
    *obj      - Object pointer of EEPROM. 
Output:
    None.
Comment:
    Pull low EEPROM write control function.
**************************************************************************************************/
void Pull_Low_Write_Control(EepromObj *obj)
{
    if (obj->member.writeControl == ENABLE_WRITE_CONTROL)
        _Clear_Default_WC;
}
/**************************************************************************************************
Function Name:
    void Pull_High_Write_Control(EepromObj *obj)
Input:
    *obj      - Object pointer of EEPROM. 
Output:
    None.
Comment:
    Pull high EEPROM write control function.
**************************************************************************************************/
void Pull_High_Write_Control(EepromObj *obj)
{
    if (obj->member.writeControl == ENABLE_WRITE_CONTROL)
        _Set_Default_WC;
}
//---------------- END LINE -----------------------------------------------------------------------
