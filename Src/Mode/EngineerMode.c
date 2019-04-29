#include "system.h"


unsigned char EngineerTestItem;
unsigned char LED_SweepCnt;
unsigned char DispBoard_SweepCnt;

unsigned char TestValue = 1;
unsigned char TestItem = 26;

unsigned char Key_Press_Record_1;
unsigned char Key_Press_Record_2;
unsigned char Key_Press_Record_3;
unsigned char Key_Press_Record_4;
unsigned char Key_Press_Record_5;
unsigned char Key_Press_Record_6;
unsigned char Key_Press_Record_7;


void RestStepValue(){

    TimerCnt = 0;
    T500ms_Flag = 0;
    LED_SweepCnt = 0;
    TestValue = 0x80; 
    Key_Press_Record_1 = 0x00;
    Key_Press_Record_2 = 0x00;
    
}
                                        // :17   :12    :8
unsigned char Oder_Adj[27] = { 0,1,2,3,4,5,6,7,8,9,10,15,11,12,13,14,20,16,17,18,19,21,22,23,24,25,26};
unsigned char KeyNameStriBuff[3];
unsigned char Test_Dot = 1; 
void Engineer_Func(){
    
    
   // if(T_Marquee_Flag){
     //  T_Marquee_Flag = 0; 
        
        //---------------------------   E0   LED矩陣測試--------------
        if(EngineerTestItem == 0){
             memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E0" ,50 ,0);
            
            
              Turn_OFF_All_Segment();
              
              if( KeyCatch(0,1 , Start) ){
                  EngineerTestItem = 1;
                  RestStepValue();
              } 
        }
        
        //-------------------------------直掃 LED  矩陣測試-------
         if(EngineerTestItem == 1){
             
             for(unsigned char i = 0; i < 32; i++){
                 LedMatrixBuffer[i] = (0x80 >> LED_SweepCnt) ;  
             }
 
             if(T500ms_Flag){
                 T500ms_Flag = 0;
                 if(LED_SweepCnt < 7){
                     LED_SweepCnt++;
                 }else{
                     LED_SweepCnt = 0;
                 }
             }
             
             if( KeyCatch(0,1 , Start) ){
                  EngineerTestItem = 3;
                  RestStepValue();
             }else if(KeyCatch(0,1 , Stop) ){
                 memset(LedMatrixBuffer,0x00,32);
                 
                 EngineerTestItem = 0;
                 RestStepValue();
             }  
             
        }
         //-------------------------------橫掃 LED  矩陣測試-------
        /*if(EngineerTestItem == 2){
             
             for(unsigned char i = 0; i < 32; i++){
                 if(i == LED_SweepCnt){
                     LedMatrixBuffer[LED_SweepCnt] =0xFF; 
                 }else{
                     LedMatrixBuffer[i] =0x00;
                 }  
             }
             if(T500ms_Flag){
                 T500ms_Flag = 0;
                 if(LED_SweepCnt < 31){
                     LED_SweepCnt++;
                 }else{
                     LED_SweepCnt = 0;
                 }
             }
            
             if( KeyCatch(0,1 , Start) ){
                 EngineerTestItem = 3;
                 RestStepValue();
             }else if(KeyCatch(0,1 , Stop) ){
                 EngineerTestItem = 1;
                 RestStepValue();
             } 
        }*/
        
        //-----------------------------      E1  七段顯示器 測試    ----------
        if(EngineerTestItem == 3){
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E1" ,50 ,0);
            Turn_OFF_All_Segment();
            
            if( KeyCatch(0,1 , Start) ){
                EngineerTestItem = 5;
                RestStepValue();
                LED_SweepCnt = 6;
            }else if(KeyCatch(0,1, Stop) ){
                EngineerTestItem = 1;
                RestStepValue();
            }  
        }
        
        /*if(EngineerTestItem == 4){  //-----------  一顆一顆測  -----------------
            
            Test_Segment(Oder_Adj[TestItem] , (TestValue>>LED_SweepCnt));
            if(T300ms_Flag){
                 T300ms_Flag = 0;
                 
                 if( (TestItem == 18) || (TestItem == 13) || (TestItem == 8) ){
                     if(LED_SweepCnt <4){
                         LED_SweepCnt = 4;
                     }
                     if(LED_SweepCnt < 5){
                         LED_SweepCnt++;
                     }else{
                         LED_SweepCnt = 4;
                     }
                 }else{
                     //-----------------------一般數字
                     if(LED_SweepCnt < 7){
                         LED_SweepCnt++;
                     }else{
                         LED_SweepCnt = 0;
                     }
                 }    
             }
             if( KeyCatch(0,1 , Enter) ){ 
                 if(TestItem == 0){
                     EngineerTestItem = 5;
                     RestStepValue(); 
                 }else{
                     RestStepValue();
                     LED_SweepCnt = 6;
                     TestItem--;
                 }
             }
             if( KeyCatch(0,1 , Start) ){ 
                 
                     EngineerTestItem = 5;
                     RestStepValue(); 
                     TestItem = 26;
             }
             if(KeyCatch(0,1 , Stop) ){
                 
                 if(TestItem == 26){
                     EngineerTestItem = 3;
                     RestStepValue();
                 }else{
                     RestStepValue();
                     LED_SweepCnt = 6;
                     TestItem++;
                 }
             } 
        }*/
        
        if(EngineerTestItem == 5){
            //-----------------------------------------全部七段一起測----
            
            

            if(LED_SweepCnt == 8){
                Test7_Segment(0xFF);
            }else{
                Test7_Segment((TestValue>>LED_SweepCnt));
            }
            
            
            SET_DisplayBoard_Data('N',Test_Dot, DispBoard_SweepCnt*111,'N',Test_Dot,DispBoard_SweepCnt*111);
           
            
            if(T300ms_Flag){
                T300ms_Flag = 0;
                
                if(DispBoard_SweepCnt == 9){
                    if(Test_Dot == 1){
                        Test_Dot = 2;
                    }else{
                        Test_Dot = 1;
                    }
                }
                
                if(LED_SweepCnt < 8){
                    LED_SweepCnt++;
                }else{
                    LED_SweepCnt = 0;
                }
                
                DispBoard_SweepCnt++;
                DispBoard_SweepCnt= DispBoard_SweepCnt%10;
                
            }
            if( KeyCatch(0,1 , Start) ){           
                EngineerTestItem = 6;
                SET_DisplayBoard_Data('X',0, 0,'X',0,0);
                RestStepValue(); 
            }else if(KeyCatch(0,1 , Stop) ){      
                EngineerTestItem = 5;
                SET_DisplayBoard_Data('X',0, 0,'X',0,0);
                RestStepValue();
            } 
        }
        //--------------------------------E2   按鍵測試------------------------------
         if(EngineerTestItem == 6){
            
             memset(LedMatrixBuffer,0x00,32);
             F_String_buffer_Auto_Middle( Stay, "E2" ,50 ,0);
             Turn_OFF_All_Segment();
             
             if( KeyCatch(0,1 , Start) ){
                 EngineerTestItem = 7;
                 RestStepValue();
             }else if(KeyCatch(0,1 , Stop) ){
                 EngineerTestItem = 5;
                 RestStepValue();
             }  
        }
        
        //--------------------------
        if(EngineerTestItem == 7){
            
             memset(LedMatrixBuffer,0x00,32);
             LedMatrixBuffer[0] = Key_Press_Record_1;
             LedMatrixBuffer[1] = Key_Press_Record_2;
             LedMatrixBuffer[2] = Key_Press_Record_3;
             LedMatrixBuffer[3] = Key_Press_Record_4;
             LedMatrixBuffer[4] = Key_Press_Record_5;
             LedMatrixBuffer[5] = Key_Press_Record_6;
             LedMatrixBuffer[6] = Key_Press_Record_7;
             
             
             if( (Key_Press_Record_1 != 0xFF) || (Key_Press_Record_2 != 0xFF) ||
                 (Key_Press_Record_3 != 0xFF) || (Key_Press_Record_4 != 0xFF) || 
                 (Key_Press_Record_5 != 0xFF) || (Key_Press_Record_6 != 0xFF) ||
                 (Key_Press_Record_7 != 0x01)   ){    //按鈕還沒全部按過一遍
                 
                 if(KeyCatch(0 ,2, cool , Start)){  //跳過 按鍵測試
                     EngineerTestItem = 8;
                     RestStepValue();
                 }
                 
                 //------------------------------------
                 if( KeyCatch(0,1 , Spd_3) ){
                      Key_Press_Record_1 |= 0x01; 
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '1'; 
                      KeyNameStriBuff[2] = 0x00; 
                      
                 }  
                 if( KeyCatch(0,1 , Spd_6) ){
                      Key_Press_Record_1 |= 0x02;
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '2';
                      KeyNameStriBuff[2] = 0x00;  
                 }    
                 if( KeyCatch(0,1 , Spd_9) ){
                      Key_Press_Record_1 |= 0x04; 
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '3'; 
                      KeyNameStriBuff[2] = 0x00;  
                 }    
                 if( KeyCatch(0,1 , Spd_12) ){
                      Key_Press_Record_1 |= 0x08; 
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '4'; 
                      KeyNameStriBuff[2] = 0x00; 
                 }    
                 
                 if( KeyCatch(0,1 , cool) ){
                      Key_Press_Record_1 |= 0x10; 
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '5'; 
                      KeyNameStriBuff[2] = 0x00;  
                 }    
                 if( KeyCatch(0,1 , Start) ){
                      Key_Press_Record_1 |= 0x20; 
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '6'; 
                      KeyNameStriBuff[2] = 0x00;  
                 }    
                 if( KeyCatch(0,1 , Stop) ){
                      Key_Press_Record_1 |= 0x40; 
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '7'; 
                      KeyNameStriBuff[2] = 0x00;   
                 }   
                 
                 if( KeyCatch(0,1 , Spd_15) ){
                      Key_Press_Record_1 |= 0x80; 
                      KeyNameStriBuff[0] = 'K';
                      KeyNameStriBuff[1] = '8'; 
                      KeyNameStriBuff[2] = 0x00;   
                 }    
                 //--------------------------------------
                 
                 
                 if( KeyCatch(0,1 , Spd_18) ){
                     Key_Press_Record_2 |= 0x01; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '9'; 
                     KeyNameStriBuff[2] = 0x00;  
                 }   
                 if( KeyCatch(0,1 , Cancel) ){
                     Key_Press_Record_2 |= 0x02; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '0';  
                 }   
                 if( KeyCatch(0,1 , Num_0) ){
                     Key_Press_Record_2 |= 0x04; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '1';  
                 }                   
                 if( KeyCatch(0,1 , Enter) ){
                     Key_Press_Record_2 |= 0x08; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '2';  
                 }
                 if( KeyCatch(0,1 , Num_7) ){
                     Key_Press_Record_2 |= 0x10; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '3';  
                 }    
                 if( KeyCatch(0,1 , Num_8) ){
                     Key_Press_Record_2 |= 0x20; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '4';  
                 }    
                 if( KeyCatch(0,1 , Num_9) ){
                     Key_Press_Record_2 |= 0x40; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '5';  
                 }   
                 if( KeyCatch(0,1 , Num_4) ){
                     Key_Press_Record_2 |= 0x80; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '6';  
                 }  
                 
                 //------------------------------------------
                 
                 if( KeyCatch(0,1 , Num_5) ){
                     Key_Press_Record_3 |= 0x01;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '7';  
                 }    
                 if( KeyCatch(0,1 , Num_6) ){
                     Key_Press_Record_3 |= 0x02;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '8';  
                 }  
                 if( KeyCatch(0,1 , Num_1) ){
                     Key_Press_Record_3 |= 0x04;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '1'; 
                     KeyNameStriBuff[2] = '9';  
                 }    
                 if( KeyCatch(0,1 , Num_2) ){
                     Key_Press_Record_3 |= 0x08;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '0';  
                 }   
                 if( KeyCatch(0,1 , Num_3) ){
                     Key_Press_Record_3 |= 0x10;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '1';  
                 }  
                 if( KeyCatch(0,1 , Key_SpdDwn) ){
                     Key_Press_Record_3 |= 0x20;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '2';  
                 } 
                 if( KeyCatch(0,1 , Key_SpdUp) ){
                     Key_Press_Record_3 |= 0x40; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '3'; 
                 }
                 if( KeyCatch(0,1 , Key_Met) ){
                     Key_Press_Record_3 |= 0x80; 
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '4'; 
                 } 
                 //----------------------------------
                 
                 if( KeyCatch(0,1 , Key_Pace) ){
                     Key_Press_Record_4 |= 0x01;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '5'; 
                 }   
                 if( KeyCatch(0,1 , Key_Time ) ){
                     Key_Press_Record_4 |= 0x02;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '6'; 
                 }
                 if( KeyCatch(0,1 , Key_Cal) ){
                     Key_Press_Record_4 |= 0x04;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '7'; 
                 } 
                 if( KeyCatch(0,1 , Key_Dist) ){
                     Key_Press_Record_4 |= 0x08;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '8';    
                 }       
                 if( KeyCatch(0,1 , ANT) ){
                     Key_Press_Record_4 |= 0x10;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '2'; 
                     KeyNameStriBuff[2] = '9'; 
                 } 
                 if( KeyCatch(0,1 , Key_IncUp) ){
                     Key_Press_Record_4 |= 0x20;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '0';
                 }  
                 if( KeyCatch(0,1 , Key_IncDwn) ){
                     Key_Press_Record_4 |= 0x40;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '1'; 
                 }  
                 if( KeyCatch(0,1 , Key_Goal) ){ 
                     Key_Press_Record_4 |= 0x80;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '2';
                 }
                 //-------------------------------------
                 if( KeyCatch(0,1 , Key_FitTest) ){ 
                     Key_Press_Record_5 |= 0x01;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '3';
                 }
                 if( KeyCatch(0,1 , Key_Custom) ){ 
                     Key_Press_Record_5 |= 0x02;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '4';
                 }
                 if( KeyCatch(0,1 , Key_Manual) ){ 
                     Key_Press_Record_5 |= 0x04;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '5';
                 }
                 if( KeyCatch(0,1 , Key_HRC) ){ 
                     Key_Press_Record_5 |= 0x08;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '6'; 
                 }
                 if( KeyCatch(0,1 , Key_Advance) ){ 
                     Key_Press_Record_5 |= 0x10;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '7';
                 }
                 if( KeyCatch(0,1 , BLE) ){
                     Key_Press_Record_5 |= 0x20;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '8';
                 }    
                 if( KeyCatch(0,1 , Inc_0) ){
                     Key_Press_Record_5 |= 0x40;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '3'; 
                     KeyNameStriBuff[2] = '9';
                 }  
                 if( KeyCatch(0,1 , Inc_3) ){
                     Key_Press_Record_5 |= 0x80;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '0';
                 } 
                 //---------------------------------
                 if( KeyCatch(0,1 , Inc_6) ){
                     Key_Press_Record_6 |= 0x01;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '1';
                 }
                 if( KeyCatch(0,1 , Inc_9) ){
                     Key_Press_Record_6 |= 0x02;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '2'; 
                 } 
                 if( KeyCatch(0,1 , Inc_12) ){
                     Key_Press_Record_6 |= 0x04;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '3';
                 }  
                 if( KeyCatch(0,1 , Inc_15) ){
                     Key_Press_Record_6 |= 0x08;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '4';
                 } 
                 if(R_KeyCatch(Spd_Up)){
                     Key_Press_Record_6 |= 0x10;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '5';
                 }
                 if(R_KeyCatch(Spd_Down)){
                     Key_Press_Record_6 |= 0x20;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '6';
                 }
                 if(R_KeyCatch(Inc_Up)){
                     Key_Press_Record_6 |= 0x40;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '7';
                 }
                 if(R_KeyCatch(Inc_Down)){
                     Key_Press_Record_6 |= 0x80;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '8';
                 }
                 
                 if(PauseKey()){
                     Key_Press_Record_7 |= 0x01;
                     KeyNameStriBuff[0] = 'K';
                     KeyNameStriBuff[1] = '4'; 
                     KeyNameStriBuff[2] = '9';
                 }

                 F_String_buffer_Auto_Middle( Stay, KeyNameStriBuff ,50 ,0);
                 
             }else{    //----------------------全部按完一輪
             
                 F_String_buffer_Auto_Middle( Stay, "OK" ,50 ,0);
                 
                 if( KeyCatch(0,1 , Start) ){
                     EngineerTestItem = 8;
                     RestStepValue();
                 }else if(KeyCatch(0,1 , Stop) ){
                     EngineerTestItem = 6;
                     RestStepValue();
                 }  
             }
       
        }
        //--------------------------------E3  Flash 讀寫測試 ------------------------
        if(EngineerTestItem == 8){
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E3" ,50 ,0);
            Turn_OFF_All_Segment();
            
            if( KeyCatch(0,1 , Start) ){
                EngineerTestItem = 9;
                RestStepValue();
            }else if(KeyCatch(0,1 , Stop) ){
                EngineerTestItem = 7;
                RestStepValue();
            }  
            
        }
        if(EngineerTestItem == 9){
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "FLASH" ,50 ,0);
            Turn_OFF_All_Segment();
            
            if( KeyCatch(0,1 , Start) ){
                
                if(Flash_Test_Engineer() != 1){   //讀寫測試   //在0x0000F0 寫入1byte 測試
                    //--------讀寫失敗-----------------------------------
                    while(1){
                        F_String_buffer_Auto_Middle( Stay, "ERR" ,50 ,0);
                        if(T250ms_Flag){
                            T250ms_Flag = 0;
                            writeLEDMatrix();
                        }
                    }
                }else{
                    EngineerTestItem = 10;
                    RestStepValue();
                }
       
            }else if(KeyCatch(0,1 , Stop) ){
                EngineerTestItem = 8;
                RestStepValue();
            }  
            
        }
        if(EngineerTestItem == 10){
            //---------------------------------Flash --- 測試OK
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "OK" ,50 ,0);
            
            if( KeyCatch(0,1 , Start) ){
                EngineerTestItem = 11;
                RestStepValue();
            }
            
            
        }
            
        //-----------------------------E4  ANT+ 心跳測試-----
        if(EngineerTestItem == 11){   
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E4" ,50 ,0);
            
            if( KeyCatch(0,1 , Start) ){   
                EngineerTestItem = 12;
            }  
            
        }
        
         if(EngineerTestItem == 12){   
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "ANT+" ,50 ,0);
            
            if( KeyCatch(0,1 , Start) ){   
                 EngineerTestItem = 13;
                 Btm_Task_Adder(Scan_ANT_HRC_Sensor);
            }  
            
        }
        if(EngineerTestItem == 13){   //--------------ANT+ 心跳搜尋中
            
            memset(LedMatrixBuffer,0x00,32);
            if(Linked_HR_info.Link_state == Linked){
                F_String_buffer_Auto_Middle( Stay, "WAIT" ,50 ,0);
            }else{
                F_String_buffer_Auto_Middle( Stay, "SCAN" ,50 ,0);
            }
            
            if(Linked_HR_info.SensorType == ANT_HR){
                if( Linked_HR_info.usHR_bpm > 0){
                    if(Linked_HR_info.Link_state == Linked){
                        EngineerTestItem = 14;
                    }
                }            
            }
            
            if( KeyCatch(0,1 , Start) ){   //重新掃描
                 if(Linked_HR_info.Link_state != Linked){
                     Btm_Task_Adder(Scan_ANT_HRC_Sensor);
                 }
            }  
            
        }
        if(EngineerTestItem == 14){   //--------------ANT+ 心跳連線
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "LINK" ,50 ,0);
            
            SET_Seg_Display(HEARTRATE  , Linked_HR_info.usHR_bpm , ND , DEC );
            writeSegmentBuffer();
            
            if( KeyCatch(0,1 , Start) ){   
                Btm_Task_Adder(ANT_HRC_Disconnect);
                Turn_OFF_All_Segment();
                EngineerTestItem = 15;
            }  
        } 
        //---------------------------------E5  藍芽心跳測試----------------
         if(EngineerTestItem == 15){
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E5" ,50 ,0);
            
            if( KeyCatch(0,1 , Start) ){
                EngineerTestItem = 16;
                RestStepValue();
            }    
        }
        if(EngineerTestItem == 16){
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "BLE" ,50 ,0);
            
            if( KeyCatch(0,1 , Start) ){
                Btm_Task_Adder(Scan_BLE_HRC_Sensor);
                EngineerTestItem = 17;
                RestStepValue();
            }    
        }
        if(EngineerTestItem == 17){   //--------------藍芽心跳搜尋中
            
            memset(LedMatrixBuffer,0x00,32);
            if(Linked_HR_info.Link_state == Linked){
                F_String_buffer_Auto_Middle( Stay, "WAIT" ,50 ,0);
            }else{
                F_String_buffer_Auto_Middle( Stay, "SCAN" ,50 ,0);
            }
            
            if(Linked_HR_info.SensorType == BLE_HR){
                if( Linked_HR_info.usHR_bpm > 0){
                    if(Linked_HR_info.Link_state == Linked){
                        EngineerTestItem = 18;
                    }
                }            
            }
            
            if( KeyCatch(0,1 , Start) ){   //重新掃描
                
                if(Ble_wait_HR_value_First_IN_Flag == 0){
                    if(Scan_Msg.Scan_State != Scaning ){  //掃描中防止又按一次
                        if(Scan_Msg.Scan_State != Scaning2 ){
                            if(Linked_HR_info.Link_state != Linked){
                                Btm_Task_Adder(Scan_BLE_HRC_Sensor);
                            }else if((Linked_HR_info.Link_state == Linked) || (Linked_HR_info.Link_state == Linked_2)){
                                Btm_Task_Adder(BLE_HRC_Disconnect);
                            }
                        }
                        
                    }
                }
            }  
        }
        if(EngineerTestItem == 18){   //--------------藍芽心跳連線
            
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "LINK" ,50 ,0);
             SET_Seg_Display(HEARTRATE  , Linked_HR_info.usHR_bpm , ND , DEC );
             writeSegmentBuffer();
             
            if( KeyCatch(0,1 , Start) ){   
                Btm_Task_Adder(BLE_HRC_Disconnect);
                Turn_OFF_All_Segment();
                EngineerTestItem = 19;
                
                Response_Message.Control_code =(Control_code_Def)0x00;
                Response_Message.Instruction_code =0x00;  
            }  
        }
        //------------------------------E6  下控測試--------------------------
        if(EngineerTestItem == 19){   
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E6" ,50 ,0);
           
            if( KeyCatch(0,1 , Start) ){
                EngineerTestItem = 20;
            }  
        }

        if(EngineerTestItem == 20){   
            
           if(T_Marquee_Flag){
               T_Marquee_Flag = 0; 
               F_String_buffer_Auto_Middle( Left, "RS485  TEST" ,50 ,0);
               writeLEDMatrix();
           }
           
           if( KeyCatch(0,1 , Start) ){
                 EngineerTestItem = 21;
           }  
        }
        
        if(EngineerTestItem == 21){   
            
            F_String_buffer_Auto_Middle( Stay, "WAIT" ,50 ,0);
            
           if(Response_Message.Control_code == ACK){         //收到ACK
               if(Response_Message.Instruction_code == 0x90){
                   EngineerTestItem = 22;
               }
           } 
        }
         if(EngineerTestItem == 22){         
             memset(LedMatrixBuffer,0x00,32);
             F_String_buffer_Auto_Middle( Stay, "OK" ,50 ,0);

             if( KeyCatch(0,1 , Start) ){
                 EngineerTestItem = 23;
            }  
        }
        if(EngineerTestItem == 23){         
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "END" ,50 ,0);
            
            if( KeyCatch(0,1 , Start) ){
                HAL_NVIC_SystemReset();
            }  
        } 
        
        //----------------Flash 重置------------------------
        if(EngineerTestItem == 24){       
            
            Turn_OFF_All_Segment();
            
            memset(LedMatrixBuffer,0x00,32);
            
            if(T_Marquee_Flag){
               T_Marquee_Flag = 0; 
               F_String_buffer_Auto_Middle( Left, "RST     FLASH" ,40 ,0);
               writeLEDMatrix();
           }        
            
            
            if( KeyCatch(0,1 , Start) ){
                
                memset(LedMatrixBuffer,0x00,32);
                F_String_buffer_Auto_Middle( Stay, "OK" ,50 ,0);
                writeLEDMatrix();
                HAL_Delay(1000);
                
                FlashErase(0);
                HAL_NVIC_SystemReset();
            }  
        } 
        
        

        //----------------LUBE 重置------------------------
        if(EngineerTestItem == 25){       
            
            Turn_OFF_All_Segment();
            
            memset(LedMatrixBuffer,0x00,32);
            
            if(T_Marquee_Flag){
                T_Marquee_Flag = 0; 
                F_String_buffer_Auto_Middle( Left, "CLR   LUBE" ,40 ,0);
                writeLEDMatrix();
            }        
            
            
            if( KeyCatch(0,1 , Start) ){
                
                memset(LedMatrixBuffer,0x00,32);
                F_String_buffer_Auto_Middle( Stay, "OK" ,50 ,0);
                writeLEDMatrix();
                HAL_Delay(1000);
                
                Clear_LUBE_Times();
                HAL_NVIC_SystemReset();
            }  
        } 
        


        if(T250ms_Flag){
            T250ms_Flag = 0;
            if(EngineerTestItem != 20 && EngineerTestItem != 24 && EngineerTestItem != 25){
                writeLEDMatrix();
            }
            if(EngineerTestItem == 21){
                F_CMD(Write,87);  //丟測試CMD 給下控
            } 
        }

}