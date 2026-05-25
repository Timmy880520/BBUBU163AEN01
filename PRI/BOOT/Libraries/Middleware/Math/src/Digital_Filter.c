/**************************************************************************************************
File Name: Digital_Filter.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description:
    Digital filter code library. This library includes digital filters as follows:

1. Simple filter.
2. Move average filter.
3. First order low pass filter.
4. Second order low pass filter.
5. Three level cascade low pass filter.
6. Average rectified calculation for sine input.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    04/08/2020 Watch Lee        1. version 1.0.
    11/10/2020 Watch Lee        1. version 1.1.
                                2. modify average rectified calculation to add default output type.
    05/13/2021 Watch Lee        1. version 1.2.
                                2. modify Simple_Filter_Calculation function.

**************************************************************************************************/

/* Includes */
#include "..\inc\Digital_Filter.h"

/**************************************************************************************************
Function Name:
    long Simple_Filter_Calculation(SimpleFilterObj *obj, short input)
Input:
    *obj    - Object pointer of simple filter.
    input   - Input data. The data range: -32768 ~ 32767
Output:
    return  - Calculation result.
Comment:
    Simple filter function. This function uses backward Euler's method with power 2 order to
implement the filter. The difference equation of the filter is as follows:

y[n] = (a0 * y[n-1] + u[n]) / 2^n, a0 = 2^n - 1, n(exponent) < 16

The cut-off frequency = 1 / (2 * pi * Ts * a0), Ts = sample period
**************************************************************************************************/
long Simple_Filter_Calculation(SimpleFilterObj *obj, short input)
{
//    obj->member.coeff.yn1 = ((obj->member.coeff.yn1 * obj->member.coeff.a0) >> obj->member.exponent) + input;
    obj->member.coeff.yn1 = obj->member.coeff.yn1 - obj->member.result + input;
    return obj->member.result = obj->member.coeff.yn1 >> obj->member.exponent;
}
/**************************************************************************************************
Function Name:
    void Simple_Filter_Reset(SimpleFilterObj *obj)
Input:
    *obj    - Object pointer of simple filter.
Output:
    None
Comment:
    Simple filter reset function.
**************************************************************************************************/
void Simple_Filter_Reset(SimpleFilterObj *obj)
{
    obj->member.coeff.yn1 = 0;
    obj->member.result = 0;
}
/**************************************************************************************************
Function Name:
    long Move_Average_Calculation(MoveAvgObj *obj, short input)
Input:
    *obj    - Object pointer of moving average filter.
    input   - Input data. The data range: -32768 ~ 32767
Output:
    return  - Calculation result.
Comment:
    Moving average filter function. This function uses power 2 order/window size and the maximum
number is 65536. The difference equation of the filter is as follows:

y[n] = (u[n] + u[n-1] + u[n-2] +... + u[n-m-1]) / m, m <= 65536

The order/window size(m) is the power of 2(1, 2, 4, 8, ...).
**************************************************************************************************/
long Move_Average_Calculation(MoveAvgObj *obj, short input)
{
    long i, calTmp = 0;

    obj->member.index = (obj->member.index + 1) & (obj->member.order - 1);
    obj->member.coeff.un[obj->member.index] = input;

    for (i = 0; i < obj->member.order; i++)
        calTmp += obj->member.coeff.un[i];
    obj->member.coeff.yn = calTmp;
    calTmp = *(long*)&obj->member.eFactor >> 23;        // Extract the exponent of eFactor
    calTmp = (calTmp >= 0x6F) ? calTmp - 0x6F : 0;      // Remove the offset
    obj->member.coeff.yn >>= calTmp;                    // Divide the power 2 order
    return obj->member.result = obj->member.coeff.yn;
}
/**************************************************************************************************
Function Name:
    void Move_Average_Reset(MoveAvgObj *obj)
Input:
    *obj    - Object pointer of moving average filter.
Output:
    None.
Comment:
    Moving average filter reset function.
**************************************************************************************************/
void Move_Average_Reset(MoveAvgObj *obj)
{
    long i;

    for (i = 0; i < obj->member.order; i++)
        obj->member.coeff.un[i] = 0;
    obj->member.coeff.yn = 0;
    obj->member.index = 0;
    obj->member.result = 0;
}
/**************************************************************************************************
Function Name:
    long DLPF_1st_Calculation(DlpFilterObj *obj, short input)
Input:
    *obj    - Object pointer of digital filter.
    input   - Input data. The data range: -32768 ~ 32767
Output:
    return  - Calculation result.
Comment:
    First order digital low pass filter(1st DLPF) program. This function uses the bilinear
transform to implement the filter. The difference equation of the filter is as follows:

y[n] = b0 * (u[n] + u[n-1]) + a0 * y[n-1]

The gain(a0 and b0) of the filter uses Q14 format.
**************************************************************************************************/
long DLPF_1st_Calculation(DlpFilterObj *obj, short input)
{
    long calTmp;

    obj->member.coeff.un = input;
    calTmp = ((obj->member.coeff.yn1 >> 14) * obj->member.coeff.a0) +
             (((obj->member.coeff.yn1 & 0x3FFF) * obj->member.coeff.a0) >> 14);

    obj->member.coeff.yn = obj->member.coeff.b0 * (obj->member.coeff.un + obj->member.coeff.un1) + calTmp;
    obj->member.coeff.un1 = obj->member.coeff.un;
    obj->member.coeff.yn1 = obj->member.coeff.yn;
    obj->member.result = obj->member.coeff.yn >> 14;
    return obj->member.result;
}
/**************************************************************************************************
Function Name:
    long DLPF_2nd_Calculation(DlpFilterObj *obj, short input)
Input:
    *obj    - Object pointer of digital filter.
    input   - Input data. The data range: -32768 ~ 32767
Output:
    return  - Calculation result.
Comment:
    Second order digital low pass filter(2nd DLPF) program. This function uses the bilinear
transform to implement the filter. The difference equation of the filter is as follows:

y[n] = b0 * (u[n] + 2 * u[n-1] + u[n-2]) + a1 * y[n-1] - a0 * y[n-2]

The gain(a1, a0, and b0) of the filter uses Q14 format.
**************************************************************************************************/
long DLPF_2nd_Calculation(DlpFilterObj *obj, short input)
{
    long calTmp1, calTmp2, calTmp3;

    obj->member.coeff.un = input;
    calTmp1 = obj->member.coeff.b0 * (obj->member.coeff.un + 2 * obj->member.coeff.un1 + obj->member.coeff.un2);

    calTmp2 = ((obj->member.coeff.yn1 >> 14) * obj->member.coeff.a1) +
              (((obj->member.coeff.yn1 & 0x3FFF) * obj->member.coeff.a1) >> 14);

    calTmp3 = ((obj->member.coeff.yn2 >> 14) * obj->member.coeff.a0) +
              (((obj->member.coeff.yn2 & 0x3FFF) * obj->member.coeff.a0) >> 14);

    obj->member.coeff.yn = calTmp1 + calTmp2 - calTmp3;
    obj->member.coeff.un2 = obj->member.coeff.un1;
    obj->member.coeff.un1 = obj->member.coeff.un;
    obj->member.coeff.yn2 = obj->member.coeff.yn1;
    obj->member.coeff.yn1 = obj->member.coeff.yn;
    obj->member.result = obj->member.coeff.yn >> 14;
    return obj->member.result;
}
/**************************************************************************************************
Function Name:
    void DLPF_Reset(DlpFilterObj *obj)
Input:
    *obj    - Object pointer of digital filter.
Output:
    None.
Comment:
    First order digital low pass filter reset function.
**************************************************************************************************/
void DLPF_Reset(DlpFilterObj *obj)
{
    obj->member.coeff.un = 0;
    obj->member.coeff.un1 = 0;
    obj->member.coeff.un2 = 0;
    obj->member.coeff.yn = 0;
    obj->member.coeff.yn1 = 0;
    obj->member.coeff.yn2 = 0;
    obj->member.result = 0;
}
/**************************************************************************************************
Function Name:
    long Cascade3_DLPF_Calculation(CascadeFilterObj *obj, short input)
Input:
    *obj    - Object pointer of cascade digital filter.
    input   - Input data. The data range: -32768 ~ 32767
Output:
    None.
Comment:
    Cascade 3 level digital low pass filter(Cascade3 DLPF) program. This function cascades 3
1st DLPFs to implement the filter.
**************************************************************************************************/
long Cascade3_DLPF_Calculation(CascadeFilterObj *obj, short input)
{
    obj->member.filterLv1.func.calculate(&obj->member.filterLv1, input);
    obj->member.filterLv2.func.calculate(&obj->member.filterLv2, obj->member.filterLv1.member.result);
    obj->member.filterLv3.func.calculate(&obj->member.filterLv3, obj->member.filterLv2.member.result);
    obj->member.result = obj->member.filterLv3.member.result;
    return obj->member.result;
}
/**************************************************************************************************
Function Name:
    void Cascade3_DLPF_Reset(CascadeFilterObj *obj)
Input:
    *obj    - Object pointer of cascade digital filter.
Output:
    None.
Comment:
    Cascade 3 level digital low pass filter reset function.
**************************************************************************************************/
void Cascade3_DLPF_Reset(CascadeFilterObj *obj)
{
    obj->member.filterLv1.member.coeff.un = 0;
    obj->member.filterLv1.member.coeff.un1 = 0;
    obj->member.filterLv1.member.coeff.un2 = 0;
    obj->member.filterLv1.member.coeff.yn = 0;
    obj->member.filterLv1.member.coeff.yn1 = 0;
    obj->member.filterLv1.member.coeff.yn2 = 0;
    obj->member.filterLv1.member.result = 0;

    obj->member.filterLv2.member.coeff.un = 0;
    obj->member.filterLv2.member.coeff.un1 = 0;
    obj->member.filterLv2.member.coeff.un2 = 0;
    obj->member.filterLv2.member.coeff.yn = 0;
    obj->member.filterLv2.member.coeff.yn1 = 0;
    obj->member.filterLv2.member.coeff.yn2 = 0;
    obj->member.filterLv2.member.result = 0;

    obj->member.filterLv3.member.coeff.un = 0;
    obj->member.filterLv3.member.coeff.un1 = 0;
    obj->member.filterLv3.member.coeff.un2 = 0;
    obj->member.filterLv3.member.coeff.yn = 0;
    obj->member.filterLv3.member.coeff.yn1 = 0;
    obj->member.filterLv3.member.coeff.yn2 = 0;
    obj->member.filterLv3.member.result = 0;

    obj->member.result = 0;
}
/************************************************************************************************
Function Name:
    long Sine_Average_Rectified_Calculation(AvgRectValueObj *obj, short input)
Input:
    *obj    - Object pointer of average rectified calculation.
    input   - Input data. The data range: -32768 ~ 32767
Output:
    return   - Calculation result.
Comment:
    Average rectified calculation function for sine input. This function uses 2nd DLPF to
calculate the mean value and approximate RMS value for sine input. The input will be converted
to absolute value before the calculation.
************************************************************************************************/
long Sine_Average_Rectified_Calculation(AvgRectObj *obj, short input)
{
    // Use digital low pass filter to calculate mean value
    obj->member.rawMean = (input >= 0) ? obj->member.filter.func.calculate(&obj->member.filter, input) :
                                         obj->member.filter.func.calculate(&obj->member.filter, -input);

    obj->member.rawRms = (obj->member.rawMean * 9099) >> 13;        // RMS = pi * sqrt(2) / 4 * Mean = 1.1107 * Mean
    obj->member.rawPeak = (obj->member.rawMean * 12868) >> 13;      // Peak = pi / 2 * Mean = 1.5708 * Mean

    // Calculate scaled value
    long calTmp = ((obj->member.rawMean * obj->member.gain) >> 12) + obj->member.offset;    // Q15

    calTmp = (calTmp * obj->member.scale) >> 15;
    obj->member.scaleMean = (calTmp > 0) ? calTmp : 0;
    obj->member.scaleRms = (obj->member.scaleMean * 9099) >> 13;
    obj->member.scalePeak = (obj->member.scaleMean * 12868) >> 13;

    switch (obj->member.outputType)
    {
        case AVG_RECT_SCALE_RMS:
        default:
            return obj->member.scaleRms;
        case AVG_RECT_SCALE_MEAN:
            return obj->member.scaleMean;
        case AVG_RECT_SCALE_PEAK:
            return obj->member.scalePeak;
        case AVG_RECT_RAW_RMS:
            return obj->member.rawRms;
        case AVG_RECT_RAW_MEAN:
            return obj->member.rawMean;
        case AVG_RECT_RAW_PEAK:
            return obj->member.rawPeak;
    }
}
/**************************************************************************************************
Function Name:
    void Sine_Average_Rectified_Reset(AvgRectValueObj *obj)
Input:
    *obj    - Object pointer of average rectified calculation.
Output:
    None.
Comment:
    Average rectified reset function.
**************************************************************************************************/
void Sine_Average_Rectified_Reset(AvgRectObj *obj)
{
    obj->member.filter.member.coeff.un = 0;
    obj->member.filter.member.coeff.un1 = 0;
    obj->member.filter.member.coeff.un2 = 0;
    obj->member.filter.member.coeff.yn = 0;
    obj->member.filter.member.coeff.yn1 = 0;
    obj->member.filter.member.coeff.yn2 = 0;
    obj->member.filter.member.result = 0;

    obj->member.rawMean = 0;
    obj->member.rawRms = 0;
    obj->member.rawPeak = 0;
    obj->member.scaleMean = 0;
    obj->member.scaleRms = 0;
    obj->member.scalePeak = 0;
}
//---------------- END LINE -----------------------------------------------------------------------
