/***********************************************************************************************************************
File Name: App.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ ----------------------------------------------------------------------------

Description:

========================================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- ----------------------------------------------------------------------------------------
    10/4/2019   Fred            Create file
    10/17/2019  Fred            Add Vector.h include directive
    03/20/2020  Fred            Add Create_CMA_Real_Value
    03/03/2021  Fred Huang      Add Get_Working_State function declaration
    03/08/2021  Fred Huang      Add offsetStandbyVolt declaration
    03/10/2021  Fred Huang      1. Modify waring code structure
                                2. Remove offsetStandbyVolt
                                3. Add UNB_RECOVERY_MODE
    03/11/2021  Fred Huang      Remove UNB_RECOVERY_MODE
    03/16/2021  Fred Huang      Add decimal point, time base constant
    03/18/2021  Fred Huang      1. Add can turn off module flag
                                2. Rearrange code placement
    05/06/2021  Watch Lee       1. Udpate global functions. 

***********************************************************************************************************************/

#ifndef _APP_H_
#define _APP_H_

//----------------------------------------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------------------------------------
//#include <strings.h>

#include "F28x_Project.h"
#include "Univ_Lib.h"
#include "c28x_data_structure.h"
#include "Slave_pmbus.h"
#include "COM_UART.h"
#include "COM_I2C.h"
#include "f28003x_boot.h"
#include "HardwareSetup.h"
#include "Initial.h"
#include "Interrupt.h"
#include "Io.h"
#include "LlcDriver.h"
#include "LookupTable.h"
#include "Vector.h"
#include "EXT_Calibration.h"
#include "../Libraries/EventLog/Include/EventLog.h"
#include "CPU_CLA_Message.h"

//----------------------------------------------------------------------------------------------------------------------
// Condition definitions
//----------------------------------------------------------------------------------------------------------------------
#define OpenLoop    0
#define FixFreq     0

#define LLCTestMode    0
#define AHBTestMode    0
//----------------------------------------------------------------------------------------------------------------------
// Macro definitions
//----------------------------------------------------------------------------------------------------------------------
// Manufacturer ID
#define Manufacturer            "Delta"

// BootCode version address
#define Boot_Version_Address    (App_ROM_Start - 0x10)

// Signature code
#ifndef Signature_Code
#ifndef DEBUG_MODE
#define Signature_Code          Bin_Char4("APP")
#else
#define Signature_Code          Bin_Char4("DBG")
#endif
#endif

// Function ID code
#ifndef Function_ID
#define Function_ID             "PRI"
#endif

// Model name
#ifndef Model_Name
#define Model_Name              "BBUBU163AEN01"
#endif

// Compliant model name
#define Compliant_Model         {\
                                    "BBUBU163AEN01",\
                                }

// Type code
#ifndef Type_Code
#define Type_Code               "APP"
#endif

// Customer code
#ifndef Customer_Code
#define Customer_Code           "BEL"
#endif

// App version
#ifndef App_Version
#define App_Version             "S1.00B09"
#endif

#ifndef Sub_Version
#define Sub_Version             "01"
#endif

// App header summary
#ifndef App_Header_List
#define App_Header_List         {\
                                    {\
                                        {Get_Char(Model_Name,  0), Get_Char(Model_Name,  1)},\
                                        {Get_Char(Model_Name,  2), Get_Char(Model_Name,  3)},\
                                        {Get_Char(Model_Name,  4), Get_Char(Model_Name,  5)},\
                                        {Get_Char(Model_Name,  6), Get_Char(Model_Name,  7)},\
                                        {Get_Char(Model_Name,  8), Get_Char(Model_Name,  9)},\
                                        {Get_Char(Model_Name, 10), Get_Char(Model_Name, 11)},\
                                        {Get_Char(Model_Name, 12), Get_Char(Model_Name, 13)},\
                                        {Get_Char(Model_Name, 14), Get_Char(Model_Name, 15)},\
                                        {Get_Char(Model_Name, 16), Get_Char(Model_Name, 17)},\
                                        {Get_Char(Model_Name, 18), Get_Char(Model_Name, 19)}\
                                    },\
                                    {\
                                        {Get_Char(Type_Code, 0), Get_Char(Type_Code, 1)},\
                                        {Get_Char(Type_Code, 2), Get_Char(Type_Code, 3)},\
                                    },\
                                    {\
                                        {Get_Char(Customer_Code, 0), Get_Char(Customer_Code, 1)},\
                                        {Get_Char(Customer_Code, 2), Get_Char(Customer_Code, 3)},\
                                    },\
                                    3,\
                                    Bin_Char4(Function_ID),\
                                    0,\
                                    0,\
                                    {\
                                        {Get_Char(App_Version,  0), Get_Char(App_Version,  1)},\
                                        {Get_Char(App_Version,  2), Get_Char(App_Version,  3)},\
                                        {Get_Char(App_Version,  4), Get_Char(App_Version,  5)},\
                                        {Get_Char(App_Version,  6), Get_Char(App_Version,  7)},\
                                        {Get_Char(App_Version,  8), Get_Char(App_Version,  9)}\
                                    }\
                                }
#endif

// Version summary
#ifndef Version_List
#define Version_List            {App_Version, Sub_Version}
#endif

// CLA message section
#define CPU2CLA                 __attribute__((section(".cpu2cla")))
#define CLA2CPU                 __attribute__((section(".cla2cpu")))

// Time base constant
#define COUNT_100us_IN_50kHz    (long)5
#define COUNT_200us_IN_50kHz    (long)10
#define COUNT_300us_IN_50kHz    (long)15
#define COUNT_500us_IN_50kHz    (long)25
#define COUNT_600us_IN_50kHz    (long)30
#define COUNT_1ms_IN_50kHz      (long)50
#define COUNT_2ms_IN_50kHz      (long)100
#define COUNT_5ms_IN_50kHz      (long)500
#define COUNT_22ms_IN_50kHz     (long)1100
#define COUNT_62ms_IN_50kHz     (long)3100
#define COUNT_FIRST_ON          (long)2
#define COUNT_1ms_IN_1kHz       (long)1
#define COUNT_2ms_IN_1kHz       (long)2
#define COUNT_3ms_IN_1kHz       (long)3
#define COUNT_4ms_IN_1kHz       (long)4
#define COUNT_5ms_IN_1kHz       (long)5
#define COUNT_6ms_IN_1kHz       (long)6
#define COUNT_8ms_IN_1kHz       (long)8
#define COUNT_10ms_IN_1kHz      (long)10
#define COUNT_15ms_IN_1kHz      (long)15
#define COUNT_18ms_IN_1kHz      (long)18
#define COUNT_20ms_IN_1kHz      (long)20
#define COUNT_25ms_IN_1kHz      (long)25
#define COUNT_30ms_IN_1kHz      (long)30
#define COUNT_40ms_IN_1kHz      (long)40
#define COUNT_50ms_IN_1kHz      (long)50
#define COUNT_55ms_IN_1kHz      (long)55
#define COUNT_60ms_IN_1kHz      (long)60
#define COUNT_62ms_IN_1kHz      (long)62
#define COUNT_70ms_IN_1kHz      (long)70
#define COUNT_80ms_IN_1kHz      (long)80
#define COUNT_100ms_IN_1kHz     (long)100
#define COUNT_150ms_IN_1kHz     (long)150
#define COUNT_200ms_IN_1kHz     (long)200
#define COUNT_250ms_IN_1kHz     (long)250
#define COUNT_500ms_IN_1kHz     (long)500
#define COUNT_20500ms_IN_1kHz   (long)20500
#define COUNT_1s_IN_1kHz        (long)1000
#define COUNT_2s_IN_1kHz        (long)2000
#define COUNT_5s_IN_1kHz        (long)5000
#define COUNT_10s_IN_1kHz       (long)10000
#define COUNT_11s_IN_1kHz       (long)11000
#define COUNT_20s_IN_1kHz       (long)20000
#define COUNT_40s_IN_1kHz       (long)40000
#define COUNT_60s_IN_1kHz       (long)60000
#define COUNT_1ms_IN_2kHz       (long)2
#define COUNT_2ms_IN_2kHz       (long)4
#define COUNT_4ms_IN_2kHz       (long)8
#define COUNT_5ms_IN_2kHz       (long)10
#define COUNT_6ms_IN_2kHz       (long)12
#define COUNT_10ms_IN_2kHz      (long)20
#define COUNT_50ms_IN_2kHz      (long)100
#define COUNT_100ms_IN_2kHz     (long)200
#define COUNT_200ms_IN_2kHz     (long)400
#define COUNT_500ms_IN_2kHz     (long)1000
#define COUNT_700ms_IN_2kHz     (long)1400
#define COUNT_800ms_IN_2kHz     (long)1600
#define COUNT_1s_IN_2kHz        (long)2000
#define COUNT_2s_IN_2kHz        (long)4000
#define COUNT_8s_IN_2kHz        (long)16000
#define COUNT_5s_IN_2kHz        (long)10000
#define COUNT_10s_IN_2kHz       (long)20000
#define COUNT_1ms_IN_5kHz       (long)5
#define COUNT_2ms_IN_5kHz       (long)10
#define COUNT_4ms_IN_5kHz       (long)20
#define COUNT_5ms_IN_5kHz       (long)25
#define COUNT_6ms_IN_5kHz       (long)30
#define COUNT_10ms_IN_5kHz      (long)50
#define COUNT_50ms_IN_5kHz      (long)250
#define COUNT_100ms_IN_5kHz     (long)500
#define COUNT_200ms_IN_5kHz     (long)1000
#define COUNT_1s_IN_5kHz        (long)5000

#define COUNT_10s_IN_2Hz        (long)20

// Decimal point constant
#define Q15_ONE_TENTH           (long)3276
#define Q15_ONE_HUNDREDTH       (long)327

// ADV Voltage unit: 0.1V, current unit: 0.01A
// Limit value
//Vout
#define VOUT_OV_FAULT_LIMIT     (long)4450  //5250//5810//5775//6000
#define VOUT_OV_FAULT_RECOVERY  (long)4250  //pending//5110//5575
#define VOUT_UV_FAULT_LIMIT     (long)3000  //4700//4700//4775
#define VOUT_FUV_FAULT_LIMIT    (long)3000  //4775
#define VOUT_UV_FAULT_RECOVERY  (long)4000  //4800//4975

//Iout
#define IOUT_OC1_FAULT_LIMIT    (long)5000  //3000  //5000
#define IOUT_OC1_FAULT_RECOVERY (long)4325  //2500  //4325
#define IOUT_OC2_FAULT_LIMIT    (long)29508 //6000//13920//TBD
#define IOUT_OC2_FAULT_RECOVERY (long)25620 //1098//12920
#define IOUT_OC3_FAULT_LIMIT    (long)6920  //14880
#define IOUT_OC3_FAULT_RECOVERY (long)25620 //13880
#define IOUT_SC4_FAULT_LIMIT    (long)48800 //19000
#define IOUT_SC4_FAULT_RECOVERY (long)25620 //18000

#define IOUT_OC1_ALERT_LIMIT    (long)4974 //115% load ////12960
#define IOUT_OC1_ALERT_RECOVERY (long)25620//11960

//OTP
#define OT_FAULT_LIMIT          (long)60
#define OT_FAULT_RECOVERY       (long)55
#define OT_WARN_LIMIT           (long)55
#define OT_WARN_RECOVERY        (long)50
//Vin
#define VIN_OV_FAULT_LIMIT      (long)3160
#define VIN_OV_FAULT_RECOVERY   (long)3110
#define VIN_OV_WARN_LIMIT       (long)2900
#define VIN_OV_WARN_RECOVERY    (long)2850
#define VIN_UV_FAULT_LIMIT      (long)1710
#define VIN_UV_FAULT_RECOVERY   (long)1760
//Iin
#define IIN_OC_FAULT_LIMIT      (long)3240
#define IIN_OC_FAULT_RECOVERY   (long)2740
#define IIN_OC_WARN_LIMIT       (long)2760
#define IIN_OC_WARN_RECOVERY    (long)2260
//Pin
#define PIN_OP_WARN_LIMIT       (long)12852
#define PIN_OP_WARN_RECOVERY    (long)12240
#define UP_CURR         850//819//55A
#define DOWN_CURR       386//372//25A

//Batt //Wait for determinte the value
#define VBATT_OV_FAULT_LIMIT    (long)3300 //3600 //3300
#define VBATT_OV_FAULT_RECOVERY (long)2964
#define VBATT_UV_FAULT_LIMIT    (long)1900
#define VBATT_UV_FAULT_RECOVERY (long)2128
#define ICHG_OC_FAULT_LIMIT     (long)500
#define ICHG_OC_FAULT_RECOVERY  (long)20000

//Discharger countdown trig voltage
#define DCHG_COUNTDOWN_VOLT     (long)2770

//Precharge mode definition
#define ABS(x)                  ((x) < 0 ? -(x) : (x))
#define PRECHG_DIFF_THREDHOLD   (long)158 //18.17 * (1.774/400) * (4095/3.3) = 100 // 18.17V * (2.794/400) * (4095/3.3) = 158
#define CONSTRAIN_MAX(val, max) if ((val) >= (max)) { (val) = (max); }
//----------------------------------------------------------------------------------------------------------------------
// Type definitions
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    POWERON_MODE = 0,
    SLEEP_MODE,
    STANDBY_MODE,
    PRECHARGE_MODE,
    DISCHARGER_SOFTSTART_MODE,
    DISCHARGER_MODE,
    CHARGER_MODE,
    RECOVERY_MODE,
    FAULT_MODE,
    LATCH_MODE,
    BACKUP_MODE,
    SOH_TEST_MODE,
    LEARNING_MODE,
} EnumWorkMode;

typedef enum {
    STATE_IDLE = 0,
    STATE_PRECHG_INIT,
    STATE_BYPASS_RLY2,
    STATE_BYPASS_RLY1,
    STATE_PRECHG_BATT_CHG_CAP,
    STATE_PRECHG_CHG_OUTPUT,
    STATE_CHECK_FOR_D_FET,
    STATE_CHECK_FOR_Pd_FET,
    STATE_COMPLETED,
    STATE_CHECK_VOLTAGE,
    STATE_PRECHG_CHG_CAP,
    STATE_CHECK_FOR_C_FET,
    STATE_FAULT,
} PrechargeState;

typedef struct
{
    union
    {
        unsigned long all;
        struct
        {
            unsigned long OVP_BATT_IO           :1;
            unsigned long OVP_CHG_BATT_IO       :1;
            unsigned long OVP_BUS_IO            :1;
            unsigned long OVP_BUS_SW            :1;
            unsigned long OVP_BUS_CHG_IO        :1;
            unsigned long UVP_BUS_SW            :1;
            unsigned long PRI_OCP_DISCHG_IO     :1;
            unsigned long OCP_DISCHG_IO         :1;

            unsigned long OCP_DISCHG_SW         :1;
            unsigned long STB_FAULT             :1;
            unsigned long OPP                   :1;
            unsigned long OVP_BATT_IO_LAT       :1;
            unsigned long OVP_CHG_BATT_IO_LAT   :1;
            unsigned long OVP_BUS_IO_LAT        :1;
            unsigned long OVP_BUS_SW_LAT        :1;
            unsigned long OVP_BUS_CHG_IO_LAT    :1;

            unsigned long UVP_BUS_SW_LAT        :1;
            unsigned long PRI_OCP_DISCHG_IO_LAT :1;
            unsigned long OCP_DISCHG_IO_LAT     :1;
            unsigned long OCP_DISCHG_SW_LAT     :1;

            unsigned long reserved              :12;
        } bit;
    } status;

    union
    {
        unsigned long all;
        struct
        {
            unsigned long bbuKill                 :1;
            unsigned long acLoss                  :1;
            unsigned long Engineer_OK             :1;
            unsigned long Discharger_OK           :1;
            unsigned long Discharger_Softstart_OK :1;
            unsigned long Charger_OK              :1;
            unsigned long Standby_OK              :1;
            unsigned long Precharge_OK            :1;

            unsigned long Latch_OK                :1;
            unsigned long ESTOP1                  :1;
            unsigned long ESTOP2                  :1;
            unsigned long OFF2ON                  :1;
            unsigned long OP2ON                   :1;
            unsigned long FAILOUT                 :1;
            unsigned long SYNC_START              :1;
            unsigned long RESET_BUTTON            :1;

            unsigned long chgEnOut                :1;
            unsigned long sohOut                  :1;
            unsigned long reserved                :15;

        } bit;
    } status2;

    union
    {
        unsigned long all;
        struct
        {
            unsigned long iOutOcFault1       :1; // 9s
            unsigned long iOutOcFault2       :1; // 55ms
            unsigned long iOutOcFault3       :1; // 15ms
            unsigned long iOutOcFault        :1;
            unsigned long iOutScFault4       :1;
            unsigned long vOutOvFault        :1;
            unsigned long vOutUvFault        :1;
            unsigned long vChargeOvFault     :1;

            unsigned long vChargeOvFault_LAT :1;
            unsigned long iChargeOcFault     :1;
            unsigned long iChargeOcFault_LAT :1;
            unsigned long vBattUvFault       :1;
            unsigned long vBattUvFault_LAT   :1;
            unsigned long vBattOvFault       :1;
            unsigned long srOTP              :1;
            unsigned long srOTW              :1;

            unsigned long oringOTP           :1;
            unsigned long oringOTW           :1;
            unsigned long chgOTP             :1;
            unsigned long chgOTW             :1;
            unsigned long dchgOTP            :1;
            unsigned long dchgOTW            :1;
            unsigned long AMB_OTP            :1;
            unsigned long AMB_OTW            :1;

            unsigned long fan1FrontFault     :1;
            unsigned long fan1RearFault      :1;
            unsigned long fan2FrontFault     :1;
            unsigned long fan2RearFault      :1;
            unsigned long BMSComFault        :1;
            unsigned long DDComFault         :1;

            unsigned long reserved           :2;

        } bit;
    } flag;

} WarningCodePkt;

// Structure & union
typedef union
{
    unsigned long all;

    struct
    {
        unsigned long llcFail           :1;
        unsigned long ready             :1;
        unsigned long startup           :1;
        unsigned long enableLlc         :1;
        unsigned long softStart         :1;
        unsigned long enableDrive       :1;
        unsigned long burst             :1;
        unsigned long highLimitFreq     :1;

        unsigned long sotcEngineerFlag  :1;
        unsigned long clearYn1          :1;
        unsigned long freqSoftStart     :1;
        unsigned long sohOutFlag        :1;
        unsigned long reserved          :22;
    }bit;
}LlcStateType;

typedef union
{
    unsigned long all;

    struct
    {
        unsigned long enableAhb         :1;
        unsigned long ccEnableDrive     :1;
        unsigned long cvEnableDrive     :1;

        unsigned long reserved         :29;
    }bit;
}AhbStateType;

typedef union
{
    unsigned long all;

    struct
    {
        unsigned long cFet              :1;
        unsigned long dFet              :1;
    }flag;
}BbuItemType;

typedef enum {
    MODE_DEFAULT = 0,
    MODE_CHARGE = 1,
    MODE_DISCHARGE = 2
} STATE_MODE;

//----------------------------------------------------------------------------------------------------------------------
// Global function prototypes
//----------------------------------------------------------------------------------------------------------------------
void Main_Task(void);
void State_Machine(void);
//----------------------------------------------------------------------------------------------------------------------
// Global data declarations
//----------------------------------------------------------------------------------------------------------------------
extern const unsigned char COMPLIANT_TABLE[10][20];
extern const BinHeaderMainType TEXT_HEADER_ROM;
extern TwoByteType appVersion[4];
extern TwoByteType bootVersion[4];
extern TwoByteType mfrID[4];
extern unsigned char energy;
extern PrechargeState powerOnStateCheck;
extern long sTemp;
extern WarningCodePkt warningCode;
extern unsigned short canAddress;
extern volatile STATE_MODE claMode;
extern CMAtoRealPkt avgOutputVolt;
extern CMAtoRealPkt avgOutputCurr;
extern CMAtoRealPkt avgCurrShareVolt;
extern CMAtoRealPkt avgOutputCurrSec;
extern CMAtoRealPkt avgBattVolt;
extern CMAtoRealPkt avgChargeCurr;
extern CMAtoRealPkt avgOutputCurrPK;
extern CMAtoRealPkt avgOutputPower;
extern CMAtoRealPkt avgInnerVolt;
extern CMAtoRealPkt avgChargeVolt;
extern CMAtoRealPkt avgChargePower;

extern CMAtoRealPkt avgTempSr;
extern CMAtoRealPkt avgTempOring;
extern CMAtoRealPkt avgFanRpm;
extern CMAtoRealPkt avgFanRpm2;

extern I2cRegObj pmbusRegObj;
extern EventListObj eventList;
extern unsigned short eraseTrig;
extern unsigned short eventTrig;
extern unsigned short eventReadPointer;
extern unsigned char highLimitFreq;
extern unsigned char protectRelease;
extern unsigned char learningMode;
extern unsigned char forceDischarge;
extern unsigned short flagPowerOn;
extern unsigned char resetButton;
extern unsigned long countdownDischargeTimes;
extern unsigned short softstartFlag;
extern unsigned short chargerSoftstart;
extern unsigned short chargerSoftstartFlag;
extern unsigned short ahbVoltChgDone;
extern unsigned short ahbCurrChgDone;
extern unsigned short softstartDone;
extern unsigned short CurrsoftstartDone;
extern unsigned char startFwUpgrade;
extern short bmsCurrentOffset;
extern unsigned char sohOutFlag;
extern unsigned char sohOutChgFlag;
// test item
extern unsigned char flagLatch;
extern uint16_t ClaFuncsLoadStart, ClaFuncsLoadSize, ClaFuncsRunStart;
extern uint16_t ClaConstLoadStart, ClaConstLoadSize, ClaConstRunStart;

extern LlcStateType cpuLlcState;
extern AhbStateType cpuAhbState;
extern BbuItemType bbuItem;

extern LlcFloatPiControlType sohCurrLoop;

extern TmrPkg dischargerOperationTimer;
//----------------------------------------------------------------------------------------------------------------------
#endif /* _APP_H_ */
