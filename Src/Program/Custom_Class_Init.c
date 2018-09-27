#include "system.h"



void Custom_1_Init(){

    UserData_Init(); 
    
    Program_Data.Age     = MyCustom_1.Custom_Age;
    Program_Data.Weight  = MyCustom_1.Custom_Weight;
    
    Program_Data.Goal_Time    = MyCustom_1.Custom_Time; 
    Program_Data.Goal_Counter = MyCustom_1.Custom_Time;
    
    Program_Data.WorkTime = MyCustom_1.Custom_WorkTime;
    Program_Data.RestTime = MyCustom_1.Custom_RestTime;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = MyCustom_1.Custom_RestTime  + MyCustom_1.Custom_WorkTime;
    
    for(unsigned char i = 0;i < Program_Data.RestTime; i++){
        Program_Data.INCLINE_Template_Table[i] = 0;
    }
    for(unsigned char i = Program_Data.RestTime;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = 4;
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Program_Data.INCLINE_Template_Table[ i % Program_Data.Template_Table_Num]; 
    }
    
    
    for(unsigned char i = 0 ; i < 32; i++){
        
        if(Program_Data.INCLINE_Table_96[i] >= 0x00 ){
            
            if(Program_Data.INCLINE_Table_96[i] > 30){
                Program_Data.BarArray_Display[i] = Index_To_Bar[30];
            }else{
                Program_Data.BarArray_Display[i] = Index_To_Bar[ Program_Data.INCLINE_Table_96[i] ];
            }
            
        }else if(Program_Data.INCLINE_Table_96[i] < 0){
            
            Program_Data.BarArray_Display[i] = Index_To_Bar[0];          //小於0 就顯示1顆
        } 
        
    }
    
    if(Program_Data.Goal_Time >= 1920){     //>32分鐘
        
        Program_Data.PeriodWidth  = 60;                              //一格區間大於60  就=60
        Program_Data.PeriodNumber = Program_Data.Goal_Time / 60;     //算總共有幾個區間
        
        Program_Data.NextPeriodValue = Program_Data.Goal_Time - Program_Data.PeriodWidth;
        
    }else if(Program_Data.Goal_Time < 1920){     //<32分鐘
        
        Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
        Program_Data.PeriodNumber = 32;
        
        Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;
        
    }
    
}


void Custom_2_Init(){

    UserData_Init(); 
    
    Program_Data.Age     = MyCustom_2.Custom_Age;
    Program_Data.Weight  = MyCustom_2.Custom_Weight;
    
    Program_Data.Goal_Time    = MyCustom_2.Custom_Time; 
    Program_Data.Goal_Counter = MyCustom_2.Custom_Time;
    
    Program_Data.WorkTime = MyCustom_2.Custom_WorkTime;
    Program_Data.RestTime = MyCustom_2.Custom_RestTime;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = MyCustom_2.Custom_RestTime  + MyCustom_2.Custom_WorkTime;
    
    for(unsigned char i = 0;i < Program_Data.RestTime; i++){
        Program_Data.INCLINE_Template_Table[i] = 0;
    }
    for(unsigned char i = Program_Data.RestTime;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = 4;
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Program_Data.INCLINE_Template_Table[ i % Program_Data.Template_Table_Num]; 
    }
    
    
    for(unsigned char i = 0 ; i < 32; i++){
        
        if(Program_Data.INCLINE_Table_96[i] >= 0x00 ){
            
            if(Program_Data.INCLINE_Table_96[i] > 30){
                Program_Data.BarArray_Display[i] = Index_To_Bar[30];
            }else{
                Program_Data.BarArray_Display[i] = Index_To_Bar[ Program_Data.INCLINE_Table_96[i] ];
            }
            
        }else if(Program_Data.INCLINE_Table_96[i] < 0){
            
            Program_Data.BarArray_Display[i] = Index_To_Bar[0];          //小於0 就顯示1顆
        } 
        
    }
    
    if(Program_Data.Goal_Time >= 1920){     //>32分鐘
        
        Program_Data.PeriodWidth  = 60;                              //一格區間大於60  就=60
        Program_Data.PeriodNumber = Program_Data.Goal_Time / 60;     //算總共有幾個區間
        
        Program_Data.NextPeriodValue = Program_Data.Goal_Time - Program_Data.PeriodWidth;
        
    }else if(Program_Data.Goal_Time < 1920){     //<32分鐘
        
        Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
        Program_Data.PeriodNumber = 32;
        
        Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;
        
    }
    
    
}



void User_1_Init(){
    
    Program_Data.Like_Program = MyUser_1.User_Program_Like;

    switch(Program_Data.Like_Program ){
        
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
    
    //-------------------------------------------------------------
    Program_Data.Goal_Time    = MyUser_1.User_Time;
    Program_Data.Goal_Counter = MyUser_1.User_Time;
    
    Program_Data.Age     = MyUser_1.User_Age;
    Program_Data.Weight  = MyUser_1.User_Weight;
    
    Program_Data.TargetHeartRate = MyUser_1.User_THR;
    Program_Data.Ez_MaxINCLINE   = MyUser_1.User_Ez_MaxINCLINE;
    Program_Data.Calories_Goal   = MyUser_1.User_Calories_Goal;
    Program_Data.Diffculty_Level = MyUser_1.Diffculty_Level;
    
    
    switch(Program_Data.Like_Program){
        
      case Heart_Rate_Hill:     
      case Heart_Rate_Interval: 
      case Extreme_Heart_Rate:  
        Time_Change_Process_HRC_Hill_Init();
        break;
      case EZ_INCLINE: 
        Setting_item_Index = SET_Time;
        EZ_MAX_INC_Change_Process();
        Time_Change_Process_Ez_INCLINE();
        break;
      default:
        Time_Change_Process();    //有修改時間的話  重新編 TimePeriod Table
        break;
    }
    
    /*
    switch(Program_Data.Like_Program ){
        
      case WeightLoss: 
      case MARATHON_Mode:
      case Calorie_Goal: 
      case Distance_Goal_160M:
      case Distance_Goal_5K:
      case Distance_Goal_10K:
      
        break; 
        
      default:
        Program_Data.Goal_Time    = MyUser_1.User_Time; 
        Program_Data.Goal_Counter = MyUser_1.User_Time;
         break; 
    }*/
    
    
    
}




void User_2_Init(){

    Program_Data.Like_Program = MyUser_2.User_Program_Like;

    switch(Program_Data.Like_Program ){
        
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
    
    //-------------------------------------------------------------
    Program_Data.Goal_Time    = MyUser_2.User_Time;
    Program_Data.Goal_Counter = MyUser_2.User_Time;
    
    Program_Data.Age     = MyUser_2.User_Age;
    Program_Data.Weight  = MyUser_2.User_Weight;
    
    Program_Data.TargetHeartRate = MyUser_2.User_THR;
    Program_Data.Ez_MaxINCLINE   = MyUser_2.User_Ez_MaxINCLINE;
    Program_Data.Calories_Goal   = MyUser_2.User_Calories_Goal;
    Program_Data.Diffculty_Level = MyUser_2.Diffculty_Level;
    
    
    switch(Program_Data.Like_Program){
        
      case Heart_Rate_Hill:     
      case Heart_Rate_Interval: 
      case Extreme_Heart_Rate:  
        Time_Change_Process_HRC_Hill_Init();
        break;
      case EZ_INCLINE: 
        Setting_item_Index = SET_Time;
        EZ_MAX_INC_Change_Process();
        Time_Change_Process_Ez_INCLINE();
        break;
      default:
        Time_Change_Process();    //有修改時間的話  重新編 TimePeriod Table
        break;
    }
    
    /*
    switch(Program_Data.Like_Program ){
        
      case WeightLoss: 
      case MARATHON_Mode:
      case Calorie_Goal: 
      case Distance_Goal_160M:
      case Distance_Goal_5K:
      case Distance_Goal_10K:
        
        break; 
        
      default:
        Program_Data.Goal_Time    = MyUser_2.User_Time; 
        Program_Data.Goal_Counter = MyUser_2.User_Time;
        break; 
    }*/
    
    
}

