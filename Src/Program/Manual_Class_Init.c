#include "system.h"


//-----------------------****--------修改-----------------


unsigned int   NowPeriodLimitValue_Temp;
unsigned short RestOfPeriod_Temp;

int PeriodAfterDiffTempValue;  //確認減完的值是否大於 0用

void Time_Change_Process_InWorkout(short TimeChangeValue){

    
    
    __asm("NOP");
    //---------------------------計算要增加幾個period 或減少------------------------------------------
    
    //先算剩下的時間 除要跑的格數 (PeriodWidth) <30秒的話就 不允許執行時間的加減
    NowPeriodLimitValue_Temp = Program_Data.NextPeriodValue + TimeChangeValue;
    
    
    if(Program_Data.NowPeriodIndex > Program_Data.PeriodIndex_After_Shift ){
        
        RestOfPeriod_Temp = 32 - (Program_Data.PeriodIndex_After_Shift + 1);
        
    }else{
        RestOfPeriod_Temp        = 32 - (Program_Data.NowPeriodIndex + 1 );
    }
    
    
    if(NowPeriodLimitValue_Temp / RestOfPeriod_Temp >= 10){
    
        Program_Data.Goal_Time    +=TimeChangeValue;
        Program_Data.Goal_Counter +=TimeChangeValue;
        
        
        //現在移動到的區間 區間最大分隔值   (時間增加減少後 )
        Program_Data.NowPeriodLimitValue = NowPeriodLimitValue_Temp;
        
        //32格 減去現在 這格 Period index  算剩下可顯示的格數     
        Program_Data.RestOfPeriod = RestOfPeriod_Temp;
        
        
        //------------------------------------------------------------------------------------------------------------
        if( Program_Data.NowPeriodLimitValue / Program_Data.RestOfPeriod >= 60 ){   //剩下時間 除剩下格數  一格>=60的情形
            
            Program_Data.PeriodWidth  = 60;
            //29                   //28
            //Program_Data.PeriodNumber = 32 + (Program_Data.NowPeriodLimitValue / 60) - Program_Data.RestOfPeriod ;
                
            
            Program_Data.NextPeriodValue = Program_Data.NextPeriodValue + TimeChangeValue; 
            
            
            unsigned int Period_Cnt = Program_Data.NowPeriodIndex + 1;
            
            do{
                
                PeriodAfterDiffTempValue = Program_Data.NowPeriodLimitValue - 60 * (Period_Cnt-(Program_Data.NowPeriodIndex + 1) + 1);
                Program_Data.PeriodNumber = (Period_Cnt);  //取得新的 PeriodNumber
                
                Period_Cnt++;
            }while(PeriodAfterDiffTempValue > 0);
            
            
            __asm("NOP");
            
        }else if(Program_Data.NowPeriodLimitValue / Program_Data.RestOfPeriod < 60){
            
            Program_Data.PeriodWidth  = Program_Data.NowPeriodLimitValue / Program_Data.RestOfPeriod;
            
            Program_Data.PeriodNumber = 32 + (Program_Data.NowPeriodLimitValue / Program_Data.PeriodWidth) - Program_Data.RestOfPeriod;
            //Program_Data.PeriodNumber = 32;
            
            Program_Data.NextPeriodValue = Program_Data.NextPeriodValue + TimeChangeValue; 
            
            unsigned int Period_Cnt = Program_Data.NowPeriodIndex + 1;
            
            do{
                
                PeriodAfterDiffTempValue = Program_Data.NowPeriodLimitValue - Program_Data.PeriodWidth * (Period_Cnt-(Program_Data.NowPeriodIndex + 1) + 1);
                Program_Data.PeriodNumber = (Period_Cnt);  //取得新的 PeriodNumber
                
                Period_Cnt++;
            }while(PeriodAfterDiffTempValue > 0);
            
            __asm("NOP");
            
        }
        //-----------------------------------------------------------------------------------------------------
 
        
    }else if(RestOfPeriod_Temp < 10){
        __asm("NOP");   
    }
    
    
}




void Time_Change_Process(){

  
    if(Program_Data.Goal_Time >= 1920){
        
        Program_Data.PeriodWidth  = 60;
        
        Program_Data.PeriodNumber = Program_Data.Goal_Time/60;  //修改Period 數量
        

        
    }else if(Program_Data.Goal_Time < 1920){
        
        Program_Data.PeriodWidth  = Program_Data.Goal_Time / 32;
        Program_Data.PeriodNumber = 32;
        
    }
    
    Program_Data.NextPeriodValue = Program_Data.Goal_Time - Program_Data.PeriodWidth;
 

}

unsigned char Manual_Profile_Template[4] = {0,0,0,0};

void Quick_Start_Init(){

    UserData_Init(); 
    
    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
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

void Manual_Init(){
    
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

//---------------------------------------------------------------------------------   
unsigned char Random_Profile_Template[32];

void Random_Init(){
    
    UserData_Init();
    
    Program_Data.Goal_Time    = 1800;
    Program_Data.Goal_Counter = 1800;
    
    //User_Stored_Setting_Init();
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;

    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 32;
    
    unsigned char Low_Limit;
    unsigned char High_Limit;
    unsigned char randomValue;
    
    //亂數揚升值
    Random_Profile_Template[0] = 1; //0.5%
    
    for(unsigned char i = 1 ; i < 32; i++){  //直接先產生32個亂數揚升
        
        //隨機數值下限 計算
        if( Random_Profile_Template[i-1] - INCLINE_Change_Range <= 0 ){  //前一個Level  < 4 下限固定為0
            Low_Limit = 0;
        }else if(Random_Profile_Template[i-1] - INCLINE_Change_Range > 0 ){
            Low_Limit = Random_Profile_Template[i-1] - INCLINE_Change_Range; 
        }
        
        //隨機數值上限 計算
        if(Random_Profile_Template[i-1] + INCLINE_Change_Range >= (INCLINE_MAX_Level_DEF/2) ){
            High_Limit = INCLINE_MAX_Level_DEF/2;
        }else if( Random_Profile_Template[i-1] + INCLINE_Change_Range < (INCLINE_MAX_Level_DEF/2) ){
            High_Limit  = Random_Profile_Template[i-1] + INCLINE_Change_Range;
        }
        
        randomValue = Low_Limit +  rand() % (High_Limit - Low_Limit); 
        
        
        while( randomValue == Random_Profile_Template[i-1]   ){
            
            randomValue = Low_Limit +  rand() % (High_Limit - Low_Limit);    //之後前後的數值不會超過最大揚升的50%。
            
        }
        
        Random_Profile_Template[i] = randomValue;
    } 
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Random_Profile_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Random_Profile_Template[ i % Program_Data.Template_Table_Num]; 
    }
    
    //----------------------------------------------------------------
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

unsigned char CrossCountry_Profile_Template[20] = {0,4,8,4,0,4,8,8,4,0,4,8,4,0,4,12,8,4,0,0};
void CrossCountry_Init(){

    
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;
    
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 20;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = CrossCountry_Profile_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = CrossCountry_Profile_Template[ i % Program_Data.Template_Table_Num]; 
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

unsigned char WeightLoss_Profile_Template[8] = {0,0,0,0,4,4,4,4};

void WeightLoss_Init(){

  
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1680; 
    Program_Data.Goal_Counter = 1680;
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 8;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = WeightLoss_Profile_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = WeightLoss_Profile_Template[ i % Program_Data.Template_Table_Num]; 
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

unsigned char Interval_1_1_Template[4] = {0,0,4,4};

void Interval_1_1_Init(){
    

    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;
    
    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Interval_1_1_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Interval_1_1_Template[ i % Program_Data.Template_Table_Num]; 
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


unsigned char Interval_1_2_Template[6] = {0,0,4,4,4,4};
void Interval_1_2_Init(){

    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800; 
    Program_Data.Goal_Counter = 1800;
    
    //User_Stored_Setting_Init();

    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 6;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Interval_1_2_Template[i];
    }
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Interval_1_2_Template[ i % Program_Data.Template_Table_Num]; 
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


unsigned char  Hill_start_Template[5] = {12,12,12,12,0};
unsigned char  Hill_Profile_Template[6] = {0,16,0,20,0,24};

void Hill_Init(){
    
   
    UserData_Init(); 
    
    Program_Data.Goal_Time    = 1800;   //15360
    Program_Data.Goal_Counter = 1800;   //15360
    
    //User_Stored_Setting_Init();

    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;

    
    //---------Start Profile 樣本  
    Program_Data.Template_Loop_Start_Index = 5;
    Program_Data.Template_Table_Num        = 6;
    
    for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Hill_Profile_Template[i];
    }
    
    
    for(unsigned char i = 0 ; i < Program_Data.Template_Loop_Start_Index; i++){
        Program_Data.INCLINE_Table_96[i] = Hill_start_Template[i]; 
    }
    
    for(unsigned char i = Program_Data.Template_Loop_Start_Index ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = Hill_Profile_Template[(i - Program_Data.Template_Loop_Start_Index) % Program_Data.Template_Table_Num]; 
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



