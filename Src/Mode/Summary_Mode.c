#include "system.h"

void IntoSummaryMode_Process(){

    __asm("NOP");  //將運動 距離,時間  寫進Flash 
    
    Machine_Data_Update();
    
    System_Mode = Summary;
    ClearStd_1_Sec_Cnt(); 
    
}


void Summary_Key(){

 
    if( KeyCatch(0,1 , Stop) ){
        
        Pace_Display_Switch = 0;
        Calories_Display_Type = Cal_;
        
        
        ucSubSystemMode = C_App_IdleVal;
        
        System_Mode = Menu;
       
        Program_Select = Quick_start;
        Cloud_Run_Initial_Busy_Flag = 0;
        Program_Init();
        ClearStd_1_Sec_Cnt(); 
    }
    
}

extern unsigned char ret_Idle_cnt;

void Summary_Idel_detect(){
    
    if(T1s_Menu_Idle == 1){
        T1s_Menu_Idle = 0;
        
        if(ret_Idle_cnt == 120){
            
            Pace_Display_Switch = 0;
            Calories_Display_Type = Cal_;
            
            ClassCnt = 0;
            ret_Idle_cnt = 0;
            Program_Select = Quick_start;
            System_Mode = Menu;
            ucSubSystemMode = C_App_IdleVal;
            Cloud_Run_Initial_Busy_Flag = 0;
            Program_Init();
        }else{
            ret_Idle_cnt++;
        }
        
    }
    
    
}

unsigned char Summary_blink_flag;

void Summary_Func(){

    ucSubSystemMode = C_App_StopVal;
   
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;

        F_String_buffer_Auto( Left, "WORKOUT   SUMMARY" ,40 ,0);
        
        SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
        
        if(T500ms_Flag == 1){
            T500ms_Flag = 0;
            
            if(Summary_blink_flag == 1){
                Summary_blink_flag = 0;
            }else{
                Summary_blink_flag = 1;
            }
        }
        
        if(Summary_blink_flag == 1){
            writeSegmentBuffer();
        }else{
            Turn_OFF_All_Segment();
        }
       
        
        writeLEDMatrix();
        
    }
    
    

    Summary_Idel_detect();
    
    Summary_Key();
    
}