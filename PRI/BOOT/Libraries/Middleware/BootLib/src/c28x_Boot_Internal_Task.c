/**************************************************************************************************
 File Name: c28x_Boot_Internal_Task.c
 External Data:
 Name                                    Source
 --------------------------------------- -----------------------------------------------------
 None
 External Functions:
 Name                                    Source
 --------------------------------------- -----------------------------------------------------
 Get_Package_CRC16                       CalcuCRC16.c
 Get_Package_CRC32                       CalcuCRC32.c
 Description:
 TI C28x DSP Boot internal task program.

 ===================================================================================================
 History:
 Date       Author           Description of Change
 ---------- ---------------- -----------------------------------------------------------------
 01/08/2020 Watch Lee        1. version 1.0.
 05/29/2020 Watch Lee        1. version 1.1.
 2. Modify Boot_Internal_State_Init function to support the program
 size over 65535 bytes in the integer data type of 16 bit MCU.
 11/12/2020 Watch Lee        1. version 1.2.
 2. Modify Check_Header_Message function to support the transport
 mode when received image without Mix Header.
 3. Modify Boot_Unlocked_State_Machine function to add FLASH_ERROR
 detection when the flash programming failed.
 11/01/2021 Watch Lee        1. version 1.3.
 2. modify Check_Boot_Buffer and Check_Header_Message functions to
 support the program mode without App header.
 3. modify Boot_Unlocked_State_Machine function to support the
 program mode without the Boot_Buffer_Size alignment of iamge.
 11/02/2021 Watch Lee        1. version 1.4.
 2. modify Boot_Internal_State_Init function to support BOOT_STEP_INIT
 feature.
 3. add Boot_Image_CRC_Calculation function.

 **************************************************************************************************/

/* Includes */
#include "Middleware_Catalog.h"

#if Add_In_C28x_ISP_Task==1
#include "..\inc\c28x_Boot_Internal_Task.h"

/* Local function prototypes */
EnumBootState Boot_Image_CRC_Calculation(BootInternalObj *obj,
                                         unsigned long crcStart,
                                         unsigned long stepSize);
EnumBootCommand Boot_Unlocked_State_Machine(BootInternalObj *obj,
                                            BootInternalObj *transportObj);
unsigned char Check_Boot_Init(BootInternalObj *obj);
unsigned char Check_Boot_Protection(BootInternalObj *obj);
unsigned char Boot_Download_Process(BootInternalObj *obj);
void Update_Boot_Buffer(BootInternalObj *obj);
EnumErrorCode Check_Boot_Buffer(BootInternalObj *obj);
EnumErrorCode Check_Header_Message(BootInternalObj *obj);
void Update_Header_Member(BootInternalObj *obj, void *headerPtr);
EnumErrorCode Configure_Boot_Mode(BootInternalObj *obj, unsigned long funcId,
                                  unsigned long size);
unsigned char Check_Module_Name(BootInternalObj *obj);
unsigned short Compare_Module_Name(BootInternalObj *obj,
                                   unsigned char *namePtr);
EnumErrorCode Check_Mix_Header_CRC(BootInternalObj *obj);
void Reset_Boot_Argument(BootInternalObj *obj);
EnumBootCommand Boot_Transport_Unlock_State_Machine(BootInternalObj *obj,
                                                    unsigned char ack);
void TiByte_Convert(void *input, void *output, unsigned long length);
void Correct_Mix_Header_Information(BootInternalObj *obj);
void Correct_App_Header_Information(StdAppHeaderType *headerPtr);
unsigned long Correct_Header_Data(unsigned long long raw);

/* Data definitions */
StdAppHeaderType appHeader;

/**************************************************************************************************
 Function Name:
 EnumBootState Boot_Internal_State_Init(BootInternalObj *obj, unsigned char feature,
 unsigned long headerStart, unsigned long appRomSize)
 Input:
 *obj        - Object pointer of boot internal task.
 feature     - Init feature index. Refer EnumBootInitFeature to configure this parameter.
 headerStart - Start address of header field.
 appRomSize  - Size of entire app ROM field(Header + code).
 Output:
 return      - Boot state code.
 Comment:
 Init boot state function. This function will verify the header information if waiveHeader flag
 is zero. The boot state will be updated and returned by this function.
 **************************************************************************************************/
EnumBootState Boot_Internal_State_Init(BootInternalObj *obj,
                                       unsigned char feature,
                                       unsigned long headerStart,
                                       unsigned long appRomSize)
{
    // Converts bin data to TiByte data
    TiByte_Convert((char*) headerStart, &appHeader, sizeof(appHeader) >> 1);

    unsigned long dataCRC = Get_Package_CRC32(
            0xFFFFFFFF, (unsigned char*) &(appHeader.header),
            sizeof(appHeader) - sizeof(appHeader.signature));

    Correct_App_Header_Information(&appHeader);

    if (appHeader.signature.headerCrc == dataCRC)
        obj->member.romHeaderPtr = &appHeader;
    else
        obj->member.romHeaderPtr = 0;

    if ((feature & BOOT_WAIVE_HEADER) == BOOT_WAIVE_HEADER)
    {
        obj->member.message.state.stateCode = (BOOT_UNLOCKED << 8) + READY;
        return BOOT_UNLOCKED;
    }
    else if (obj->member.romHeaderPtr)
    {
        // Check function code and code size
        if (appHeader.header.programFunction.word
                == obj->member.ownFunctionId.word
                && appHeader.header.programSize > sizeof(appHeader)
                && appHeader.header.programSize <= (appRomSize * 2)
                && obj->member.checksum.calculate)
        {
            unsigned long crcStart = headerStart + (sizeof(appHeader) >> 1);
            unsigned long stepSize = (appHeader.header.programSize
                    - sizeof(appHeader)) >> 1;

            if ((feature & BOOT_STEP_INIT) == BOOT_STEP_INIT)
                stepSize = Boot_Buffer_Size >> 1;
            return Boot_Image_CRC_Calculation(obj, crcStart, stepSize);
        }
        else
            obj->member.message.state.stateCode = (BOOT_ERROR << 8)
                    + HEADER_ERROR;
    }
    else
        obj->member.message.state.stateCode = (BOOT_ERROR << 8) + HEADER_ERROR;
    return BOOT_ERROR;
}
/**************************************************************************************************
 Function Name:
 EnumBootState Boot_Image_CRC_Calculation(BootInternalObj *obj, unsigned long crcStart, unsigned long stepSize)
 Input:
 *obj        - Object pointer of boot internal task for program mode.
 crcStart    - CRC calculating start address.
 stepSize    - CRC calculating step size.
 Output:
 return      - Boot state code.
 Comment:
 Boot image CRC calculation function. This function will return the boot state code.
 **************************************************************************************************/
EnumBootState Boot_Image_CRC_Calculation(BootInternalObj *obj,
                                         unsigned long crcStart,
                                         unsigned long stepSize)
{
    unsigned long appSize = (appHeader.header.programSize - sizeof(appHeader))
            >> 1;
    unsigned char bufferNumber = 0, buffer[C28_Write_Size];
    static unsigned long byteCount = 0;
    static unsigned long crcStep = 0;

    if (!byteCount)
        crcStep = obj->member.checksum.seed;
    crcStart += byteCount;

    if ((byteCount + stepSize) < appSize)
    {
        bufferNumber = stepSize;
        byteCount += stepSize;
    }
    else if (byteCount < appSize)
    {
        bufferNumber = appSize - byteCount;
        byteCount = 0;
    }

    for (long i = 0; i < bufferNumber; i += (C28_Write_Size >> 1))
    {
        TiByte_Convert((char*) crcStart + i, buffer, C28_Write_Size >> 1);
        crcStep = obj->member.checksum.calculate(crcStep, buffer,
                                                 C28_Write_Size);
    }

    if (!byteCount)
    {
        if (crcStep == obj->member.romHeaderPtr->signature.imageCrc)
            return BOOT_LOCKED;
        else
            obj->member.message.state.stateCode = (BOOT_ERROR << 8)
                    + CHECKSUM_ERROR;
        return BOOT_ERROR;
    }
    return BOOT_INITIATING;
}
/**************************************************************************************************
 Function Name:
 EnumBootCommand Boot_Internal_State_Machine(BootInternalObj *obj, BootInternalObj *transportObj)
 Input:
 *obj            - Object pointer of boot internal task for program mode.
 *transportObj   - Object pointer of boot internal task for transport mode.
 Output:
 return          - Boot command.
 Comment:
 Boot Internal state machine function. This function will return the boot command when the
 state machine has processed the command.
 **************************************************************************************************/
EnumBootCommand Boot_Internal_State_Machine(BootInternalObj *obj,
                                            BootInternalObj *transportObj)
{
    switch (obj->member.message.state.byte.bootState)
    {
    case BOOT_LOCKED:
        if (Check_Boot_Protection(obj))
            return BOOT_PROTECTION;
        break;
    case BOOT_UNLOCKED:
        if (Check_Boot_Protection(obj))
            return BOOT_PROTECTION;
        else if (Check_Boot_Init(obj))
            return BOOT_INIT;
        else
            return Boot_Unlocked_State_Machine(obj, transportObj);
//            break;
    case BOOT_ERROR:
    case BOOT_TRANSPORT_ERROR:
        if (Check_Boot_Protection(obj))
            return BOOT_PROTECTION;
        else if (Check_Boot_Init(obj))
            return BOOT_INIT;
        break;
    case BOOT_RECOVERING:
        return BOOT_RECOVERY;
//            break;
    }
    return BOOT_NONE_COMMAND;
}
/**************************************************************************************************
 Function Name:
 EnumBootCommand Boot_Unlocked_State_Machine(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Boot command.
 Comment:
 Boot unlocked state machine function.
 **************************************************************************************************/
EnumBootCommand Boot_Unlocked_State_Machine(BootInternalObj *obj,
                                            BootInternalObj *transportObj)
{
    switch (obj->member.message.state.byte.subState)
    {
    case PREPARING:
        if (Check_Boot_Protection(obj))
            return BOOT_PROTECTION;
        break;
    case READY:
        if (obj->member.message.reloaded)
        {
            obj->member.message.reloaded = 0;

            if (Boot_Download_Process(obj))
                return BOOT_DOWNLOAD;
        }
        else if (obj->member.message.completeKey == BOOT_COMPLETE_KEY)
        {
            obj->member.message.completeKey = 0;

            if (obj->member.argument.accumulation & (Boot_Buffer_Size - 1))
            {
                if (Check_Boot_Buffer(obj) == NO_ERROR)
                    obj->member.message.state.byte.subState = PROCESSING;
                else
                    obj->member.message.state.stateCode = (BOOT_ERROR << 8)
                            + Check_Boot_Buffer(obj);
            }
            else
            {
                obj->member.message.state.byte.subState = VERIFYING;
                return BOOT_COMPLETED;
            }
        }
        break;
    case PROCESSING:
        // Check package state before acknowledge
        if (obj->member.argument.mode == TRANSPORT_MODE
                || obj->member.argument.mode == PROGRAM_MODE)
        {
            if (obj->member.argument.mode == PROGRAM_MODE
                    && obj->method.program)
            {
                unsigned long offset = Boot_Buffer_Size
                        * obj->member.argument.blockOffset;
                unsigned long size = obj->member.argument.accumulation
                        & (Boot_Buffer_Size - 1);

                size = (!size) ? Boot_Buffer_Size : size;

                if (obj->method.program(offset, obj->member.argument.buffer,
                                        size))
                {
                    if (size == Boot_Buffer_Size)
                    {
                        obj->member.message.state.byte.subState = READY;
                        return BOOT_DOWNLOAD;
                    }
                    else
                    {
                        obj->member.message.state.byte.subState = VERIFYING;
                        return BOOT_COMPLETED;
                    }
                }
                else
                    obj->member.message.state.stateCode = (BOOT_ERROR << 8)
                            + FLASH_ERROR;
            }
            else if (obj->member.argument.mode == TRANSPORT_MODE
                    && obj->method.transport)
            {
                ArrayType buffer = { obj->member.argument.buffer,
                                     Boot_Buffer_Size };
                EnumErrorCode errorCode = obj->method.transport(transportObj,
                                                                buffer, 0);

                if (errorCode == NO_ERROR)
                {
                    obj->member.message.state.byte.subState = READY;
                    return BOOT_DOWNLOAD;
                }
                else if (errorCode < PENDING)
                    obj->member.message.state.stateCode = (BOOT_TRANSPORT_ERROR
                            << 8) + errorCode;
            }
            else
                obj->member.message.state.stateCode = (BOOT_ERROR << 8)
                        + UNKNOWN_ERROR;
        }
        else if (obj->member.argument.mode == GET_MIX_HEADER)
        {
            obj->member.message.state.byte.subState = READY;
            return BOOT_DOWNLOAD;
        }
        else
            obj->member.message.state.stateCode = (BOOT_ERROR << 8)
                    + UNKNOWN_ERROR;
        break;
    case VERIFYING:
        if (obj->member.argument.transportEnable && obj->method.transport)
        {
            ArrayType buffer = { 0, 0 };
            EnumErrorCode errorCode = obj->method.transport(transportObj,
                                                            buffer, 1);

            if (errorCode == NO_ERROR)
                obj->member.message.state.byte.subState = VERIFIED;
            else if (errorCode < PENDING)
                obj->member.message.state.stateCode = (BOOT_ERROR << 8)
                        + errorCode;
        }
        else
            obj->member.message.state.byte.subState = VERIFIED;
        break;
    case VERIFIED:
        return BOOT_FINISHED;
//            break;
    }
    return BOOT_NONE_COMMAND;
}
/**************************************************************************************************
 Function Name:
 unsigned char Check_Boot_Init(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Result. 1 = init key data is correct. 0 = init key data is incorrect.
 Comment:
 Check boot init key data function.
 **************************************************************************************************/
unsigned char Check_Boot_Init(BootInternalObj *obj)
{
    unsigned short currentKey = obj->member.message.initKey;

    if (currentKey == BOOT_INIT_KEY)
    {
        obj->member.message.state.stateCode = (BOOT_UNLOCKED << 8) + READY;
        Reset_Boot_Argument(obj);
        obj->member.message.initKey = 0;
        return 1;
    }
    else if (currentKey == BOOT_RECOVER_KEY)
    {
        if (obj->member.message.state.byte.bootState == BOOT_ERROR)
            obj->member.message.state.stateCode = (BOOT_RECOVERING << 8);
        obj->member.message.initKey = 0;
    }
    return 0;
}
/**************************************************************************************************
 Function Name:
 unsigned char Check_Boot_Protection(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Result. 1 = protect key data is correct. 0 = protect key data is incorrect.
 Comment:
 Check boot protection key data function.
 **************************************************************************************************/
unsigned char Check_Boot_Protection(BootInternalObj *obj)
{
    unsigned short currentKey = obj->member.message.protectKey;

    if (currentKey == BOOT_LOCK_KEY)
    {
        obj->member.message.state.stateCode = (BOOT_LOCKED << 8);
        obj->member.message.protectKey = 0;
        return 1;
    }
    else if (currentKey == BOOT_UNLOCK_KEY)
    {
        if (obj->member.message.state.byte.bootState == BOOT_LOCKED)
            obj->member.message.state.stateCode = (BOOT_UNLOCKED << 8)
                    + PREPARING;
        obj->member.message.protectKey = 0;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
 Function Name:
 unsigned char Boot_Download_Process(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Result. 1 = buffer is not full. 0 = buffer is full.
 Comment:
 Boot download process function. This function will copy download data to the buffer and check
 the buffer state.
 **************************************************************************************************/
unsigned char Boot_Download_Process(BootInternalObj *obj)
{
    Update_Boot_Buffer(obj);
    obj->member.argument.accumulation = obj->member.argument.blockCount
            * Boot_Buffer_Size + obj->member.argument.byteCount;

    if (obj->member.argument.byteCount == Boot_Buffer_Size)
    {
        EnumErrorCode state = Check_Boot_Buffer(obj);

        if (state == NO_ERROR)
            obj->member.message.state.byte.subState = PROCESSING;
        else
            obj->member.message.state.stateCode = (BOOT_ERROR << 8) + state;
        return 0;
    }
    else
        return 1;
}
/**************************************************************************************************
 Function Name:
 void Update_Boot_Buffer(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 None.
 Comment:
 Update boot buffer data function. This function will copy the download data to the buffer.
 **************************************************************************************************/
void Update_Boot_Buffer(BootInternalObj *obj)
{
    int i;

    for (i = 0; i < obj->member.message.download.member.size; i++)
    {
        obj->member.argument.buffer[obj->member.argument.byteCount
                & (Boot_Buffer_Size - 1)] =
                obj->member.message.download.byte[i];

        obj->member.argument.byteCount++;
    }
}
/**************************************************************************************************
 Function Name:
 EnumErrorCode Check_Boot_Buffer(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Error code.
 Comment:
 Check boot buffer data function. This function will use the app/mix header to identify the
 buffer data.
 **************************************************************************************************/
EnumErrorCode Check_Boot_Buffer(BootInternalObj *obj)
{
    int bodyImageBlock = Boot_Header_Block_Size;
    EnumErrorCode result = NO_ERROR;
    StdHeaderSubType *subHeaderPtr;

    if (obj->member.argument.blockCount >= bodyImageBlock)
    {
        switch (obj->member.argument.headerNumber)
        {
        case 0:
            if (obj->member.argument.headerCheckDisable)
                result = Configure_Boot_Mode(obj,
                                             obj->member.ownFunctionId.word,
                                             0xFFFFFFFF);
            else
            {
                result = Configure_Boot_Mode(
                        obj, obj->member.appHeader.header.programFunction.word,
                        obj->member.appHeader.header.programSize);
            }
            break;
        case 1:
            result = Configure_Boot_Mode(
                    obj, obj->member.mixHeader.firstIndex.programFunction.word,
                    obj->member.mixHeader.firstIndex.programSize);
            break;
        case 2:
            result = Configure_Boot_Mode(
                    obj, obj->member.mixHeader.secondIndex.programFunction.word,
                    obj->member.mixHeader.secondIndex.programSize);
            break;
        case 3:
            result = Configure_Boot_Mode(
                    obj, obj->member.mixHeader.thirdIndex.programFunction.word,
                    obj->member.mixHeader.thirdIndex.programSize);
            break;
        case 4:
            result = Configure_Boot_Mode(
                    obj, obj->member.mixHeader.fourthIndex.programFunction.word,
                    obj->member.mixHeader.fourthIndex.programSize);
            break;
        case 5:
            subHeaderPtr = (&obj->member.mixHeader.secondIndex
                    + obj->member.argument.headerNumber - 2);
            result = Configure_Boot_Mode(obj,
                                         subHeaderPtr->programFunction.word,
                                         subHeaderPtr->programSize);
            break;
        default:
            result = UNKNOWN_ERROR;
            break;
        }
    }
    else
        result = Check_Header_Message(obj);

    if (obj->member.argument.startBlock == 0xFFFFFFFF)
    {
        if (!obj->member.appHeader.signature.text.word)
        {
            obj->member.argument.headerNumber++;
            obj->member.argument.blockCount++;
            obj->member.argument.startBlock = obj->member.argument.blockCount;
        }
        else
        {
            obj->member.argument.blockCount = 0;
            obj->member.argument.startBlock = 0;
        }
    }
    else
        obj->member.argument.blockCount++;
    obj->member.argument.byteCount = 0;
    return result;
}
/**************************************************************************************************
 Function Name:
 EnumErrorCode Check_Header_Message(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Error code.
 Comment:
 Check header message function. This function will check the text signature and function code
 to identify the boot mode.
 **************************************************************************************************/
EnumErrorCode Check_Header_Message(BootInternalObj *obj)
{
    EnumErrorCode result = NO_ERROR;

    if (!obj->member.argument.blockCount)
    {
        if (*(long long*) &obj->member.argument.buffer == *(long long*) "MIX")
        {
            obj->member.argument.mode = GET_MIX_HEADER;
            Update_Header_Member(obj, &obj->member.mixHeader);
        }
        else if (*(long long*) &obj->member.argument.buffer
                == *(long long*) "APP"
                || obj->member.argument.headerCheckDisable)
        {
            obj->member.argument.mode = GET_APP_HEADER;
            Update_Header_Member(obj, &obj->member.appHeader);
        }
        else
            result = HEADER_ERROR;
    }
    else
    {
        if (obj->member.argument.mode == GET_MIX_HEADER)
        {
            Update_Header_Member(obj, &obj->member.mixHeader);
            result = Check_Mix_Header_CRC(obj);
        }
        else
            Update_Header_Member(obj, &obj->member.appHeader);
    }

    if (obj->member.argument.blockCount == (Boot_Header_Block_Size - 1))
    {
        if (obj->member.argument.mode == GET_MIX_HEADER)
        {
            Correct_Mix_Header_Information(obj);
            result = Check_Module_Name(obj) ? result : HEADER_ERROR;
        }
        else if (obj->member.argument.headerCheckDisable)
        {
            result = Configure_Boot_Mode(obj, obj->member.ownFunctionId.word,
                                         0xFFFFFFFF);
        }
        else
        {
            Correct_App_Header_Information(&obj->member.appHeader);
            result = Configure_Boot_Mode(
                    obj, obj->member.appHeader.header.programFunction.word,
                    obj->member.appHeader.header.programSize);
        }
    }
    else
    {
        if (obj->member.argument.mode != GET_MIX_HEADER)
        {
            unsigned long functionId;

            if (Boot_Buffer_Size < 64
                    || obj->member.argument.headerCheckDisable)
                functionId = obj->member.ownFunctionId.word;
            else
                functionId = Correct_Header_Data(
                        obj->member.appHeader.header.programFunction.word);
            result = Configure_Boot_Mode(obj, functionId,
                                         sizeof(StdMixHeaderType));
        }

        if (!obj->member.argument.blockCount && Boot_Buffer_Size >= 32)
            result =
                    (Check_Module_Name(obj)
                            || obj->member.argument.headerCheckDisable) ?
                            result : HEADER_ERROR;
    }
    return result;
}
/**************************************************************************************************
 Function Name:
 void Update_Header_Member(BootInternalObj *obj, void *headerPtr)
 Input:
 *obj        - Object pointer of boot internal task.
 *headerPtr  - Header pointer.
 Output:
 None.
 Comment:
 Update header member function. This function will copy the buffer data to the header member of
 the object.
 **************************************************************************************************/
void Update_Header_Member(BootInternalObj *obj, void *headerPtr)
{
    int i;

    for (i = 0; i < Boot_Buffer_Size; i++)
    {
        *((unsigned char*) headerPtr
                + obj->member.argument.blockCount * Boot_Buffer_Size + i) =
                obj->member.argument.buffer[i];
    }
}
/**************************************************************************************************
 Function Name:
 EnumErrorCode Configure_Boot_Mode(BootInternalObj *obj, unsigned long funcId, unsigned long size)
 Input:
 *obj        - Object pointer of boot internal task.
 funcId      - Long integer of function code.
 size        - Size of the update package.
 Output:
 return      - Error code.
 Comment:
 Check boot mode function. This function will configure the boot mode and check the download data
 size.
 **************************************************************************************************/
EnumErrorCode Configure_Boot_Mode(BootInternalObj *obj, unsigned long funcId,
                                  unsigned long size)
{
    if (funcId == obj->member.ownFunctionId.word)
    {
        obj->member.argument.mode = PROGRAM_MODE;
        obj->member.argument.programEnable = 1;
    }
    else
    {
        obj->member.argument.mode = TRANSPORT_MODE;
        obj->member.argument.transportEnable = 1;
    }
    obj->member.argument.blockOffset = obj->member.argument.blockCount
            - obj->member.argument.startBlock;

    if (size == (Boot_Buffer_Size * obj->member.argument.blockOffset
                 + obj->member.argument.byteCount))
    {
        obj->member.argument.startBlock = 0xFFFFFFFF;
        return NO_ERROR;
    }
    else if (size > (Boot_Buffer_Size * obj->member.argument.blockOffset
                    + obj->member.argument.byteCount))
        return NO_ERROR;
    else
        return SIZE_ERROR;
}
/**************************************************************************************************
 Function Name:
 unsigned char Check_Module_Name(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Check result. 1 = pass. 0 = failed.
 Comment:
 Check module name function. The module name has to check only the app header of ROM exists.
 **************************************************************************************************/
unsigned char Check_Module_Name(BootInternalObj *obj)
{
    // Check module name if app header exists
    if (obj->member.romHeaderPtr)
    {
        if (obj->member.argument.mode == GET_MIX_HEADER)
        {
            if (Compare_Module_Name(obj,
                                    obj->member.mixHeader.firstIndex.moduleName)
                    < sizeof(obj->member.romHeaderPtr->header.moduleName))
                return 0;
        }
        else
        {
            if (Compare_Module_Name(obj,
                                    obj->member.appHeader.header.moduleName)
                    < sizeof(obj->member.romHeaderPtr->header.moduleName))
                return 0;
        }
    }
    return 1;
}
/**************************************************************************************************
 Function Name:
 unsigned short Compare_Module_Name(BootInternalObj *obj, unsigned char *namePtr)
 Input:
 *obj        - Object pointer of boot internal task.
 *namePtr    - String pointer of module name.
 Output:
 return      - Total number of bytes of the matched characters.
 Comment:
 Compare module name function. The compared result equals the total number of bytes of the
 module name string if mathed.
 **************************************************************************************************/
unsigned short Compare_Module_Name(BootInternalObj *obj, unsigned char *namePtr)
{
    int i, size = sizeof(obj->member.romHeaderPtr->header.moduleName);

    for (i = 0; i < size; i++)
    {
        if (namePtr[i] != obj->member.romHeaderPtr->header.moduleName[i])
            break;
    }

    if (i < size && obj->member.romHeaderPtr->compliantTableAddress)
    {
        ArrayType compliant =
                { (unsigned char*) (long) obj->member.romHeaderPtr->compliantTableAddress,
                  size };
        unsigned long lastTable =
                obj->member.romHeaderPtr->compliantTableAddress + 10 * size; // support 10 models

        while (compliant.byte[size - 1] != 0xFF)
        {
            for (i = 0; i < size; i++)
            {
                if (namePtr[i] != compliant.byte[i])
                    break;
            }

            if (i < size && (unsigned long) compliant.member.array < lastTable)
                compliant.member.array += size;
            else
                break;
        }
    }
    return i;
}
/**************************************************************************************************
 Function Name:
 EnumErrorCode Check_Mix_Header_CRC(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 return      - Error code.
 Comment:
 Check mixheader CRC function. This function will check CRC of the mix header when received
 data has reached size of the mix header.
 **************************************************************************************************/
EnumErrorCode Check_Mix_Header_CRC(BootInternalObj *obj)
{
    if (obj->member.argument.accumulation == sizeof(obj->member.mixHeader))
    {
        // Check CRC of mixheader                
        long mixCRC = Get_Package_CRC32(
                0xFFFFFFFF,
                (unsigned char*) &obj->member.mixHeader.firstIndex,
                sizeof(obj->member.mixHeader)
                        - sizeof(obj->member.mixHeader.signature));

        // Checksum correction
        long crcData = Correct_Header_Data(
                obj->member.mixHeader.signature.headerCrc);

        if (crcData != mixCRC)
            return HEADER_ERROR;
        else
            obj->member.argument.startBlock = 0xFFFFFFFF;
    }
    return NO_ERROR;
}
/**************************************************************************************************
 Function Name:
 void Reset_Boot_Argument(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 None.
 Comment:
 Reset boot internal arguments function.
 **************************************************************************************************/
void Reset_Boot_Argument(BootInternalObj *obj)
{
    obj->member.argument.headerNumber = 0;
    obj->member.argument.mode = 0;
    obj->member.argument.transportEnable = 0;
    obj->member.argument.programEnable = 0;
    obj->member.argument.accumulation = 0;
    obj->member.argument.startBlock = 0;
    obj->member.argument.blockOffset = 0;
    obj->member.argument.blockCount = 0;
    obj->member.argument.byteCount = 0;
}
/**************************************************************************************************
 Function Name:
 EnumErrorCode Boot_Transport(BootInternalObj *obj, ArrayType data, unsigned char finish)
 Input:
 *obj        - Object pointer of boot internal task for transport mode.
 data        - Data array.
 finish      - Finish flag. 1 = finish transport mode. 0 = download data transport mode.
 Output:
 return      - Error code.
 Comment:
 Boot download data transport function. This function will transport download data to another
 BootInternal object and return error code when error(s) occurred.
 **************************************************************************************************/
EnumErrorCode Boot_Transport(BootInternalObj *obj, ArrayType data,
                             unsigned char finish)
{
    if (obj && !obj->member.message.reloaded)
    {
        if (!finish)
        {
            if (data.member.size != Boot_Buffer_Size)       // Check array size
                return UNKNOWN_ERROR;

            if (obj->member.argument.accumulation < sizeof(StdMixHeaderType))
            {
                unsigned char *appHeaderPtr =
                        (unsigned char*) &obj->member.appHeader
                                + obj->member.argument.blockCount
                                        * Boot_Buffer_Size;

                for (int i = 0; i < Boot_Buffer_Size; i++)
                    *(appHeaderPtr + i) = data.byte[i];
            }

            if (obj->member.argument.byteCount < Boot_Buffer_Size)
            {
                for (int i = 0; i < obj->member.message.download.member.size;
                        i++)
                    obj->member.message.download.byte[i] =
                            data.byte[obj->member.argument.byteCount + i];
                obj->member.argument.byteCount +=
                        obj->member.message.download.member.size;
                obj->member.message.reloaded = 1;
            }
            else
            {
                ++obj->member.argument.blockCount;
                obj->member.argument.byteCount = 0;
                return NO_ERROR;
            }

            obj->member.argument.accumulation = obj->member.argument.blockCount
                    * Boot_Buffer_Size + obj->member.argument.byteCount;
        }
        else
        {
            if (obj->member.transportFlag.bit.failed)
            {
                if (obj->member.message.state.byte.bootState == BOOT_ERROR)
                    return (EnumErrorCode) obj->member.message.state.byte.subState;
                else
                    return UNKNOWN_ERROR;
            }
            else if (obj->member.transportFlag.bit.complete)
                return NO_ERROR;
            else
                obj->member.message.reloaded = 2;
        }
    }
    return PENDING;
}
/**************************************************************************************************
 Function Name:
 void Boot_Transport_Init(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 None.
 Comment:
 Init boot transport mode function.
 **************************************************************************************************/
void Boot_Transport_Init(BootInternalObj *obj)
{
    obj->member.transportFlag.all = 0;
    obj->member.message.completeKey = 0;
    obj->member.message.initKey = 0;
    obj->member.message.protectKey = 0;
    obj->member.message.reloaded = 0;
    obj->member.message.state.stateCode = 0;
    Reset_Boot_Argument(obj);
}
/**************************************************************************************************
 Function Name:
 EnumBootCommand Boot_Transport_State_Machine(BootInternalObj *obj, unsigned char ack)
 Input:
 *obj        - Object pointer of boot internal task.
 ack         - Acknowledge of the transport mode. 1 = acknowledged, 0 = no acknowledge.
 Output:
 return      - Boot command.
 Comment:
 Boot transport mode state machine function.
 **************************************************************************************************/
EnumBootCommand Boot_Transport_State_Machine(BootInternalObj *obj,
                                             unsigned char ack)
{
    if (obj->member.message.reloaded)
    {
        switch (obj->member.message.state.byte.bootState)
        {
        case BOOT_LOCKED:
            if (!obj->member.transportFlag.bit.protect)
            {
                obj->member.message.protectKey = BOOT_UNLOCK_KEY;
                obj->member.transportFlag.bit.protect = 1;
                return BOOT_PROTECTION;
            }
            else if (ack)
            {
                if (obj->member.transportFlag.bit.state)
                {
                    obj->member.transportFlag.bit.complete = 1;
                    obj->member.message.reloaded = 0;
                }
                else
                {
                    obj->member.message.state.byte.bootState = BOOT_UNLOCKED;
                    obj->member.message.state.byte.subState = PREPARING;
                }
            }
            break;
        case BOOT_UNLOCKED:
            return Boot_Transport_Unlock_State_Machine(obj, ack);
//                break;
        case BOOT_ERROR:
            obj->member.transportFlag.bit.failed = 1;
            obj->member.message.reloaded = 0;

            if (!obj->member.transportFlag.bit.recovery)
            {
                obj->member.message.initKey = BOOT_RECOVER_KEY;
                obj->member.transportFlag.bit.recovery = 1;
                return BOOT_RECOVERY;
            }
            else if (ack)
                obj->member.message.state.stateCode = (BOOT_RECOVERING << 8);
            break;
        case BOOT_RECOVERING:
            obj->member.message.reloaded = 0;
            break;
        }
    }
    return BOOT_NONE_COMMAND;
}
/**************************************************************************************************
 Function Name:
 EnumBootCommand Boot_Transport_Unlock_State_Machine(BootInternalObj *obj, unsigned char ack)
 Input:
 *obj        - Object pointer of boot internal task.
 ack         - Acknowledge of the transport mode. 1 = acknowledged, 0 = no acknowledge.
 Output:
 return      - Boot command.
 Comment:
 Boot transport mode unlocked state machine function. This function will return the boot command
 when the state machine has processed the command.
 **************************************************************************************************/
EnumBootCommand Boot_Transport_Unlock_State_Machine(BootInternalObj *obj,
                                                    unsigned char ack)
{
    switch (obj->member.message.state.byte.subState)
    {
    case PREPARING:
        if (!obj->member.transportFlag.bit.init)
        {
            obj->member.message.initKey = BOOT_INIT_KEY;
            obj->member.transportFlag.bit.init = 1;
            return BOOT_INIT;
        }
        else if (ack)
            obj->member.message.state.byte.subState = READY;
        break;
    case READY:
        if (obj->member.message.reloaded == 1)
        {
            obj->member.message.state.byte.subState = PROCESSING;
            obj->member.transportFlag.bit.download = 1;
            return BOOT_DOWNLOAD;
        }
        else if (obj->member.message.reloaded == 2)
        {
            obj->member.message.completeKey = BOOT_COMPLETE_KEY;
            obj->member.message.state.byte.subState = VERIFYING;
            return BOOT_COMPLETED;
        }
//        else if (obj->member.message.reloaded == 3) //For transport erase OK
//        {
//            obj->member.message.reloaded = 0;
//        }
        break;
    case PROCESSING:
        if (ack)
        {
            obj->member.message.state.byte.subState = READY;
            obj->member.message.reloaded = 0;
        }
        break;
    case VERIFYING:
    case VERIFIED:
        if (ack)
        {
            obj->member.transportFlag.bit.state = 1;
            return BOOT_STATE;
        }
        break;
    }
    return BOOT_NONE_COMMAND;
}
/**************************************************************************************************
 Function Name:
 void TiByte_Convert(void *input, void *output, unsigned long length)
 Input:
 *input          - Data pointer of input raw data.
 *output         - Data pointer of output buffer.
 length          - Number TiBytes of output buffer.
 Output:
 None.
 Comment:
 TiByte converter function. This function will separate input raw data to output data buffer.
 **************************************************************************************************/
void TiByte_Convert(void *input, void *output, unsigned long length)
{
    for (int i = 0; i < length; i++)
    {
        *((char*) output + 2 * i) = *((char*) input + i) & 0xFF;
        *((char*) output + 2 * i + 1) = (*((char*) input + i) >> 8) & 0xFF;
    }
}
/**************************************************************************************************
 Function Name:
 void Correct_Mix_Header_Information(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 None.
 Comment:
 Correct Mix Header information function. This function will fix incorrect 16 and 32 bits data.
 **************************************************************************************************/
void Correct_Mix_Header_Information(BootInternalObj *obj)
{
    // Signature
    obj->member.mixHeader.signature.headerCrc = Correct_Header_Data(
            obj->member.mixHeader.signature.headerCrc);

    obj->member.mixHeader.signature.imageCrc = Correct_Header_Data(
            obj->member.mixHeader.signature.imageCrc);

    obj->member.mixHeader.signature.imageSize = Correct_Header_Data(
            obj->member.mixHeader.signature.imageSize);

    // First index
    obj->member.mixHeader.firstIndex.programFunction.word = Correct_Header_Data(
            obj->member.mixHeader.firstIndex.programFunction.word);

    obj->member.mixHeader.firstIndex.programSize = Correct_Header_Data(
            obj->member.mixHeader.firstIndex.programSize);

    // Second index
    obj->member.mixHeader.secondIndex.programFunction.word =
            Correct_Header_Data(
                    obj->member.mixHeader.secondIndex.programFunction.word);

    obj->member.mixHeader.secondIndex.programSize = Correct_Header_Data(
            obj->member.mixHeader.secondIndex.programSize);

    // Third index
    obj->member.mixHeader.thirdIndex.programFunction.word = Correct_Header_Data(
            obj->member.mixHeader.thirdIndex.programFunction.word);

    obj->member.mixHeader.thirdIndex.programSize = Correct_Header_Data(
            obj->member.mixHeader.thirdIndex.programSize);

    // Fourth index
    obj->member.mixHeader.fourthIndex.programFunction.word =
            Correct_Header_Data(
                    obj->member.mixHeader.fourthIndex.programFunction.word);

    obj->member.mixHeader.fourthIndex.programSize = Correct_Header_Data(
            obj->member.mixHeader.fourthIndex.programSize);

    // Fifth index
    obj->member.mixHeader.fifthIndex.programFunction.word = Correct_Header_Data(
            obj->member.mixHeader.fifthIndex.programFunction.word);

    obj->member.mixHeader.fifthIndex.programSize = Correct_Header_Data(
            obj->member.mixHeader.fifthIndex.programSize);
}
/**************************************************************************************************
 Function Name:
 void Correct_App_Header_Information(BootInternalObj *obj)
 Input:
 *obj        - Object pointer of boot internal task.
 Output:
 None.
 Comment:
 Correct App Header information function. This function will fix incorrect 16 and 32 bits data.
 **************************************************************************************************/
void Correct_App_Header_Information(StdAppHeaderType *headerPtr)
{
    // Signature
    headerPtr->signature.headerCrc = Correct_Header_Data(
            headerPtr->signature.headerCrc);
    headerPtr->signature.imageCrc = Correct_Header_Data(
            headerPtr->signature.imageCrc);

    // Header
    headerPtr->header.programFunction.word = Correct_Header_Data(
            headerPtr->header.programFunction.word);
    headerPtr->header.programSize = Correct_Header_Data(
            headerPtr->header.programSize);

    // Compliant Table
    headerPtr->compliantTableAddress = Correct_Header_Data(
            headerPtr->compliantTableAddress);
}
/**************************************************************************************************
 Function Name:
 unsigned long Correct_Header_Data(unsigned long long raw)
 Input:
 raw         - 16 or 32 bits raw data of header.
 Output:
 return      - Corrected 16 or 32 bits data.
 Comment:
 Correct header data function. This function will fix 16 or 32 bits data of header.
 **************************************************************************************************/
unsigned long Correct_Header_Data(unsigned long long raw)
{
    return (raw & 0xFF) | ((raw >> 8) & 0xFF00) | ((raw >> 16) & 0xFF0000)
            | ((raw >> 24) & 0xFF000000);
}
#endif
//---------------- END LINE -----------------------------------------------------------------------
