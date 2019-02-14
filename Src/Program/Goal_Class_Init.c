#include "system.h"

void Calorie_Goal_Init(){
    
    UserData_Init(); 
    
    Program_Data.Calories_Goal = 500;
    
    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;

    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
    
    /*
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Manual_Profile_Template[i];
    }
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Manual_Profile_Template[ i % Program_Data.Template_Table_Num]; 
    }*/
    
    Table_96_To_BarArray_Mapping();
  
}

unsigned char INCLINE_HIGH_Limit_Table[21] = {0,10,20,30,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120};
unsigned char INCLINE_LOW_Limit_Table[21]  = {0,0, 0, 0, 0, 5,10,15,20,25,30,35,40,45,50,55, 60, 65, 70, 75, 80};


void Distance_Goal_160M_Init(){
    
    UserData_Init(); 
        
    
    Program_Data.Diffculty_Level = 8;
    
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = 1600;
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = 1000;
    }

    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;

    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
    /*
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Manual_Profile_Template[i];
    }
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Manual_Profile_Template[ i % Program_Data.Template_Table_Num]; 
    }*/
    
    Table_96_To_BarArray_Mapping();

    //Program_Data.PeriodNumber = 32;
    //Program_Data.PeriodWidth = Program_Data.Distance_Goal/32;
    //Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;    

}


void Distance_Goal_5K_Init(){

    
    UserData_Init(); 
    Program_Data.Diffculty_Level = 8;
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = 5000;
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = 3000;
    }

    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
    
    /*
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Manual_Profile_Template[i];
    }
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Manual_Profile_Template[ i % Program_Data.Template_Table_Num]; 
    }*/
    
    Table_96_To_BarArray_Mapping();
   
}


void Distance_Goal_10K_Init(){
    
    UserData_Init(); 
        
    Program_Data.Diffculty_Level = 8;
    

    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = 10000;
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = 6000;
    }
    
    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;

    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
    /*for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Manual_Profile_Template[i];
    }
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Manual_Profile_Template[ i % Program_Data.Template_Table_Num]; 
    }*/
  
    Table_96_To_BarArray_Mapping();



}
