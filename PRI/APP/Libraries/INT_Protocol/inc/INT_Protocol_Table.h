/**************************************************************************************************
File Name: INT_Protocol_Table.h
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

#ifndef __INVINTERCOMM_TABLE_H__
#define __INVINTERCOMM_TABLE_H__

/* Includes */
#include "INT_Protocol.h"

/* Macro definitions */

// Statement Replacement

/* Type definitions */
// Enumeration

// Struct & Union

/* Global function prototypes */

/* Global data declarations */
extern CommandArrayType intCommand;
extern CommandArrayType intPage0Command;
#endif
