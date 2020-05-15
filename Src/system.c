#include "stm32f4xx_hal.h"
#include "system.h"

char ucProductionSerialNumber[14];

System_Mode_Def   System_Mode;
SafeKey_State_def safekey;

Time_Display_Def       Time_Display_Type;
Calories_Display_Def   Calories_Display_Type;
Dist_Display_Def       Dist_Display_Type;
HeartRate_Display_Def  HeartRate_Display_Type;
unsigned char str_cNt = 0;

unsigned short  System_INCLINE; //0~150   => 0.0~15.0 %   =>0~30
unsigned short  System_SPEED;   //0 ~ 120 

System_Unit_Def System_Unit;

//unsigned int Total_Machine_Distants = 254;  //機器跑的總里程  單位 公里/10 => 1:100公尺
//unsigned int Total_Machine_Times    = 10;     //機器跑的總時間  單位 小時 

unsigned short usNowHeartRate;
unsigned char  HeartRate_Is_Exist_Flag = 0;

///主要功能   判斷心跳裝置是否有數值   有數值 FTMS 心跳Flag = 1  數值變0  或斷線  Flag = 0
// Zwift 特殊情況  FTMS 心跳送0bpm 或將Flag取消 Zwift心跳數值都會停留在最後大於0的整數值

//用來確認0xCB 或 0xCC 有沒有進來
unsigned int ant_hr_CC_exisit_chk_cnt;
unsigned int ble_hr_CB_exisit_chk_cnt;

unsigned char ant_CC_exisit_flag;
unsigned char ble_CB_exisit_flag;


void F_HeartRate_Supervisor(){

    if(T1s_HR_Monitor_Flag){
        T1s_HR_Monitor_Flag = 0;      
        
        //---------ant_hr_CC_exisit_chk_cnt  一直累加 如果有進來就會清0----
        if(ant_hr_CC_exisit_chk_cnt <= 2){
            ant_CC_exisit_flag = 1;
        }else if(ant_hr_CC_exisit_chk_cnt > 2){
            ant_CC_exisit_flag = 0;           
        }
        ant_hr_CC_exisit_chk_cnt++; 
 
        //----------ble_hr_CB_exisit_chk_cnt  一直累加 如果有進來就會清0----
        if(ble_hr_CB_exisit_chk_cnt <= 2){
            ble_CB_exisit_flag = 1;
        }else if(ble_hr_CB_exisit_chk_cnt > 2){
            ble_CB_exisit_flag = 0;
            
        }            
        ble_hr_CB_exisit_chk_cnt++;
        
        if( (ble_CB_exisit_flag == 0) && (ant_CC_exisit_flag == 0)){
            Linked_HR_info.Link_state = disconnect;
            
            if(NFC_Connect_Wait_flag == 1){  //斷線完要補連NFC
                NFC_Connect_Wait_flag = 0;              
      
                Pairing_Msg.Pairing_Sensor_Type = BLE_HR;                
                memcpy( Pairing_Msg.BLE_Addrs,   ucNFC_BleID , 6);
                Pairing_Msg.BLE_ADDR_TYPE = 0x01;
                
                Btm_Task_Adder(Connect_Paired_BLE_HR_E2);
            }               
        }
        
          
        if(HeartRate_Is_Exist_Flag == 0){
          
            if(Linked_HR_info.Link_state == wait_disconnect){
                Linked_HR_info.Link_state =  disconnect;
            }
            wait_HR_disconnect_Flag = 0;
          
          
            if(usNowHeartRate > 0){
                HeartRate_Is_Exist_Flag = 1; 
                //掃描中有心跳數值 90%是斷線後的 殘留CB 或 CC 所以這個情形必須排除
                if( (Scan_Msg.Scan_State != Scaning) && (Scan_Msg.Scan_State != Scaning2)){
                    if(Linked_HR_info.SensorType == ANT_HR){
                        ANT_Icon_Display_Cnt = 10;
                    }else if(Linked_HR_info.SensorType == BLE_HR){
                        Ble_Icon_Display_Cnt = 10;
                    }
                }

                
            } 
        }
        else if(HeartRate_Is_Exist_Flag == 1){
            
            if(usNowHeartRate == 0){
                HeartRate_Is_Exist_Flag = 0;
                Linked_HR_info.SensorType = (Sensor_UUID_Type_Def)0; //防止斷線後 因為手握心跳  觸發錯誤 ICON
                //Linked_HR_info.Link_state = disconnect; //++
            } 
        }
        
        //ANT 斷線重新連線 (只連原本連住的那一個)
        if( (HeartRate_Is_Exist_Flag == 0) && (Scan_Msg.ScanType == ANT_HR) && (ANT_ID_Paired_legacy !=0 ) ){
        
          if((System_Mode == WarmUp) || (System_Mode == Workout)  || (System_Mode == CooolDown) ||  (System_Mode == Paused)){
            //如果是正在運動中的情況下要去連原本連線的那一個
             Btm_Task_Adder(Connect_Paired_ANT_HR_E2);
          }          
        }
        
        //BLE 斷線重新連線 (只連原本連住的那一個)
        if( (HeartRate_Is_Exist_Flag == 0) && (Scan_Msg.ScanType == BLE_HR ) && (Linked_HR_info.Link_state == disconnect) && (BLE_Paired_legacy_Info.BLE_Addrs[0] != 0) ){
        
          if((System_Mode == WarmUp) || (System_Mode == Workout)  || (System_Mode == CooolDown) ||  (System_Mode == Paused)){
            //如果是正在運動中的情況下要去連原本連線的那一個
             Btm_Task_Adder(Connect_Paired_BLE_HR_E2);
          }          
        }
        
        
    }
}

unsigned char charArrayEquals( char A1[], char A2[]){
    
    unsigned int A1_len = strlen(A1);
    unsigned int A2_len = strlen(A2);
    
    if(A1_len != A2_len){
       return 0;    
    }else if(A1_len == A2_len){
        
        for(int i = 0; i < A1_len;i++ ){
            if(A1[i] != A2[i]){
                return 0;  
            }
        }
        return 1;
    }
     return 0; 
}