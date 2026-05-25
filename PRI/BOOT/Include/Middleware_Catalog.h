/**************************************************************************************************
File Name: Middleware_Catalog.h
Global Data:
    Name                      Type               Description
    ------------------------- ------------------ -------------------------------------------------

Description: 
    Middleware catalog file. This header file shall be included in the project before using the
middleware components. 

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    05/13/2021 Watch Lee        1. version 1.0.

**************************************************************************************************/

#ifndef __MIDDLEWARE_CATALOG_H__
#define __MIDDLEWARE_CATALOG_H__

/* Preprocessor */
// Set 0 to exclude the component
#define Add_In_ISP_Task         0
#define Add_In_C28x_ISP_Task    1
#define Add_In_Common           1
#define Add_In_INT_Protocol     1
#define Add_In_INT_CAN          0
#define Add_In_Checksum         1
#define Add_In_Math             0
#define Add_In_Legacy           1

/* Includes */
#if Add_In_ISP_Task==1
#include "..\Libraries\Middleware\BootLib\inc\Boot_Internal_Task.h"
#include "..\Libraries\Middleware\BootLib\inc\Data_Structure.h"
#endif

#if Add_In_C28x_ISP_Task==1
#include "..\Libraries\Middleware\BootLib\inc\c28x_Boot_Internal_Task.h"
#include "..\Libraries\Middleware\BootLib\inc\c28x_data_structure.h"
#endif

#if Add_In_Common==1
#include "..\Libraries\Middleware\Common\inc\CircularBuffer.h"
#endif

#if Add_In_INT_Protocol==1
#include "..\Libraries\Middleware\INT_Protocol\inc\INT_Protocol.h"
#include "..\Libraries\Middleware\INT_Protocol\inc\SLIP.h"
#include "..\Libraries\Middleware\INT_Protocol\inc\c28x_intp_data_access.h"
#include "..\Libraries\Middleware\INT_Protocol\inc\INTP_Table_Template.h"
#endif

#if Add_In_INT_CAN==1
#include "..\Libraries\Middleware\INT_CAN\inc\INT_CAN.h"
#include "..\Libraries\Middleware\INT_CAN\inc\c28x_int_can_data_access.h"
#include "..\Libraries\Middleware\INT_CAN\inc\INT_CAN_Table_Template.h"
#endif

#if Add_In_Checksum==1
#include "..\Libraries\Middleware\Checksum\inc\Checksum.h"
#endif

#if Add_In_Math==1
#include "..\Libraries\Middleware\Math\inc\Digital_Filter.h"
#include "..\Libraries\Middleware\Math\inc\Sine_Process.h"
#endif

#if Add_In_Legacy==1
#include "..\Libraries\Middleware\Legacy\inc\Univ_Lib.h"
#include "..\Libraries\Middleware\Legacy\inc\AC_Math_Lib.h"
#endif

#endif
