#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "structDef.h"
#include "Buzzer.h"
#include "HT1632_C52.h"
#include "SevenSegDisplay.h"
#include "LED_Matrix.h"
#include "RM6T6.h"
#include "Key.h"
#include "Mode.h"
#include "Program.h"
#include "btm.h"
#include "SystemStick.h"
#include "DisplayBoard.h"
#include "RealityKey.h"
#include "FAN.h"
#include "BT_SPK.h"

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define System_Version "V14"        //APP 只判斷這個
#define Modify_Version "A02"        // AXX 細分版本 V10A01 V10A02 代表都是V10 但是 有稍微修改


//--------功能開關---------  1 開   0關
#define Use_BTSPK   0
#define Use_FAN     0

#define Use_SafeKey    1
#define Use_Buzzer     1     
#define RM6T6_IN_USE   1

#define configUSE_WHR  1
#define configUSE_HHR  1

#define AutoPause  1

#define FAKE_RM6T6_Mode  0
#define SpdIncSegBlink   0
#define RM6_Param_Debug  0

#define FTMS_Activated_Permission 1
//-------------------------------

extern unsigned char L_Mode;
extern unsigned char R_Mode;

extern unsigned char AutoPause_Flag; //隱藏功能

extern char ucProductionSerialNumber[14];     //生產序號

//----------IWDG ---------
//#include "stm32f0xx_hal_iwdg.h"
#define Config_WDG   0
extern unsigned char Feed_Dog_Flag;

extern Training_status_Def  Training_status;
extern unsigned char  console_status;
/*
#define System_INCLINE_Max 150
#define System_SPEED_Max   120
*/
//App Mode
extern unsigned short usAppStageNumber;

//Power 5V Control
extern void Power_5V_ON();
extern void Power_5V_OFF();

//---  5K WHR  hand HHR-----
extern unsigned char ucWhr;	
extern unsigned char HR5KPairOkFlag;
extern unsigned char ucHhr;

void HR_5K_Init();
void Hand_HR__Init();

extern void F_HR(void);
extern void F_HRProcess(void);

//----  Safe Key  -----
void SafeKey_Init();
void SafeKey_Detect();
extern SafeKey_State_def safekey;
unsigned char PauseKey();
extern unsigned short Time_Set;
extern unsigned char  Cnt_Set;

//  --- Flash  -----
extern void Write_SerialNumber_To_Flash(char *Data);
extern void Read_SerialNumber_From_Flash(char *Data);

extern void Flash_Read_Test(unsigned int Sector_Addr);
extern void FlashErase(unsigned int Sector);
extern void Flash_Init();
extern void FlashTest();
extern void Flash_Custom_Data_Loading();
extern void Flash_Custom_Data_Saving();
extern void SaveWorkoutData();

extern void Flash_Write_OTA_Mode();
extern void Flash_Write_Applicantion_Mode();
extern void Flash_Write_BootLoader_Mode();

extern void OTA_Mode_Check();
extern void Update_FW_Flash_Erase();
extern void Write_EE_Flash_FW_Data_16Byte( unsigned int page , unsigned char *Data);
extern void Fw_Write_Finish_Check();

extern unsigned char Flash_Test_Engineer();

extern OTA_FW_Transmit_Info_Def FW;
extern unsigned char OTA_Mode_Flag;

extern Machine_Data_Def  Machine_Data;
extern void Write_Machine_Data_Init(System_Unit_Def Unit);
extern void Machine_Data_Update();
extern void Flash_Machine_Data_Loading();

extern unsigned short ONE_SEC_Cnt_Value;
extern unsigned short ODO_RecordCnt;      //紀錄時間
extern unsigned int   ODO_RecordDistance; //紀錄距離


extern Custom_Program_Data_Def MyCustom_1;
extern Custom_Program_Data_Def MyCustom_2;

extern User_Program_Data_Def MyUser_1;
extern User_Program_Data_Def MyUser_2;

extern unsigned short usNowHeartRate;    //使用者心跳 
extern unsigned char  HeartRate_Is_Exist_Flag;
extern void F_HeartRate_Supervisor();

//extern unsigned int Total_Machine_Distants;  //機器跑的總里程  單位 公里/10 => 1:100公尺
//extern unsigned int Total_Machine_Times;     //機器跑的總時間  單位 ? 

extern Time_Display_Def       Time_Display_Type;
extern Calories_Display_Def   Calories_Display_Type;
extern Dist_Display_Def       Dist_Display_Type;
extern HeartRate_Display_Def  HeartRate_Display_Type;
extern unsigned char          Pace_Display_Switch;

extern unsigned char ContuineBeepFlag;  //連續 bb叫

extern System_Mode_Def System_Mode;
extern System_Unit_Def System_Unit;
extern unsigned char   str_cNt;

extern unsigned char  ret_Idle_cnt;
extern unsigned short Go_Sleep_cnt;

extern unsigned short System_INCLINE; //0~150   => 0.0~15.0 %   =>0~31
extern unsigned short System_SPEED;   //0 ~ 120 

extern unsigned int uiAvgSpeed;
extern unsigned int uiAvgIncline;
extern unsigned int uiAvgHeartRate;

extern unsigned short usMET;

extern unsigned int  Pace_Freq;  //步頻
extern unsigned int  Pace_Dist;  //步幅
extern unsigned int  Pace_Spd;   //步速
extern unsigned long ulAltitude;

unsigned char charArrayEquals( char A1[], char A2[]);

//----------------  Bitmap ------------------------------
extern const unsigned char *CapitalAlphabetArray[];
extern const unsigned char *NumberArray[];
extern const unsigned char *Pattern_Array[];

extern const unsigned char ATTACUS_Logo[];
extern const unsigned char Light_Eye[];

extern const unsigned char Heart_Empty[];
extern const unsigned char Heart_Solid[];
extern const unsigned char EZ_I_Icon[];
extern const unsigned char BLE_HR_Icon[];
extern const unsigned char ANT_HR_Icon[];
extern const unsigned char Download_Icon[];
extern const unsigned char SPK_ON_Icon[];
extern const unsigned char SPK_OFF_Icon[];

//----------------  Debug  ------------------------------

#endif