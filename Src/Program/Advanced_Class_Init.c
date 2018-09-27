#include "system.h"




unsigned char Hill_Climb_Template[32] = {0,0,
                                          8,8,8,8,
                                          12,12,12,12,12,12,
                                          16,16,16,16,16,16,16,16,
                                          20,20,20,20,20,20,
                                          16,16,16,16,
                                          12,12};

void Hill_Climb_Init(){
    
     
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;
    

    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 32;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Hill_Climb_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Hill_Climb_Template[ i % Program_Data.Template_Table_Num]; 
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


//---------------------------------------------------------------------------------   

unsigned char Aerobic_Template[7] = {0,4,8,12,12,8,4};


void Aerobic_Init(){
    
  
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 7;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Aerobic_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Aerobic_Template[ i % Program_Data.Template_Table_Num]; 
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
//----------------------------------------------------------------------------------


unsigned char Interval_1_4_Template[5] = {0,0,0,0,20};

void Interval_1_4_Init(){
    
   
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;

    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 5;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Interval_1_4_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Interval_1_4_Template[ i % Program_Data.Template_Table_Num]; 
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

unsigned char Interval_2_1_Template[3] = {0,0,20};


void Interval_2_1_Init(){
    

    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;

    //User_Stored_Setting_Init();

    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 3;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Interval_2_1_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Interval_2_1_Template[ i % Program_Data.Template_Table_Num]; 
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
    
 
    
}

void Time_Change_Process_Ez_INCLINE(){
    
    Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
    Program_Data.PeriodNumber = 32;
    Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;
    
}

unsigned char EZ_INCLINE_Template[32];


void EZ_MAX_INC_Change_Process(){
    
    for(unsigned char i = 0 ;i <24; i++){
        EZ_INCLINE_Template[i] = i * ((Program_Data.Ez_MaxINCLINE) /24)/5 ;
    }
    
    for(unsigned char i = 24 ;i <32; i++){
        EZ_INCLINE_Template[i] = (32 - i) * ((Program_Data.Ez_MaxINCLINE) /24)/5 ;
    }
    
    for(unsigned char i = 0;i <32; i++){
        Program_Data.INCLINE_Template_Table[i] = EZ_INCLINE_Template[i];
    }
    
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = EZ_INCLINE_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = EZ_INCLINE_Template[ i % Program_Data.Template_Table_Num]; 
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
    
}


void EZ_INCLINE_Init(){
   
    
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;
    

    //User_Stored_Setting_Init();

    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    Program_Data.Ez_MaxINCLINE = 75;
     
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 32;
    
    for(unsigned char i = 0 ;i <24; i++){
        EZ_INCLINE_Template[i] = i * ((Program_Data.Ez_MaxINCLINE) /24)/5 ;
    }
    
    for(unsigned char i = 24 ;i <32; i++){
        EZ_INCLINE_Template[i] = (32 - i) * ((Program_Data.Ez_MaxINCLINE) /24)/5 ;
    }
    
    for(unsigned char i = 0;i <32; i++){
        Program_Data.INCLINE_Template_Table[i] = EZ_INCLINE_Template[i];
    }
    
   
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = EZ_INCLINE_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = EZ_INCLINE_Template[ i % Program_Data.Template_Table_Num]; 
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
    
    //---- EZ-I  固定為32格
    Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
    Program_Data.PeriodNumber = 32;
    Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;
    

}


void MARATHON_Init(){
    
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    
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


