#include "system.h"


unsigned short PauseTimeOut_Cnt = 300;

void IntoPauseMode_Process(){
    
    F_SetFEC_State(FINISHED);
    
    PauseTimeOut_Cnt = 300;
    System_Mode = Paused;
    ClearStd_1_Sec_Cnt(); 
    
}


void Pause_Key(){

    if(ContuineBeepFlag!=1){  //等蜂鳴器B完
        
        if( KeyCatch(0,1 , Start) ){
            IntoWorkoutModeProcess();
            Set_SPEED_Value(Machine_Data.System_SPEED_Min);
            RM6_Task_Adder(Set_SPEED);
            RM6_Task_Adder(Motor_FR);
        }
        
        if( KeyCatch(0,1 , Stop) ){
            IntoSummaryMode_Process();
            
            System_INCLINE = 0;
            RM6_Task_Adder(Set_INCLINE);
        }
        
        SCREEN_OPTION_Key();
        HR_SENSOR_LINK_Key(); 
    }
      
}


void Pause_Func(){

      
    if(T1s_Flag){
        T1s_Flag = 0;
        
        F_Number_buffer_Auto( Stay, PauseTimeOut_Cnt , 35 ,DEC ,0);
        writeLEDMatrix();
       
        if(PauseTimeOut_Cnt == 0){
            IntoSummaryMode_Process();
        }else{ 
            PauseTimeOut_Cnt--; 
        }
    }
    
    if(T5ms_Workout_Display_Flag){
        T5ms_Workout_Display_Flag = 0;
        
        switch(Program_Select){
          case MARATHON_Mode:      //時間上數顯示
          case Quick_start:
          case Calorie_Goal:
          case Distance_Goal_160M:
          case Distance_Goal_5K:
          case Distance_Goal_10K:
          case APP_Cloud_Run:
          case APP_Train_Dist_Run:  
          case APP_Train_Time_Run:  
            SET_Seg_TIME_Display( TIME  ,Program_Data.Goal_Time - Program_Data.Goal_Counter);
            break;
            
          default:   //預設下數顯示
            if(Time_Display_Type == Remaining){
                SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Counter);                           //剩下多少時間  下數
            }else if(Time_Display_Type == Elspsed){
                SET_Seg_TIME_Display( TIME  ,Program_Data.Goal_Time - Program_Data.Goal_Counter);    //經過多少時間 上數
            }
            break;
            
        }
        
         writeSegmentBuffer();
    }
    
    Pause_Key();

}