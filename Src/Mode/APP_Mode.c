#include "system.h"

unsigned short stageNumber;
void Cloud_Run_Program_Init(){

    UserData_Init(); 
    
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = uiAppTotalDist;
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = (uiAppTotalDist * 10)/16;
    }
    
    
    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;


    Program_Data.PeriodNumber = uiAppTotalDist/uiAppStageDist + 1;

    for(unsigned int i = 0 ; i < Program_Data.PeriodNumber; i++){
        if(i < 96){
            Program_Data.INCLINE_Table_96[i] = ucAppINCBuffer[i]; 
        }
        
    }
    for(unsigned int i = Program_Data.PeriodNumber; i < 96;  i++){
        if(i < 96){
            Program_Data.INCLINE_Table_96[i] = 0; 
        }
       
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

    
    if(System_Unit == Metric){  
        Program_Data.PeriodWidth  = uiAppStageDist; 
    }else if(System_Unit == Imperial){
        Program_Data.PeriodWidth  = (uiAppStageDist * 10)/16; 
    }
    
    Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;


}


void APP_Display(){
    
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;
        
 
        F_String_buffer_Auto_Middle( Stay, "APP" ,50 ,0);
        SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
        Program_Select = APP_Cloud_Run;
        
        writeSegmentBuffer();
        writeLEDMatrix();       
    }
    
}


void APP_Key(){
     
    if( KeyCatch(0,1 , Inc_15) ){
        
    }   
    
    if( KeyCatch(0,1 , Inc_10) ){
        

    }    
    
    if( KeyCatch(0,1 , Key_Manual) ){ 
        
    } 
    if( KeyCatch(0,1 , Key_HRC) ){ 
        
        
    }
    
    if( KeyCatch(0,1 , Key_Advance) ){ 
 
    }
    
    if( KeyCatch(0,1 , BLE) ){ 
        
        
    }
    
    if( KeyCatch(0,1 , Enter) ){
        

    }
    
    if( KeyCatch(0,1 , Stop) ){
        System_Mode    = Menu;
        Program_Select = Quick_start;
    }
    
    if( KeyCatch(0,1 , Start) ){
        
      
        
    }    
}



void APP_Mode_Func(){
     
    APP_Key();
    APP_Display();
    
}