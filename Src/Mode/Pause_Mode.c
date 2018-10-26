#include "system.h"


unsigned short PauseTimeOut_Cnt = 300;

void IntoPauseMode_Process(){
    
    PauseTimeOut_Cnt = 300;
    System_Mode = Paused;
    ClearStd_1_Sec_Cnt(); 
    
}


void Pause_Key(){

    if( KeyCatch(0,1 , Start)  || PauseKey()){
        IntoWorkoutModeProcess();
        
        if(System_Unit == Metric  ){  
            Set_SPEED_Value(8);
        }else if(System_Unit == Imperial  ){
            Set_SPEED_Value(5);
        }
        RM6_Task_Adder(Set_SPEED);
        RM6_Task_Adder(Motor_FR);
        
    }
    
    if( KeyCatch(0,1 , Stop) ){
        IntoSummaryMode_Process();
        
        System_INCLINE = 0;
        RM6_Task_Adder(Set_INCLINE);
    }
    
}


void Pause_Func(){

    
    
    ucSubSystemMode = C_App_PauseVal;
  
    
    
    if(T1s_Flag){
        T1s_Flag = 0;
        
        F_Number_buffer_Auto( Stay, PauseTimeOut_Cnt , 35 ,DEC ,0);
        writeLEDMatrix();
        writeSegmentBuffer();

        if(PauseTimeOut_Cnt == 0){
            
            IntoSummaryMode_Process();
            
        }else{ 
            PauseTimeOut_Cnt--; 
        }
        
    }
    
    Pause_Key();

}