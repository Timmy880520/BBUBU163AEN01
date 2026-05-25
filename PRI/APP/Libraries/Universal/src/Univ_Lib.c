/**************************************************************************************************
File Name: Univ_Lib.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description: 
    Universal library program, include:
    1. Hexadecimal to Decimal conversion function
    2. Square root calculation function
    3. Counter & debounce function
    4. Timer function
    5. 16bit Data table lookup function
    6. 12bit ADC data calibration
    7. 12bit 2nd DLPF calculation
    8. Check signal state function
    9. Check event state function
    10. Decimal data calibration

==================================================================================================
History:
    Date        Author          Description Of Change
    ----------- --------------- -----------------------------------------------------------------
    07/22/2013  Watch Lee       Ver.1.0
    06/02/2017  Watch Lee       Update to Ver.1.1 for DLPF_Calculation.
    06/21/2017  Watch Lee       Update to Ver.1.2 for signal and event state function.
    02/22/2018  Watch Lee       Update to Ver.2.0 for standard coding style.
    03/25/2020  Fred Huang      Add CMA_Calculation and In_Range_Check
    03/07/2021  Fred Huang      1. Modify CMA_Calculation to meet new CMAtoRealPkt
                                2. Modify Real_Value_Calculation to meet new CMAtoRealPkt
    03/11/2021  Fred Huang      1. Add Calibration_Calculation function
                                2. Modify calibration calculation inside CMA_Calculation and Real_Value_Calculation
    03/19/2021  Fred Huang      1. Modify three range check function to meet new ProtectionPkt

**************************************************************************************************/
/* Includes */ 
#include "Univ_Lib.h"

/**************************************************************************************************
Function Name:
    void ADC_Data_Conversion(AdcVal *data, long stepSize, unsigned long accuracy)
Input:
    *data     - The pointer points to an ADC data structure.
    stepSize  - Step size of data conversion, the value = (Full Scale of ADC * 100163) >> 12. The 
                maximum value = (2^31 - 2^12 - 1) = 524287.
                Full Scale of ADC = Integer of Full Scale * 10, ex. 100V = 100 * 10 = 1000.
    accuracy  - The divisor for the integer calculation of the decimal conversion. The maximum 
                value = 10^(int(log(2^16 / 67))) = 100.
Output:
    None.
Comment:
    ADC Hexadecimal to decimal data conversion function.
**************************************************************************************************/
void ADC_Data_Conversion(AdcVal *data, long stepSize, unsigned long accuracy)
{
    long tmp1, tmp2;
    
    tmp1 = ((*data->valPtr) * stepSize);
    tmp2 = (tmp1 >> 16) * (long)accuracy * 67;
    tmp1 = (tmp2 + (((tmp1 & 0xFFFF) * (long)accuracy * 67) >> 16)) >> 10;
    data->decVal = (long)tmp1;
    
    if (accuracy == 10) 
        data->decInt = (short)(((long)data->decVal * 3277) >> 15);
    else if (accuracy == 100) 
        data->decInt = (short)(((long)data->decVal * 5243) >> 19);		
}

/**************************************************************************************************
Function Name:
    long Sqrt_Calculation(long data)
Input:
    data      - Input data.
Output:
    return    - Square root of input data.
Comment:
    Square root calculation function
**************************************************************************************************/
long Sqrt_Calculation(long data)
{
    long opr = 0, bit = 0x40000000;
    
    while (bit > data) 
        bit >>= 2;
    
    while (bit != 0)
    {
        if (data >= opr + bit)
        {
            data -= opr + bit;
            opr = (opr >> 1) + bit;
        }
        else 
            opr >>= 1;
        bit >>= 2;
    }
    return opr;
}
/**************************************************************************************************
Function Name:
	char Software_Counter(TmrPkg *pkg, char flag)
Input:
	*pkg      - The pointer points to a timer structure which is counted up by this 
                function to detect a tick count condition.
	flag      - Count flag. 1 = count up, 0 = clear count.
Output:
	return    - Result. 1 = ticked, 0 = not tick.
Comment:
	Software counter/debounce function.
**************************************************************************************************/
char Software_Counter(TmrPkg *pkg, char flag)
{
    if (pkg->reg.bits.cnt == 0) 
        pkg->reg.bits.flg = 0;

    if (pkg->reg.bits.flg == 0)
    {
        if (flag > 0)
        {
            if (++pkg->reg.bits.cnt >= pkg->cmp)
            {
                pkg->reg.bits.cnt = 0;
                pkg->reg.bits.flg = 1; 
            }
        }
        else 
            pkg->reg.bits.cnt = 0;
    }
    else if (flag == 0) 
        pkg->reg.bits.flg = 0;
    
    return (char)pkg->reg.bits.flg;
}
/**************************************************************************************************
Function Name:
    unsigned char Software_Timer(TmrPkg *pkg)
Input:
    *pkg      - The pointer points to a timer structure which is counted up by this 
                function to detect a timeout condition.
Output:
    return    - Result. 1 = a timeout condition is detected, 0 = no timeout condition.
Comment:
    Software timer function.
**************************************************************************************************/
unsigned char Software_Timer(TmrPkg *pkg)
{
    if (pkg->reg.bits.flg == 0)
    {  
        if (++pkg->reg.bits.cnt >= pkg->cmp)
        {
            pkg->reg.bits.cnt = 0;
            pkg->reg.bits.flg = 1; 
        }
    }
    return pkg->reg.bits.flg;
}
/**************************************************************************************************
Function Name:
    short Data_Lookup(short data, unsigned short *tab, unsigned short size)
Input:
    data      - Input data.
    *tab      - The pointer points to an address of an lookup table. 
    size      - Size of the specified table.
Output:
    return    - Result of the search.
Comment:
    16bit Data table lookup function 
                                             (int(log2(Size)) - 1)
    - Maximum search time = int(log2(Size)) +   £U int(Size / 2^n) mod 2         
                                              n = 0
**************************************************************************************************/
short Data_Lookup(short data, unsigned short *tab, unsigned short size)
{
    unsigned short opr = 0, bit = 1, index = 0xFFFF;

    while (index == 0xFFFF)
    {
        if ((size >> bit) > 0)
        {				
            if (data < tab[(size >> bit) + opr]) 
                opr += (size >> bit);
        }
        else
        {
            if (tab[opr] < data)
            {
                if ((data - tab[opr]) < (tab[opr - 1] - data)) 
                    index = opr;
                else 
                    index = opr - 1;					
            }
            else if (opr >= (size - 1)) 
                index = size - 1;
            else 
                opr++;
        }
        bit++;			
    }
    return index;		
}
/**************************************************************************************************
Function Name:
    void ADC_Data_Calibration(AdcCalib *par)
Input:
    *par     - The pointer points to a ADC calibration structure.
Output:
    None.
Comment:
    12bit ADC data calibration.
    - Gain->Q15 format data
    - Offset->15bit data
    - LimMax->Maximum calibration data value
    - LimMin->Minimum calibration data value
**************************************************************************************************/
void ADC_Data_Calibration(AdcCalib *par)
{	
    par->data->calibHex = (((par->gain * par->data->hexVal) >> 12) + par->offset) >> 3;
    
    if (par->data->calibHex < par->limMin) 
        par->data->calibHex = par->limMin;
    else if (par->data->calibHex > par->limMax) 
        par->data->calibHex = par->limMax;
    par->data->valPtr = &par->data->calibHex;
}
/**************************************************************************************************
Function Name:
    long DLPF_Calculation(Equation_2ndPar *data)
Input:
    *data    - The pointer points to a 2nd DLPF structure.
Output:
    return   - Output of DLPF.
Comment:
    12bit 2nd DLPF calculation
    - Difference equation->y[n] = b2 * (u[n] + 2 * u[n-1] + u[n-2]) + a1 * y[n-1] - a0 * y[n-2]
    - Maximum input data = 4095
    - Coefficient Q format = 14
    - Maximum coefficient(a0, a1, and b2) = 2^14(1pu)
**************************************************************************************************/
long DLPF_Calculation(Equation_2ndPar *data)
{
    long calTmp1,calTmp2;
    
    data->par.un = data->val->hexVal;
    calTmp1 = (((data->par.yn1 >> 14) * data->par.a1) + (((data->par.yn1 & 0x3FFF) * data->par.a1) >> 14));
    calTmp2 = (((data->par.yn2 >> 14) * data->par.a0) + (((data->par.yn2 & 0x3FFF) * data->par.a0) >> 14));	
    data->par.yn = data->par.b2 * (data->par.un + 2 * data->par.un1 + data->par.un2) + calTmp1 - calTmp2;
    data->par.un2 = data->par.un1;
    data->par.un1 = data->par.un;
    data->par.yn2 = data->par.yn1;
    data->par.yn1 = data->par.yn;
    return data->par.yn >> 14;
}
/**************************************************************************************************
Function Name:
    unsigned char Check_Signal_State(unsigned char state, TmrPkg *count, unsigned char signal)
Input:
    state     - The previously accepted state. It shall be a boolean value(0/1).
    *count    - The pointer points to a timer structure which is counted up by this 
                function to detect a tick count condition.
    signal    - The current state. It shall be a boolean value(0/1). 
Output:
    return    - Result. It will be the current state if count ticked, or the previously accepted 
                state if count does not tick.  
Comment:
    Check signal state function
    - state and signal shall be boolean variables.
**************************************************************************************************/
unsigned char Check_Signal_State(unsigned char state, TmrPkg *count, unsigned char signal)
{	
    if (Call_Counter(*count, state != signal)) 
        return signal;
    return state;
}
/**************************************************************************************************
Function Name:
    unsigned char Check_Event_State(unsigned char state, EventTmrPkg *count, 
                                    unsigned char trueEvent, unsigned char falseEvent)
Input:
    state      - The previously accepted state. It shall be a boolean value(0/1).
    *count     - The pointer points to a timer structure which is counted up by this 
                 function to detect a tick count condition.
    trueEvent  - The current state of the true condition. It shall be a boolean value(0/1).
    falseEvent - The current state of the false condition. It shall be a boolean value(0/1).
Output:
    return     - Result. It will be the current state if count ticked, or the previously accepted 
                 state if count does not tick.  
Comment:
    Check event state function
    - state, trueEvent, and falseEvent shall be boolean variables.
**************************************************************************************************/
unsigned char Check_Event_State(unsigned char state, EventTmrPkg *count, 
                                unsigned char trueEvent, unsigned char falseEvent)
{
    if (!state) 
        return Check_Signal_State(state, &count->trueCount, trueEvent);
    else 
        return Check_Signal_State(state, &count->falseCount, !falseEvent);
}
/**************************************************************************************************
Function Name:
    void Decimal_Data_Calibration(DecCalib *par, unsigned long accuracy)
Input:
    *par       - The pointer points to a decimal calibration structure.
    accuracy   - The divisor for the integer calculation of the decimal conversion. The maximum 
                 value = 10^(int(log(2^16 / 67))) = 100.
Output:
    None.
Comment:
    Decimal data calibration
    - Gain->Decimal data * 1000, ex. 1 = 1000
    - Offset->Decimal data 
    - MaxGain->Maximum gain value
    - MaxOffset->Minimum offset value
    - Maximum Accuracy = 100
**************************************************************************************************/
void Decimal_Data_Calibration(DecCalib *par, unsigned long accuracy)
{		
    long i, gain, offset;
    
    if (par->gain > par->maxGain) 
        gain = par->maxGain;
    else if (par->gain < 0) 
        gain = 0;
    else 
        gain = par->gain;
    
    if (par->offset > par->maxOffset) 
        offset = par->maxOffset;
    else if (par->offset < -par->maxOffset) 
        offset = -par->maxOffset;
    else 
        offset = par->offset;	
    i = ((long)par->data->decVal * ((gain * 2097) >> 6)) >> 15;     // 0.001 * 2^21 = 2097
    
    if ((i + offset) > 0) 
        par->data->decVal = i + offset;
    else 
        par->data->decVal = 0;
    
    if (accuracy == 10) 
        par->data->decInt = (short)(((long)par->data->decVal * 3277) >> 15);
    else if (accuracy == 100) 
        par->data->decInt = (short)(((long)par->data->decVal * 5243) >> 19);		
}
/**************************************************************************************************
Function Name:
    short NTC_Lookup(short data, unsigned short const *ptr)
Input:
    
Output:
    
Comment:
    
**************************************************************************************************/
short NTC_Lookup(short data, unsigned short const *ptr)
{
    unsigned short opr = 0, bit = 1, index = 0xFFFF;

    while (index == 0xFFFF)
    {
        if ((NTC_Table_Size >> bit) > 0) 
        {				
            if (data < ptr[(NTC_Table_Size >> bit) + opr]) 
                opr += (NTC_Table_Size >> bit);
        }
        else
        {
            if (ptr[opr] < data)
            {
                if (opr > 0)
                {
                    if ((data - ptr[opr]) < (ptr[opr - 1] - data)) 
                        index = opr;
                    else 
                        index = opr - 1;
                }
                else 
                    index = 0;						
            }
            else if (opr >= (NTC_Table_Size - 1)) 
            {
                index = NTC_Table_Size - 1;
            }
            else 
                opr++;
        }
        bit++;			
    }
	
    return index;		
}
/**************************************************************************************************
Function Name:
    long Average_Calculation(DLPFtoRealPkt *data)
Input:
    
Output:
    return   - Real Value.
Comment:
    ADC Value -> DLPF Value -> Real Value
    - Difference equation->y[n] = b2 * (u[n] + 2 * u[n-1] + u[n-2]) + a1 * y[n-1] - a0 * y[n-2]
    - Maximum input data = 4095
    - Coefficient Q format = 14
    - Maximum coefficient(a0, a1, and b2) = 2^14(1pu)
    - Sample frequency = 1kHz, Cut-off frequency = 5Hz
**************************************************************************************************/
long Average_Calculation(DLPFtoRealPkt *data)
{
    long calTmp1, calTmp2, dataAvg;
	
    // 2nd digital low pass filter for calculate VAC average value	
    data->un  = data->val->hexVal;										// Q12 
    calTmp1   = (((data->yn1 >> 14) * 32047) + (((data->yn1 & 0x3FFF) * 32047) >> 14));				// Q26
    calTmp2   = (((data->yn2 >> 14) * 15679) + (((data->yn2 & 0x3FFF) * 15679) >> 14));				// Q26	
    data->yn  = 4 * (data->un + 2 * data->un1 + data->un2) + calTmp1 - calTmp2;					// Q26
    data->un2 = data->un1;											// Q26
    data->un1 = data->un;											// Q26
    data->yn2 = data->yn1;											// Q26
    data->yn1  = data->yn;											// Q26

    // RMS value calculate (for sine wave approximate: RMS = (Pi / sqr(2)) * AVG = 1.11 * Vavg)
    dataAvg = (((data->yn >> 11) * data->gain) >> 15) + data->offset;                                           // Q15
    if (dataAvg < 0) 
        dataAvg = 0;
	
    dataAvg = (dataAvg * data->scale) >> 15;															// Q0(x10)
    //Data_RMS = (Data_AVG * 1137) >> 10;
	
    return dataAvg;
}
/***********************************************************************************************************************
Function Name:
    long CMA_Calculation(CMAtoRealPkt *data)
Input:
    *data       - The pointer points to an CMAtoRealPkt data structure.
Output:
    return      - Real Value.
Comment:
    ADC Value -> CMA Value -> Real Value
    - Difference equation
        -> y[n] = (a0 * y[n-1] + u[n]) / 2^n, a0 = 2^n - 1, n(exponent) < 16
    - Maximum input data = 4095
    - Coefficient Q format = 15
    - Sample frequency = 1kHz
    -----------------------------
    | Shift | Cut-off frequency |
    |----------------------------
    |   1   |      115 Hz       |
    |   2   |       46 Hz       |
    |   3   |       21 Hz       |
    |   4   |       10 Hz       |
    |   5   |        5 Hz       |
    |   6   |      2.5 Hz       |
    -----------------------------
    - Sample frequency = 50kHz
    -----------------------------
    | Shift | Cut-off frequency |
    |----------------------------
    |   1   |     5737 Hz       |
    |   2   |     2300 Hz       |
    |   3   |     1062 Hz       |
    |   4   |      512 Hz       |
    |   5   |      252 Hz       |
    |   6   |      125 Hz       |
    |   7   |       62 Hz       |
    |   8   |       31 Hz       |
    |   9   |       15 Hz       |
    |  10   |        8 Hz       |
    -----------------------------
***********************************************************************************************************************/
long CMA_Calculation(CMAtoRealPkt *data)
{
    long temp;
    long dataAvg;

    // Gain transform
    temp = (*data->gain * __IQ(0.001, 28)) >> 14;

    // Cumulative sum
    data->sum = data->sum - data->cma + (long)*data->raw;
    data->cma = data->sum >> data->shift;

    // RMS value calculate (for sine wave approximate: RMS = (Pi / sqr(2)) * AVG = 1.11 * Vavg)
    dataAvg = ((data->cma * temp) >> 14);
//    if(dataAvg < 0)
//    {
//        dataAvg = 0;
//    }
    dataAvg = (dataAvg * data->scale) >> 12;
    dataAvg += *data->offset;
    dataAvg = __IQsat(dataAvg, dataAvg, 0);
    //Data_RMS = (Data_AVG * 1137) >> 10;

    return dataAvg;
}
/***********************************************************************************************************************
Function Name:
    long Real_Value_Calculation(CMAtoRealPkt *data)
Input:
    *data       - The pointer points to an CMAtoRealPkt data structure.
Output:
    return      - Real Value.
Comment:
    ADC Value -> CMA Value -> Real Value
***********************************************************************************************************************/
long Real_Value_Calculation(CMAtoRealPkt *data)
{
    long temp;
    long dataAvg;

    // Gain transform
    temp = (*data->gain * __IQ(0.001, 28)) >> 14;

    // RMS value calculate (for sine wave approximate: RMS = (Pi / sqr(2)) * AVG = 1.11 * Vavg)
    dataAvg = ((data->cma * temp));
    dataAvg = dataAvg >> 14;
//    if(dataAvg < 0)
//    {
//        dataAvg = 0;
//    }
    dataAvg = (dataAvg * data->scale) >> 12;
    dataAvg += *data->offset;
    dataAvg = __IQsat(dataAvg, dataAvg, 0);
    //Data_RMS = (Data_AVG * 1137) >> 10;

    return dataAvg;
}
/***********************************************************************************************************************
Function Name:
    long Real_Value_Calculation1(CMAtoRealPkt *data)
Input:
    *data       - The pointer points to an CMAtoRealPkt data structure.
Output:
    return      - Real Value.
Comment:
    ADC Value -> CMA Value -> Real Value
***********************************************************************************************************************/
long Real_Value_Calculation1(CMAtoRealPkt *data)
{
    long temp;
    long dataAvg;

    // Gain transform
    temp = (*data->gain * __IQ(0.001, 28)) >> 14;

    // RMS value calculate (for sine wave approximate: RMS = (Pi / sqr(2)) * AVG = 1.11 * Vavg)
    dataAvg = ((data->cma1 * temp));
    dataAvg = dataAvg >> 14;
//    if(dataAvg < 0)
//    {
//        dataAvg = 0;
//    }
    dataAvg = (dataAvg * data->scale) >> 12;
    dataAvg += *data->offset;
    dataAvg = __IQsat(dataAvg, dataAvg, 0);
    //Data_RMS = (Data_AVG * 1137) >> 10;

    return dataAvg;
}
/***********************************************************************************************************************
Function Name:
    long Calibration_Calculation(long data,short gain, short offset)
Input:
    *data       - The pointer points to an CMAtoRealPkt data structure.
Output:
    return      - Real Value.
Comment:
    ADC Value -> CMA Value -> Real Value
***********************************************************************************************************************/
long Calibration_Calculation(long data,short gain, short offset)
{
    long temp;
    long dataCalibration;

    // Gain transform
    temp = (gain * __IQ(0.001, 28)) >> 14;

    dataCalibration = ((data * temp) >> 14) + offset;
    dataCalibration = __IQsat(dataCalibration, dataCalibration, 0);

    return dataCalibration;
}
/***********************************************************************************************************************
Function Name:
    void In_Range_Check(CMAtoRealPkt *data)
Input:
    *data       - The pointer points to an ProtectionPkt data structure.
Output:
    flag        - 0 = IN_RANGE
                  1 = OVER_RANGE
                  2 = UNDER_RANGE
Comment:
    Check if data is over, under or between limit.
***********************************************************************************************************************/
void Range_Check(ProtectionPkt *data)
{
    if((*data->val > data->upperlevel))
    {
        data->underCount = 0;
        data->betweenCount = 0;
        if(++(data->overCount) >= data->overLatency)
        {
            data->overCount = 0;
            data->flag = OVER_RANGE;
        }
    }
    else if((*data->val < data->lowerlevel))
    {
        data->overCount = 0;
        data->betweenCount = 0;
        if(++(data->underCount) >= data->underLatency)
        {
            data->underCount = 0;
            data->flag = UNDER_RANGE;
        }
    }
    else
    {
        data->overCount = 0;
        data->underCount = 0;
        if(++(data->betweenCount) >= data->betweenLatency)
        {
            data->betweenCount = 0;
            data->flag = IN_RANGE;
        }
    }
}
/**************************************************************************************************
Function Name:
    long Data_Saturation(long data, long max, long min)
Input:
    data
    max
    min
Output:
    data been saturation
Comment:
    Shut down LLC controller immediately.
    Mainly use when fault occurs.
**************************************************************************************************/
long Data_Saturation(long data, long max, long min)
{
    if(data > max)
    {
        data = max;
    }
    else if(data < min)
    {
        data = min;
    }

    return data;
}
// ------------------------------------ End Line --------------------------------------------------
