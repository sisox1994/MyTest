#include "system.h"


unsigned short PauseTimeOut_Cnt = 180;

void IntoPauseMode_Process(){
    
    
    console_status = 2;      // 1 : stop  2：pause  3: stop by safeKey   4: start 
    Btm_Task_Adder(FEC_Data_Config_Page_1);
    
    
    //如果是fit test模式 按 暫停  或  30s沒步數 直接停止運動測試
    if( (Program_Select == FIT_ARMY)     || (Program_Select == FIT_NAVY) ||
        (Program_Select == FIT_AIRFORCE) || (Program_Select == FIT_USMC) ||
        (Program_Select == FIT_WFI) ){  
        
        Program_Data.FitTest_Score = 0;
        Program_Data.FitTest_RISK  = 3;  //不顯示Risk?
        
        IntoSummaryMode_Process();
        
        System_INCLINE = 0;
        RM6_Task_Adder(Set_INCLINE);
        
    }else{
        
        F_SetFEC_State(FINISHED);
        PauseTimeOut_Cnt = 180;
        System_Mode = Paused;
        ClearStd_1_Sec_Cnt(); 
    }
    
}


void Pause_Key(){

    if(ContuineBeepFlag!=1){  //等蜂鳴器B完
        
        if( KeyCatch(0,1 , Start) ){
            IntoWorkoutModeProcess();
            Set_SPEED_Value(Machine_Data.System_SPEED_Min);
            RM6_Task_Adder(Set_SPEED);
            RM6_Task_Adder(Motor_FR);
        }
        
        if( KeyCatch(0,1 , Stop) || PauseKey()){
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
        
        //F_Number_buffer_Auto( Stay, PauseTimeOut_Cnt , 35 ,DEC ,0);
        F_Time_buffer(5 ,1 , PauseTimeOut_Cnt);
        writeLEDMatrix();
       
        if(PauseTimeOut_Cnt == 0){
            IntoSummaryMode_Process();
            
            System_INCLINE = 0;
            RM6_Task_Adder(Set_INCLINE);
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
          case FIT_ARMY:
          case FIT_NAVY:
          case FIT_AIRFORCE: 
          case FIT_USMC:
          case FIT_WFI: 
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