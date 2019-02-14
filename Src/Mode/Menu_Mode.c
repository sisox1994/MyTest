#include "system.h"

Program_Type_Def Program_Select = Quick_start;
Program_Data_Def Program_Data;


void IntoIdleMode_Process(){

    Pace_Display_Switch    = 0;
    Calories_Display_Type  = Cal_;
    Dist_Display_Type      = DIST;
    HeartRate_Display_Type = HR;
    
    System_Mode = Idle;
    F_BtmReply39Cmd();//立馬告訴APP 進入Idle了 
    
    F_SetFEC_State(READY);
    
    Program_Select = Quick_start;
    Cloud_Run_Initial_Busy_Flag = 0;
    Program_Init();
    ClearStd_1_Sec_Cnt();
    
    
}

void Program_Init(){

    switch(Program_Select){
 
      case Quick_start:
        Quick_Start_Init();
        break;
      case Manual:
        Manual_Init();
        break;
      case Random: 
        Random_Init();
        break; 
      case CrossCountry: 
        CrossCountry_Init();
        break;
      case WeightLoss: 
        WeightLoss_Init();
        break;
      case Interval_1_1: 
        Interval_1_1_Init();
        break;
      case Interval_1_2:
        Interval_1_2_Init();
         break;
      case Hill:  
        Hill_Init(); 
        break;
        
        
      case Target_HeartRate_Goal:
        Target_HeartRate_Goal_Init();
        break;
      case Fat_Burn: 
        Fat_Burn_Init();
        break;
      case Cardio:   
        Cardio_Init();
        break;
      case Heart_Rate_Hill: 
        Heart_Rate_Hill_Init();  
        break;
      case Heart_Rate_Interval: 
        Heart_Rate_Interval_Init();  
        break;
      case Extreme_Heart_Rate: 
        Extreme_Heart_Rate_Init();
        break;
        
        
      case Hill_Climb:   
        Hill_Climb_Init();
        break;
      case Aerobic: 
        Aerobic_Init();
        break;
      case Interval_1_4: 
        Interval_1_4_Init();
        break;
      case Interval_2_1:
        Interval_2_1_Init();
        break;
      case EZ_INCLINE:  
        EZ_INCLINE_Init();
        break;
      case MARATHON_Mode:
        MARATHON_Init();
        break;
        
        
      case Calorie_Goal:
        Calorie_Goal_Init();
        break;
      case Distance_Goal_160M:
        Distance_Goal_160M_Init();
        break;
      case Distance_Goal_5K:
        Distance_Goal_5K_Init();
        break;
      case Distance_Goal_10K:
        Distance_Goal_10K_Init();
        break; 
        
      case Custom_1: 
        Custom_1_Init();
        break; 
      case Custom_2: 
        Custom_2_Init();
        break; 
      case User_1: 
        User_1_Init();
        break; 
      case User_2:   
        User_2_Init();
        break; 
        
      case FIT_ARMY:
        FIT_ARMY_Init();
        break; 
      case FIT_NAVY:
        FIT_NAVY_Init();
        break; 
      case FIT_AIRFORCE: 
        FIT_AIRFORCE_Init();
        break; 
      case FIT_USMC:
        FIT_USMC_Init();
        break; 
      case FIT_WFI: 
        FIT_WFI_Init();
        break; 
        
        
        
    }
    

}

void User_Like_Program_Init(){

    switch(Program_Data.Like_Program){
 
      case Manual:
        Manual_Init();
        break;
      case Random: 
        Random_Init();
        break; 
      case CrossCountry: 
        CrossCountry_Init();
        break;
      case WeightLoss: 
        WeightLoss_Init();
        break;
      case Interval_1_1: 
        Interval_1_1_Init();
        break;
      case Interval_1_2:
        Interval_1_2_Init();
         break;
      case Hill:  
        Hill_Init(); 
        break;
        
        
      case Target_HeartRate_Goal:
        Target_HeartRate_Goal_Init();
        break;
      case Fat_Burn: 
        Fat_Burn_Init();
        break;
      case Cardio:   
        Cardio_Init();
        break;
      case Heart_Rate_Hill: 
        Heart_Rate_Hill_Init();  
        break;
      case Heart_Rate_Interval: 
        Heart_Rate_Interval_Init();  
        break;
      case Extreme_Heart_Rate: 
        Extreme_Heart_Rate_Init();
        break;
        
        
      case Hill_Climb:   
        Hill_Climb_Init();
        break;
      case Aerobic: 
        Aerobic_Init();
        break;
      case Interval_1_4: 
        Interval_1_4_Init();
        break;
      case Interval_2_1:
        Interval_2_1_Init();
        break;
      case EZ_INCLINE:  
        EZ_INCLINE_Init();
        break;
      case MARATHON_Mode:
        MARATHON_Init();
        break;
        
        
      case Calorie_Goal:
        Calorie_Goal_Init();
        break;
      case Distance_Goal_160M:
        Distance_Goal_160M_Init();
        break;
      case Distance_Goal_5K:
        Distance_Goal_5K_Init();
        break;
      case Distance_Goal_10K:
        Distance_Goal_10K_Init();
        break; 
         
    }
    

}



void Idle_Display(){
    
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;
        
        
        switch(Program_Select){

            
          case Quick_start:
            //F_String_buffer_Auto( Left, "ATTACUS       FITNESS  " ,50 ,0);
            F_String_buffer_Auto( Left, "WELCOME" ,60 ,0);
            break;
          case Manual:
          case Random: 
          case CrossCountry: 
          case WeightLoss:   
          case Interval_1_1: 
          case Interval_1_2:
          case Hill: 
            DrawBarArray(Program_Data.BarArray_Display);
            break;
            
            
          case Target_HeartRate_Goal:
            F_String_buffer_Auto_Middle( Stay, "THR" ,35 ,0);
            break;
          case Fat_Burn:
            Draw(21 ,0 ,Heart_Solid ,35);
            F_String_buffer_Auto(Stay,"     65" ,35 ,0);
            /*
            switch(str_cNt){
              case 0:
                Draw(21 ,0 ,Heart_Empty ,500);
                F_String_buffer_Auto(Stay,"     65" ,500 ,0);
                str_cNt++;
                str_cNt = str_cNt%2;
                break; 
              case 1:
                Draw(21 ,0 ,Heart_Solid ,500);
                F_String_buffer_Auto(Stay,"     65" ,500 ,0);
                str_cNt++;
                str_cNt = str_cNt%2;
                break; 
            }*/
            break;
          case Cardio:    
            Draw(21 ,0 ,Heart_Solid ,35);
            F_String_buffer_Auto(Stay,"     80" ,35 ,0);
            
            /*
            switch(str_cNt){
              case 0:
                Draw(21 ,0 ,Heart_Empty ,500);
                F_String_buffer_Auto(Stay,"     80" ,500 ,0);
                str_cNt++;
                str_cNt = str_cNt%2;
                break; 
              case 1:
                Draw(21 ,0 ,Heart_Solid ,500);
                F_String_buffer_Auto(Stay,"     80" ,500 ,0);
                str_cNt++;
                str_cNt = str_cNt%2;
                break; 
            }*/
            break;
            
          case Heart_Rate_Hill:
          case Heart_Rate_Interval:
          case Extreme_Heart_Rate: 
            DrawBarArray(Program_Data.BarArray_Display);
            break;
            
            
          case Hill_Climb:
          case Aerobic:     
          case Interval_1_4: 
          case Interval_2_1: 
            DrawBarArray(Program_Data.BarArray_Display);
            break;
          case EZ_INCLINE:   
             Draw(7 ,0 ,EZ_I_Icon ,30);
             break;
          case MARATHON_Mode:
            F_String_buffer_Auto_Middle(Stay,"MAR" ,30 ,0);
            break;
            

          case Calorie_Goal:
            F_String_buffer_Auto_Middle(Stay,"CAL" ,30 ,0);
            break;
          case Distance_Goal_160M:
            
            if(System_Unit == Metric){  
                F_String_buffer_Auto_Middle(Stay,"1.6K" ,30 ,0);
            }else if(System_Unit == Imperial){
                F_String_buffer_Auto_Middle(Stay,"1M" ,30 ,0);
            }
            
            break;
          case Distance_Goal_5K:
            
            if(System_Unit == Metric){  
                F_String_buffer_Auto_Middle(Stay,"5K" ,30 ,0);
            }else if(System_Unit == Imperial){
                F_String_buffer_Auto_Middle(Stay,"3M" ,30 ,0);
            }
            
            
            break;
          case Distance_Goal_10K:
            
            if(System_Unit == Metric){  
                F_String_buffer_Auto_Middle(Stay,"10K" ,30 ,0);
            }else if(System_Unit == Imperial){
                F_String_buffer_Auto_Middle(Stay,"6M" ,30 ,0);
            }
            
            break;
            
            
          case Custom_1: 
            //F_String_buffer_Auto_Middle(Stay,"CSTM1" ,30 ,0);
            DrawBarArray(Program_Data.BarArray_Display);
            break; 
          case Custom_2: 
            //F_String_buffer_Auto_Middle(Stay,"CSTM2" ,30 ,0);
            DrawBarArray(Program_Data.BarArray_Display);
            break; 
          case User_1: 
            F_String_buffer_Auto_Middle(Stay,"USER1" ,30 ,0);
            break; 
          case User_2:   
            F_String_buffer_Auto_Middle(Stay,"USER2" ,30 ,0);
            break; 
            
          case FIT_ARMY: 
            F_String_buffer_Auto_Middle(Stay,"ARMY" ,30 ,0);
            break; 
          case FIT_NAVY: 
            F_String_buffer_Auto_Middle(Stay,"NAVY" ,30 ,0);
            break; 
          case FIT_AIRFORCE: 
            F_String_buffer_Auto_Middle(Stay,"AIRF" ,30 ,0);
            break; 
          case FIT_USMC:   
            F_String_buffer_Auto_Middle(Stay,"USMC" ,30 ,0);
            break; 
          case FIT_WFI:   
            F_String_buffer_Auto_Middle(Stay,"WFI" ,30 ,0);
            break; 
            
        }
        
        
        switch(Program_Select){
        
          case Quick_start:            //上數
          case MARATHON_Mode:
          case Calorie_Goal:
          case Distance_Goal_160M:
          case Distance_Goal_5K:
          case Distance_Goal_10K:
          case FIT_ARMY:
          case FIT_NAVY:
          case FIT_AIRFORCE: 
          case FIT_USMC:
          case FIT_WFI: 
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
            SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time);   //下數
            break;
        }

        
        
        writeSegmentBuffer();
        writeLEDMatrix();       
    }
    
}

Key_Name_Def Key_Name_Temp;

unsigned char ClassCnt;

void KeyChangeDetect(Key_Name_Def key){

    if(Key_Name_Temp != key){
        Key_Name_Temp = key;
        
        if((Program_Select != User_1)&&(Program_Select != User_2)){
            Program_Select = Quick_start;
        }
        
        
        ClassCnt = 0;
    }
    
}



unsigned char no_Beep_Flag;

extern void Enter_STOPMode_Process();

extern UART_HandleTypeDef huart2;

unsigned char AutoPause_Flag = 1; //隱藏功能

void Idle_Key(){
     
 
    

    HR_SENSOR_LINK_Key(); 
    
    
    if( KeyCatch(5,3 , Num_1,Num_5,Num_9) ){ 
    
        if(AutoPause_Flag == 0){
            AutoPause_Flag = 1;
        }else{
            AutoPause_Flag = 0;
        }
    }
    
    if( KeyCatch(0,1 , Key_Manual) ){ 
        KeyChangeDetect(Key_Manual);
        Program_Select = (Program_Type_Def)(1 + (ClassCnt % 7));
        ClassCnt++;
        Program_Init();
        str_cNt = 0;
        //writeLEDMatrix();
    } 
    
    if( KeyCatch(0,1 , Key_HRC) ){ 
        KeyChangeDetect(Key_HRC);
        Program_Select = (Program_Type_Def)(11 + (ClassCnt % 6));
        ClassCnt++;
        Program_Init();
        str_cNt = 0;
        //writeLEDMatrix(); 
    }
    
    if( KeyCatch(0,1 , Key_Advance) ){ 
        KeyChangeDetect(Key_Advance);
        Program_Select = (Program_Type_Def)(22 + (ClassCnt % 6)  );
        ClassCnt++;
        Program_Init();
        str_cNt = 0;
        //writeLEDMatrix();
    }

    if( KeyCatch(0,1 , Key_Goal) ){ 
        KeyChangeDetect(Key_Goal);
        Program_Select = (Program_Type_Def)(31 + (ClassCnt % 4)  );
        ClassCnt++;
        Program_Init();
        str_cNt = 0; 
    }
    
    if( KeyCatch(0,1 , Key_Custom) ){
        KeyChangeDetect(Key_Custom);
        Program_Select = (Program_Type_Def)(41 + (ClassCnt % 4)  );
        ClassCnt++;
        Program_Init();
        str_cNt = 0; 
    } 
    
    if( KeyCatch(0,1 , Key_FitTest) ){
        KeyChangeDetect(Key_FitTest);
        Program_Select = (Program_Type_Def)(51 + (ClassCnt % 5));
        ClassCnt++;
        Program_Init();
        str_cNt = 0; 
    }   
        
    
    if( KeyCatch(0,1 , Enter) ){
        if(Program_Select){
            switch(Program_Select){
              case Quick_start:
              case Manual:
              case Random: 
              case CrossCountry: 
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Time;
                str_cNt = 0;
                break;
                
              case WeightLoss: 
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Age;
                str_cNt = 0;
                break;
              case Interval_1_1: 
              case Interval_1_2:
              case Hill:  
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Time;
                str_cNt = 0;
                break;
                
              case Target_HeartRate_Goal:
              case Fat_Burn:
              case Cardio: 
              case Heart_Rate_Hill:   
              case Heart_Rate_Interval: 
              case Extreme_Heart_Rate: 
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Time;
                str_cNt = 0;
                break;
                
              case Hill_Climb: 
              case Aerobic:   
              case Interval_1_4: 
              case Interval_2_1: 
              case EZ_INCLINE: 
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Time;
                str_cNt = 0;
                break;
              case MARATHON_Mode: 
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Age;
                str_cNt = 0;
                break;
                
              case Calorie_Goal:
              case Distance_Goal_160M:
              case Distance_Goal_5K:
              case Distance_Goal_10K:
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Age;
                str_cNt = 0;
                break;
                
              case Custom_1: 
              case Custom_2:
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Time;
                str_cNt = 0;
                break; 
              case User_1: 
              case User_2:
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_LIKE_Prog;
                str_cNt = 0;
                break; 
                
              case FIT_ARMY:
              case FIT_NAVY:
              case FIT_AIRFORCE: 
              case FIT_USMC:
              case FIT_WFI: 
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_GENDER;
                str_cNt = 0;
                break; 
            }
        }else{
            if(Program_Select == Quick_start){  //快速啟動模式按下Enter 就變成Manual Mode
                Program_Select = Manual;
                Program_Init();
                System_Mode = Prog_Set; 
                Setting_item_Index = SET_Time;
                str_cNt = 0; 
            }
        }
        ClearBlinkCnt2();
    }
    
    if( KeyCatch(0,1 , Stop) || PauseKey()){
        if(Program_Select){
            Program_Select = Quick_start;
            str_cNt = 0;
            ClassCnt = 0;
        } 
    }
    
    if( KeyCatch(0,1 , Start)){
        
        if(Program_Select == User_1 || Program_Select == User_2){
            Program_Select = Program_Data.Like_Program; 
        }else if(Program_Select == Quick_start){
            Quick_Start_Init();
        }
        
        IntoReadyMode_Process();
        
        //-------------沒有心跳  禁止進入---------------
        switch(Program_Select){
          case Target_HeartRate_Goal:
          case Fat_Burn: 
          case Cardio:   
          case Heart_Rate_Hill: 
          case Heart_Rate_Interval: 
          case Extreme_Heart_Rate: 
          case FIT_WFI: 
            if(!usNowHeartRate){
                HR_BlinkOneTime_Cnt = 2;
                System_Mode = Idle;
            }
            break;
            
        }
        
    }    
}

extern void Idel_detect();
void Idle_Func(){

    Idle_Key();
    Idle_Display();
    Idel_detect();
    
    if(T1s_Flag){     //時間計數  單位:秒
        T1s_Flag = 0;
        
        Btm_Task_Adder(FTMS_Data_Broadcast);
    }
}