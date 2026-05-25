/**************************************************************************************************
File Name: Checksum.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description: 
    Checksum algorithm.

==================================================================================================
History: 
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    06/06/2018 Watch Lee        Ver.1.0
    12/26/2019 Watch Lee        1. version 2.0.
                                2. change argument "crcValue" and output data type to unsigned long.
								
**************************************************************************************************/

/* Local function prototypes */
unsigned long Calculate_Checksum(unsigned long checksum, unsigned char data);
 
/* Data definitions */

/**************************************************************************************************
Function Name:
    unsigned char Calculate_Checksum(unsigned char checksum, unsigned char data)
Input:
    crcValue - The last CRC value. 
    data     - Input data.
Output:
    return   - CRC value. 
Comment:
    Calculate checksum function.
**************************************************************************************************/
unsigned long Calculate_Checksum(unsigned long checksum, unsigned char data)
{
    return (checksum + data) & 0xFF;
}
//---------------- END LINE -----------------------------------------------------------------------
