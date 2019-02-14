#include "system.h"

unsigned short WarmUp_3_Minute_Cnt = 180;


void IntoWarmUpModeProcess(){
    
    System_Mode = WarmUp;
    ClearStd_1_Sec_Cnt();
    WarmUp_3_Minute_Cnt = 180;
    
}


void WarmUp_Key(){
    
    if( KeyCatch(0,1 , Start) ){
        
        IntoWorkoutModeProcess();
     
    }
    
    if( KeyCatch(0,1 , Stop) || PauseKey() ){
        IntoPauseMode_Process();
        Set_SPEED_Value(0);
        RM6_Task_Adder(Set_SPEED);
        RM6_Task_Adder(Motor_STOP);
    }
    
    
    if(Quick_SPEED__Key() == 1){
    
        RM6_Task_Adder(Set_SPEED);
    
    }
    
    HR_SENSOR_LINK_Key();   
    SCREEN_OPTION_Key();
    
    
}


void WarmUp_Func(){

    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;
        F_String_buffer_Auto_Middle( Left, "WARM    UP" ,55 ,0);
        writeLEDMatrix();
    }
    
    if(T1s_Flag){
        T1s_Flag = 0;
        
        Calorie_Calculate(); 
        Program_Data.Distance += (System_SPEED *10000) / 3600 ;   //�C+1��
        
        
        //SET_Seg_TIME_Display( TIME  , WarmUp_3_Minute_Cnt);
        //writeSegmentBuffer();
        
        if(WarmUp_3_Minute_Cnt == 0){
            
            IntoWorkoutModeProcess();
            
            //�]�w ��l ����
            System_INCLINE = Program_Data.INCLINE_Table_96[0] * 5;
            RM6_Task_Adder(Set_INCLINE);
        
        }else{ 
            WarmUp_3_Minute_Cnt--; 
        }
        
         Btm_Task_Adder(FTMS_Data_Broadcast);
   
    }
    
    if(T5ms_Workout_Display_Flag){
        T5ms_Workout_Display_Flag = 0;
        
        if(Time_Display_Type == Remaining){
            SET_Seg_TIME_Display( TIME  , WarmUp_3_Minute_Cnt);            //�ѤU�h�֮ɶ�  �U��
        }else if(Time_Display_Type == Elspsed){
            SET_Seg_TIME_Display( TIME  , 180 - WarmUp_3_Minute_Cnt);      //�g�L�h�֮ɶ�  �W��
        }
        
        
         writeSegmentBuffer();
    }
    
    
        
    WarmUp_Key();

    
}