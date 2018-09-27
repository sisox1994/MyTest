#ifndef MODE_H_
#define MODE_H_

extern void Program_Init();
extern void User_Like_Program_Init();

extern void KeyChangeDetect(Key_Name_Def key);

extern Program_Type_Def Program_Select;
extern Program_Data_Def Program_Data;

extern unsigned char no_Beep_Flag;

extern unsigned short WarmUp_3_Minute_Cnt;
extern unsigned short PauseTimeOut_Cnt;

extern unsigned char No_HR_Value_Cnt;

extern void IntoPauseMode_Process();
extern void IntoWorkoutModeProcess();
extern void IntoReadyMode_Process();
extern void IntoWarmUpModeProcess();
extern void IntoCoolDownModeProcess();
extern void IntoSummaryMode_Process();

extern void StartUp_Func();
extern void Menu_Func();

extern void Engineer_Func();

extern void Safe_Func();
extern void ProgramSetting_Func();
extern void Ready_Func();
extern void Workout_Func();
extern void WarmUp_Func();
extern void CoolDown_Func();
extern void Pause_Func();
extern void Summary_Func();
extern void APP_Mode_Func();
extern void OTA_Mode_Func();
extern void RS485_Test_Func();
#endif