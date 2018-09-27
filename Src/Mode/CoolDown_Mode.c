#include "system.h"

unsigned short CoolDown_3_Minute_Cnt = 180;


void IntoCoolDownModeProcess(){
    
    Time_Display_Type = Remaining;  // 清為 預設為 下數顯示
    
    System_Mode = CooolDown;
    ClearStd_1_Sec_Cnt();
    CoolDown_3_Minute_Cnt = 180;
    
}


void CoolDown_Key(){
    

    
    if( KeyCatch(0,1 , Stop) ){
        IntoSummaryMode_Process();
        Set_SPEED_Value(0);
        RM6_Task_Adder(Set_SPEED);
        RM6_Task_Adder(Motor_STOP);
    }
    
    if(Quick_SPEED__Key() == 1){
        
        RM6_Task_Adder(Set_SPEED);
        
    }
    
    
    if( KeyCatch(0,1 , Key_Advance) ){
        
        if(Time_Display_Type == Remaining){
            Time_Display_Type = Elspsed;
        }else if(Time_Display_Type == Elspsed){
            Time_Display_Type = Remaining;
        }
        
        __asm("NOP");
    } 
    
    
}


void CoolDown_Func(){

    
    ucSubSystemMode = C_App_CoolDownVal;
  
    
    
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;

        F_String_buffer_Auto_Middle( Left, "COOL    DOWN" ,40 ,0);
         
        writeLEDMatrix();
   
    }
    
    if(T1s_Flag){
        T1s_Flag = 0;
        
        Calorie_Calculate(); 
        Program_Data.Distance += (System_SPEED *10000) / 3600 ;   //每+1秒
        
        if(Time_Display_Type == Remaining){
            SET_Seg_TIME_Display( TIME  , CoolDown_3_Minute_Cnt);            //剩下多少時間  下數
        }else if(Time_Display_Type == Elspsed){
            SET_Seg_TIME_Display( TIME  , 180 - CoolDown_3_Minute_Cnt);      //經過多少時間  上數
        }
        
        writeSegmentBuffer();
        
        if(CoolDown_3_Minute_Cnt == 0){
            
            IntoSummaryMode_Process();
            
            Set_SPEED_Value(0);
            RM6_Task_Adder(Set_SPEED);
            RM6_Task_Adder(Motor_STOP);
            
            
        
        }else{ 
            CoolDown_3_Minute_Cnt--; 
        }
        
        Btm_Task_Adder(FTMS_Data_Broadcast);
   
    }
        
    CoolDown_Key();

    
}