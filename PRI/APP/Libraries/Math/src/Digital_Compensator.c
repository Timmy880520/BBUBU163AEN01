/**************************************************************************************************
File Name: Digital_Compensator.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description:
    Digital compensator code library. This library includes digital compensators as follows:

1. Type III compensator.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    03/08/2022 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "..\inc\Digital_Compensator.h"

/* Local function prototypes */

/* Data definitions */
float Float_Qn_Divisor[16] =
{
              1.0f,          5.e-1f,         2.5e-1f,         1.25e-1f,
          6.25e-2f,       3.125e-2f,      1.5625e-2f,       7.8125e-3f,
       3.90625e-3f,    1.953125e-3f,    9.765625e-4f,    4.8828125e-4f,
    2.44140625e-4f, 1.220703125e-4f, 6.103515625e-5f, 3.0517578125e-5f
};

/**************************************************************************************************
Function Name:
    long Float_Type_III_Compensation(CompensatorObj *obj, long reference, long feedback)
Input:
    *obj        - Object pointer of Compensator.
    reference   - Reference data of the compensation.
    feedback    - Feedback data of the compensation.
Output:
    return      - Output data. Q format of the integer data is defined by qNotation.
Comment:
    Digital type III compensation. This compensator has be separated to a high stop filter and
a PI controller for implementation of the cascade mode.

The difference equation of the high stop filter is as follows:

y[n] = b0 * u[n] + b1 * u[n-1] + b2 * u[n-2] - a1 * y[n-1] - a2 * y[n-2]

The control diagram of the PI controller is as follows:

    pYn = kp * error, error = ref - fdb
    iYn = ki *error + iYn1 - kc * satError, iYn1 = iYn of previous period, satError = output - piYn
    piYn = pYn + iYn
    output = min <= piYn <= max

                   .------------>[kp]-------------.
                   |                        pYn   |
                   |                            + v  piYn         max
    ref --->+(x)---*--->[ki]--->+(x)--->[¡ì]---->+(x)------*---->[/]----*----[ka]----> output
            - ^  error          + ^         iYn           |   min      |
              |                   |                       v -          |
             fdb                  .---------[kc]<--------(x)+<--------.
                                                  satError

**************************************************************************************************/
long Float_Type_III_Compensation(CompensatorObj *obj, long reference, long feedback)
{
    obj->member.reference = reference;
    obj->member.feedback = feedback;
    obj->member.error = obj->member.reference - obj->member.feedback;

    // High stop filter process
    obj->member.hsFilter.un = (float)obj->member.error * Float_Qn_Divisor[obj->member.qNotation & 0x0F];
    obj->member.hsFilter.yn = obj->member.hsFilter.b0 * obj->member.hsFilter.un +
                              obj->member.hsFilter.b1 * obj->member.hsFilter.un1 +
                              obj->member.hsFilter.b2 * obj->member.hsFilter.un2 -
                              obj->member.hsFilter.a1 * obj->member.hsFilter.yn1 -
                              obj->member.hsFilter.a2 * obj->member.hsFilter.yn2;

    obj->member.hsFilter.un2 = obj->member.hsFilter.un1;
    obj->member.hsFilter.un1 = obj->member.hsFilter.un;
    obj->member.hsFilter.yn2 = obj->member.hsFilter.yn1;
    obj->member.hsFilter.yn1 = obj->member.hsFilter.yn;

    // PI compensation process
    obj->member.piControl.error = obj->member.hsFilter.yn;
    obj->member.piControl.pYn = obj->member.piControl.kp * obj->member.piControl.error;
    obj->member.piControl.iYn = obj->member.piControl.ki * obj->member.piControl.error + obj->member.piControl.iYn1;
    obj->member.piControl.iYn += obj->member.piControl.kc * obj->member.piControl.satError;
/*
    // Unnecessary if anti wind-up mechanism is executing correctly.
    if (obj->member.piControl.iYn > 1.0e4f)
        obj->member.piControl.iYn = 1.0e4f;
    else if (obj->member.piControl.iYn < -1.0e4f)
        obj->member.piControl.iYn = -1.0e4f;
*/
    obj->member.piControl.iYn1 = obj->member.piControl.iYn;
    obj->member.piControl.piYn = obj->member.piControl.pYn + obj->member.piControl.iYn;

    if (obj->member.piControl.piYn > obj->member.piControl.max)
        obj->member.piControl.output = obj->member.piControl.max;
    else if (obj->member.piControl.piYn < obj->member.piControl.min)
        obj->member.piControl.output = obj->member.piControl.min;
    else
        obj->member.piControl.output = obj->member.piControl.piYn;
    obj->member.piControl.satError = obj->member.piControl.output - obj->member.piControl.piYn;
    obj->member.piControl.output *= obj->member.piControl.ka;

    // Float to integer conversion
    obj->member.output = (long)(obj->member.piControl.output * (float)(1 << obj->member.qNotation));

    if (obj->member.output > obj->member.maximum)
        obj->member.output = obj->member.maximum;
    else if (obj->member.output < obj->member.minimum)
        obj->member.output = obj->member.minimum;
    return obj->member.output;
}
/**************************************************************************************************
Function Name:
    void Float_Type_III_Compensation_Reset(CompensatorObj *obj)
Input:
    *obj        - Object pointer of Compensator.
Output:
    None.
Comment:
    Digital type III compensation reset function.
**************************************************************************************************/
void Float_Type_III_Compensation_Reset(CompensatorObj *obj)
{
    // Integer parameters
    obj->member.reference = 0;
    obj->member.feedback = 0;
    obj->member.error = 0;
    obj->member.output = 0;

    // High stop filter parameters
    obj->member.hsFilter.un = 0.0f;
    obj->member.hsFilter.un1 = 0.0f;
    obj->member.hsFilter.un2 = 0.0f;
    obj->member.hsFilter.yn = 0.0f;
    obj->member.hsFilter.yn1 = 0.0f;
    obj->member.hsFilter.yn2 = 0.0f;

    // PI compensation parameters
    obj->member.piControl.error = 0.0f;
    obj->member.piControl.pYn = 0.0f;
    obj->member.piControl.iYn = 0.0f;
    obj->member.piControl.iYn1 = 0.0f;
    obj->member.piControl.piYn = 0.0f;
    obj->member.piControl.satError = 0.0f;
    obj->member.piControl.output = 0.0f;
}
//---------------- END LINE -----------------------------------------------------------------------
