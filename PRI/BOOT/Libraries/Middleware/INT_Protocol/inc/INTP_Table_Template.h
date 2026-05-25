/**************************************************************************************************
File Name: INTP_Table_Template.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    intCommand           CommandArrayType   Command list of INT protocol.
Description:
    Header file of INT_Protocol_Table.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    12/26/2019 Watch Lee        1. support version 1.0.

**************************************************************************************************/

#ifndef __INTP_TABLE_TEMPLATE_H__
#define __INTP_TABLE_TEMPLATE_H__

/* Includes */
#include "INT_Protocol.h"

/* Macro definitions */

// Statement Replacement

/* Type definitions */
// Enumeration

// Struct & Union

/* Global function prototypes */

/* Global data declarations */
extern CommandArrayType intpPriCommand, intpSecCommand;
extern unsigned short uartCalibrationMode;
extern unsigned short pfcCalibrationCtrl;
#endif
