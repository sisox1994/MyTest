#include "system.h"

void IntoReadyMode_Process(){

    if(Program_Select == APP_Cloud_Run){
        Cloud_Run_Program_Init();
    }
    
    Time_Display_Type     = Remaining;    // 預設為 下數顯示
    Calories_Display_Type = Cal_;        // 預設為 卡路里顯示
    Pace_Display_Switch   = 0;           //預設為 總步數
    
    //Program_Select = (Program_Type_Def)0;
    str_cNt = 0;
    System_Mode = Ready;
   
    Buzzer_Set(150);
}

void Next_Mode_Process(){
    
    if(System_Unit == Metric  ){  
        Set_SPEED_Value(8);
    }else if(System_Unit == Imperial  ){
        Set_SPEED_Value(5);
    }
    
    RM6_Task_Adder(Set_SPEED);
    RM6_Task_Adder(Motor_FR);
    
    
    ClassCnt = 0;
    
    switch(Program_Select){            
      case Quick_start:
      case APP_Cloud_Run:
        IntoWorkoutModeProcess();   //Quick Start Mode 跳過Warm Up
        break;
      default:
        IntoWarmUpModeProcess();
        break;
    }
    
}

void Ready_Func(){

    ucSubSystemMode = C_App_toRunGoVal;

    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;
        
        switch(str_cNt){
          case 0:
            if(F_Number_buffer_Auto( Down_and_Stay, 3 , 35 ,DEC ,0)){
                str_cNt += 1;  
                Buzzer_Set(150);
            }           
            break;
          case 1:
            if(F_Number_buffer_Auto( Down_and_Stay, 2 , 35 ,DEC ,0)){
                str_cNt += 1;  
                Buzzer_Set(150);
            }    
            break;    
          case 2:
            if(F_Number_buffer_Auto( Down_and_Stay, 1 , 35 ,DEC ,0)){
                str_cNt += 1;
                Buzzer_Set(500);
            } 
            break;
          case 3:
            if(F_String_buffer_Auto_Middle( Up_and_Stay, "GO" ,40 ,0)){
                str_cNt += 1;
                //Buzzer_Set(200);
            }      
            break; 
          case 4:
            Next_Mode_Process();
            str_cNt = 0;
            break;   
        }

        switch(Program_Select){
          case Quick_start:
          case MARATHON_Mode:
          case Calorie_Goal:
          case Distance_Goal_160M:
          case Distance_Goal_5K:
          case Distance_Goal_10K:
          case APP_Cloud_Run:
            SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
            break;
            
          case User_1:
          case User_2:
            switch(Program_Data.Like_Program){
                
              case Quick_start:
              case MARATHON_Mode:
              case Calorie_Goal:
              case Distance_Goal_160M:
              case Distance_Goal_5K:
              case Distance_Goal_10K:
                SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
                break;
                
              default:
                SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time);
                break;
            }
            break;
            
            
          default:
            SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time);
            break;
        }

        writeLEDMatrix();
        writeSegmentBuffer();
        
    }

}