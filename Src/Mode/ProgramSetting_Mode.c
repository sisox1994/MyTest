#include "system.h"


Program_Setting_item_Def  Setting_item_Index;
unsigned char StringCnt;

unsigned char LIKE_Flag;


Setting_Class_Def  Setting_Class;


void Program_Like_Display(){
    
    //--------------------------矩陣區顯示--------------------------
    switch(Program_Data.Like_Program){
        
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
        
        /*switch(str_cNt){
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
        F_String_buffer_Auto(Stay,"     80" ,35 ,0);\
        /*switch(str_cNt){
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
        
    }
    
    
    //--------------------時間顯示----------------------
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
    
    
}

//當按 +  或按住+   的動作
void TimeIncreaseProcess(){
    
    if(Program_Data.Goal_Time < 356400 ){ 
        Program_Data.Goal_Time+=60;
    }
    
    TIME_SET_Display( TIME  , Program_Data.Goal_Time ,0x0F , 1);  
    writeSegmentBuffer();
    
}

//當按 -  或按住 --   的動作
void TimeDecreaseProcess(){
    
    
    
    if(Program_Data.Goal_Time > 600){  
        Program_Data.Goal_Time-=60;
    }
    TIME_SET_Display( TIME  , Program_Data.Goal_Time ,0x0F , 1);
    writeSegmentBuffer();
    
}

unsigned short NumberTemp;
unsigned char  Number_Digit_Tmp = 0;


void NumberInsert_Time(){

    //----------------------------------------------
    if(KeyCatch(0,1 , Inc_15)){     // 1
        
        if(Number_Digit_Tmp == 0){
            NumberTemp = 1;
        }else if(Number_Digit_Tmp == 1){
            NumberTemp = NumberTemp * 10 + 1;
        }else if(Number_Digit_Tmp == 2){
            NumberTemp = NumberTemp * 10 + 1;
        }
        
        Number_Digit_Tmp++;
        Program_Data.Goal_Time = NumberTemp * 60;
    }
    
    if(KeyCatch(0,1 , Inc_10)){     //2
        
        if(Number_Digit_Tmp == 0){
            NumberTemp = 2;
        }else if(Number_Digit_Tmp == 1){
            NumberTemp = NumberTemp * 10 + 2;
        }else if(Number_Digit_Tmp == 2){
            NumberTemp = NumberTemp * 10 + 2;
        }
        
        Number_Digit_Tmp++;
        Program_Data.Goal_Time = NumberTemp * 60;
        
    }   
    
    if(KeyCatch(0,1 , Inc_5)){     //3
        
        if(Number_Digit_Tmp == 0){
            NumberTemp = 3;
        }else if(Number_Digit_Tmp == 1){
            NumberTemp = NumberTemp * 10 + 3;
        }else if(Number_Digit_Tmp == 2){
            NumberTemp = NumberTemp * 10 + 3;
        }
        
        Number_Digit_Tmp++;
        Program_Data.Goal_Time = NumberTemp * 60;
        
    }
    
    if(KeyCatch(0,1 , Inc_0)){      //0
        
        
        if(Number_Digit_Tmp == 0){
            NumberTemp = 0;
        }else if(Number_Digit_Tmp == 1){
            NumberTemp = NumberTemp * 10;
            Number_Digit_Tmp++;
        }else if(Number_Digit_Tmp == 2){
            NumberTemp = NumberTemp * 10;
            Number_Digit_Tmp++;
        }     
        Program_Data.Goal_Time = NumberTemp * 60;
    }
    
    Number_Digit_Tmp = Number_Digit_Tmp % 3;
    
    
    //---------------------------------------------
}


unsigned short Number_Digits_Index;
unsigned short Number_Buffer = 0;


void Setting_item_Display(){
    
    if(Setting_item_Index == SET_Age){
        
        Program_Data.Age = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_Weight){
        
        Program_Data.Weight = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_THR){
        
        Program_Data.TargetHeartRate = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_MAX_INC){
        
        Program_Data.Ez_MaxINCLINE = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_CAL_GOAL){
        
        Program_Data.Calories_Goal = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_Dif_LEVEL){
        
        Program_Data.Diffculty_Level = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_WORK_Time){
        
        Program_Data.WorkTime = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_REST_Time){
        
        Program_Data.RestTime = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
        
    }else if(Setting_item_Index == SET_LIKE_Prog){
        /*
        Program_Data.RestTime = Number_Buffer;
        F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
        */
        
    }
    

}

void NumberKeyboard(){

    
    if(KeyCatch(0,1 , Inc_15)){     // 1
        
        if(Number_Digits_Index == 0){
            Number_Buffer = 1;
        }else if(Number_Digits_Index == 1){
            Number_Buffer = Number_Buffer * 10 + 1;
        }else if(Number_Digits_Index == 2){
            Number_Buffer = Number_Buffer * 10 + 1;
        }
        
        Number_Digits_Index++;
        Setting_item_Display();
        writeLEDMatrix(); 
        
    }
    
    if(KeyCatch(0,1 , Inc_10)){     //2
        
        if(Number_Digits_Index == 0){
            Number_Buffer = 2;
        }else if(Number_Digits_Index == 1){
            Number_Buffer = Number_Buffer * 10 + 2;
        }else if(Number_Digits_Index == 2){
            Number_Buffer = Number_Buffer * 10 + 2;
        }
        
        Number_Digits_Index++;
        Setting_item_Display();
        writeLEDMatrix(); 
    }   
    
    if(KeyCatch(0,1 , Inc_5)){     //3
        
        if(Number_Digits_Index == 0){
            Number_Buffer = 3;
        }else if(Number_Digits_Index == 1){
            Number_Buffer = Number_Buffer * 10 + 3;
        }else if(Number_Digits_Index == 2){
            Number_Buffer = Number_Buffer * 10 + 3;
        }
        
        Number_Digits_Index++;
        Setting_item_Display();
        writeLEDMatrix(); 
        
    }
    
    if(KeyCatch(0,1 , Inc_0)){      //0
        
        
        if(Number_Digits_Index == 0){
            Number_Buffer = 0;
        }else if(Number_Digits_Index == 1){
            Number_Buffer = Number_Buffer * 10;
            Number_Digits_Index++;
        }else if(Number_Digits_Index == 2){
            Number_Buffer = Number_Buffer * 10;
            Number_Digits_Index++;
        } 
        
        Setting_item_Display();
        writeLEDMatrix(); 
        
    }
    
    Number_Digits_Index = Number_Digits_Index % 3;
    //-----------------------------------------------------------
    
}



void NumberKeyProcess(){

    switch(Setting_item_Index){
        
      case SET_Time:
        NumberInsert_Time();
        break;
      case SET_Age:
      case SET_Weight:
      case SET_THR:
      case SET_MAX_INC:
      case SET_CAL_GOAL:
      case SET_Dif_LEVEL:
      case SET_WORK_Time:
      case SET_REST_Time:
        NumberKeyboard();
        break;
    
    }
    
}

Key_Name_Def Key_Name_Temp_User;
unsigned char ClassCnt_User;
void KeyChangeDetect_User(Key_Name_Def key){

    if(Key_Name_Temp_User != key){
        Key_Name_Temp_User = key;
        Program_Data.Like_Program = Quick_start;
        ClassCnt_User = 0;
    }
    
}

void Key_Profile_Setting(){
    
    switch(Setting_item_Index){
        
      case SET_Time:
       
        //  ++   單點                      按住 連續+
        if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
            TimeIncreaseProcess();
        }
      
        //  --   單點                      按住 連續-
        if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9) ){            
            TimeDecreaseProcess();             
        }
        
        NumberKeyProcess();
       
        if(KeyCatch(0,1 , Enter)){
            TIME_SET_Display( TIME  , Program_Data.Goal_Time ,0x0F , 1);
            writeSegmentBuffer();
            StringCnt = 0;
            Setting_item_Index = SET_Age;
            ClearBlinkCnt2();
        }
        if(KeyCatch(0,1 , Stop)){
            ClassCnt = 0;
            Program_Select = Quick_start;
            Clear_DataCheckBuffer();
            Program_Init();
            StringCnt = 0;
            System_Mode = Menu;
        }
        break;
      case SET_Age:
        if(StringCnt < 2){
            //++
            if(KeyCatch(0,1 , Spd_12)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //------- 顯示 設定"年齡"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,0);
                writeLEDMatrix();
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                switch(Setting_Class){
                  case Time_Age_Weight:
                  case Time_Age_Weight_THR:
                  case Time_Age_Weight_MAX_INC:  
                  case Time_Age_Weight_Work_Rest:
                    Setting_item_Index = SET_Time;
                    break;
                    
                  case Age_Weight:
                  case Age_Weight_Cal:
                  case Age_Weight_Lvl:
                    ClassCnt = 0;
                    Program_Select = Quick_start;
                    Clear_DataCheckBuffer();
                    Program_Init();
                    System_Mode = Menu;
                    break;
                }
                ClearBlinkCnt2();
            }
            
        }else if(StringCnt == 2){           //設定年齡
            
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                Number_Digits_Index = 0;
                if(Program_Data.Age < 100){
                    Program_Data.Age++;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
              
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                Number_Digits_Index = 0;
                if(Program_Data.Age > 10){
                    Program_Data.Age--;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            
            NumberKeyProcess();
            
            if(KeyCatch(0,1 , Enter)){
                Number_Digits_Index = 0;
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                
                //輸入數值超出範圍
                if( (Program_Data.Age > 100) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_Age;
                    Program_Data.Age = 100;
                }else if(Program_Data.Age < 10){
                    StringCnt = 2;
                    Setting_item_Index = SET_Age;
                    Program_Data.Age = 10;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                switch(Setting_Class){
                  case Time_Age_Weight:
                  case Time_Age_Weight_THR:
                  case Time_Age_Weight_MAX_INC:  
                  case Time_Age_Weight_Work_Rest:
                    Setting_item_Index = SET_Time;
                    break;
 
                  case Age_Weight:
                  case Age_Weight_Cal:
                  case Age_Weight_Lvl:
                    ClassCnt = 0;
                    Program_Select = Quick_start;
                    Clear_DataCheckBuffer();
                    Program_Init();
                    System_Mode = Menu;
                    break;
                }
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_Weight:
        if(StringCnt < 2){
            //------- 顯示 設定"體重"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                
                StringCnt = 0;
                Setting_item_Index = SET_Age;
                
                ClearBlinkCnt2();
            }
            //++
            if(KeyCatch(0,1 , Spd_12)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            
        }else if(StringCnt == 2){           //設定 體重
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                
                if(System_Unit == Metric ){
                    if(Program_Data.Weight < 200){   //公制最大體重上限 200 KG
                        Program_Data.Weight++;
                    }
                }else if(System_Unit == Imperial){
                     if(Program_Data.Weight < 440){  //英制最大體重上限 200 LB
                        Program_Data.Weight++;
                    }
                } 
  
                F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                if(System_Unit == Metric ){
                    if(Program_Data.Weight > 40){
                        Program_Data.Weight--;
                    }
                }else if(System_Unit == Imperial){
                    if(Program_Data.Weight > 88){
                        Program_Data.Weight--;
                    }
                }       
                F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            
            NumberKeyProcess();
            
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                switch(Setting_Class){
                  case Time_Age_Weight:
                    Setting_item_Index = SET_END;
                    break;
                  case Time_Age_Weight_THR:
                    Setting_item_Index = SET_THR;
                    break;
                  case Time_Age_Weight_MAX_INC:
                    Setting_item_Index = SET_MAX_INC;
                    break;
                  case Age_Weight:
                    Setting_item_Index = SET_END;
                    break;
                  case Age_Weight_Cal:
                    Setting_item_Index = SET_CAL_GOAL;
                    break;
                  case Age_Weight_Lvl:
                    Setting_item_Index = SET_Dif_LEVEL;
                    break;
                  case Time_Age_Weight_Work_Rest:
                    Setting_item_Index = SET_WORK_Time;
                    break;
                }
                
                //輸入數值超出範圍
                if(System_Unit == Metric ){
                    if( (Program_Data.Weight > 200) ){
                        StringCnt = 2;
                        Setting_item_Index = SET_Weight;
                        Program_Data.Weight = 200;
                    }else if(Program_Data.Weight < 40){
                        StringCnt = 2;
                        Setting_item_Index = SET_Weight;
                        Program_Data.Weight = 40;
                    }  
                }else if(System_Unit == Imperial){
                    
                    if( (Program_Data.Weight > 440) ){
                        StringCnt = 2;
                        Setting_item_Index = SET_Weight;
                        Program_Data.Weight = 440;
                    }else if(Program_Data.Weight < 88){
                        StringCnt = 2;
                        Setting_item_Index = SET_Weight;
                        Program_Data.Weight = 88;
                    }  
                }      
                //跑馬燈 防止鬼影                
                if( Setting_item_Index != SET_MAX_INC){
                    if(Setting_item_Index != SET_Dif_LEVEL){
                        F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,0);
                        writeLEDMatrix();
                    }
                }
   
                ClearBlinkCnt2();
            }
            
            if(KeyCatch(0,1 , Stop)){
                
                StringCnt = 0;
                Setting_item_Index = SET_Age;
                
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_THR:
        if(StringCnt < 2){
            //------- 顯示 設定"目標心率"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
            //++
            if(KeyCatch(0,1 , Spd_12) ){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            
        }else if(StringCnt == 2){           //設定"目標心率"
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                if(Program_Data.TargetHeartRate < Program_Data.MaxHeartRate){
                    Program_Data.TargetHeartRate++;
                }
                F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                if(Program_Data.TargetHeartRate > 80){
                    Program_Data.TargetHeartRate--;
                }
                F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            NumberKeyProcess();
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                Setting_item_Index = SET_END;
                //輸入數值超出範圍
                if( (Program_Data.TargetHeartRate > Program_Data.MaxHeartRate) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_THR;
                    Program_Data.TargetHeartRate = Program_Data.MaxHeartRate;
                }else if(Program_Data.TargetHeartRate < 80){
                    StringCnt = 2;
                    Setting_item_Index = SET_THR;
                    Program_Data.TargetHeartRate = 80;
                }
                F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_MAX_INC:
        if(StringCnt < 2){
            //------- 顯示 設定"最大揚升"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
            //++
            if(KeyCatch(0,1 , Spd_12) ){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            
        }else if(StringCnt == 2){           //設定""
            
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                if(Program_Data.Ez_MaxINCLINE < Machine_Data.System_INCLINE_Max ){
                    Program_Data.Ez_MaxINCLINE+=5;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                if(Program_Data.Ez_MaxINCLINE > 0){
                    Program_Data.Ez_MaxINCLINE-=5;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            NumberKeyProcess();
            
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                Setting_item_Index = SET_END;
                
                if(Program_Data.Ez_MaxINCLINE % 5 != 0){
                    Program_Data.Ez_MaxINCLINE = (Program_Data.Ez_MaxINCLINE / 5) * 5;
                }               
                //輸入數值超出範圍
                if( (Program_Data.Ez_MaxINCLINE > Machine_Data.System_INCLINE_Max) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_MAX_INC;
                    Program_Data.Ez_MaxINCLINE = Machine_Data.System_INCLINE_Max;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_CAL_GOAL:
        if(StringCnt < 2){
            //------- 顯示 設定"目標卡路里"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
            //++
            if(KeyCatch(0,1 , Spd_12) ){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            
        }else if(StringCnt == 2){           //設定""
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                if(Program_Data.Calories_Goal < 10000){
                    Program_Data.Calories_Goal++;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                if(Program_Data.Calories_Goal > 0){
                    Program_Data.Calories_Goal--;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,0);
                writeLEDMatrix(); 
                
            }
            NumberKeyProcess();
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                Setting_item_Index = SET_END;
                //輸入數值超出範圍
                if( (Program_Data.Calories_Goal <100) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_CAL_GOAL;
                    Program_Data.Calories_Goal = 100;
                }else if((Program_Data.Calories_Goal >9999) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_CAL_GOAL;
                    Program_Data.Calories_Goal = 9999;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2(); 
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_Dif_LEVEL:
        if(StringCnt < 2){
            //------- 顯示 設定"困難度"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
            //++
            if(KeyCatch(0,1 , Spd_12) ){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            
        }else if(StringCnt == 2){           //設定""
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                if(Program_Data.Diffculty_Level < 20){
                    Program_Data.Diffculty_Level++;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                if(Program_Data.Diffculty_Level > 1){
                    Program_Data.Diffculty_Level--;
                }
                F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            NumberKeyProcess();
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                Setting_item_Index = SET_END;
                
                //輸入數值超出範圍
                if( (Program_Data.Diffculty_Level < 1) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_Dif_LEVEL;
                    Program_Data.Diffculty_Level = 1;
                }else if((Program_Data.Diffculty_Level > 20) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_Dif_LEVEL;
                    Program_Data.Diffculty_Level = 20;
                }
                
                F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_WORK_Time:
        if(StringCnt < 2){
            //------- 顯示 設定"WorkTime"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
            //++
            if(KeyCatch(0,1 , Spd_12) ){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
        }else if(StringCnt == 2){           //設定""
            
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                if(Program_Data.WorkTime < 20){
                    Program_Data.WorkTime++;
                }
                F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                if(Program_Data.WorkTime > 1){
                    Program_Data.WorkTime--;
                }
                F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            NumberKeyProcess();
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                Setting_item_Index = SET_REST_Time;
                //輸入數值超出範圍
                if( (Program_Data.WorkTime < 1) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_WORK_Time;
                    Program_Data.WorkTime = 1;
                }else if((Program_Data.WorkTime > 20) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_WORK_Time;
                    Program_Data.WorkTime = 20;
                }
                if(Program_Select == Custom_1){
                     MyCustom_1.Custom_Time = Program_Data.Goal_Time;
                     MyCustom_1.Custom_Age  = Program_Data.Age;
                     MyCustom_1.Custom_Weight = Program_Data.Weight;
                     MyCustom_1.Custom_WorkTime = Program_Data.WorkTime;
                     MyCustom_1.Custom_RestTime = Program_Data.RestTime;
                     Custom_1_Init();
                }else if(Program_Select == Custom_2){
                    MyCustom_2.Custom_Time = Program_Data.Goal_Time;
                    MyCustom_2.Custom_Age  = Program_Data.Age;
                    MyCustom_2.Custom_Weight = Program_Data.Weight;
                    MyCustom_2.Custom_WorkTime = Program_Data.WorkTime;
                    MyCustom_2.Custom_RestTime = Program_Data.RestTime;
                    Custom_2_Init();
                }
                F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }   
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_Weight;
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_REST_Time:
        if(StringCnt < 2){
            //------- 顯示 設定"RESTTime"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 2;
                //F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
                //writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            //++
            if(KeyCatch(0,1 , Spd_12) ){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9)){
                StringCnt = 2;
                F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            if(KeyCatch(0,1 , Stop)){
                
                StringCnt = 0;
                Setting_item_Index = SET_WORK_Time;
 
                ClearBlinkCnt2();
            }
        }else if(StringCnt == 2){           //設定""
            //++
            if(KeyCatch(0,1 , Spd_12) || KeyForContinueProcess(Spd_12)){
                
                if(Program_Data.RestTime < (20 - Program_Data.WorkTime)){
                    Program_Data.RestTime++;
                }
                F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            //--
            if(KeyCatch(0,1 , Spd_9) || KeyForContinueProcess(Spd_9)){
                
                if(Program_Data.RestTime > 1){
                    Program_Data.RestTime--;
                }
                F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
                writeLEDMatrix(); 
            }
            NumberKeyProcess();
            
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                Setting_item_Index = SET_END;
                //輸入數值超出範圍
                if( (Program_Data.RestTime < 1) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_REST_Time;
                    Program_Data.WorkTime = 1;
                }else if(Program_Data.RestTime > (20 - Program_Data.WorkTime) ){
                    StringCnt = 2;
                    Setting_item_Index = SET_REST_Time;
                    Program_Data.WorkTime = (20 - Program_Data.WorkTime);
                }
                F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,0);
                
                if(Program_Select == Custom_1){
                    
                    MyCustom_1.Custom_Time = Program_Data.Goal_Time;
                    MyCustom_1.Custom_Age  = Program_Data.Age;
                    MyCustom_1.Custom_Weight = Program_Data.Weight;
                    MyCustom_1.Custom_WorkTime = Program_Data.WorkTime;
                    MyCustom_1.Custom_RestTime = Program_Data.RestTime;
                    Custom_1_Init();
                }else if(Program_Select == Custom_2){
                    
                    MyCustom_2.Custom_Time = Program_Data.Goal_Time;
                    MyCustom_2.Custom_Age  = Program_Data.Age;
                    MyCustom_2.Custom_Weight = Program_Data.Weight;
                    MyCustom_2.Custom_WorkTime = Program_Data.WorkTime;
                    MyCustom_2.Custom_RestTime = Program_Data.RestTime;
                    Custom_2_Init();
                }
                writeLEDMatrix(); 
                ClearBlinkCnt2();
            }
            if(KeyCatch(0,1 , Stop)){
                StringCnt = 0;
                Setting_item_Index = SET_WORK_Time;
                ClearBlinkCnt2();
            }
        }
        break;
      case SET_LIKE_Prog:
        
        if(StringCnt < 2){
            //------- 顯示 設定"LIKE_Program"時按 (Enter) ,(+) ,(-) 鍵進入下一個flow    --------
            if(KeyCatch(0,1 , Enter)){
                //User_Like_Program_Init();
                Program_Like_Display();
                StringCnt = 2;
                writeLEDMatrix();
                writeSegmentBuffer();
                
                ClearBlinkCnt2();
            }
            if( KeyCatch(0,1 , Key_Manual) ){ 
                
                KeyChangeDetect_User(Key_Manual);
                Program_Data.Like_Program = (Program_Type_Def)(1 + (ClassCnt_User % 7));
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                StringCnt = 2;
                writeLEDMatrix(); 
                writeSegmentBuffer();
            } 
            if( KeyCatch(0,1 , Key_HRC) ){ 
                
                KeyChangeDetect_User(Key_HRC);
                Program_Data.Like_Program = (Program_Type_Def)(11 + (ClassCnt_User % 6));
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                StringCnt = 2;
                writeLEDMatrix(); 
                writeSegmentBuffer();
            }
            if( KeyCatch(0,1 , Key_Advance) ){ 
                
                KeyChangeDetect_User(Key_Advance);
                Program_Data.Like_Program = (Program_Type_Def)(22 + (ClassCnt_User % 6)  );
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                StringCnt = 2;
                writeLEDMatrix(); 
                writeSegmentBuffer();
            }
            
            if( KeyCatch(0,1 , BLE) ){ 
                KeyChangeDetect_User(BLE);
                Program_Data.Like_Program = (Program_Type_Def)(31 + (ClassCnt_User % 4)  );
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                StringCnt = 2;
                writeLEDMatrix(); 
                writeSegmentBuffer();
            }
            
            if(KeyCatch(0,1 , Stop)){
                ClassCnt = 0;
                Program_Select = Quick_start;
                Clear_DataCheckBuffer();
                Program_Init();
                StringCnt = 0;
                System_Mode = Menu;
            }
            
        }else if(StringCnt == 2){           //設定""
            if( KeyCatch(0,1 , Key_Manual) ){ 
                
                KeyChangeDetect_User(Key_Manual);
                Program_Data.Like_Program = (Program_Type_Def)(1 + (ClassCnt_User % 7));
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                writeLEDMatrix();  
                writeSegmentBuffer();
                
            } 
            if( KeyCatch(0,1 , Key_HRC) ){ 
                KeyChangeDetect_User(Key_HRC);
                Program_Data.Like_Program = (Program_Type_Def)(11 + (ClassCnt_User % 6));
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                writeLEDMatrix();  
                writeSegmentBuffer();
            }
            if( KeyCatch(0,1 , Key_Advance) ){ 
                KeyChangeDetect_User(Key_Advance);
                Program_Data.Like_Program = (Program_Type_Def)(22 + (ClassCnt_User % 6)  );
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                writeLEDMatrix();  
                writeSegmentBuffer();
            }
            
            if( KeyCatch(0,1 , BLE) ){ 
                KeyChangeDetect_User(BLE);
                Program_Data.Like_Program = (Program_Type_Def)(31 + (ClassCnt_User % 4)  );
                ClassCnt_User++;
                User_Like_Program_Init();
                Program_Like_Display();
                writeLEDMatrix();  
                writeSegmentBuffer();
            }
            
            if(KeyCatch(0,1 , Enter)){
                StringCnt = 0;
                switch(Program_Data.Like_Program){
                  case Manual:
                  case Random: 
                  case CrossCountry: 
                    System_Mode = Program_Setting; 
                    Setting_item_Index = SET_Time;
                    str_cNt = 0;
                    break;
                  case WeightLoss: 
                    System_Mode = Program_Setting; 
                    Setting_item_Index = SET_Age;
                    str_cNt = 0;
                    break;
                  case Interval_1_1: 
                  case Interval_1_2:
                  case Hill:  
                    System_Mode = Program_Setting; 
                    Setting_item_Index = SET_Time;
                    str_cNt = 0;
                    break;
                  case Target_HeartRate_Goal:
                  case Fat_Burn:
                  case Cardio: 
                  case Heart_Rate_Hill:   
                  case Heart_Rate_Interval: 
                  case Extreme_Heart_Rate: 
                    System_Mode = Program_Setting; 
                    Setting_item_Index = SET_Time;
                    str_cNt = 0;
                    break;
                  case Hill_Climb: 
                  case Aerobic:   
                  case Interval_1_4: 
                  case Interval_2_1: 
                  case EZ_INCLINE: 
                    System_Mode = Program_Setting; 
                    Setting_item_Index = SET_Time;
                    str_cNt = 0;
                    break;
                  case MARATHON_Mode: 
                    System_Mode = Program_Setting; 
                    Setting_item_Index = SET_Age;
                    str_cNt = 0;
                    break;
                  case Calorie_Goal:
                  case Distance_Goal_160M:
                  case Distance_Goal_5K:
                  case Distance_Goal_10K:
                    System_Mode = Program_Setting; 
                    Setting_item_Index = SET_Age;
                    str_cNt = 0;
                    break;
                }
                LIKE_Flag = 1;
                //Setting_item_Index = SET_END;
                ClearBlinkCnt2();
            } 
            if(KeyCatch(0,1 , Stop)){
                
                ClassCnt = 0;
                Program_Select = Quick_start;
                Clear_DataCheckBuffer();
                Program_Init();
                StringCnt = 0;
                System_Mode = Menu;
            }
        }
        break;
      case SET_END:
        
        if(KeyCatch(0,1 , Enter)){
            StringCnt = 0;
            if(LIKE_Flag == 1 ){
                LIKE_Flag = 0; 
                if(Program_Select == User_1){
                    MyUser_1.User_Time      = Program_Data.Goal_Time;
                    MyUser_1.User_Age       = Program_Data.Age;
                    MyUser_1.User_Weight    = Program_Data.Weight;
                    MyUser_1.User_Program_Like = Program_Data.Like_Program;
                    
                    MyUser_1.User_THR           = Program_Data.TargetHeartRate;
                    MyUser_1.User_Ez_MaxINCLINE = Program_Data.Ez_MaxINCLINE;
                    MyUser_1.User_Calories_Goal = Program_Data.Calories_Goal;
                    MyUser_1.Diffculty_Level    = Program_Data.Diffculty_Level;
                    
                }else if(Program_Select == User_2){
                    
                    MyUser_2.User_Time = Program_Data.Goal_Time;
                    MyUser_2.User_Age  = Program_Data.Age;
                    MyUser_2.User_Weight = Program_Data.Weight;
                    MyUser_2.User_Program_Like = Program_Data.Like_Program;
                    
                    MyUser_2.User_THR           = Program_Data.TargetHeartRate;
                    MyUser_2.User_Ez_MaxINCLINE = Program_Data.Ez_MaxINCLINE;
                    MyUser_2.User_Calories_Goal = Program_Data.Calories_Goal;
                    MyUser_2.Diffculty_Level    = Program_Data.Diffculty_Level;
                }
                Setting_item_Index = SET_LIKE_Prog;     
            }else{
                switch(Setting_Class){
                  case Time_Age_Weight:
                  case Time_Age_Weight_THR:
                  case Time_Age_Weight_MAX_INC:
                  case Time_Age_Weight_Work_Rest:
                    Setting_item_Index = SET_Time;
                    break;
                    
                  case Age_Weight:
                  case Age_Weight_Cal:
                  case Age_Weight_Lvl:
                    Setting_item_Index = SET_Age;
                    break;   
                }
            }
            ClearBlinkCnt2();
        }
        
        if(KeyCatch(0,1 , Stop)){
            StringCnt = 0;
            switch(Setting_Class){
              case Time_Age_Weight:
              case Age_Weight:
                Setting_item_Index = SET_Weight;
                break;
              case Time_Age_Weight_THR:
                Setting_item_Index = SET_THR;
                break;
              case Time_Age_Weight_MAX_INC:
                Setting_item_Index = SET_MAX_INC;
                break;
              case Time_Age_Weight_Work_Rest:
                Setting_item_Index = SET_REST_Time;
                break;
              case Age_Weight_Cal:
                Setting_item_Index = SET_CAL_GOAL;
                break;
              case Age_Weight_Lvl:
                Setting_item_Index = SET_Dif_LEVEL;
                break;   
            }
            ClearBlinkCnt2();
        }
        
        break;
    }
    if(KeyCatch(0,1 , Start)){   //--------設定模式   按下 START  開始運動
         
        //-----------------------------時間修改 重新計算 格數 一格時間寬度
        Program_Data.Goal_Counter = Program_Data.Goal_Time;

        switch(Program_Select){
            
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
        
        if(Program_Select == Custom_1 || Program_Select == Custom_2  || Program_Select == User_1 || Program_Select == User_2  ){
            
            Flash_Custom_Data_Saving();
            
            switch(Program_Select){
              case Custom_1:
                Custom_1_Init();
                break;
              case Custom_2:
                Custom_2_Init();
                break;
              case User_1:
              case User_2:
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
                Program_Select = Program_Data.Like_Program;
                break;
                
            }
        }
 
        //---------------------  時間下數 或上數顯示-----------
        switch(Program_Select){
          case Quick_start:
          case MARATHON_Mode:
          case Calorie_Goal:
          case Distance_Goal_160M:
          case Distance_Goal_5K:
          case Distance_Goal_10K:
            SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
            break;
          default:
            SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Counter);
            break;
        }
 
        writeSegmentBuffer();
        IntoReadyMode_Process();
        
        //----------------如果是心跳模式  但是沒有心跳 就不允許進入運動
        switch(Program_Select){
          case Target_HeartRate_Goal:
          case Fat_Burn: 
          case Cardio:   
          case Heart_Rate_Hill: 
          case Heart_Rate_Interval: 
          case Extreme_Heart_Rate: 
            if(!usNowHeartRate){
                System_Mode = Menu;
            }
            break;  
        }  
    }
}

void ProgramSetting_Key(){
    
    switch(Program_Select){
      case Quick_start:
        break;
      case Manual:
      case Random:   
      case CrossCountry:
        Setting_Class = Time_Age_Weight;
        break;
      case WeightLoss: 
        Setting_Class = Age_Weight;
        break;
      case Interval_1_1:
      case Interval_1_2:
      case Hill:  
        Setting_Class = Time_Age_Weight;
        break;
      case Target_HeartRate_Goal:
      case Fat_Burn:  
      case Cardio:     
      case Heart_Rate_Hill:   
      case Heart_Rate_Interval: 
      case Extreme_Heart_Rate: 
        Setting_Class = Time_Age_Weight_THR;
        break;
      case Hill_Climb: 
      case Aerobic:   
      case Interval_1_4: 
      case Interval_2_1: 
        Setting_Class = Time_Age_Weight;
        break;
      case EZ_INCLINE: 
        Setting_Class = Time_Age_Weight_MAX_INC;
        break;
      case MARATHON_Mode: 
        Setting_Class = Age_Weight;
        break;
      case Calorie_Goal:
        Setting_Class = Age_Weight_Cal;
        break;
      case Distance_Goal_160M:
      case Distance_Goal_5K:
      case Distance_Goal_10K:
        Setting_Class = Age_Weight_Lvl;
        break; 
      case Custom_1: 
      case Custom_2: 
        Setting_Class = Time_Age_Weight_Work_Rest; 
        break; 
      case User_1: 
      case User_2:
        Setting_Class = P_Like;
        break; 
    }
    Key_Profile_Setting();
    
}

void ProgramSetting_LIKE_Key(){
    
    switch(Program_Data.Like_Program){
      case Quick_start:
        break;
        
      case Manual:
      case Random:   
      case CrossCountry:
        Setting_Class = Time_Age_Weight;
        break;
        
      case WeightLoss: 
        Setting_Class = Age_Weight;
        break;
        
      case Interval_1_1:
      case Interval_1_2:
      case Hill:  
        Setting_Class = Time_Age_Weight;
        break;
        
      case Target_HeartRate_Goal:
      case Fat_Burn:  
      case Cardio:     
      case Heart_Rate_Hill:   
      case Heart_Rate_Interval: 
      case Extreme_Heart_Rate: 
        Setting_Class = Time_Age_Weight_THR;
        break;   
        
      case Hill_Climb: 
      case Aerobic:   
      case Interval_1_4: 
      case Interval_2_1: 
        Setting_Class = Time_Age_Weight;
        break;
        
      case EZ_INCLINE: 
        Setting_Class = Time_Age_Weight_MAX_INC;
        break;
        
      case MARATHON_Mode: 
        Setting_Class = Age_Weight;
        break;
        
      case Calorie_Goal:
        Setting_Class = Age_Weight_Cal;
        break;
        
      case Distance_Goal_160M:
      case Distance_Goal_5K:
      case Distance_Goal_10K:
        Setting_Class = Age_Weight_Lvl;
        break; 
        
    }
    Key_Profile_Setting();
}

void ProgramSetting_Display(){
    
   // if(T_Marquee_Flag){
        //T_Marquee_Flag = 0;
        switch(Setting_item_Index){
          case SET_Time:
            if( (Program_Select == Custom_1 ) || (Program_Select == Custom_2) ){
                DrawBarArray(Program_Data.BarArray_Display);
            }else{
                F_String_buffer(4,1,"TIME" ,4 ,0);
            }
            //--------------時間  七段顯示器-----------------------------
            
            if(Program_Data.Goal_Time < 3600){
                TIME_SET_Display( TIME  , Program_Data.Goal_Time ,0x0C , 0);
            }else if(Program_Data.Goal_Time >= 3600){ 
                TIME_SET_Display( TIME  , Program_Data.Goal_Time ,0x03 , 0);
            }
            //-------------------------------------------------------------
            
            break;
          case SET_Age:
            switch(StringCnt){
              case 0:
                //F_String_buffer(7,1,"AGE" ,4 ,0);
                F_String_buffer_Auto_Middle(Stay,"AGE",1,1);
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.Age , 10 ,DEC ,1);
                break;
            }
            break;
          case SET_Weight:
            switch(StringCnt){
              case 0:
                //F_String_buffer(10,1,"KG" ,2 ,0);
                if(System_Unit == Metric){
                    F_String_buffer_Auto_Middle(Stay,"KG",1,1);
                }else if(System_Unit == Imperial){
                    F_String_buffer_Auto_Middle(Stay,"LB",1,1);
                }
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.Weight , 10 ,DEC ,1);
                break;
            }
            break;
          case SET_THR:
            switch(StringCnt){
              case 0:
                //F_String_buffer(7,1,"THR" ,3 ,0);
                F_String_buffer_Auto_Middle(Stay,"THR",1,1);
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.TargetHeartRate , 10 ,DEC ,1);
                break;
            }
            break;
          case SET_MAX_INC:
            switch(StringCnt){
              case 0:
                //F_String_buffer(0,1,"MAX  INC" ,8 ,0);
                if(T_Marquee_Flag){
                    T_Marquee_Flag = 0;
                    F_String_buffer_Auto(Left,"MAX  INC",50 ,0);
                    writeLEDMatrix();
                }
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.Ez_MaxINCLINE , 10 ,DEC ,1);
                break;
            }
            break; 
          case SET_CAL_GOAL:
            switch(StringCnt){
              case 0:
                F_String_buffer_Auto_Middle(Stay,"CAL" ,30 ,1);
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.Calories_Goal , 10 ,DEC ,1);
                break;
            }
            break; 
          case SET_Dif_LEVEL:
            switch(StringCnt){
              case 0:
                //F_String_buffer_Auto_Middle(Stay,"LVL" ,30 ,1);
                if(T_Marquee_Flag){
                    T_Marquee_Flag = 0;
                    F_String_buffer_Auto(Left,"DIFF    LEVEL",50 ,0);
                    writeLEDMatrix();
                }
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.Diffculty_Level , 10 ,DEC ,1);
                break;
            }
            break; 
          case SET_WORK_Time:
            switch(StringCnt){
              case 0:
                F_String_buffer_Auto_Middle(Stay,"WT" ,30 ,1);
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.WorkTime , 10 ,DEC ,1);
                break;
            }
            break; 
          case SET_REST_Time: 
            switch(StringCnt){
              case 0:
                F_String_buffer_Auto_Middle(Stay,"RT" ,30 ,1);
                break;
              case 1:
                break;
              case 2:
                F_Number_buffer_Auto( Stay, Program_Data.RestTime , 10 ,DEC ,1);
                break;
            }
            break; 
          case SET_LIKE_Prog: 
            switch(StringCnt){
              case 0:
                F_String_buffer_Auto_Middle(Stay,"LIKE" ,30 ,1);
                break;
              case 1:
                break;
              case 2:
                Program_Like_Display();
                break;
            }
            break; 
          case SET_END:
            F_String_buffer_Auto_Middle(Stay,"END" ,30 ,0);
            break;
        }
        if(System_Mode == Program_Setting){
            writeSegmentBuffer();
        }
        if((Setting_item_Index == SET_MAX_INC)&&(StringCnt == 0) ){}
        else if( (Setting_item_Index == SET_Dif_LEVEL) && (StringCnt == 0) ){}
        else{writeLEDMatrix();}   
    //}

}

extern unsigned char ret_Idle_cnt;
void Program_Idel_detect(){
    
        if(T1s_Menu_Idle == 1){
            T1s_Menu_Idle = 0;

        if(ret_Idle_cnt == 30){
            
            ClassCnt = 0;
            ret_Idle_cnt = 0;
            Program_Select = Quick_start;
            StringCnt = 0;
            System_Mode = Menu;
        }
        
        if(Program_Select != Quick_start){
            ret_Idle_cnt++;
        }else{
            
            ret_Idle_cnt = 0;
        }
    }
}

void ProgramSetting_Func(){
    
    ucSubSystemMode = C_App_SetupVal;
    
    if(LIKE_Flag == 0){
        ProgramSetting_Key();
    }else if(LIKE_Flag == 1){
        ProgramSetting_LIKE_Key();
    }
    ProgramSetting_Display();
    Program_Idel_detect();
    
}