#include "system.h"


unsigned char EngineerTestItem;
unsigned char LED_SweepCnt;

unsigned char TestValue = 1;
unsigned char TestItem = 26;
unsigned char Key_Press_Record_1;
unsigned char Key_Press_Record_2;

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

void Engineer_Func(){
    
    
   // if(T_Marquee_Flag){
     //  T_Marquee_Flag = 0; 
        
        //---------------------------   E0   LED�x�}����--------------
        if(EngineerTestItem == 0){
             memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E0" ,50 ,0);
            
            
              Turn_OFF_All_Segment();
              
              if( KeyCatch(0,1 , Start) ){
                  EngineerTestItem = 1;
                  RestStepValue();
              } 
        }
        
        //-------------------------------���� LED  �x�}����-------
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
                  EngineerTestItem = 2;
                  RestStepValue();
             }else if(KeyCatch(0,1 , Stop) ){
                 memset(LedMatrixBuffer,0x00,32);
                 
                 EngineerTestItem = 0;
                 RestStepValue();
             }  
             
        }
         //-------------------------------� LED  �x�}����-------
        if(EngineerTestItem == 2){
             
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
        }
        
        //-----------------------------      E1  �C�q��ܾ� ����    ----------
        if(EngineerTestItem == 3){
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "E1" ,50 ,0);
            Turn_OFF_All_Segment();
            
            if( KeyCatch(0,1 , Start) ){
                EngineerTestItem = 4;
                RestStepValue();
                LED_SweepCnt = 6;
            }else if(KeyCatch(0,1, Stop) ){
                EngineerTestItem = 2;
                RestStepValue();
            }  
        }
        
        if(EngineerTestItem == 4){  //-----------  �@���@����  -----------------
            
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
                     //-----------------------�@��Ʀr
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
        }
        
        if(EngineerTestItem == 5){
            //-----------------------------------------�����C�q�@�_��----
            Test7_Segment((TestValue>>LED_SweepCnt));
            if(T300ms_Flag){
                T300ms_Flag = 0;
                if(LED_SweepCnt < 7){
                    LED_SweepCnt++;
                }else{
                    LED_SweepCnt = 0;
                }
            }
            if( KeyCatch(0,1 , Start) ){           
                EngineerTestItem = 6;
                RestStepValue(); 
            }else if(KeyCatch(0,1 , Stop) ){      
                EngineerTestItem = 4;
                RestStepValue();
            } 
        }
        //--------------------------------E2   �������------------------------------
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

             if( (Key_Press_Record_1 != 0xFF) || (Key_Press_Record_2 != 0x7F) ){    //���s�٨S�������L�@�M
                 if(KeyCatch(0 ,2, Spd_9 , Spd_12)){  //���L �������
                     EngineerTestItem = 8;
                     RestStepValue();
                 }
                 
                 if( KeyCatch(0,1 , Inc_15) ){
                     Key_Press_Record_1 ^= 0x01; 
                 }
                 if( KeyCatch(0,1 , Inc_10) ){
                     Key_Press_Record_1 ^= 0x02; 
                 }
                 if( KeyCatch(0,1 , Inc_5) ){
                     Key_Press_Record_1 ^= 0x04; 
                 }
                 if( KeyCatch(0,1 , Inc_0) ){
                     Key_Press_Record_1 ^= 0x08; 
                 } 
                 if( KeyCatch(0,1 , Start) ){
                     Key_Press_Record_1 ^= 0x10; 
                 }
                 if( KeyCatch(0,1 , Stop) ){
                     Key_Press_Record_1 ^= 0x20; 
                 }
                 if( KeyCatch(0,1 , Spd_3) ){
                     Key_Press_Record_1 ^= 0x40; 
                 }
                 if( KeyCatch(0,1 , Spd_6) ){
                     Key_Press_Record_1 ^= 0x80; 
                 }
                 if( KeyCatch(0,1 , Spd_9) ){
                     Key_Press_Record_2 ^= 0x01; 
                 }
                 if( KeyCatch(0,1 , Spd_12) ){
                     Key_Press_Record_2 ^= 0x02; 
                 }
                 if( KeyCatch(0,1 , Key_Manual) ){
                     Key_Press_Record_2 ^= 0x04; 
                 }
                 if( KeyCatch(0,1 , Key_HRC) ){
                     Key_Press_Record_2 ^= 0x08; 
                 }
                 if( KeyCatch(0,1 , Key_Advance) ){
                     Key_Press_Record_2 ^= 0x10; 
                 }
                 if( KeyCatch(0,1 , BLE) ){
                     Key_Press_Record_2 ^= 0x20; 
                 }
                 if( KeyCatch(0,1 , Enter) ){
                     Key_Press_Record_2 ^= 0x40; 
                 }   
                 
                 
                 
                 
             }else{    //----------------------���������@��
             
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
        //--------------------------------E3  Flash Ū�g���� ------------------------
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
                
                if(Flash_Test_Engineer() != 1){   //Ū�g����
                    //--------Ū�g����-----------------------------------
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
            //---------------------------------Flash --- ����OK
            memset(LedMatrixBuffer,0x00,32);
            F_String_buffer_Auto_Middle( Stay, "OK" ,50 ,0);
            
            if( KeyCatch(0,1 , Start) ){
                EngineerTestItem = 11;
                RestStepValue();
            }
            
            
        }
            
        //-----------------------------E4  ANT+ �߸�����-----
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
        if(EngineerTestItem == 13){   //--------------ANT+ �߸��j�M��
            
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
            
            if( KeyCatch(0,1 , Start) ){   //���s���y
                 if(Linked_HR_info.Link_state != Linked){
                     Btm_Task_Adder(Scan_ANT_HRC_Sensor);
                 }
            }  
            
        }
        if(EngineerTestItem == 14){   //--------------ANT+ �߸��s�u
            
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
        //---------------------------------E5  �Ūޤ߸�����----------------
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
        if(EngineerTestItem == 17){   //--------------�Ūޤ߸��j�M��
            
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
            
            if( KeyCatch(0,1 , Start) ){   //���s���y
                
                if(Ble_wait_HR_value_First_IN_Flag == 0){
                    if(Scan_Msg.Scan_State != Scaning ){  //���y������S���@��
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
        if(EngineerTestItem == 18){   //--------------�Ūޤ߸��s�u
            
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
        //------------------------------E6  �U������--------------------------
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
            
           if(Response_Message.Control_code == ACK){         //����ACK
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

        if(T250ms_Flag){
            T250ms_Flag = 0;
            if(EngineerTestItem != 20){
                writeLEDMatrix();
            }
            if(EngineerTestItem == 21){
                F_CMD(Write,87);  //�����CMD ���U��
            } 
        }

}