/**************************************************************************************************
File Name: c28x_intp_data_access.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of c28x_intp_data_access.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    12/30/2019 Watch Lee        1. support version 1.0.

**************************************************************************************************/

#ifndef __C28X_INTP_DATA_ACCESS_H__
#define __C28X_INTP_DATA_ACCESS_H__

/* Includes */
#include "INT_Protocol.h"

/* Macro definitions */

/* Type definitions */

/* Global function prototypes */
unsigned char C28x_Read_INTP_Packet_Data(IntProtocolObj *obj, DataFrameStr data);
unsigned char C28x_Read_INTP_Not_Packet_Data(IntProtocolObj *obj, DataFrameStr data);
unsigned char C28x_Write_INTP_Packet_Data(IntProtocolObj *obj, DataPackStr data);
unsigned char C28x_Write_INTP_Not_Packet_Data(IntProtocolObj *obj, DataPackStr data);

/* Global data declarations */

#endif
