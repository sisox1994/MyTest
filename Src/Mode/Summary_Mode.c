#include "system.h"

void IntoSummaryMode_Process(){

    __asm("NOP");  //將運動 距離,時間  寫進Flash 
    
    Machine_Data_Update();
    
    System_Mode = Summary;
    F_SetFEC_State(FINISHED);
    
    ClearStd_1_Sec_Cnt(); 
    
    F_BtmReply39Cmd();//立馬告訴APP 進入summary了 
    
    __asm("NOP");
    
}


void Summary_Key(){

 
    if( KeyCatch(0,1 , Stop) ){
        IntoIdleMode_Process();
    }
    SCREEN_OPTION_Key();
    HR_SENSOR_LINK_Key(); 
    
}



void Summary_Idel_detect(){
    
    if(T1s_Idle == 1){
        T1s_Idle = 0;
        
        if(ret_Idle_cnt == 120){
            
            IntoIdleMode_Process();
            
            ClassCnt = 0;
            ret_Idle_cnt = 0;
        }else{
            ret_Idle_cnt++;
        }
        
    }
    
    
}

unsigned char Summary_blink_flag;

void Summary_Func(){

    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;

        F_String_buffer_Auto( Left, "WORKOUT   SUMMARY" ,55 ,0);
        
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