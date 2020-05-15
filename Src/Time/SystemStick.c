#include "stm32f4xx_hal.h"
#include "system.h"

Flags_32bit TimeFlag;

unsigned int   TimerCnt;
unsigned int   NeverClearCnt;
unsigned short BlinkCnt;
unsigned short BlinkCnt2;
unsigned short BlinkCnt3;
unsigned short BlinkCnt4;
unsigned short MarqueeCnt = 75;

unsigned short Ble_Icon_Display_Cnt;
unsigned short ANT_Icon_Display_Cnt;

unsigned char  APP_Connected_Display_Cnt;

unsigned char  DIST_Icon_Display_Cnt;
unsigned char  ALTI_Icon_Display_Cnt;
              
unsigned char  CAL_Icon_Display_Cnt;
unsigned char  CALH_Icon_Display_Cnt;
              
unsigned char  ELAPSED_Icon_Display_Cnt;
unsigned char  REMAINNING_Icon_Display_Cnt;
              
unsigned char  PACE_Icon_Display_Cnt;
unsigned char  STEP_Icon_Display_Cnt;
              
unsigned char  MET_Icon_Display_Cnt;
unsigned char  HR_Icon_Display_Cnt;

unsigned char  HR_BlinkOneTime_Cnt;  //心跳 七段顯示器閃一下 


unsigned short BTSPK_Icon_Display_Cnt;
unsigned short BTSPK_OFF_Icon_Display_Cnt;

unsigned short Ble_wait_disconnect_Time_out_Cnt;
unsigned short Btm_Timer_Cnt;
unsigned short RM6t6_Timer_Cnt;
unsigned short BarArrayBlinkCnt;
unsigned int   standard_1_Sec_Cnt;

unsigned short ONE_SEC_Cnt_Value = 1000;
//unsigned char T500ms_Matrix_Blink_Flag;

void ClearBlinkCnt(){
    BlinkCnt = 0;
    T500ms_Matrix_Blink_Flag = 1;
}

void ClearBlinkCnt2(){
    BlinkCnt2 = 0;
    T500ms_Blink_Flag = 1; 
    MatrixBlink_Flag  = 0;
}

void Rst_Speed_Blink(){
    BlinkCnt4 = 0;
    T500ms_Blink_SPEED_Flag = 1;
}

void Rst_Incline_Blink(){    
    BlinkCnt3 = 0;
    T500ms_Blink_INCL_Flag = 1;
}

void ClearBlinkCnt_BarArray(){    
    BarArrayBlinkCnt = 0;
    T500ms_BarArray_Blink_Flag = 1; 
    
}

void ClearStd_1_Sec_Cnt(){    
    standard_1_Sec_Cnt = 0;
    T1s_Flag = 0;  
}
void time(){
    
    F_NFCSendCmd();
    
    BuzzerCheck();  
    //------------------------------------------------//
    if(Btm_Timer_Cnt%200 == 0){    //BTM 排程器用
        T100ms_Btm_Task_Flag = 1;
        
        if(Ble_Icon_Display_Cnt > 0){
            Ble_Icon_Display_Cnt--;
        }
        if(ANT_Icon_Display_Cnt > 0){
            ANT_Icon_Display_Cnt--;
        } 
        if(APP_Connected_Display_Cnt > 0){
            APP_Connected_Display_Cnt--;
        }  
        if(BTSPK_Icon_Display_Cnt > 0){
            BTSPK_Icon_Display_Cnt--;
        } 
        if(BTSPK_OFF_Icon_Display_Cnt > 0){
            BTSPK_OFF_Icon_Display_Cnt--;
        } 
        //------------------------------------------
        if(DIST_Icon_Display_Cnt > 0){
            DIST_Icon_Display_Cnt--;
        }
        if(ALTI_Icon_Display_Cnt > 0){
            ALTI_Icon_Display_Cnt--;
        } 
        if(CAL_Icon_Display_Cnt > 0){
            CAL_Icon_Display_Cnt--;
        } 
        if(CALH_Icon_Display_Cnt > 0){
            CALH_Icon_Display_Cnt--;
        } 
        //--
        if(ELAPSED_Icon_Display_Cnt > 0){
            ELAPSED_Icon_Display_Cnt--;
        }
        if(REMAINNING_Icon_Display_Cnt > 0){
            REMAINNING_Icon_Display_Cnt--;
        } 
        if(PACE_Icon_Display_Cnt > 0){
            PACE_Icon_Display_Cnt--;
        } 
        if(STEP_Icon_Display_Cnt > 0){
            STEP_Icon_Display_Cnt--;
        } 
        //--
        if(MET_Icon_Display_Cnt > 0){
            MET_Icon_Display_Cnt--;
        }
        if(HR_Icon_Display_Cnt > 0){
            HR_Icon_Display_Cnt--;
        } 
        if(HR_BlinkOneTime_Cnt > 0){
            HR_BlinkOneTime_Cnt--;
        }             
        //------------------------------------------------        
    }
    
    if((Btm_Timer_Cnt + 100)%300 == 0 ){  //RM6t6_Timer_Cnt%150 ==0){  //與下控 RM6T6溝通用 刻意與BTM 差 50 ms
        T100ms_RM6T6_Task_Flag = 1;
    }    
    //---------------------------------------------------//
    //------------   5K 心跳  + 手握 --------------------//   

    if(NeverClearCnt % 100 == 0){   //每隔10ms 算5k心跳數值 
#if configUSE_WHR  
        F_HRProcess();    // 5K心跳數值計算
#endif  
    
    }    
    //-----------------------------------------//

    if(TimerCnt%1000 == 0){ //偵測按鈕按住多少(秒)
        T1s_Key_Hold_Flag = 1;  
    }
    if(TimerCnt%500 == 0){    //Summary_blink
        T500ms_Flag = 1;
    }
    if(TimerCnt%300 == 0){
        T300ms_Flag = 1;
    }
    if(TimerCnt%250 == 0){   //Workout Display
        T250ms_Flag = 1;       
    }
    if(TimerCnt%5 == 0){   //Workout Display
        T5ms_Workout_Display_Flag = 1;
    }
    if(TimerCnt%100 == 0){     //偵測按鈕按住多少(100ms)
        T100ms_KeyHold_Flag = 1; 
    }
    if(TimerCnt%100 == 0){     //實體按鍵(SPD+ SPD-  INC+  INC-)   偵測按鈕按住多少(100ms)
        T100ms_R_KeyHold_Flag = 1; 
    }
    
    if(TimerCnt%100 == 0){     //跑馬燈用
        T100ms_Flag = 1;  
    }
    if(TimerCnt%5 == 0){      //按鍵掃描
        T5ms_Flag = 1;        
    }
    //-------------------------------------------------------------------//
    if(NeverClearCnt%5000 == 0){  //五秒丟一次廣播訊號給雲跑APP (閒置)
        T5s_cmd39_Flag = 1;
    }
    if(NeverClearCnt%500 == 0){   ////500ms 丟一次廣播訊號給雲跑APP  (比賽中)
        T500ms_cmd39_Flag = 1;
    }
    if(NeverClearCnt%500 == 0){      //分號閃爍用
        T500ms_Time_Colon_Blink_Flag = 1;
    }
    if(NeverClearCnt%1000 == 0){   //監控心跳數值
        T1s_HR_Monitor_Flag = 1;
    }
    if(NeverClearCnt%1000 == 0){   //偵測幾秒回IDLE 模式
        T1s_Idle = 1;
        
        if(ucNFCCmdCnt != C_ReadCard){  //非read 模式 1s慢掃就好
           b_NFCTXFlag = 1;
        }      
        
        if(ucNFC_UIDBuf[10] == 0xAA){
            clear_0xAA_cnt++;
        }
        if(clear_0xAA_cnt > 5){
            ucNFC_UIDBuf[10] = 0x00;
            clear_0xAA_cnt = 0;
        }
        
    }
    if(NeverClearCnt % 12 == 0){       //read 直接催到12ms
        if(ucNFCCmdCnt == C_ReadCard){
           b_NFCTXFlag = 1;
        } 
    }  
    
    if(NeverClearCnt%500 == 0){  //偵測藍芽喇叭連線用
        T500ms_BTSPK_Det_Flag = 1;
    }
    if(NeverClearCnt%100 == 0){  //偵測pauseKey用
        T100ms_PauseKey_Flag = 1;
    }
    //-------------------------------------------------------------------//    
    if(standard_1_Sec_Cnt % ONE_SEC_Cnt_Value == 0){  //標準一秒
        T1s_Flag = 1;
    }    
    //---------------------------------------------------------------------//    
    if(BlinkCnt%600 == 0){             //LED 矩陣  閃爍
        T500ms_Matrix_Blink_Flag = 1;
    }
    if(BlinkCnt2%600 == 0){           //七段顯示器 閃爍
        T500ms_Blink_Flag = 1;
    }
    if(BlinkCnt3%500 == 0){            //揚升 七段專用
        T500ms_Blink_INCL_Flag = 1;
    }
    if(BlinkCnt4%500 == 0){            //速度 七段專用
        T500ms_Blink_SPEED_Flag = 1;
    }
    //--------------------------------------------------------------------//   
    if(BarArrayBlinkCnt%500 == 0){
        T500ms_BarArray_Blink_Flag = 1;  //BarArray0.5秒閃爍用
    }
    
    KeyDelay();
    Debounce();
    
    if(TimerCnt%MarqueeCnt == 0){
        T_Marquee_Flag = 1;
    }

    if(TimerCnt == 99999) TimerCnt = 0;
    else TimerCnt++;
    
    if(BlinkCnt == 9999) BlinkCnt = 0;
    else BlinkCnt++;
    
    if(BlinkCnt2 == 19999) BlinkCnt2 = 0;
    else BlinkCnt2++;
    
    if(BlinkCnt3 == 9999) BlinkCnt3 = 0;
    else BlinkCnt3++;
    
    if(BlinkCnt4 == 9999) BlinkCnt4 = 0;
    else BlinkCnt4++;    
    
    if(BarArrayBlinkCnt == 9999) BarArrayBlinkCnt = 0;
    else BarArrayBlinkCnt++;
    
    if(standard_1_Sec_Cnt == 99999)standard_1_Sec_Cnt = 0;
    else standard_1_Sec_Cnt++;
    
    if(Btm_Timer_Cnt == 9999) Btm_Timer_Cnt = 0;
    else Btm_Timer_Cnt++;
    
    if(RM6t6_Timer_Cnt == 9999) RM6t6_Timer_Cnt = 0;
    else RM6t6_Timer_Cnt++;
      
    if(NeverClearCnt == 99999) NeverClearCnt = 0;
    else NeverClearCnt++;
        
}