#ifndef  __SYSTEMSTICK_H
#define  __SYSTEMSTICK_H

extern unsigned int TimerCnt;
extern Flags_32bit TimeFlag;

#define T1s_Flag     TimeFlag.B0
#define T500ms_Flag  TimeFlag.B1
#define T250ms_Flag  TimeFlag.B2
#define T100ms_Flag  TimeFlag.B3
#define T300ms_Flag  TimeFlag.B4
#define T5ms_Flag       TimeFlag.B5
#define T100ms_R_KeyHold_Flag       TimeFlag.B6
#define T_Marquee_Flag                       TimeFlag.B7
#define  T500ms_BTSPK_Det_Flag      TimeFlag.B8
#define T1s_Key_Hold_Flag              TimeFlag.B10
#define T500ms_Blink_Flag               TimeFlag.B11

#define T500ms_Matrix_Blink_Flag    TimeFlag.B12
//extern unsigned char T500ms_Matrix_Blink_Flag;

#define T100ms_KeyHold_Flag         TimeFlag.B13
#define T500ms_BarArray_Blink_Flag  TimeFlag.B14

#define T100ms_Btm_Task_Flag           TimeFlag.B15
#define T100ms_RM6T6_Task_Flag         TimeFlag.B16

#define T500ms_Blink_INCL_Flag         TimeFlag.B17
#define T500ms_Blink_SPEED_Flag        TimeFlag.B18
#define T1s_Menu_Idle                  TimeFlag.B19
#define T5s_cmd39_Flag                 TimeFlag.B20
#define T500ms_Time_Colon_Blink_Flag   TimeFlag.B21
#define T1s_HR_Monitor_Flag            TimeFlag.B23
#define T500ms_cmd39_Flag              TimeFlag.B24
#define T100ms_PauseKey_Flag              TimeFlag.B25
#define T5ms_Workout_Display_Flag       TimeFlag.B26


extern unsigned short Ble_wait_disconnect_Time_out_Cnt;
extern unsigned short Ble_Icon_Display_Cnt;
extern unsigned short ANT_Icon_Display_Cnt;
extern unsigned short BTSPK_Icon_Display_Cnt;
extern unsigned short BTSPK_OFF_Icon_Display_Cnt;
extern unsigned short MarqueeCnt;

void time();
void ClearBlinkCnt();
void ClearBlinkCnt2();
void ClearBlinkCnt_BarArray();
void ClearStd_1_Sec_Cnt();
void Rst_Speed_Blink();
void Rst_Incline_Blink();

#endif