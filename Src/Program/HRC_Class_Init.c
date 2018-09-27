#include "system.h"



void Time_Change_Process_HRC_Hill_In_WorkOut(short TimeChangeValue){


    __asm("NOP");
   

    NowPeriodLimitValue_Temp = Program_Data.NextPeriodValue + TimeChangeValue;
    RestOfPeriod_Temp        = 32 - (Program_Data.NowPeriodIndex + 1 );
  

    if(NowPeriodLimitValue_Temp / RestOfPeriod_Temp >= 30){
    
        Program_Data.Goal_Time    +=TimeChangeValue;
        Program_Data.Goal_Counter +=TimeChangeValue;
        
        
        //現在移動到的區間 區間最大分隔值   (時間增加減少後 )
        Program_Data.NowPeriodLimitValue = NowPeriodLimitValue_Temp;
        
        //32格 減去現在 這格 Period index  算剩下可顯示的格數     
        Program_Data.RestOfPeriod = RestOfPeriod_Temp;
 
        Program_Data.PeriodWidth  = Program_Data.NowPeriodLimitValue / Program_Data.RestOfPeriod;
        
        Program_Data.PeriodNumber = 32; 
        
        Program_Data.NextPeriodValue = Program_Data.NextPeriodValue + TimeChangeValue; 
        

            __asm("NOP");
            
    }else if(RestOfPeriod_Temp < 30){
        __asm("NOP");   
    }
    
}


void Time_Change_Process_HRC_Hill_Init(){


    __asm("NOP");
   
    if(Program_Data.Goal_Counter <=960){
        Program_Data.Goal_Time    = 960;
        Program_Data.Goal_Counter = 960;    
    }

    Program_Data.PeriodNumber = 32; 
    Program_Data.PeriodWidth  = Program_Data.Goal_Time / Program_Data.PeriodNumber;
    

    Program_Data.NextPeriodValue = Program_Data.Goal_Time - Program_Data.PeriodWidth;
    
}


void Target_HeartRate_Goal_Init(){
    
    

    UserData_Init(); 
    Program_Data.Goal_Time    = 1800;  //15360
    Program_Data.Goal_Counter = 1800;
    
    //User_Stored_Setting_Init();
   
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Manual_Profile_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Manual_Profile_Template[ i % Program_Data.Template_Table_Num]; 
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


void Fat_Burn_Init(){
    
  
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800;  //15360
    Program_Data.Goal_Counter = 1800;

    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Manual_Profile_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Manual_Profile_Template[ i % Program_Data.Template_Table_Num]; 
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
    
    Program_Data.HR_Range_Low  = (Program_Data.TargetHeartRate * 60) /100;
    Program_Data.HR_Range_High = (Program_Data.TargetHeartRate * 72) /100;

}


void Cardio_Init(){
    
    
    
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800;  //15360
    Program_Data.Goal_Counter = 1800;
    
    //User_Stored_Setting_Init();
        
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Manual_Profile_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Manual_Profile_Template[ i % Program_Data.Template_Table_Num]; 
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
    
    Program_Data.HR_Range_Low = (Program_Data.TargetHeartRate * 72) /100;
    Program_Data.HR_Range_High   = (Program_Data.TargetHeartRate * 85) /100;
    
}


unsigned char Heart_Rate_Hill_Template[32] = {0 , 0 , 0,  0,
                                               4 , 4 , 4,  4,
                                               8 , 8 , 8,  8,
                                              12 ,12 ,12, 12,
                                              16 ,16 ,16, 16,
                                              20 ,20 ,20, 20,
                                              24 ,24 ,24, 24,
                                               4 , 4 , 0,  0 };


unsigned char HR_Hill_Rate_Table[32] =  {65 ,65 ,65, 65,
                                          70 ,70 ,70, 70,
                                          65 ,65 ,65, 65,
                                          75 ,75 ,75, 75,
                                          65 ,65 ,65, 65,
                                          70 ,70 ,70, 70,
                                          80 ,80 ,80, 80,
                                          65 ,65 ,65, 65};
void Heart_Rate_Hill_Init(){
    
    
    
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800;  //15360
    Program_Data.Goal_Counter = 1800;
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 32;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Heart_Rate_Hill_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Heart_Rate_Hill_Template[ i % Program_Data.Template_Table_Num]; 
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
    //----------------------------Hill  %  Table--------------
    for(unsigned char i = 0; i < 32; i++){
        Program_Data.HRC_INCLINE_Rate_Table[i] = HR_Hill_Rate_Table[i];
    }
    
    
   //-----------固定32格   一個區間允許>60秒的情形---------------------------
    
    Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
    Program_Data.PeriodNumber = 32;
    
    Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;
    

    
}



unsigned char Heart_Rate_Interval_Template[6] = {4,4,4,24,24,24};
unsigned char HR_Interval_Rate_Table[6] =  {65 ,65 ,65, 80, 80 ,80};

void Heart_Rate_Interval_Init(){
    
   
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800;  //15360
    Program_Data.Goal_Counter = 1800;
    
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 6;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Heart_Rate_Interval_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Heart_Rate_Interval_Template[ i % Program_Data.Template_Table_Num]; 
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
    //----------------------------Hill  %  Table--------------
    for(unsigned char i = 0; i < 32; i++){
        Program_Data.HRC_INCLINE_Rate_Table[i] = HR_Interval_Rate_Table[i% Program_Data.Template_Table_Num];
    }
    
    
    //-----------固定32格   一個區間允許>60秒的情形---------------------------
    
    Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
    Program_Data.PeriodNumber = 32;
    
    Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;
    
    
    
}

unsigned char Extreme_Heart_Rate_Template[9] = {0 ,0 ,28,24,20,16,12,8,4} ;
unsigned char HR_Extreme_Rate_Table[9]       = {65,65,85,85,85,70,70,65,65};


void Extreme_Heart_Rate_Init(){
    
     
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800;  //15360
    Program_Data.Goal_Counter = 1800;  
    
    //User_Stored_Setting_Init();
   
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 9;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Extreme_Heart_Rate_Template[i];
    }
    
    Program_Data.INCLINE_Table_96[0] = 0;
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Extreme_Heart_Rate_Template[i % Program_Data.Template_Table_Num]; 
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
    //----------------------------Hill  %  Table--------------
    for(unsigned char i = 0; i < 32; i++){
        Program_Data.HRC_INCLINE_Rate_Table[i] = HR_Extreme_Rate_Table[i% Program_Data.Template_Table_Num];
    }
    
    
    //-----------固定32格   一個區間允許>60秒的情形---------------------------
    
    Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
    Program_Data.PeriodNumber = 32;
    
    Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;
    
    
    
    
    
}


