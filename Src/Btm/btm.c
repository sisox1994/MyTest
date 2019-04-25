#include "stm32f4xx_hal.h"
#include "string.h"
#include "system.h"
#include "btm.h"

#define FTMS_Test 0
#define configUSE_SPDRamSize 32
#define BtmData 20

unsigned char FW_Transmiting_03_Flag;
unsigned char ucRFVersion[3];          //FTMS SetFeature B0 cmd 取得RF版本 
unsigned short EepromVer_3;       // V1.2  
unsigned short EepromVer_4;        // A01

UART_HandleTypeDef huart2;

unsigned char btm_is_ready;      //等待BTM F1
unsigned char btm_Rx_is_busy;    
unsigned char Cloud_Run_Initial_Busy_Flag = 0;

void BtmRst(){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
    HAL_Delay(1300);
}

void Btm_Recive();

//Tx
unsigned char ucBtmTxBuf[BtmData];

//Rx
unsigned char ucRxAddrs;
unsigned char ucBtmRxTemp;
unsigned char ucBtmRxBuf[BtmData];
unsigned char ucBtmRxData[BtmData];

Scan_Meseseage_def    Scan_Msg;
BLE_Device_List_def   BLE_Scan_Device_List;
Pairing_Meseseage_def Pairing_Msg;             //當下配對取得的地址資訊
BLE_Paired_Device_Addr_List_def  BLE_Paired_device_list; // 成功配對 就把資訊加入清單裡

Now_Linked_HR_Sensor_Info_Def Linked_HR_info;

Btm_Task_Def btmTask_List[Task_Amount];

unsigned char btm_Task_Cnt = 0;


unsigned char wait_HR_disconnect_Flag;
unsigned short disconnect_buffer_0xFF_Cnt;

Btm_Task_Def   btm_HRC_disconnect_Task_Temp;

unsigned char Ble_wait_HR_value_First_IN_Flag;

void BTM_UART_Transmit(){
    
    if(btm_Rx_is_busy == 0){
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);           //禁止Uart 中斷收資料
        HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);      // Tx 資料發送
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);            //啟動Uart 中斷收資料
    }
    
    btm_Rx_is_busy = 0;

}

//---------------------0x02  Firmware Update 更新 允許確認-------------------------
void F_BtmReply02Cmd(){

    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0x02;
    ucBtmTxBuf[2] = 0x40;

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

} 

OTA_FW_Transmit_Info_Def FW;
unsigned char FW_Buff_16Byte[16];
unsigned int  nextPageShould = 0;

#if Config_WDG	
extern IWDG_HandleTypeDef hiwdg;
#endif

void F_BtmRead02Cmd(){

    
    if((ucBtmRxData[2] != 0x40 ) && (ucBtmRxData[6] > 0)){ //至少一個page   0x40不要收 
        
        //取得 檔案大小    Page數量
        FW.total_Size = (unsigned int)(ucBtmRxData[5]<<24)
                      + (unsigned int)(ucBtmRxData[4]<<16)
                      + (unsigned int)(ucBtmRxData[3]<<8)  
                      + (unsigned int)(ucBtmRxData[2]);
        
        FW.total_Page = (unsigned short)(ucBtmRxData[7]<<8)
                      + (unsigned short)(ucBtmRxData[6]);

        FW.Byte_Cnt = 0;
        FW.Error_Cnt  = 0;
        
#if Config_WDG	
        HAL_IWDG_Refresh(&hiwdg);    // 取得總容量 先餵一下狗 免得等太久睡著
#endif
        
        if(FW_Transmiting_03_Flag != 1){  //0x03 已經在丟資料了 就不用再回了'
            if(System_Mode != Sys_OTA_Mode){
                System_Mode = Sys_OTA_Mode; //如果0x50沒有丟 還是可以更新
                
                Turn_OFF_All_Segment();
                Turn_OFF_All_LEDMatrix();
            }
            
            Update_FW_Flash_Erase();
            F_BtmReply02Cmd();
            //Btm_Task_Adder(C_02Val);   // 回復APP可以開始丟資料了
        }
        
        OTA_Mode_Flag = 1;

    }
    __asm("NOP");
    
    
}
//---------------------0x02 End-----------------------------------------



//---------------------0x03  Firmware Data 傳輸  -------------------------
void F_BtmReply03Cmd(unsigned short page){

    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0x03;
    ucBtmTxBuf[2] = 0x40;
    
    ucBtmTxBuf[3] = (unsigned char)page;
    ucBtmTxBuf[4] = (unsigned char)(page>>8);
    
    

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

} 

void F_BtmRead03Cmd(){

    
    //Feed_Dog_Flag = 0;

#if Config_WDG	
    HAL_IWDG_Refresh(&hiwdg);    // 
#endif 
    
    FW.Transmit_Page = (unsigned short)(ucBtmRxData[3]<<8)
                     + (unsigned short)(ucBtmRxData[2]);
    
    
    if(FW.Transmit_Page > (FW.total_Page + 2) ){ //  iOS 會有2個page的落差
        while(1){}
          
    }
    
    if(FW.Transmit_Page != nextPageShould){
        
        if(FW.Transmit_Page == (nextPageShould - 1)){ //如果前一筆又丟一次出來
            nextPageShould = nextPageShould - 1;
            FW.Byte_Cnt = FW.Byte_Cnt - 15;
            FW.Error_Cnt ++;
        }else{
            Flash_Write_Applicantion_Mode();
            Turn_OFF_All_LEDMatrix();
            HAL_UART_DeInit(&huart2);
            FW.Error_Type = Data_Lost;               //Data Lost 跳包了
        }
        
       
    }else{
        FW_Transmiting_03_Flag = 1;
    }
    
    nextPageShould = FW.Transmit_Page + 1;
    
    
    for(unsigned char i = 4; i < 19; i++){
        
        if( (FW.Byte_Cnt % 16 == 0) && (FW.Byte_Cnt > 0)){
            Write_EE_Flash_FW_Data_16Byte( (FW.Byte_Cnt/16 - 1) , FW_Buff_16Byte);
            memset(FW_Buff_16Byte,0x00,16);
        }
        
        
        FW_Buff_16Byte[FW.Byte_Cnt%16] = ucBtmRxData[i];
        
        FW.Byte_Cnt++;
         
    }
    //----如果最後size不是16的倍數  ---補最後一包資料-------------
    if((FW.Byte_Cnt>=FW.total_Size) && (FW.Byte_Cnt%16!=0)){
       Write_EE_Flash_FW_Data_16Byte( (FW.Byte_Cnt/16) , FW_Buff_16Byte);
    }
    
        
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    
    //F_BtmReply03Cmd(FW.Transmit_Page );
    
   
}
//---------------------0x03 End-----------------------------------------

//---------------------0x04  Firmware Data 傳送完成 -------------------------
void F_BtmReply04Cmd(){

    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0x04;
    ucBtmTxBuf[2] = 0x40;
    

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

} 

 
void F_BtmRead04Cmd(){

   Fw_Write_Finish_Check();   //驗證有沒有寫進flash

   if(  (FW.Transmit_Page >= (FW.total_Page - 1))  && (FW.Byte_Cnt  >= FW.total_Size) && (FW.Error_Cnt == 0)){
       
       Flash_Write_BootLoader_Mode();      //Flash寫入 Bootloader_Mode 旗標
       
       HAL_NVIC_SystemReset();
       
   }else{

       HAL_UART_DeInit(&huart2);
       Flash_Write_Applicantion_Mode();
       FW.Error_Type = Size_Error;                   //資料傳送page數量  size容量 有問題
   }

   // Btm_Task_Adder(C_04Val); //告知APP  跑步機更新完成
   
}
//---------------------0x04 End-----------------------------------------


//設定藍芽名稱
void Btm_SetDeviceName(char* name,unsigned char Len){
    
    memset(ucBtmTxBuf,0x00,20);
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0xC9;
    
    if(Len>17){
      Len = 17;
    }
    for(unsigned char i =0 ; i < Len; i++){
        ucBtmTxBuf[2+i] = name[i];
    }
    
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

}


//----------------E0  --掃描Sensor--  Tx------------------------------

void Clear_BLE_Scan_Device_List(){
    
    BLE_Scan_Device_List.Device_Cnt = 0;
    
    Scan_Meseseage_def EmptyMsg ={
        .ANT_ID = 0,
        .DeviceName={""},
        .DeviceNumber = 0,
        .RSSI = 0,
        .ScanType = (Sensor_UUID_Type_Def)0,
        .TransDir = Txx,
        .Type_Check = (Sensor_UUID_Type_Def)0,
        .UUID = 0
            
    };
    for(unsigned char i =0 ;i<24; i++){
        BLE_Scan_Device_List.messeage_List[i] = EmptyMsg;
    }
    
}

void Clear_Scan_Msg(){
    
    Scan_Msg.DeviceNumber = 0;
    Scan_Msg.RSSI = 0;
    Scan_Msg.UUID = 0;
    Scan_Msg.Type_Check = (Sensor_UUID_Type_Def)0;
    memset(Scan_Msg.DeviceName,0x00,13);

}


void ScanSensorE0(Sensor_UUID_Type_Def  Sensor_Type){
 
    
    memset(ucBtmTxBuf,0x00,20);
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0xE0;
    ucBtmTxBuf[2] = (unsigned char)Sensor_Type;  
    ucBtmTxBuf[3] = (unsigned char)((unsigned short)(Sensor_Type >> 8));
   
    //---------------------  Scan_Meseseage  ---------------------------
    //----Common
    Scan_Msg.ScanType   =  Sensor_Type;
    Scan_Msg.Scan_State = (Scan_State_Def)0x00;
    Scan_Msg.TransDir   = Txx;
    
    //--- ANT+
    Scan_Msg.ANT_ID = 0;
    
    //--- BLE       
    //--------------------藍芽掃描清單初始化-----------------
    Clear_BLE_Scan_Device_List();
    //-------------------------------------------------------
    Clear_Scan_Msg(); 
    //-------------------------------------------------------------------------
    
    BTM_UART_Transmit();
   // __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    //HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);  

}

//----------------------------E0  --掃描Sensor--  Rx-----------------------------------



unsigned char NearestDevieNumber;
unsigned char RSSI_Compare;

void Scan_Re_E0(){


    //---------------------  Scan_Meseseage  ---------------------------
    //----Common

    Scan_Msg.TransDir   = Rxx;
    
    ////Tx 送出就決定了
    if(Scan_Msg.ScanType == ANT_HR){
        
        
        Scan_Msg.Scan_State = (Scan_State_Def)ucBtmRxData[2];
   
        if(Scan_Msg.Scan_State == Paired_OK){
            Scan_Msg.ANT_ID = ((unsigned short)ucBtmRxData[4]<< 8)  + ucBtmRxData[3]  ;
            
            Linked_HR_info.ANT_ID = Scan_Msg.ANT_ID; 
            Linked_HR_info.SensorType = ANT_HR;
            
            ANT_Icon_Display_Cnt = 10;
            
        }else if(Scan_Msg.Scan_State == No_Device){
            Scan_Msg.ANT_ID = 0;
        }
        
        //---掃描對象類型是ANT+    BLE資料初始化      
        Scan_Msg.DeviceNumber = 0;
        Scan_Msg.RSSI = 0;
        Scan_Msg.UUID = 0;
        Scan_Msg.Type_Check = (Sensor_UUID_Type_Def)0;
        memset(Scan_Msg.DeviceName,0x00,13);
        
    }else if(Scan_Msg.ScanType == BLE_HR ){
    
        if(ucBtmRxData[2] == 0x4F || ucBtmRxData[2] == 0x40 || ucBtmRxData[2] == 0x4C){
            
            Scan_Msg.Scan_State = (Scan_State_Def)ucBtmRxData[2];
            
            if(Scan_Msg.Scan_State == Scan_Time_Out){ //掃描 15 秒 連訊號最強的那個
                //Ask_Link_State_CE();
                Btm_Task_Adder(BLE_HRC_Pairing);  //連RSSI最強的那一個
            }
          
        }else{
              
           //--- BLE       
            Scan_Msg.DeviceNumber = ucBtmRxData[2];
            Scan_Msg.RSSI = ucBtmRxData[3];
            Scan_Msg.UUID = ((unsigned short)(ucBtmRxData[5]<< 8))  + ucBtmRxData[4] ;
            Scan_Msg.Type_Check = (Sensor_UUID_Type_Def)Scan_Msg.UUID;
            
            for(unsigned char i = 6;i < 6 + 13; i++){  //BLE Device Name長度為13
                Scan_Msg.DeviceName[i-6] = ucBtmRxData[i];
            }    
            
            
            
            char RejectDeviceName[] = {"Chandler_HRM"};
            if(!charArrayEquals(Scan_Msg.DeviceName , RejectDeviceName )){
            
                //尋找訊號最強的裝置  Brian 說 RSSI越大 訊號愈強
                if(BLE_Scan_Device_List.Device_Cnt == 0){
                    NearestDevieNumber = Scan_Msg.DeviceNumber;
                    RSSI_Compare = Scan_Msg.RSSI;
                }else if(BLE_Scan_Device_List.Device_Cnt>0){
                    if(Scan_Msg.RSSI > RSSI_Compare){
                        RSSI_Compare = Scan_Msg.RSSI;
                        NearestDevieNumber = Scan_Msg.DeviceNumber;
                    }  
                }
                
                //-------------------- 把掃到的裝置先存進清單裡 -----------------------
                BLE_Scan_Device_List.messeage_List[ BLE_Scan_Device_List.Device_Cnt] = Scan_Msg;
                BLE_Scan_Device_List.Device_Cnt++;
                //------------------------------------------------------- 
                
                
                
                //---掃描對象類型是BLE+    ANT資料初始化   
                Scan_Msg.ANT_ID = 0;
                
            
            }

        }         
        
    }
    
    if(Linked_HR_info.Link_state == Linked ){ //藍芽心跳 =>0xFF時  ble會回E0 4F
        Ask_Link_State_CE();
    }
 

    //-------------------------------------------------------------------------
    
}



//--------------------   E1  Tx 配對   經過E0掃描 Sensor 藉由掃描到的裝置流水號 去選擇配對   ----------------------
void Pairing_BLE_Sensor_E1(unsigned char DeviceNumber){
   
    
    
    if((BLE_Scan_Device_List.Device_Cnt>=1)){ //掃描完去連線??  至少要掃到一個device 才能去連線
        
        memset(ucBtmTxBuf,0x00,20);
        ucBtmTxBuf[0] = '[';	
        ucBtmTxBuf[19] = ']';
        
        ucBtmTxBuf[1] = 0xE1;
        ucBtmTxBuf[2] = DeviceNumber;   
        
        
         BTM_UART_Transmit();
        //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
        //HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
        
    }
}


//---------------------  E1  Rx  配對 成功  取得地址並存進清單  ------------------------
void BLE_Pairing_Re_E1(){
    
    if(ucBtmRxData[2] == 0x40){  //if -- [OK]
        Pairing_Msg.Pairing_Sensor_Type = (Sensor_UUID_Type_Def)(((unsigned short)(ucBtmRxData[4]<< 8))  + ucBtmRxData[3]);
        for(unsigned char i = 0; i < 6; i++ ){
            Pairing_Msg.BLE_Addrs[i] = ucBtmRxData[i + 5];
        }
        Pairing_Msg.BLE_ADDR_TYPE =  ucBtmRxData[11];
        
        
        //-------每次掃都清掉 配對清單  固定去連 最先掃到的那一個Sensor
        BLE_Paired_device_list.Paired_Device_Cnt = 0;
        
        
        //沒有配對過
        if(BLE_Paired_device_list.Paired_Device_Cnt == 0){
            BLE_Paired_device_list.BLE_Paired_Device_Addr_List[BLE_Paired_device_list.Paired_Device_Cnt] = Pairing_Msg;
            BLE_Paired_device_list.Paired_Device_Cnt++;
        }else{
            
            for(unsigned char i = 0; i < (BLE_Paired_device_list.Paired_Device_Cnt); i++){
                //如果清單裡面沒有才新增
                if(!charArrayEquals(BLE_Paired_device_list.BLE_Paired_Device_Addr_List[i].BLE_Addrs , Pairing_Msg.BLE_Addrs)){
                    BLE_Paired_device_list.BLE_Paired_Device_Addr_List[BLE_Paired_device_list.Paired_Device_Cnt] = Pairing_Msg;
                    BLE_Paired_device_list.Paired_Device_Cnt++;
                    break;
                }
            }
            
        }
        
        Btm_Task_Adder(BLE_HRC_Link);
       
        
    }
   
}



//-------------------------------------------------------------------------------------------

//-----------------------E2  BLE  Tx  -------------------------------------------------------
void Link_Sensor_E2_BLE(Pairing_Meseseage_def paired_msg){

    
    memset(ucBtmTxBuf,0x00,20);
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0xE2;
    
    ucBtmTxBuf[2] = (unsigned char)paired_msg.Pairing_Sensor_Type;  
    ucBtmTxBuf[3] = (unsigned char)((unsigned short)(paired_msg.Pairing_Sensor_Type >> 8));  
    
    ucBtmTxBuf[4] = paired_msg.BLE_Addrs[0];
    ucBtmTxBuf[5] = paired_msg.BLE_Addrs[1];
    ucBtmTxBuf[6] = paired_msg.BLE_Addrs[2];
    ucBtmTxBuf[7] = paired_msg.BLE_Addrs[3];
    ucBtmTxBuf[8] = paired_msg.BLE_Addrs[4];
    ucBtmTxBuf[9] = paired_msg.BLE_Addrs[5];
    ucBtmTxBuf[10] = paired_msg.BLE_ADDR_TYPE;
    
   
    Linked_HR_info.SensorType = BLE_HR;
    
        
        
     BTM_UART_Transmit();
    //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    //HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}

//-----------------------E2  ANT+  Tx  -------------------------------------------------------
void Link_Sensor_E2_ANT( unsigned short ANT_ID){

    
    memset(ucBtmTxBuf,0x00,20);
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0xE2;
    
    ucBtmTxBuf[2] = 0x0D;  
    ucBtmTxBuf[3] = 0xF8;  
    
    ucBtmTxBuf[4] = (unsigned char)ANT_ID;
    ucBtmTxBuf[5] = (unsigned char)(ANT_ID >>8);
    
    
    Linked_HR_info.SensorType = ANT_HR;
    

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}


//-----------------E2  Rx-------------------------------------------

unsigned short FirstCB_Time_Cnt;

void Link_Sensor_Re_E2(){


    if(ucBtmRxData[2] == 0x40 || ucBtmRxData[2] == 0x42){  //if -- [OK]

        Linked_HR_info.Link_state = (Linking_State_Def)ucBtmRxData[2];
        
        // Linked  info 填入目前連線的裝置名稱   跟 藍芽位址       
        memcpy( Linked_HR_info.DeviceName,BLE_Scan_Device_List.messeage_List[NearestDevieNumber].DeviceName,13);
        memcpy( Linked_HR_info.BLE_Addrs,  Pairing_Msg.BLE_Addrs , 6); 
       
        
        //--------------------藍芽掃描清單初始化-----------------------
        Clear_BLE_Scan_Device_List();
        //-------------------------------------------------------
        Clear_Scan_Msg();
        //----------------------------------------------
        
        
        Ble_Icon_Display_Cnt = 10;
        Ble_wait_HR_value_First_IN_Flag = 1;
        FirstCB_Time_Cnt = 0;
        
    }
    
}
    
//---------------------------------------------------------

//unsigned char BLE_HR_Force_disconnct_Flag;

//--------------- E4 Tx  disconnect Sensor ----------------------
void disconnect_Sensor_E4(Sensor_UUID_Type_Def  Sensor_Type){

    
    memset(ucBtmTxBuf,0x00,20);
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0xE4;
     
    ucBtmTxBuf[2] = (unsigned char)Sensor_Type;  
    ucBtmTxBuf[3] = (unsigned char)((unsigned short)(Sensor_Type >> 8));
    
    
    Clear_BLE_Scan_Device_List();  // 清掉掃描資訊


    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}


unsigned char DisConDeviceName[13];

//unsigned char Rest_of_HR_Value_Mask_Cnt; // 下E4 之後 CB還會下3筆友心跳數值的 Ack回來 

void disconnect_Sensor_Re_E4(){

    if( (ucBtmRxData[2] == 0x40) || (ucBtmRxData[2]==0x4B) ){
        
        if(Linked_HR_info.SensorType == BLE_HR){
            
            memset( Pairing_Msg.BLE_Addrs,0x00,6);
            Linked_HR_info.Link_state = wait_disconnect;
            disconnect_buffer_0xFF_Cnt = 0;
            memcpy(DisConDeviceName ,Linked_HR_info.DeviceName , 13 ); //把目前斷線的藍稱先記起來
            //Ask_Link_State_CE();
        
        }else if(Linked_HR_info.SensorType == ANT_HR){
            
            Linked_HR_info.Link_state = wait_disconnect;
            
            disconnect_buffer_0xFF_Cnt = 0;
            //Ask_Link_State_CF();
        }
        
        
        //Rest_of_HR_Value_Mask_Cnt = 5;
        
        //清除目前連線的心跳帶資訊
        Linked_HR_info.SensorType = (Sensor_UUID_Type_Def)0;  
        Linked_HR_info.ANT_ID = 0;
        Linked_HR_info.usHR_bpm = 0;
        memset(Linked_HR_info.DeviceName,0x00,13);
        memset(Linked_HR_info.BLE_Addrs,0x00,6);
        //Linked_HR_info.Link_state = disconnect;  
    }
}

void SensorReceive_CB(){
    
    if(ucBtmRxData[3]!=0xFF){                   //有心跳
        wait_HR_disconnect_Flag = 0;
        
        if(Ble_wait_HR_value_First_IN_Flag == 1){
            Ble_wait_HR_value_First_IN_Flag = 0;
        }
        
        disconnect_buffer_0xFF_Cnt = 0;
        
        if(Linked_HR_info.Link_state != wait_disconnect){
            Linked_HR_info.ANT_ID =0;
            Linked_HR_info.SensorType = BLE_HR;
            Linked_HR_info.usHR_bpm = ucBtmRxData[3];
            Linked_HR_info.Link_state = Linked;
        }
        
        
        
           
    }else if(ucBtmRxData[3] == 0xFF){           //心跳0xFF    暫時離線狀態
      
        if(Linked_HR_info.SensorType == BLE_HR){
            
            disconnect_buffer_0xFF_Cnt++;
            Linked_HR_info.usHR_bpm = 0;
            
            if(Ble_wait_HR_value_First_IN_Flag == 1){
                Linked_HR_info.Link_state = wait_hr_value;
            }else{
                Linked_HR_info.Link_state = hr_disconnect;      
            }
            
            wait_HR_disconnect_Flag = 0;
            
            
            if(disconnect_buffer_0xFF_Cnt == 5){
                disconnect_Sensor_E4(Linked_HR_info.SensorType);
          
            }else if(disconnect_buffer_0xFF_Cnt >5){
                Ask_Link_State_CE();
            }
             
        }

       
    }
    
    Ble_wait_disconnect_Time_out_Cnt = 20;
    
    //---------強制斷線 -------------
    /*
    if(BLE_HR_Force_disconnct_Flag == 1){
        disconnect_Sensor_E4(BLE_HR);
    }*/


}


void SensorReceive_CC(){

    if(ucBtmRxData[3]!=0xFF){
        wait_HR_disconnect_Flag = 0;
        
        disconnect_buffer_0xFF_Cnt = 0;
        memset(Linked_HR_info.DeviceName,0x00,13);
        Linked_HR_info.ANT_ID = Scan_Msg.ANT_ID;
        Linked_HR_info.SensorType = ANT_HR;
        Linked_HR_info.usHR_bpm = ucBtmRxData[3];
        Linked_HR_info.Link_state = Linked;
        
    }else if(ucBtmRxData[3] == 0xFF){                //心跳沒有數值 暫時離線狀態
        
        if(Linked_HR_info.SensorType == ANT_HR){
            disconnect_buffer_0xFF_Cnt++;
            
            Linked_HR_info.usHR_bpm = 0;
            wait_HR_disconnect_Flag = 0;
            
            if(disconnect_buffer_0xFF_Cnt == 5){
                disconnect_Sensor_E4(Linked_HR_info.SensorType);
            }else if(disconnect_buffer_0xFF_Cnt >5){
                Ask_Link_State_CF();
            }
            
        }
        

    }

}




//------------0xCE Tx  -------------------
void Ask_Link_State_CE(){
    
    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0xCE;
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}
//------------0xCE Rx  -------------------


void LinkStateReceive_CE(){
   
    Linked_HR_info.Link_state = (Linking_State_Def)ucBtmRxData[4];
    
    //確定離線了 才能進行新的掃描
    if(Linked_HR_info.Link_state == disconnect){
        
        wait_HR_disconnect_Flag = 0;
        //把原本還沒離線時存下來的任務拿出來執行
        
        Btm_Task_Adder(btm_HRC_disconnect_Task_Temp);
        btm_HRC_disconnect_Task_Temp = No_Task;
    }
    
    
    
    //---------------------手機 連線----------------------------------------
    if((ucBtmRxData[3] == 0x44) || (ucBtmRxData[3] == 0x46)){  //手機配對成功 丟一個39
        
        if(ucBtmRxData[3] == 0x44){
            
            if(System_Mode != StartUp)
            APP_Connected_Display_Cnt = 10;
            
        }else if(ucBtmRxData[3] == 0x46){

        }
        
        Btm_Task_Adder(C_39Val);
    }
    
}



//-------------------------------
//------------0xCF Tx  -------------------
void Ask_Link_State_CF(){
    
    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';	
    ucBtmTxBuf[19] = ']';
    
    ucBtmTxBuf[1] = 0xCF;
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}

//--------------0xCF Rx
void LinkStateReceive_CF(){


    Linked_HR_info.Link_state = (Linking_State_Def)ucBtmRxData[4];
    
    //確定離線了 才能進行新的掃描
    if(Linked_HR_info.Link_state == disconnect){
        wait_HR_disconnect_Flag = 0;
        //把原本還沒離線時存下來的任務拿出來執行
        
        disconnect_buffer_0xFF_Cnt = 0;
        
        Btm_Task_Adder(btm_HRC_disconnect_Task_Temp);
        btm_HRC_disconnect_Task_Temp = No_Task;
    }
    
}

//---------------------------------------------------



unsigned char  ucCmdPage;
unsigned short usAppStageNumber;
unsigned char  ucAppDataPage = 27;
unsigned char  ucAppErrflag;

ucAppTypeModeDef    ucAppTypeMode;

CloudRun_Init_INFO_Def CloudRun_Init_INFO;

unsigned char ucSPDBuffer[16];


void F_BtmReplyCmd(unsigned char data){
    
    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = data;
    ucBtmTxBuf[2] = ucCmdPage;
    
    if(ucAppErrflag)
        ucBtmTxBuf[3] = 0x41;
    else
        ucBtmTxBuf[3] = 0x40;
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}

void F_BtmGetSpd(unsigned char Addr,unsigned char num,unsigned char data){
    
    if(data < (Machine_Data.System_SPEED_Min) && data > (Machine_Data.System_SPEED_Max)){
        ucSPDBuffer[Addr+num] = (Machine_Data.System_SPEED_Min);
    }else{
        ucSPDBuffer[Addr+num] = data;
    }
    
}



//-------------------------------------------------0x35 Start-----------------------------------------------------

void F_BtmReply35Cmd(void){
    
    memset(ucBtmTxBuf,0x00,20);
    
    ucCmdPage = 0; //只有page 0有指令
    
    ucBtmTxBuf[0] = '[';					
    ucBtmTxBuf[1] = 0x35;
    ucBtmTxBuf[2] = ucCmdPage;
    //Type SystemMode Max SPD	Min SPD	Max Inc	MinInc	Inc NO	stageNumbers	stageGrowMode	 UNIT
    ucBtmTxBuf[3] = 0x02;        // 跑步機有揚升	
    //------------------
    if(System_Mode == Idle)
        ucBtmTxBuf[4] = 0;
    if(System_Mode == Prog_Set)
        ucBtmTxBuf[4] = 1;
    if(System_Mode == Ready)
        ucBtmTxBuf[4] = 2;
    if(System_Mode == WarmUp)
        ucBtmTxBuf[4] = 3;
    if(System_Mode == Workout)
        ucBtmTxBuf[4] = 4;
    if(System_Mode == CooolDown)
        ucBtmTxBuf[4] = 5;
    if(System_Mode == Paused)
        ucBtmTxBuf[4] = 6;
    if(System_Mode == Summary)
        ucBtmTxBuf[4] = 7;
    //------------------
    if(System_Unit == Metric){
        ucBtmTxBuf[5] = Machine_Data.System_SPEED_Max;    //公制 
        ucBtmTxBuf[6] = Machine_Data.System_SPEED_Min;
    }else{
        ucBtmTxBuf[5] = Machine_Data.System_SPEED_Max;    //英制 
        ucBtmTxBuf[6] = Machine_Data.System_SPEED_Min;
    }
    ucBtmTxBuf[7] = (Machine_Data.System_INCLINE_Max / 5);        // 0.0 % ~ 15.0% 共30階
    ucBtmTxBuf[8] = 0;
    ucBtmTxBuf[9] = 5;          //每階階數  0.5 or 1.0
    ucBtmTxBuf[10] = configUSE_SPDRamSize;
    ucBtmTxBuf[11] = 0;
    ucBtmTxBuf[12] = (unsigned char)System_Unit;
    //ucBtmTxBuf[13] =  0x00;  appProgrm(目前沒用)
    ucBtmTxBuf[19] = ']';
  
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
  HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
  
}
    

void F_BtmRead35Cmd(void){
    
    //unsigned char uci;
    ucCmdPage = ucBtmRxData[2];
    
    
    //取得APP模式     電子錶0/App1/雲跑(競賽)2 /虛跑3 訓練計畫4
    ucAppTypeMode               = (ucAppTypeModeDef)ucBtmRxData[3];   
    CloudRun_Init_INFO.TypeMode = (ucAppTypeModeDef)ucBtmRxData[3];  // TypeMode:電子錶0/App1/雲跑2/虛跑3/訓練距離)4/訓練時間)5
    
    
    //取得Stage Number
    usAppStageNumber         = ucBtmRxData[4]+(256*ucBtmRxData[5]);       // Stage:20 ~ 400
    CloudRun_Init_INFO.Stage = (unsigned short)(ucBtmRxData[5]<<8) + (unsigned short)ucBtmRxData[4];
    
    //--清掉 速度  揚升 TABLE
    memset(CloudRun_Init_INFO.CloudRun_Spd_Buffer,0x00,432);
    memset(CloudRun_Init_INFO.CloudRun_Inc_Buffer,0x00,432);
    
    
    Btm_Task_Adder(C_35Val);
    Cloud_Run_Initial_Busy_Flag = 1;
    ClearStd_1_Sec_Cnt();
}
//-------------------------------------------0x35 End-----------------------------------------------------------



//---------------------------0x36 Start----------------------------------------------
unsigned int uiAppTotalDist;
unsigned int uiAppStageDist;
unsigned short usSetTimeMin;
unsigned char  ucSetAge; 
unsigned short usSetWeight; 
unsigned short usLapDistance; 
unsigned char ucProgramIndex;

void F_BtmRead36Cmd(void){
    
    //unsigned char uci;  
    ucCmdPage = ucBtmRxData[2];
    Btm_Task_Adder(C_36Val);
    
    if((ucBtmRxData[3] == 0X02) && (System_Mode == Idle)){
        //--- 雲跑 ---
        uiAppTotalDist = (ucBtmRxData[11] + (256*ucBtmRxData[12]))*10;   // (10m)
        uiAppStageDist =  ucBtmRxData[13] + (256*ucBtmRxData[14]);        // (1m)
        
        CloudRun_Init_INFO.Total_Dist = ((unsigned short)ucBtmRxData[11] + (unsigned short)(ucBtmRxData[12]<<8)) *10; 
        CloudRun_Init_INFO.Stage_Dist =  (unsigned short)ucBtmRxData[13] + (unsigned short)(ucBtmRxData[14]<<8);  

        //------------
#if AppErrChk
        temp1 = (uiAppTotalDist*10)/uiAppStageDist;
        if(temp1!=usAppStageNumber && temp1!=(usAppStageNumber-1)){
            ucAppTypeMode = 0;     // TotalDistance與Stage異常取消設定
            ucAppErrflag = 1;
        }else{
            ucAppErrflag = 0;
        }
#endif
        if(ucAppTypeMode>=1 && ucAppTypeMode<=5){  //1.App  2.競賽  3.虛跑  4.訓練(距離) 5.訓練(時間)
            
            if(ucAppTypeMode == 1){
                ucProgramIndex = C_App_UserVal;
            }
            if(ucAppTypeMode == 2){
                
                ucProgramIndex = C_App_Dist1Val;     // 競賽
                Program_Select = APP_Cloud_Run;
                
            }
            if(ucAppTypeMode == 3){                // 虛跑
                ucProgramIndex = C_App_Dist2Val;
                
            }
            if(ucAppTypeMode == 4){                //訓練計畫(距離)
                ucProgramIndex = C_App_Training_Val;
                Program_Select = APP_Train_Dist_Run;
            }
            if(ucAppTypeMode == 5){                //訓練計畫(時間)
                ucProgramIndex = C_App_Training_Val;
                Program_Select = APP_Train_Time_Run;
            }
            
            
            CloudRun_Init_INFO.C_Time     = (unsigned short)ucBtmRxData[5] + (unsigned short)(ucBtmRxData[6]<< 8);
            CloudRun_Init_INFO.C_Age      =  ucBtmRxData[7];
            CloudRun_Init_INFO.C_Weight   = (unsigned short)ucBtmRxData[8] + (unsigned short)(ucBtmRxData[9]<<8);
            CloudRun_Init_INFO.PlayGround = ucBtmRxData[10];
            
            usSetTimeMin  = (ucBtmRxData[5] + (256*ucBtmRxData[6]))/60;
            ucSetAge      =  ucBtmRxData[7];
            usSetWeight   =  ucBtmRxData[8] + (256*ucBtmRxData[9]);
            usLapDistance =  ucBtmRxData[10] * 100;
            
            
            IntoReadyMode_Process();
                
        }
    }
}


void F_BtmReply36Cmd(void){
    
    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0x36;
    ucBtmTxBuf[2] = ucCmdPage;
    
    //00 =＞NONE。
    //01 =＞處於Workout設定模式。
    //02=＞處於321 倒數。
    //03=＞處於Workout Warm Up模式。
    //04 =＞處於Workout Running模式。
    //05 =＞處於Workout Cool Down模式。
    //06=＞處於Pause模式。
    //07=＞處於summary模式。
    //
    
    ucBtmTxBuf[3] = System_Mode;
    ucBtmTxBuf[4] = ucProgramIndex; 
    
    
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

    
    IntoReadyMode_Process(); //r進入運動
    
    Btm_Task_Adder(C_39Val);
    
    Cloud_Run_Initial_Busy_Flag = 0;
}

// -----------------------------  0x36 End  ----------------------------------------------------------






//-------------------------------------------0x37  存速度 table  Start-----------------------------------------------------------

unsigned char ucNextPage;
void F_BtmRead37Cmd(void){
    
    unsigned char  uci;
    unsigned short temp1;
    
    ucAppErrflag = 0;
    
    ucCmdPage = ucBtmRxData[2];
    
    if(ucCmdPage == 0){
        ucNextPage = 1;
    }else if(ucCmdPage != ucNextPage){
        ucAppErrflag = 1;          //丟錯包 或是掉包的情況發生 回0x41
        ucCmdPage = ucNextPage;    //告訴APP要重傳     回傳應該要傳的page
    }else{
        ucNextPage = ucCmdPage + 1;
    }
    
    if(ucAppErrflag != 1){
        switch(ucAppTypeMode){
            
          case Digital_Watch:    //電子錶
            // 一般模式不接受APP的Profile
            break;
          case App:     //ALA Fitness
            //暫時不支援
            break;
          case Cloud_Run:   //雲跑
            break;
            
          case void_Run:   //虛跑
          case Train_Run_Dist:  //訓練(距離)
          case Train_Run_Time:  //訓練(時間)
            if(ucCmdPage < ucAppDataPage){        // 限制<432筆資料(420)
                temp1 = 16 * ucCmdPage;
                for(uci = 0 ;uci < 16;uci++){
                    F_BtmGetSpd( 0 ,temp1 + uci ,ucBtmRxData[uci+3]);
                    CloudRun_Init_INFO.CloudRun_Spd_Buffer[temp1 + uci] = ucBtmRxData[uci+3];
                }
            }else{
                ucAppErrflag = 1;
            }
            break;
        }
    }
    
    
    F_BtmReplyCmd(0x37);
    
}
//-------------------------------------------0x37 End-----------------------------------------------------------

//-------------------------------------------0x38  存揚升  table   Start-----------------------------------------------------------

void F_BtmRead38Cmd(void){
    
  unsigned char uci;
  unsigned short temp1;
  
  ucAppErrflag = 0;
  ucCmdPage = ucBtmRxData[2];

  if(ucCmdPage == 0){
      ucNextPage = 1;
  }else if(ucCmdPage != ucNextPage){
      ucAppErrflag = 1;          //丟錯包 或是掉包的情況發生 回0x41
      ucCmdPage = ucNextPage;    //告訴APP要重傳     回傳應該要傳的page
  }else{
      ucNextPage = ucCmdPage + 1;
  }
  
  if(ucAppErrflag != 1){
      switch(ucAppTypeMode){
          
        case Digital_Watch:   //電子錶
          // 一般模式不接受APP的Profile
          break;
        case App:   //ALA Fitness
          //暫時不支援
          break;
        case Cloud_Run:   //雲跑
        case void_Run:    //虛跑
        case Train_Run_Dist:  //訓練(距離)
        case Train_Run_Time:  //訓練(時間)
          if(ucCmdPage < ucAppDataPage){        // 限制<432筆資料
              temp1 = 16*ucCmdPage;
              for(uci = 0 ;uci < 16;uci++){
                  if(ucBtmRxData[uci+3] <= Machine_Data.System_INCLINE_Max){ 
                      CloudRun_Init_INFO.CloudRun_Inc_Buffer[temp1 + uci] = ucBtmRxData[uci+3];
                  }else{
                      CloudRun_Init_INFO.CloudRun_Inc_Buffer[temp1 + uci] = 0;
                  }
              }
          }else{
              ucAppErrflag = 1;
          }
          break;
      }
  }
  
  F_BtmReplyCmd(0x38);
  
}

//---------------------------0x38 END----------------------------------------------



//-------------------------------0x39  Start-----------------------------------------------------

extern unsigned short WarmUp_3_Minute_Cnt;
extern unsigned short CoolDown_3_Minute_Cnt;
unsigned short         usSetTimeMin;
unsigned long          ulAltitude;


unsigned short  usSegmentNumber;

Cmd_39_outData_Def  Cloud_0x39_Info;


void F_BtmReply39Cmd(void){
    
    
    //__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
   //__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
    
    unsigned char uci;
    unsigned int  temp32;
    
    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0x39;
    ucBtmTxBuf[2] = ucCmdPage;
    //---------------

    
    ucBtmTxBuf[3] = System_Mode;
    
    Cloud_0x39_Info.c_System_Mode = System_Mode;
    
    //--------------- 時間 ------------------------
    if(System_Mode == WarmUp){ 
        ucBtmTxBuf[4] = WarmUp_3_Minute_Cnt%256;
        ucBtmTxBuf[5] = WarmUp_3_Minute_Cnt/256;
        
        Cloud_0x39_Info.c_Times = WarmUp_3_Minute_Cnt;
        
    }else{
        if(System_Mode == CooolDown){     
            ucBtmTxBuf[4] = CoolDown_3_Minute_Cnt%256;
            ucBtmTxBuf[5] = CoolDown_3_Minute_Cnt/256;
            
            Cloud_0x39_Info.c_Times = CoolDown_3_Minute_Cnt;
        }else{
            
            usSetTimeMin = Program_Data.Goal_Time / 60;
            
            if(usSetTimeMin == 0){
                ucBtmTxBuf[4] = Program_Data.Goal_Counter%256;
                ucBtmTxBuf[5] = Program_Data.Goal_Counter/256;
                
                 Cloud_0x39_Info.c_Times = Program_Data.Goal_Counter;
                 
            }else{
                ucBtmTxBuf[4] = (((usSetTimeMin*60)-Program_Data.Goal_Counter)%256);
                ucBtmTxBuf[5] = (((usSetTimeMin*60)-Program_Data.Goal_Counter)/256);
                
                Cloud_0x39_Info.c_Times = (usSetTimeMin*60)-Program_Data.Goal_Counter;
            }
            
        }
    }
     
    
    //-------------------------------------------------------------------------------------
    
    
    
    //-----------------------心跳 ------------------------------------
    if((usNowHeartRate > 0)){
        ucBtmTxBuf[6] = usNowHeartRate;        
    }else{
        ucBtmTxBuf[6] = 0;
    }
    
    Cloud_0x39_Info.c_HeartRate = usNowHeartRate;
    
    //----------------------------------------------------------------
    
    
    
    //------------------- 階段 ---------------------------------------
    
    usSegmentNumber = (Cloud_0x39_Info.c_Distance / uiAppStageDist);
    
    /*if(usSegmentNumber> 40){     // 參展先這樣處理(只處理40行以內資料)  190329 取消限制
        usSegmentNumber &= 0xFF;
    }*/
    
    ucBtmTxBuf[7] = (usSegmentNumber%256);
    ucBtmTxBuf[8] = (usSegmentNumber/256);
    
    Cloud_0x39_Info.c_Stage = usSegmentNumber;
    //----------------------------------------------------------------
    
    
    //------------ Speed --------------------------------------------
    unsigned short SpeedTmp;
    
    
    if(System_Unit == Metric){  
       SpeedTmp = System_SPEED;
    }else if(System_Unit == Imperial){
       SpeedTmp = (System_SPEED * 16)/10;
    }
    
    ucBtmTxBuf[9] = SpeedTmp;   
    Cloud_0x39_Info.c_Speed = SpeedTmp;
    //------------------------------------------------------------
    
    
    //----------- Incline ---------------
    ucBtmTxBuf[10] = System_INCLINE/5;  //ucTargetIncline
    Cloud_0x39_Info.c_Incline = System_INCLINE/5;
    //--------------------------------------
    
    if(ucCmdPage == 0){
        
        ucCmdPage = 1;
        
        /* 0 :距離H 距離 距離L 卡路里H 卡路里 卡路里L */
        
        if(System_Unit == Metric){  
            temp32 = Program_Data.Distance/100;  
        }else if(System_Unit == Imperial){
            temp32 = (Program_Data.Distance*16)/1000;  
        }
        
          
        ucBtmTxBuf[11] = (temp32 & 0xFF);
        ucBtmTxBuf[12] = ((temp32 >> 8) & 0xFF);
        ucBtmTxBuf[13] = ((temp32 >> 16) & 0xFF);
        
        Cloud_0x39_Info.c_Distance = Program_Data.Distance/100; 
        
        
        temp32 = Program_Data.Calories/100000;
        ucBtmTxBuf[14] = (temp32 & 0xFF);
        ucBtmTxBuf[15] = ((temp32 >> 8) & 0xFF);
        ucBtmTxBuf[16] = ((temp32 >> 16) & 0xFF);
        
        Cloud_0x39_Info.c_Calories = Program_Data.Calories/100000;
        
    }else{
        
        ucCmdPage = 0;
        
        /* 1 : STEP_H STEP STEP_L ALT_H ALT ALT_L */
        temp32 = Program_Data.Pace;
        
        ucBtmTxBuf[11] =  temp32&0xFF;        // step
        ucBtmTxBuf[12] = (temp32>>8)&0xFF;
        ucBtmTxBuf[13] = (temp32>>16)&0xFF;
        
        Cloud_0x39_Info.c_Step = Program_Data.Pace;
        
        //---------------
        temp32 = ulAltitude/100;
        ucBtmTxBuf[14] = (temp32 & 0xFF);   // ALT
        ucBtmTxBuf[15] = ((temp32 >> 8) & 0xFF);
        ucBtmTxBuf[16] = ((temp32 >> 16) & 0xFF);
        //---------------
        Cloud_0x39_Info.c_ALTI = ulAltitude;
        
        ucBtmTxBuf[17] =  Pace_Freq;  //usStepFrequencyFlag
        Cloud_0x39_Info.c_SPFreq = Pace_Freq;
    
    }
    //--------------------------
    temp32 = ucBtmTxBuf[4];
    
    for(uci=3;uci<9;uci++){
        temp32 += ucBtmTxBuf[uci*2];
    }
    
    ucBtmTxBuf[18] = temp32;
    //--------------------------
    ucBtmTxBuf[19] = ']';
    
    
    BTM_UART_Transmit();
    
    __asm("NOP");
    //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    
}

//----------------------------------------0x39 End --------------------------------------------------------



//----------------------------------------0x44 Start -----生產序號-----------------------------
void F_BtmReply44Cmd(void){
    
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    
    //從Flash   讀Serial Number回來  -- ucProductionSerialNumber --
      
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0x44;
    
    ucBtmTxBuf[2] = 0;  //  "0: Write  1: Read     
    
    ucBtmTxBuf[3] = ucProductionSerialNumber[0];     // Year
    
    ucBtmTxBuf[4] = ucProductionSerialNumber[1];     // week
    ucBtmTxBuf[5] = ucProductionSerialNumber[2];     // week
    
    ucBtmTxBuf[6]  = ucProductionSerialNumber[3];    // Type
    ucBtmTxBuf[7]  = ucProductionSerialNumber[4];    // Type
    ucBtmTxBuf[8]  = ucProductionSerialNumber[5];    // Type
    ucBtmTxBuf[9]  = ucProductionSerialNumber[6];    // Type
    ucBtmTxBuf[10] = ucProductionSerialNumber[7];    // Type
    
    //-----------------新的序號定義--------------------------------
    
    ucBtmTxBuf[11] = ucProductionSerialNumber[8];   // 廠商ID
    ucBtmTxBuf[12] = ucProductionSerialNumber[9];   // 廠商ID
    
    ucBtmTxBuf[13] = ucProductionSerialNumber[10];  // Serial Number
    ucBtmTxBuf[14] = ucProductionSerialNumber[11];  // Serial Number
    ucBtmTxBuf[15] = ucProductionSerialNumber[12];  // Serial Number
    ucBtmTxBuf[16] = ucProductionSerialNumber[13];  // Serial Number
    
    //SN  Tool連線才能在 Serial 看的到這兩碼    這兩碼亂丟 IOS 雲跑連線會閃退
    ucBtmTxBuf[17] = '0';  
    ucBtmTxBuf[18] = '0'; 
    
    ucBtmTxBuf[19] = ']';
    
    
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    
    
}


void F_BtmRead44Cmd(void){
    
  ucCmdPage = ucBtmRxData[2];
  
  if(ucCmdPage == 0){     // App 0:Write 1:Read
      
      //-------------- Write --------------------------------
      
      // 2017/6/21生產第一台 => A25T030000001
      ucProductionSerialNumber[0] = ucBtmRxData[3];      // Year
      ucProductionSerialNumber[1] = ucBtmRxData[4];      // week
      ucProductionSerialNumber[2] = ucBtmRxData[5];      // week
      ucProductionSerialNumber[3] = ucBtmRxData[6];      // Type
      ucProductionSerialNumber[4] = ucBtmRxData[7];      // Type
      ucProductionSerialNumber[5] = ucBtmRxData[8];      // Type
      ucProductionSerialNumber[6] = ucBtmRxData[9];      // Type
      ucProductionSerialNumber[7] = ucBtmRxData[10];     // Type
      
      //---------新SN2 DEF---------2碼 廠商ID   4碼流水碼-----------------
      ucProductionSerialNumber[8] = ucBtmRxData[11];     //   廠商辨識碼  Serial Number
      ucProductionSerialNumber[9] = ucBtmRxData[12];     //   廠商辨識碼  Serial Number
      
      ucProductionSerialNumber[10] = ucBtmRxData[13];    // Serial Number
      ucProductionSerialNumber[11] = ucBtmRxData[14];    // Serial Number
      ucProductionSerialNumber[12] = ucBtmRxData[15];    // Serial Number
      ucProductionSerialNumber[13] = ucBtmRxData[16];    // Serial Number
      
    
      //改名字 Btm 需要reset   等待F1  重置完成   btm_is_ready --> 1  藍芽任務回復正常執行
      Btm_SetDeviceName(ucProductionSerialNumber,14);  // BTM Device 修改0x44收到的SerialNumber名字
      btm_is_ready = 0; 
      
      //-----------------------Flash------------------------------------
      Write_SerialNumber_To_Flash(ucProductionSerialNumber);  //將序號寫入Flash
      Read_SerialNumber_From_Flash(ucProductionSerialNumber);  //讀出Flash
        
      
      
      __asm("NOP");

  }else if(ucCmdPage == 1){
      //-----------------  Read  --------------------------------------------
      F_BtmReply44Cmd();
      
  }

}

//----------------------------------------0x44 End --------------------------------------------------------

//----------------------------------------0x46 Start ---------總里程 ODO  -----------------------------



unsigned int Machine_Total_Time_tmp;
unsigned int Machine_Total_Distance_tmp;

void F_BtmReply46Cmd(void){
    
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);       //防止資料 資料丟到一半 RX中斷來亂
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0x46;
    ucBtmTxBuf[2] = 0;      //Page 0   
    
    Machine_Total_Time_tmp     = Machine_Data.Total_Times;
    Machine_Total_Distance_tmp = Machine_Data.TotalDistance;

    ucBtmTxBuf[3] = (unsigned char)Machine_Total_Time_tmp;
    ucBtmTxBuf[4] = (unsigned char)(Machine_Total_Time_tmp >> 8);
    ucBtmTxBuf[5] = (unsigned char)(Machine_Total_Time_tmp >> 16);
    ucBtmTxBuf[6] = (unsigned char)(Machine_Total_Time_tmp >> 24);
    
    ucBtmTxBuf[7]  = (unsigned char)Machine_Total_Distance_tmp;
    ucBtmTxBuf[8]  = (unsigned char)(Machine_Total_Distance_tmp >> 8);
    ucBtmTxBuf[9]  = (unsigned char)(Machine_Total_Distance_tmp >> 16);
    ucBtmTxBuf[10] = (unsigned char)(Machine_Total_Distance_tmp >> 24);
    
    if((Machine_Data.Total_Times>=360000)||(Machine_Data.TotalDistance>=10000000)){
        ucBtmTxBuf[11] = 1;           // Lube ?
    }else{
        ucBtmTxBuf[11] = 0;
    }
    
    ucBtmTxBuf[12] = 0x00;       // NA
    ucBtmTxBuf[13] = 0x00;       // NA
    ucBtmTxBuf[14] = 0x00;       // NA
    ucBtmTxBuf[15] = 0x00;       // NA
    ucBtmTxBuf[16] = 0x00;       // NA
    ucBtmTxBuf[17] = 0x00;       // NA
    ucBtmTxBuf[18] = 0x00;       // NA
    ucBtmTxBuf[19] = ']';
    
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //-----------資料丟完再開啟中斷-----
    
}

void F_BtmRead46Cmd(void){
    
    ucCmdPage = ucBtmRxBuf[2];
    F_BtmReply46Cmd();
}

//----------------------------------------0x46 End --------------------------------------------------------

//----------------------------------------0x47 Start--------回應 Console版本 + RF版本  現在是OTA Mode?  -------------------------------

void F_BtmReply47Cmd(void){
    
     __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);       //防止資料 資料丟到一半 RX中斷來亂

    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0x47;
    ucBtmTxBuf[2] = ucCmdPage;
    
    if(System_Mode == Sys_OTA_Mode)
        ucBtmTxBuf[3] = 0x01;         // OTA Mode
    else
        ucBtmTxBuf[3] = 0x00;         // Not OTA
    
    //Console版本
    ucBtmTxBuf[4] = System_Version[2]-'0';   //EepromVer_3%10;       // Ver Low        Console version L
    ucBtmTxBuf[5] = System_Version[1]-'0';   //EepromVer_3/10;       // Ver High        Console version H

    ucBtmTxBuf[6] = (Modify_Version[1]-'0')*10 + (Modify_Version[2]-'0');  //EepromVer_4 // Axx   Console modify
        
    //RF版本
    ucBtmTxBuf[7] = ucRFVersion[0];       // RF L    BLE version L
    ucBtmTxBuf[8] = ucRFVersion[1];       // RF M    BLE version M
    ucBtmTxBuf[9] = ucRFVersion[2];       // RF H     BLE version H
    

    //控制器版本 
    ucBtmTxBuf[10] = 0;//((ucProductionSerialNumber[23]%10)+0x30);    
    ucBtmTxBuf[11] = 0;//((ucProductionSerialNumber[23]/10)+0x30);
    ucBtmTxBuf[19] = ']';
    
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    
}

void F_BtmRead47Cmd(void){
    
  ucCmdPage = ucBtmRxData[2];
  
  if(ucCmdPage == 0){
      //Btm_Task_Adder(C_47Val);
      F_BtmReply47Cmd();
  }
}


//----------------------------------------0x47 End --------------------------------------------------------


//---------------------------------------0x50  Start -------------------------------------------------------------------

void F_BtmRead50Cmd(void){
    

    if(System_Mode == Idle ){ //只有在 Idle Idle Mode才能進入更新
        
        //畫面清空 顯示OTA
        Turn_OFF_All_Segment();
        Turn_OFF_All_LEDMatrix();
        
        Flash_Write_OTA_Mode();
        HAL_NVIC_SystemReset();
        
    }else{ 
        //其他模式 warm up   pause  workout ... 都拒絕更新
        Btm_Task_Adder(C_50Val);
    }
 
}

void F_BtmReply50Cmd(void){
    
    
    if(System_Mode != Sys_OTA_Mode){
        memset(ucBtmTxBuf,0x00,20);
        
        ucBtmTxBuf[0] = '[';
        ucBtmTxBuf[1] = 0x50;
        ucBtmTxBuf[2] = 0;
          
        ucBtmTxBuf[3] = 0x41;         //  0x41 告知無法更新
        
        ucBtmTxBuf[19] = ']';
        
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
        HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    }
    
}


//--------------------------------------0x50  End-------------------------------------------------------


//----------------------------------------0xB0 Start --------------------------------------------------------




unsigned int  FTMS_Data_Flag;          //實際上只用3個byte
unsigned char FTMS_Data_Flag_3byte[3];

void Config_FTMS_Data_Flag(FTMS_Machine_Type_Def Type){


    if(Type == treadmill){

        
        //--------------------FTMS Treadmill data flag------------------//
        typedef enum{
            average_speed_present   = 0x000002,                               
            total_distance_present  = 0x000004,
            inclination_and_ramp_angle_present = 0x000008,
            elevation_gain_present             = 0x000010,
            treadmill_instantaneouse_pace_present = 0x000020,
            treadmill_average_pace_present        = 0x000040,
            expended_energy_present         = 0x000080,
            heart_rate_present              = 0x000100,
            metalbolic_equivalent_present   = 0x000200,
            elapsed_time_present            = 0x000400,
            remaining_time_present 	   = 0x000800,
            force_belt_power_output_present = 0x001000,
            
        }FTMS_Treadmill_Flag_Def;
        
        
#if FTMS_Test
           //-----------------------------------設定 要廣播的  運動資料-------------------------------------------------------------
        FTMS_Data_Flag =  heart_rate_present | treadmill_instantaneouse_pace_present | treadmill_average_pace_present | 
                          elevation_gain_present | inclination_and_ramp_angle_present | metalbolic_equivalent_present |
                          force_belt_power_output_present | total_distance_present | elapsed_time_present | remaining_time_present;
                   
#else
           //-----------------------------------設定 要廣播的  運動資料-------------------------------------------------------------
        FTMS_Data_Flag =  heart_rate_present | treadmill_instantaneouse_pace_present | treadmill_average_pace_present | 
                          elevation_gain_present | inclination_and_ramp_angle_present | metalbolic_equivalent_present |
                          force_belt_power_output_present | total_distance_present | elapsed_time_present | remaining_time_present;
                   
#endif
        
          
     
          // expended_energy_present  
        
        //average_speed_present       
        //-----------------------------------------------------------------------------------------------------------------------
        
    }else if(Type == cross_trainer){
     
        //--------------------FTMS   Cross Trainer data flag------------------//
        typedef enum{
            average_speed_present  = 0x000002,
            total_distance_present = 0x000004,
            step_count_present     = 0x000008,
            stride_count_present   = 0x000010,
            elevation_gain_present = 0x000020,
            inclination_and_ramp_angle_present = 0x000040,
            resistance_level_present      = 0x000080,
            instantaneouse_power_present  = 0x000100,
            average_power_present         = 0x000200,
            expended_energy_present       = 0x000400,
            heart_rate_present            = 0x000800,
            metalbolic_equivalent_present = 0x001000,
            elapsed_time_present    = 0x002000,
            remaining_time_present  = 0x004000,	
            move_direction_backward_present = 0x008000,	
            
        }FTMS_Cross_Trainer_Flag_Def;
        
        
        
    }else if(Type == step_climber){
    
    }else if(Type == stair_climber){
    
    }else if(Type == rower){
    
    }else if(Type == indoor_bike){
    
         //--------------------FTMS   Indoor Bike data flag------------------//
        typedef enum{
            average_speed_present           = 0x000002,
            instantaneouse_cadence_present  = 0x000004,
            average_cadence_present         = 0x000008,
            total_distance_present          = 0x000010,
            resistance_level_present	    = 0x000020,
            instantaneouse_power_present    = 0x000040,
            average_power_present           = 0x000080,
            expended_energy_present         = 0x000100,
            heart_rate_present              = 0x000200,
            metalbolic_equivalent_present   = 0x000400,
            elapsed_time_present            = 0x000800,
            remaining_time_present          = 0x001000,
  	
        }FTMS_Indoor_Bike_Flag_Def;
        
        //-----------------------------------設定 要廣播的  運動資料-------------------------------------------------------------
        /* FTMS_Data_Flag = average_speed_present   | average_power_present | average_cadence_present
                           total_distance_present 
                           metalbolic_equivalent_present 
                          elapsed_time_present | remaining_time_present;*/
        //-----------------------------------------------------------------------------------------------------------------------
        
         //-----------------------------------設定 要廣播的  運動資料-------------------------------------------------------------
        FTMS_Data_Flag = resistance_level_present | instantaneouse_cadence_present | instantaneouse_power_present |  heart_rate_present | expended_energy_present;                   
                           
        //-----------------------------------------------------------------------------------------------------------------------
        
        
 
    }
    

    FTMS_Data_Flag_3byte[0] = (unsigned char)FTMS_Data_Flag;
    FTMS_Data_Flag_3byte[1] = (unsigned char)(FTMS_Data_Flag>>8);
    FTMS_Data_Flag_3byte[2] = (unsigned char)(FTMS_Data_Flag>>16);
}


unsigned int FTMS_Feature;
unsigned int FTMS_Control;

unsigned char FTMS_Feature_4byte[4];
unsigned char FTMS_Control_4byte[4];




//----------------------------------FTMS Feature---------------------------------//

typedef enum{
    
    average_speed_support    = 0x000001,                   
    cadence_support          = 0x000002,                   
    total_distance_support   = 0x000004,                   
    inclination_support      = 0x000008,                   
    elevation_gain_support   = 0x000010,
    pace_support             = 0x000020,
    step_count_support       = 0x000040,
    resistance_level_support = 0x000080,
    stride_count_support     = 0x000100,
    expended_energy_support  = 0x000200,
    heart_rate_support       = 0x000400,
    metalbolic_equivalent_support = 0x000800,
    elapsed_time_support          = 0x001000,
    remaining_time_support        = 0x002000,
    power_measurement_support     = 0x004000,
    forceOnbelt_and_powerOutput_support = 0x008000,
    user_data_retention_support         = 0x010000,
    
}FTMS_Feature_Def;

//------------------------FTMS Feature  Control ---------------------------//
typedef enum{
    
    speed_target_setting_support        = 0x000001,                           
    inclination_target_setting_support  = 0x000002,            
    resistance_target_setting_support   = 0x000004,            
    power_target_setting_support        = 0x000008,            
    heart_rate_target_setting_support   = 0x000010,
    target_expended_energy_configuration_support = 0x000020,
    target_step_number_configuartion_support     = 0x000040,
    target_stride_number_configuration_support   = 0x000080,
    target_distance_configuration_support        = 0x000100,
    target_training_time_configuration_support   = 0x000200,
    target_time_in_two_heart_rate_zone_configuration_support   = 0x000400,
    target_time_in_three_heart_rate_zone_configuration_support = 0x000800,
    target_time_in_five_heart_rate_zone_configuration_support  = 0x001000,
    indoor_bike_simulation_parameter_support  = 0x002000,
    wheel_circumference_configuration_support = 0x004000,
    spindown_control_support                  = 0x008000,
    target_cadence_configuration_support      = 0x010000,
    
}FTMS_Control_Def;



//0x2ACC
void FTMS_Feature_Config(FTMS_Machine_Type_Def Type){

    
    if(Type == treadmill){
    

#if FTMS_Test
        //---------------------------- FTMS  設定 Treadmill  Feature -------------------------------------------------- 
        FTMS_Feature = heart_rate_support | pace_support | elevation_gain_support | inclination_support  |
                       metalbolic_equivalent_support | forceOnbelt_and_powerOutput_support | total_distance_support |
                       elapsed_time_support | remaining_time_support | expended_energy_support;
            
        //  expended_energy_support (只要丟卡路里資料出來  Kinomap就收不到心跳= =)
        //average_speed_support                     
         
        //---------------------------- FTMS  設定 Treadmill Control Feature-------------------------------------------------- 
        FTMS_Control = inclination_target_setting_support | heart_rate_target_setting_support |
             target_expended_energy_configuration_support | target_step_number_configuartion_support |
             target_distance_configuration_support        | target_training_time_configuration_support;    
        
#else
        
        //---------------------------- FTMS  設定 Treadmill  Feature -------------------------------------------------- 
        FTMS_Feature = heart_rate_support | pace_support | elevation_gain_support | inclination_support  |
                       metalbolic_equivalent_support | forceOnbelt_and_powerOutput_support | total_distance_support |
                       elapsed_time_support | remaining_time_support | expended_energy_support | expended_energy_support;
            
        //  expended_energy_support (只要丟卡路里資料出來  Kinomap就收不到心跳= =)
        //average_speed_support                     
         
        //---------------------------- FTMS  設定 Treadmill Control Feature-------------------------------------------------- 
        FTMS_Control = inclination_target_setting_support | heart_rate_target_setting_support |
             target_expended_energy_configuration_support | target_step_number_configuartion_support |
             target_distance_configuration_support        | target_training_time_configuration_support;      
#endif

        
        
    }else if(Type == indoor_bike){
    
        //---------------------------- FTMS  設定 Indoor Bike  Feature -------------------------------------------------- 
        FTMS_Feature = resistance_level_support | cadence_support | power_measurement_support | heart_rate_support | expended_energy_support ;
        
        
        // total_distance_support |  average_speed_support | elapsed_time_support | remaining_time_support; 
        
        //---------------------------- FTMS  設定Indoor Bike Control Feature-------------------------------------------------- 
       FTMS_Control =  spindown_control_support;// resistance_target_setting_support | power_target_setting_support |
                       // heart_rate_target_setting_support | target_expended_energy_configuration_support |
                        //target_distance_configuration_support | target_training_time_configuration_support |
                        //wheel_circumference_configuration_support | target_cadence_configuration_support |
                        //indoor_bike_simulation_parameter_support;
         
    }    

    FTMS_Feature_4byte[0] = (unsigned char)FTMS_Feature;
    FTMS_Feature_4byte[1] = (unsigned char)(FTMS_Feature>>8);
    FTMS_Feature_4byte[2] = (unsigned char)(FTMS_Feature>>16);
    FTMS_Feature_4byte[3] = (unsigned char)(FTMS_Feature>>24);

    FTMS_Control_4byte[0] = (unsigned char)FTMS_Control;
    FTMS_Control_4byte[1] = (unsigned char)(FTMS_Control>>8);
    FTMS_Control_4byte[2] = (unsigned char)(FTMS_Control>>16);
    FTMS_Control_4byte[3] = (unsigned char)(FTMS_Control>>24);

}


FTMS_Machine_Type_Def  Machine_Type = treadmill;   //  treadmill  indoor_bike

void F_Btm_FTMS_B0(unsigned char ucPage){
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB0;
    ucBtmTxBuf[2] = ucPage;           // 

    
    if(ucPage == 0){     //  設定 Feature   fitness machine feature（uuid：0x2ACC)
        
        FTMS_Feature_Config(Machine_Type);
           
        ucBtmTxBuf[3] = FTMS_Feature_4byte[0];   // 0~7
        ucBtmTxBuf[4] = FTMS_Feature_4byte[1];   // 8~15
        ucBtmTxBuf[5] = FTMS_Feature_4byte[2];   // 16~23
        ucBtmTxBuf[6] = FTMS_Feature_4byte[3];   // 24~31
        
        ucBtmTxBuf[7]  = FTMS_Control_4byte[0];   // 0~7
        ucBtmTxBuf[8]  = FTMS_Control_4byte[1];   // 8~15
        ucBtmTxBuf[9]  = FTMS_Control_4byte[2];   // 16~23
        ucBtmTxBuf[10] = FTMS_Control_4byte[3];   // 24~31
          
        btm_is_ready = 0; //如果有修改  BTM 會重新 Reset  等下一個F1
        
    }else if(ucPage == 1){
        
        // 設定    Machine   Type
        //treadmill       0x0001              v 設定為跑步機
        //cross trainer   0x0002
        //step climber    0x0004
        //stair climber   0x0008
        //rower           0x0010
        //indoor bike     0x0020

        ucBtmTxBuf[3] = (unsigned char)Machine_Type;    //Type Low byte
        ucBtmTxBuf[4] = 0x00;                            //Type High byte high byte 暫時固定0x00  
        
        
    }else if(ucPage == 2){ 
        
 
        Config_FTMS_Data_Flag(Machine_Type);
        
        ucBtmTxBuf[3] = FTMS_Data_Flag_3byte[0];      
        ucBtmTxBuf[4] = FTMS_Data_Flag_3byte[1];      
        ucBtmTxBuf[5] = FTMS_Data_Flag_3byte[2];      
        
    }
    

         
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
    
}

/*
void F_Btm_FTMS_B0_HR_Switch(unsigned char SW){
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB0;
    ucBtmTxBuf[2] = 2;           // 

    ucBtmTxBuf[3] = 0xEE;   //0x02 + 0x04 + 0x08 + 0x20 + 0x40 + 0x80;  //  0xEE   0xFF;   // 
    
    if(SW){
        ucBtmTxBuf[4] = 0x0F;   //0x01 + 0x02 + 0x04 + 0x08;     //0x0F;   //  有心跳
    }else if(SW == 0){
        ucBtmTxBuf[4] = 0x0E;   // 0x02 + 0x04 + 0x08;           //0x0F;   //  沒心跳
    }
    ucBtmTxBuf[5] = 0x00;           //     
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
    
}*/


void F_Btm_FTMS_B0_Read(void){
    
    btm_is_ready = 1;  //如果有收到 B0 的 Response 代表Feature 沒有修改 不需要Reset 直接  讓btm_is_ready = 1
    
    ucRFVersion[0] = ucBtmRxData[3];
    ucRFVersion[1] = ucBtmRxData[4];
    ucRFVersion[2] = ucBtmRxData[5];  
}


//----------------------------------------0xB0 End --------------------------------------------------------


//----------------------------------------0xB1 Start --------------------------------------------------------

unsigned short i_RPM_sim = 0;
unsigned short a_RPM_sim = 0;

unsigned char PageSwitch = 1;  //跑步機資料在page 0 和 1

unsigned int  uiDistance_1m;   //把 Program_Data.Distance 轉成一公尺
unsigned int  uiCalories_1kcal; //把 Program_Data.Calories 轉成1 kcal

unsigned short sim_speed     = 0;
unsigned short sim_HeartRate = 0x64;
unsigned int   sim_Distance_1m  = 0; //
unsigned short sim_Aspeed = 0;

unsigned short sim_i_Power = 87;
unsigned short sim_a_Power = 0;

unsigned short sim_incline = 0;
unsigned short simLevel = 0;


unsigned short sim_Cal    = 0;
unsigned short sim_Cal_HR = 0;
unsigned char  sim_Cal_MI = 0;

unsigned char  sim_Met_eq = 0;


unsigned char Met_Tmp = 0;


void IndooeBike_B1(){

    memset(ucBtmTxBuf,0x00,20);
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB1;

    if(PageSwitch == 4){
        PageSwitch = 0;
    }else{
        PageSwitch++;
    }
    
    ucBtmTxBuf[2] =   PageSwitch ;  
    
    if(PageSwitch == 0){
        
        //-------SPEED--------
        //Kinomap  Indoor Bike  吃這筆 i-speed ------------------------------------
        ucBtmTxBuf[3] =   (unsigned char)sim_speed;       // instantaeous speed L      
        ucBtmTxBuf[4] =   (unsigned char)(sim_speed>>8);  // instantaeous speed H      
        //-------------------------------------------------------------------------

    }else if(PageSwitch == 1){
        
        //------Calorie---------------------------------------------------------------------------
        uiCalories_1kcal = Program_Data.Calories/100000;
        ucBtmTxBuf[4] = (unsigned char)sim_Cal;      //(unsigned char)uiCalories_1kcal;           // total energy L
        ucBtmTxBuf[5] = (unsigned char)(sim_Cal>>8); //(unsigned char)(uiCalories_1kcal>>8);      // total energy H
        // (0xFFFF => Data Not Available)( 1 kilo calorie)
        //---------------------------------------------------------------------------------------
        
        //-----Calorie/Hour ----------------------------------------------------------------------------------------
        ucBtmTxBuf[6] = (unsigned char)sim_Cal_HR;       //(unsigned char)Program_Data.Calories_HR;          // energy per hour L  (unsigned char)sim_Cal;     
        ucBtmTxBuf[7] = (unsigned char)(sim_Cal_HR>>8);  //(unsigned char)(Program_Data.Calories_HR>>8);     // energy per hour H  (unsigned char)(sim_Cal>>8);
        // (0xFF => Data Not Available)( 1 kilo calorie)
        //---------------------------------------------------------------------------------------------------
        
        //-----Calorie/Minute --------------------------------
        ucBtmTxBuf[8] = (unsigned char)sim_Cal_MI;//Program_Data.Calories_HR/60;   // energy per minute
        //----------------------------------------------------
        
        //---- Heart Rate -------------------------------------
        if(usNowHeartRate){
            ucBtmTxBuf[9] = usNowHeartRate;  //sim_HeartRate;
        }else{
            ucBtmTxBuf[9] = 0;    //0x01;
        }
        //-----------------------------------------------------

    }else if(PageSwitch == 2){

        //------Resistance  LEVEL  -------------------------------------------------------------------
        //模組丟出去會 "除以 10"
        ucBtmTxBuf[9]  =  (unsigned char)(simLevel*10);       // resistance level(sign)  L     單位 0.1    
        ucBtmTxBuf[10] =  (unsigned char)((simLevel*10)>>8);  // resistance level(sign)  H
        //------------------------------------------------------------------------------
        
        //-------Power----------------------------------------------------------------------------------
        //Kinomap 吃 I-power   // Zwift 吃得到 Power
        ucBtmTxBuf[11] = (unsigned char)sim_i_Power;         // instantaneous power(sign) L    
        ucBtmTxBuf[12] = (unsigned char)(sim_i_Power>>8);    // instantaneous power(sign) H
        //-----------------------------------------------------------------------------------------------   
 
    }else if(PageSwitch == 3){
        
        //-----   RPM  ---------------------------------------------------------------
        //kinomap 吃  i_RPM     
        ucBtmTxBuf[3] = (unsigned char)i_RPM_sim;       // instantaneous cadence L  
        ucBtmTxBuf[4] = (unsigned char)(i_RPM_sim>>8);  // instantaneous cadence H     
        //-----------------------------------------------------------------------------
    }

    ucBtmTxBuf[19] = ']';
    
    
    BTM_UART_Transmit();
    //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    //HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}

short sim_INCL = 0x0000; //(sign)	
short sim_Ramp = 0x0000; //(sign)	

unsigned short sim_Pos_Ele_Gain;   //(us)
unsigned short sim_Neg_Ele_Gain;   //(us)

unsigned char sim_I_Pace;
unsigned char sim_A_Pace;

short sim_Force_Belt;
short sim_Power_Output;

unsigned short sim_Remain_Time;   //(us)
unsigned short sim_Elapse_Time;   //(us)


#if FTMS_Test

void Test_Data_Brocast(){
    
    if(PageSwitch == 0){
        
        
        //------SPEED--------
        //Kinomap  Indoor Bike  吃這筆 i-speed ------------------------------------
        ucBtmTxBuf[3] =  0XC1;       // instantaeous speed L      
        ucBtmTxBuf[4] =  0XC1;  // instantaeous speed H      
        //-------------------------------------------------------------------------
        
        //------INCLINE---------------------------------------------------
        //current inclination(當前的傾斜角度) (0.1 percent)
        ucBtmTxBuf[10] =  0XC4;  // sim_incline;       // inclination (sign)L
        ucBtmTxBuf[11] =  0XC4;  // inclination (sign)H
        //current setting of the ramp angle (斜坡角度當前設置) (0.1 degree)
        
        //-------RAMP-----------------------------------------
        ucBtmTxBuf[12] = 0XC4;          // ramp and angle set (sign)L
        ucBtmTxBuf[13] = 0XC4;     // ramp and angle set (sign)H
        //-----------------------------------------------------------
        
        //-----Elevation Gain------------------------------------------
        //(正高度增益)
        ucBtmTxBuf[14] = 0XC5;     // positive elevation gain L 
        ucBtmTxBuf[15] = 0XC5;     // positive elevation gain H
        //(負仰角增益)
        ucBtmTxBuf[16] =  0XC5;          // negative elevation gain L 
        ucBtmTxBuf[17] =  0XC5;     // negative elevation gain H        
        //--------------------------------------------------------
       
        //----Instantaneous Pace-----------------------------------------------------------
        ucBtmTxBuf[18] =  0XC6;  //Pace_Spd/60;  // treadmill instantaneous pace  // (瞬間步伐) (0.1km per minute)
        //-------------------------------------------------------------------------------

        //---Total Distance---------------------------------------------------------------------------------
        uiDistance_1m = Program_Data.Distance/100;                  //  單位 1公尺
        ucBtmTxBuf[7] = 0XC3;          //(unsigned char)(uiDistance_1m);          // total distance L     
        ucBtmTxBuf[8] = 0XC3 ;    //(unsigned char)(uiDistance_1m >> 8) ;    // total distance M     
        ucBtmTxBuf[9] = 0XC3;    //(unsigned char)(uiDistance_1m >> 16);    // total distance H  
        //--------------------------------------------------------------------------------------------------
   
    }else if(PageSwitch == 1){
        
        //----Average Pace--------------------------------------------------------------------
        ucBtmTxBuf[3] = 0XC7;  ///(600*usAvgCount)/uiAvgSpd;  // treadmill average pace // (平均步伐) (0.1km per minute)
        //------------------------------------------------------------------------------------

        //------Calorie---------------------------------------------------------------------------
        uiCalories_1kcal = Program_Data.Calories/100000;
        ucBtmTxBuf[4] = 0XC8;   //(unsigned char)uiCalories_1kcal;           // total energy L
        ucBtmTxBuf[5] = 0XC8;   //(unsigned char)(uiCalories_1kcal>>8);      // total energy H
        // (0xFFFF => Data Not Available)( 1 kilo calorie)
        //---------------------------------------------------------------------------------------
        
        //-----Calorie/Hour ----------------------------------------------------------------------------------------
        ucBtmTxBuf[6] = 0XC8;       //(unsigned char)Program_Data.Calories_HR;          // energy per hour L  (unsigned char)sim_Cal;     
        ucBtmTxBuf[7] = 0XC8;  //(unsigned char)(Program_Data.Calories_HR>>8);     // energy per hour H  (unsigned char)(sim_Cal>>8);
        // (0xFF => Data Not Available)( 1 kilo calorie)
        //---------------------------------------------------------------------------------------------------
        
        //-----Calorie/Minute --------------------------------
        ucBtmTxBuf[8] = 0XC8;   //  Program_Data.Calories_HR/60// energy per minute
        //----------------------------------------------------

         //---- Heart Rate -------------------------------------
        if(usNowHeartRate){
            ucBtmTxBuf[9] = 0XC9;  //sim_HeartRate;
        }else{
            ucBtmTxBuf[9] = 0XC9;    //0x01;
        }
        //-----------------------------------------------------
        
        //----Metalolic Equivalent --------------------------------------------------------------------
        ucBtmTxBuf[10] = 0XCA;  //usMET  // metalbolic equivalent  // ( 0.1 metalbolic equivalent)
        //----------------------------------------------------------------------------------------------
       
        //---Elapsed Time------------------------------------------------------------ 
        ucBtmTxBuf[11] = 0XCB;   //elapsed time L
        ucBtmTxBuf[12] = 0XCB;   //elapsed time H 
        //--------------------------------------------------------------------------

        //---Remaining Time------------------------------------------------------------ 
        ucBtmTxBuf[13] = 0XCC;      //remaining time L
        ucBtmTxBuf[14] = 0XCC;      //remaining time H
        //--------------------------------------------------------------------------
        
        //----Force on Belt-------------------------------------------------------------------------------------------------
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[15] = 0XCD;   //0xFF;   // force on belt(sign)L   // (使用者步伐對跑帶加減速造成的力)(1 newton)
        ucBtmTxBuf[16] = 0XCD;   //0x7F;   // force on belt(sign)H
       //-----------------------------------------------------------------------------------------------------------
        
        //---Power Output-------------------------------------------------------------------------------------------
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[17] = 0XCD;  //0xFF;   // power output(sign)L     // (使用者步伐對跑步機加減速造成的力)(1 watt)
        ucBtmTxBuf[18] = 0XCD;  //0x7F;   // power output(sign)H
        //-------------------------------------------------------------------------------------------------------------
        
        
        //----------------------------------------------時間資料---------------------------------------------

    }else if(PageSwitch == 2){
       
    }else if(PageSwitch == 3){

    } 
    
}

#endif

void Treadmill_B1(){

    memset(ucBtmTxBuf,0x00,20);

    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB1;
    
    //page 0 --> page4 loop
    
    if(PageSwitch == 1){
        PageSwitch = 0;
    }else{
        PageSwitch++;
    }
    
    ucBtmTxBuf[2] =   PageSwitch ;   
    
#if FTMS_Test
    Test_Data_Brocast();
#else
    
    if(PageSwitch == 0){
        
        
        //------SPEED--------
        //Kinomap  Indoor Bike  吃這筆 i-speed ------------------------------------
        ucBtmTxBuf[3] =   (unsigned char)(System_SPEED*10);       // instantaeous speed L      
        ucBtmTxBuf[4] =   (unsigned char)((System_SPEED*10)>>8);  // instantaeous speed H      
        //-------------------------------------------------------------------------
        
        //------INCLINE---------------------------------------------------
        //current inclination(當前的傾斜角度) (0.1 percent)
        ucBtmTxBuf[10] =  (unsigned char)System_INCLINE;  // sim_incline;       // inclination (sign)L
        ucBtmTxBuf[11] =  (unsigned char)(System_INCLINE >> 8);                 // inclination (sign)H
        //current setting of the ramp angle (斜坡角度當前設置) (0.1 degree)
        
        //-------RAMP-----------------------------------------
        ucBtmTxBuf[12] = (unsigned char)sim_Ramp;          // ramp and angle set (sign)L
        ucBtmTxBuf[13] = (unsigned char)(sim_Ramp>>8);     // ramp and angle set (sign)H
        //-----------------------------------------------------------
        
        //-----Elevation Gain------------------------------------------
        //(正高度增益)
        ucBtmTxBuf[14] = (unsigned char)sim_Pos_Ele_Gain;          // positive elevation gain L 
        ucBtmTxBuf[15] = (unsigned char)(sim_Pos_Ele_Gain>>8);     // positive elevation gain H
        //(負仰角增益)
        ucBtmTxBuf[16] = (unsigned char)sim_Neg_Ele_Gain;          // negative elevation gain L 
        ucBtmTxBuf[17] = (unsigned char)(sim_Neg_Ele_Gain>>8);     // negative elevation gain H        
        //--------------------------------------------------------
       
        //----Instantaneous Pace-----------------------------------------------------------
        ucBtmTxBuf[18] =  System_SPEED/60;  //sim_I_Pace;  // treadmill instantaneous pace  // (瞬間步伐) (0.1km per minute)
        //-------------------------------------------------------------------------------

        //---Total Distance---------------------------------------------------------------------------------
        uiDistance_1m = Program_Data.Distance/100;             //  單位 1公尺                
        ucBtmTxBuf[7] = (unsigned char)(uiDistance_1m);        // total distance L    (unsigned char)(sim_Distance_1m );     
        ucBtmTxBuf[8] = (unsigned char)(uiDistance_1m >> 8) ;  // total distance M     (unsigned char)(sim_Distance_1m >> 8) ;
        ucBtmTxBuf[9] = (unsigned char)(uiDistance_1m >> 16);  // total distance H     (unsigned char)(sim_Distance_1m >> 16);
        //--------------------------------------------------------------------------------------------------
   
    }else if(PageSwitch == 1){
        
        //----Average Pace--------------------------------------------------------------------
        ucBtmTxBuf[3] = System_SPEED/60;  ///(600*usAvgCount)/uiAvgSpd;  // treadmill average pace // (平均步伐) (0.1km per minute)
        //------------------------------------------------------------------------------------

        //------Total Calorie---------------------------------------------------------------------------
        uiCalories_1kcal = Program_Data.Calories/1000;
        ucBtmTxBuf[4] = (unsigned char)uiCalories_1kcal;      //(unsigned char)sim_Cal;          // total energy L
        ucBtmTxBuf[5] = (unsigned char)(uiCalories_1kcal>>8); //(unsigned char)(sim_Cal>>8);     // total energy H
        // (0xFFFF => Data Not Available)( 1 kilo calorie)
        //---------------------------------------------------------------------------------------
        
        //-----Calorie/Hour ----------------------------------------------------------------------------------------
        ucBtmTxBuf[6] = (unsigned char)Program_Data.Calories_HR;       //(unsigned char)sim_Cal_HR;        // energy per hour L 
        ucBtmTxBuf[7] = (unsigned char)(Program_Data.Calories_HR>>8);  //(unsigned char)(sim_Cal_HR>>8);   // energy per hour H 
        // (0xFF => Data Not Available)( 1 kilo calorie)
        //---------------------------------------------------------------------------------------------------
        
        //-----Calorie/Minute --------------------------------
        ucBtmTxBuf[8] = Program_Data.Calories_HR/60;   // (unsigned char)sim_Cal_MI // energy per minute
        //----------------------------------------------------

         //---- Heart Rate -------------------------------------
        if(usNowHeartRate){
            ucBtmTxBuf[9] = usNowHeartRate;  //sim_HeartRate;
        }else{
            ucBtmTxBuf[9] = 0;    //0x01;
        }
        //-----------------------------------------------------
        
        //----Metalolic Equivalent --------------------------------------------------------------------
        ucBtmTxBuf[10] = usMET/10;  //sim_Met_eq   // metalbolic equivalent  // ( 0.1 metalbolic equivalent)
        //----------------------------------------------------------------------------------------------
       
        //---Elapsed Time------------------------------------------------------------ 
        //ucBtmTxBuf[11] = (unsigned char) sim_Elapse_Time;       //elapsed time L
        //ucBtmTxBuf[12] = (unsigned char)(sim_Elapse_Time>>8);   //elapsed time H 
        
        ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;  // elapsed time L  流逝的時間
        ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;  // elapsed time H
        //--------------------------------------------------------------------------

        
        
        
        //---Remaining Time------------------------------------------------------------ 
        ucBtmTxBuf[13] = (unsigned char)sim_Remain_Time;       //remaining time L     sim_Remain_Time
        ucBtmTxBuf[14] = (unsigned char)(sim_Remain_Time>>8);   //remaining time H    sim_Remain_Time>>8
        //--------------------------------------------------------------------------
        
        //----Force on Belt-------------------------------------------------------------------------------------------------
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[15] = (unsigned char)sim_Force_Belt;         //0xFF;   // force on belt(sign)L   // (使用者步伐對跑帶加減速造成的力)(1 newton)
        ucBtmTxBuf[16] = (unsigned char)(sim_Force_Belt>>8);    //0x7F;   // force on belt(sign)H
       //-----------------------------------------------------------------------------------------------------------
        
        //---Power Output-------------------------------------------------------------------------------------------
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[17] = (unsigned char)sim_Power_Output;       //0xFF;   // power output(sign)L     // (使用者步伐對跑步機加減速造成的力)(1 watt)
        ucBtmTxBuf[18] = (unsigned char)(sim_Power_Output>>8);  //0x7F;   // power output(sign)H
        //-------------------------------------------------------------------------------------------------------------
        
        
        //----------------------------------------------時間資料---------------------------------------------
        /*
        usSetTimeMin = Program_Data.Goal_Time / 60;
        if(usSetTimeMin == 0){
            ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;  // elapsed time L  流逝的時間
            ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;  // elapsed time H
            //Cloud_0x39_Info.c_Times = (Program_Data.Goal_Time - Program_Data.Goal_Counter);
            
        }else{
            ucBtmTxBuf[13] = Program_Data.Goal_Counter%256;  // remaining time L   剩下的時間
            ucBtmTxBuf[14] = Program_Data.Goal_Counter/256;  // remaining time H
            
            //Cloud_0x39_Info.c_Times = Program_Data.Goal_Counter;
        }
        // (1 second)
        if(System_Mode == WarmUp){
            ucBtmTxBuf[11] =(180 - WarmUp_3_Minute_Cnt);    // elapsed time L
            ucBtmTxBuf[12] = 0;                             // elapsed time H
        }else{
            if(System_Mode == CooolDown){
                ucBtmTxBuf[11] = (180-CoolDown_3_Minute_Cnt);     // elapsed time L
                ucBtmTxBuf[12] = 0;                               // elapsed time H
            }else{
                ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;     // elapsed time L
                ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;     // elapsed time H
            }
        }
        //---------------
        if(System_Mode == WarmUp){
            ucBtmTxBuf[13] =  WarmUp_3_Minute_Cnt;   // remaining time L
            ucBtmTxBuf[14] = 0;                             // remaining time H
        }else{
            if(System_Mode == CooolDown){
                ucBtmTxBuf[13] = CoolDown_3_Minute_Cnt;       // remaining time L
                ucBtmTxBuf[14] = 0;                           // remaining time H
            }else{
                
                ucBtmTxBuf[13] = Program_Data.Goal_Counter%256;    // remaining time L
                ucBtmTxBuf[14] = Program_Data.Goal_Counter/256;    // remaining time H
            }
        }*/
        
     
    }else if(PageSwitch == 2){
       
    }else if(PageSwitch == 3){
        /*
        ucBtmTxBuf[15] = 0x00;     // lap  (for ANT+ FEC)
        
        ucBtmTxBuf[16] = 0x00;     // negative vertical distance difference  for ANT+ FEC
        ucBtmTxBuf[17] = 0x00;     // positive vertical distance difference  for ANT+ FEC
        
        ucBtmTxBuf[18] = 0x00;     //Reserve
        */
    }
#endif
    
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}


void OLDTreadmill_B1__(){

    memset(ucBtmTxBuf,0x00,20);

    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB1;
    
    //page 0 --> page4 loop
    
    if(PageSwitch == 4){
        PageSwitch = 0;
    }else{
        PageSwitch++;
    }
    
    ucBtmTxBuf[2] =   PageSwitch ;           //  先固定丟 Page 3  的RPM資料出來
    
    if(PageSwitch == 0){
        
        //---------------
        //Kinomap  Indoor Bike  吃這筆 i-speed
        ucBtmTxBuf[3] =   (unsigned char)sim_speed;        //(unsigned char)( System_SPEED * 10);         // instantaeous speed L         (unsigned char)( sim_speed * 10);    
        ucBtmTxBuf[4] =   (unsigned char)(sim_speed>>8);  //(unsigned char)((System_SPEED * 10)>>8);    // instantaeous speed H          (unsigned char)((sim_speed * 10)>>8);
        //---------------
        ucBtmTxBuf[5] = (unsigned char)sim_Aspeed;        //(unsigned char)uiAvgSpeed;          // average speed L   //先丟0
        ucBtmTxBuf[6] = (unsigned char)(sim_Aspeed >>8);  //(unsigned char)(uiAvgSpeed >> 8);   // average speed H
        //---------------
        uiDistance_1m = Program_Data.Distance/100;                  //  單位 1公尺
        
        ucBtmTxBuf[7] =  (unsigned char)(sim_Distance_1m);          //(unsigned char)(uiDistance_1m);          // total distance L     
        ucBtmTxBuf[8] =  (unsigned char)(sim_Distance_1m >> 8) ;    //(unsigned char)(uiDistance_1m >> 8) ;    // total distance M     
        ucBtmTxBuf[9] =  (unsigned char)(sim_Distance_1m >> 16);    //(unsigned char)(uiDistance_1m >> 16);    // total distance H     
        
        //---------------
        //current inclination(當前的傾斜角度) (0.1 percent)
        ucBtmTxBuf[10] =  (System_INCLINE);  // sim_incline;       // inclination (sign)L
        ucBtmTxBuf[11] = 0x00;                                     // inclination (sign)H
        //current setting of the ramp angle (斜坡角度當前設置) (0.1 degree)
        
        //(0x7FFF => Data Not Available)
        ucBtmTxBuf[12] = 0x00;     // ramp and angle set (sign)L
        ucBtmTxBuf[13] = 0x00;     // ramp and angle set (sign)H
        //(正高度增益)
        ucBtmTxBuf[14] = 0x00;     // positive elevation gain L 
        ucBtmTxBuf[15] = 0x00;     // positive elevation gain H
        //(負仰角增益)
        ucBtmTxBuf[16] = 0x00;     // negative elevation gain L 
        ucBtmTxBuf[17] = 0x00;     // negative elevation gain H
        // (瞬間步伐) (0.1km per minute)
        ucBtmTxBuf[18] =  0; //Pace_Spd/60;        // treadmill instantaneous pace
        
        
    }else if(PageSwitch == 1){
        
        //---------------------
        // (平均步伐) (0.1km per minute)
        ucBtmTxBuf[3] = 0;   ///(600*usAvgCount)/uiAvgSpd;       // treadmill average pace
        // (0xFFFF => Data Not Available)( 1 kilo calorie)
        
        uiCalories_1kcal = Program_Data.Calories/100000;
        
        ucBtmTxBuf[4] = 0;//(unsigned char)uiCalories_1kcal;           // total energy L
        ucBtmTxBuf[5] = 0;//(unsigned char)(uiCalories_1kcal>>8);      // total energy H
        // (0xFFFF => Data Not Available)( 1 kilo calorie)
        
        ucBtmTxBuf[6] = 0;//(unsigned char)Program_Data.Calories_HR;          // energy per hour L  (unsigned char)sim_Cal;     
        ucBtmTxBuf[7] = 0;//(unsigned char)(Program_Data.Calories_HR>>8);     // energy per hour H  (unsigned char)(sim_Cal>>8);
        // (0xFF => Data Not Available)( 1 kilo calorie)
        ucBtmTxBuf[8] = 0;//Program_Data.Calories_HR/60;   // energy per minute
        
        if(usNowHeartRate){
            ucBtmTxBuf[9] = usNowHeartRate;  //sim_HeartRate;
        }else{
            ucBtmTxBuf[9] = 0;//0x01;
        }
        
        // ( 0.1 metalbolic equivalent)
        ucBtmTxBuf[10] = Met_Tmp;//usMET;   //;  // metalbolic equivalent
        
        //----------------------------------------------時間資料---------------------------------------------
        /*
        usSetTimeMin = Program_Data.Goal_Time / 60;
        
        if(usSetTimeMin == 0){
        ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;  // elapsed time L  流逝的時間
        ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;  // elapsed time H
        
        //Cloud_0x39_Info.c_Times = (Program_Data.Goal_Time - Program_Data.Goal_Counter);
        
    }else{
        ucBtmTxBuf[13] = Program_Data.Goal_Counter%256;  // remaining time L   剩下的時間
        ucBtmTxBuf[14] = Program_Data.Goal_Counter/256;  // remaining time H
        
        //Cloud_0x39_Info.c_Times = Program_Data.Goal_Counter;
    }
        
        
        // (1 second)
        if(System_Mode == WarmUp){
        ucBtmTxBuf[11] =(180 - WarmUp_3_Minute_Cnt);    // elapsed time L
        ucBtmTxBuf[12] = 0;                             // elapsed time H
    }else{
        if(System_Mode == CooolDown){
        ucBtmTxBuf[11] = (180-CoolDown_3_Minute_Cnt);     // elapsed time L
        ucBtmTxBuf[12] = 0;                               // elapsed time H
    }else{
        ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;     // elapsed time L
        ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;     // elapsed time H
    }
    }
        //---------------
        if(System_Mode == WarmUp){
        ucBtmTxBuf[13] =  WarmUp_3_Minute_Cnt;   // remaining time L
        ucBtmTxBuf[14] = 0;                             // remaining time H
    }else{
        if(System_Mode == CooolDown){
        ucBtmTxBuf[13] = CoolDown_3_Minute_Cnt;       // remaining time L
        ucBtmTxBuf[14] = 0;                           // remaining time H
    }else{
        
        ucBtmTxBuf[13] = Program_Data.Goal_Counter%256;    // remaining time L
        ucBtmTxBuf[14] = Program_Data.Goal_Counter/256;    // remaining time H
        
    }
    }*/
        
        ucBtmTxBuf[11] = 0;
        ucBtmTxBuf[12] = 0;
        ucBtmTxBuf[13] = 0;
        ucBtmTxBuf[14] = 0;
        
        //----------------------------------------------時間資料---------------------------------------------
        
        // (使用者步伐對跑帶加減速造成的力)(1 newton)
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[15] = 0;//0xFF;   // force on belt(sign)L
        ucBtmTxBuf[16] = 0;//0x7F;   // force on belt(sign)H
        // (使用者步伐對跑步機加減速造成的力)(1 watt)
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[17] = 0;//0xFF;   // power output(sign)L
        ucBtmTxBuf[18] = 0;//0x7F;   // power output(sign)H
        //---------------------------
        
    }else if(PageSwitch == 2){
        
        //---------------
        ucBtmTxBuf[3] =    0;    // step per minute L       (unsigned char)( sim_speed * 10);    
        ucBtmTxBuf[4] =    0;    // step per minute H          (unsigned char)((sim_speed * 10)>>8);
        //---------------
        ucBtmTxBuf[5] = 0;   // average step rate L   //先丟0
        ucBtmTxBuf[6] = 0;   // average step rate H
        //---------------
        
        ucBtmTxBuf[7] =  0;    //stride count L        
        ucBtmTxBuf[8] =  0;   // stride count H    
        
        //-----------------------------------------------------------------------------
        //模組丟出去會 "除以 10"
        ucBtmTxBuf[9]  =  (unsigned char)(simLevel*10);       // resistance level(sign)  L     單位 0.1    丟1 假資料
        ucBtmTxBuf[10] =  (unsigned char)((simLevel*10)>>8);  // resistance level(sign)  H
        //------------------------------------------------------------------------------
        
        //------------------------------------------------------------------------------------------------
        //Kinomap 吃 I-power
        ucBtmTxBuf[11] = (unsigned char)sim_i_Power;       // instantaneous power(sign) L    丟100 測試
        ucBtmTxBuf[12] = (unsigned char)(sim_i_Power>>8);  // instantaneous power(sign) H
        //-----------------------------------------------------------------------------------------------
        
        //---------------------------------------------------------------------------------------------
        ucBtmTxBuf[13] = (unsigned char)sim_a_Power;       // average power(sign)  L        丟100 測試
        ucBtmTxBuf[14] = (unsigned char)(sim_a_Power>>8);  // average power(sign)  H
        //-----------------------------------------------------------------------------------------------
        
        ucBtmTxBuf[15] = 0x00;     // floors L
        ucBtmTxBuf[16] = 0x00;     // floors H 
        
        ucBtmTxBuf[17] = 0x00;     // step count L
        ucBtmTxBuf[18] = 0x00;     // step count H
        
        
    }else if(PageSwitch == 3){
        
        //---------------
        //kinomap 吃  i_RPM     //單獨丟  Zwift 吃得到
        ucBtmTxBuf[3] = (unsigned char)i_RPM_sim;       // instantaneous cadence L   //先丟87*2 0xAE
        ucBtmTxBuf[4] = (unsigned char)(i_RPM_sim>>8);  // instantaneous cadence H     
        //---------------
        ucBtmTxBuf[5] = (unsigned char)a_RPM_sim;       // average cadence  L   ////先丟87*2 0xAE
        ucBtmTxBuf[6] = (unsigned char)(a_RPM_sim>>8);  // average cadence  H
        //---------------
        
        ucBtmTxBuf[7] =  0;    //average stroke rate
        
        ucBtmTxBuf[8] =  0;   // stroke rate （or cadence of FEC Rower page )   
        
        
        ucBtmTxBuf[9]  = 0;    // stroke count  L
        ucBtmTxBuf[10] = 0;    // stroke count  H
        
        ucBtmTxBuf[11] = 0x00;     // rower instantaneous pace L    
        ucBtmTxBuf[12] = 0x00;     // rower instantaneous pace H
        
        ucBtmTxBuf[13] = 0x00;     // rower average pace  L        丟100 測試
        ucBtmTxBuf[14] = 0x00;     // rower average pace  H
        
        ucBtmTxBuf[15] = 0x00;     // lap  (for ANT+ FEC)
        
        ucBtmTxBuf[16] = 0x00;     // negative vertical distance difference  for ANT+ FEC
        ucBtmTxBuf[17] = 0x00;     // positive vertical distance difference  for ANT+ FEC
        
        ucBtmTxBuf[18] = 0x00;     //Reserve
        
        
    }
    
    
    
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}


void F_Btm_FTMS_B1(){
    
    if(Machine_Type == indoor_bike){
        IndooeBike_B1();
    }else if(Machine_Type == treadmill){
        Treadmill_B1();
    }

}

void F_Btm_FTMS_B1_Read(void){
      
}

//----------------------------------------0xB1 End --------------------------------------------------------



//----------------------------------------0xB2 Start --------------------------------------------------------

typedef enum{

   reset                       = 0x01,
   set_target_speed            = 0x02,
   set_target_inclination      = 0x03,
   set_target_resistance_level = 0x04,
   set_target_power            = 0x05,
   set_target_heart_rate       = 0x06,
   start_or_resume             = 0x07,
   stop_or_pause               = 0x08,
   set_target_expended_energy  = 0x09,
   set_target_number_of_step    = 0x0A,
   set_target_number_of_stride  = 0x0B,
   set_target_distance          = 0x0C,
   set_target_training_time     = 0x0D,
   set_target_time_in_two_heart_rate_zone = 0x0E,
   set_target_time_in_three_heart_rate_zone = 0x0F,
   set_target_time_in_five_heart_rate_zone = 0x10,
   set_indoor_bike_simulation_parameter = 0x11,
   set_wheel_circumference              = 0x12,
   spindown_control                     = 0x13,
   set_target_cadence                   = 0x14,
   
   //------------------FEC page ------------------
   page_0_start_calibration  = 0x2F,     //page 0
   page_48_resistance        = 0x30,     //page 48
   page_49_power             = 0x31,     //page 49
   page_50_wind_resistance   = 0x32,     //page 50
   page_51_track_resistance  = 0x33,     //page 51

}OP_Code_Def;

OP_Code_Def FTMS_OP_Code;


unsigned short usGrade_FEC_Temp;
short sGrade_FEC;     //只取整數部分
float fGrade_FEC;     //有小數點    
unsigned char RR_FEC; //rolling resistance  摩擦力   //單位 5*10 exp-5

void FEC_Page51_Track_Decode(){
    
    usGrade_FEC_Temp = ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);
    RR_FEC = ucBtmRxData[5];
    
    
    if(usGrade_FEC_Temp >= 20000){
        
        fGrade_FEC = (float)((usGrade_FEC_Temp - 20000) * 0.01f);
        
        if((usGrade_FEC_Temp - 20000) < 50){
            sGrade_FEC = 0;
        }else{
            for(unsigned char i = 0 ;i < 30; i++){
                if(( (usGrade_FEC_Temp - 20000)  > (i*100 +50)) && ((usGrade_FEC_Temp - 20000) < ((i+1)*100+50) )){
                    sGrade_FEC = (i+1);
                }
            }   
        }
        
    }else{
        
        fGrade_FEC = (float)((usGrade_FEC_Temp - 20000 ) * 0.01f * 2);
        
        if( (20000 - usGrade_FEC_Temp) < 50){
            sGrade_FEC = 0;
        }else{
            for(unsigned char i = 0 ;i < 30; i++){
                if(( (20000 - usGrade_FEC_Temp)  > (i*50 +25)) && ((20000 - usGrade_FEC_Temp) < ((i+1)*50+25) ) ){
                    sGrade_FEC = (-1*i)-1;
                }
            }   
        } 
    }
}

short usGrade_Temp_FTMS;   //FTMS 傳過來的原始資料
short sGrade_FTMS;      //只取整數部分
unsigned char ucCRR_FTMS; //coefficient of rolling resistance  輪胎磨擦係數   單位 0.0001
unsigned short usWindSpeed_FTMS; // 風速   單位  0.001 meter per second

void Bike_Simulation_Decode(){

    usWindSpeed_FTMS  = ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);
    usGrade_Temp_FTMS = ucBtmRxData[5] + (unsigned short)(ucBtmRxData[6]<<8);
    ucCRR_FTMS        = ucBtmRxData[7];
        
    sGrade_FTMS =(short)usGrade_Temp_FTMS*2;

    if(sGrade_FTMS >= 0){
        
        if((sGrade_FTMS % 100) >= 50){
            
            sGrade_FTMS = (sGrade_FTMS/100) +1;
                
        }else if((sGrade_FTMS %100) <50){
            
            sGrade_FTMS = sGrade_FTMS/100;
        }
        
    }else if(sGrade_FTMS < 0){
        
        sGrade_FTMS = (short)sGrade_FTMS * 2;
        
        
        if((sGrade_FTMS % 100) <= -50){
            
            sGrade_FTMS = (sGrade_FTMS/100) - 1;
            
        }else if((sGrade_FTMS %100) > -50){
            
            sGrade_FTMS = sGrade_FTMS/100;
        }
    }
}


unsigned int B2_Event_Cnt = 0;

typedef struct{

    unsigned short target_Speed;
    unsigned short target_Incline;
    unsigned char  target_R_Level;
    unsigned short target_Power;
    
    unsigned char  target_HeartRate;
    unsigned short target_Calorie;
    unsigned short target_Step;
    unsigned short target_Stride;
    unsigned int   target_Distance;  
    unsigned short target_Training_Time;
    
    unsigned short target_FatBurn_Time;
    unsigned short target_FitnessBurn_Time;
    
    unsigned short target_VeryLight_Time;
    unsigned short target_Light_Time;
    unsigned short target_Moderate_Time;
    unsigned short target_Hard_Time;
    unsigned short target_Maximun_Time;
        
    unsigned short wheel_Circumference;
    unsigned short target_Cadence;
    

}FTMS_Control_Value_Def;

FTMS_Control_Value_Def FTMS_Control_Value;

OP_Code_Def B2_Event_List[50];

void F_Btm_FTMS_B2_Read(){
    
    FTMS_OP_Code = (OP_Code_Def)ucBtmRxData[2];
    B2_Event_List[B2_Event_Cnt%50] = FTMS_OP_Code;
    B2_Event_Cnt++;
    
    
     //------------接收 FEC Page 51 坡度----------------------------
    if(FTMS_OP_Code == page_51_track_resistance){   
        FEC_Page51_Track_Decode();
    }
    if(FTMS_OP_Code == set_indoor_bike_simulation_parameter){
        Bike_Simulation_Decode();
    }
    
    if(FTMS_OP_Code == set_target_speed){
        FTMS_Control_Value.target_Speed = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }
 
    if(FTMS_OP_Code == set_target_inclination){
        
        Buzzer_BeeBee(300, 3);
        
        FTMS_Control_Value.target_Incline = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
        if(System_Mode == Workout){
            
            unsigned short tmp = FTMS_Control_Value.target_Incline;
            System_INCLINE = (tmp/5)*5;
            Rst_Incline_Blink();
            INCL_Moveing_Flag = 0;
            RM6_Task_Adder(Set_INCLINE);
            Update_BarArray_Data_Ex();
        }
    
    }
    
#if FTMS_Activated_Permission   //防止被Zwift隨機啟動
    if(FTMS_OP_Code == start_or_resume){
        if(System_Mode == Idle){
            Quick_Start_Init();       //Zwift 沒事就會一直下這個cmd (包括掃描裝置時)
            IntoReadyMode_Process();
        }
    }
#endif
    
    if(FTMS_OP_Code == set_target_resistance_level){
        FTMS_Control_Value.target_R_Level = (unsigned char)ucBtmRxData[3];  
    }
    
    if(FTMS_OP_Code == set_target_power){
        FTMS_Control_Value.target_Power = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }
    
    if(FTMS_OP_Code == set_target_heart_rate){
        FTMS_Control_Value.target_HeartRate = (unsigned char)ucBtmRxData[3];  
    }
    
    if(FTMS_OP_Code == stop_or_pause){
        if(ucBtmRxData[3] == 0x01){ 
            //----Stop---/
            
            if(System_Mode == Workout){
                
               
                IntoSummaryMode_Process(); //190115 直接進入summary
                
                Btm_Task_Adder(FTMS_Data_Broadcast); //Page 0 //廣播運動資料 歸零
                Btm_Task_Adder(FTMS_Data_Broadcast); //Page 1
                
                Set_SPEED_Value(0);
                Set_INCLINE_Value(0);
                RM6_Task_Adder(Set_SPEED);
                RM6_Task_Adder(Set_INCLINE);
                RM6_Task_Adder(Motor_STOP);
                
               Buzzer_BeeBee(Time_Set, Cnt_Set);
            }
            __asm("NOP");
        }else if(ucBtmRxData[3] == 0x02){
            //---Pause---/
             __asm("NOP");
        }                
    }
    
    if(FTMS_OP_Code == set_target_expended_energy){
        FTMS_Control_Value.target_Calorie = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }
    
    if(FTMS_OP_Code == set_target_number_of_step){
        FTMS_Control_Value.target_Step = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }
    
    if(FTMS_OP_Code == set_target_number_of_stride){
        FTMS_Control_Value.target_Stride = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }

    if(FTMS_OP_Code == set_target_distance){
        FTMS_Control_Value.target_Distance = (unsigned int)ucBtmRxData[3] + (unsigned int)(ucBtmRxData[4]<<8) + (unsigned int)(ucBtmRxData[5]<<16);  
    }
    
    if(FTMS_OP_Code == set_target_training_time){
        FTMS_Control_Value.target_Training_Time = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }
    
    
    if(FTMS_OP_Code == set_target_time_in_two_heart_rate_zone){
        FTMS_Control_Value.target_FatBurn_Time = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
        FTMS_Control_Value.target_FitnessBurn_Time = (unsigned short)ucBtmRxData[5] + (unsigned short)(ucBtmRxData[6]<<8);  
    }
    
    
    if(FTMS_OP_Code == set_target_time_in_three_heart_rate_zone){
        
        FTMS_Control_Value.target_VeryLight_Time = 0;
        FTMS_Control_Value.target_Light_Time    = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);
        FTMS_Control_Value.target_Moderate_Time = (unsigned short)ucBtmRxData[5] + (unsigned short)(ucBtmRxData[6]<<8);
        FTMS_Control_Value.target_Hard_Time     = (unsigned short)ucBtmRxData[7] + (unsigned short)(ucBtmRxData[8]<<8);
        FTMS_Control_Value.target_Maximun_Time   = 0;
        
    }
        
    if(FTMS_OP_Code == set_target_time_in_five_heart_rate_zone){
        
        FTMS_Control_Value.target_VeryLight_Time = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);
        FTMS_Control_Value.target_Light_Time     = (unsigned short)ucBtmRxData[5] + (unsigned short)(ucBtmRxData[6]<<8);
        FTMS_Control_Value.target_Moderate_Time  = (unsigned short)ucBtmRxData[7] + (unsigned short)(ucBtmRxData[8]<<8);
        FTMS_Control_Value.target_Hard_Time      = (unsigned short)ucBtmRxData[9] + (unsigned short)(ucBtmRxData[10]<<8);
        FTMS_Control_Value.target_Maximun_Time   = (unsigned short)ucBtmRxData[11] + (unsigned short)(ucBtmRxData[12]<<8);
        
    }
    
    if(FTMS_OP_Code == set_wheel_circumference){
        FTMS_Control_Value.wheel_Circumference = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }
    
    if(FTMS_OP_Code == set_target_cadence){
        FTMS_Control_Value.target_Cadence = (unsigned short)ucBtmRxData[3] + (unsigned short)(ucBtmRxData[4]<<8);  
    }
    
    
}



//----------------------------------------0xB2 End --------------------------------------------------------


//----------------------------------------0xB3 Start---------------------------------------------------------

void Set_Spindown_SPEED(unsigned short Speed_Low, unsigned short Speed_High ){

    memset(ucBtmTxBuf,0x00,20);
      
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB3;
    ucBtmTxBuf[2] = 0x13;   //Spindown OP code 
    
    ucBtmTxBuf[3] = 0x01;   // Parameter:  0x01 spindown speed   0x02 Succes   0x03:error  0x04:stop pedaling
    
    ucBtmTxBuf[4] = (unsigned char)Speed_Low;
    ucBtmTxBuf[5] = (unsigned char)(Speed_Low >> 8);
    
    ucBtmTxBuf[6] = (unsigned char)Speed_High;
    ucBtmTxBuf[7] = (unsigned char)(Speed_High >> 8);
    
    ucBtmTxBuf[19] = ']';

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

}

void Set_Spindown_Success(unsigned short SpindownTime){

    memset(ucBtmTxBuf,0x00,20);
      
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB3;
    ucBtmTxBuf[2] = 0x13;   //Spindown OP code 
    
    ucBtmTxBuf[3] = 0x02;   // Parameter:  0x01 spindown speed   0x02 Succes   0x03:error  0x04:stop pedaling
    
    ucBtmTxBuf[4] = (unsigned char)SpindownTime;
    ucBtmTxBuf[5] = (unsigned char)(SpindownTime >> 8);
    
    
    ucBtmTxBuf[19] = ']';

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

}

void Set_Spindown_Error(){

    memset(ucBtmTxBuf,0x00,20);
      
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB3;
    ucBtmTxBuf[2] = 0x13;   //Spindown OP code 
    
    ucBtmTxBuf[3] = 0x03;   // Parameter:  0x01 spindown speed   0x02 Succes   0x03:error  0x04:stop pedaling

    ucBtmTxBuf[19] = ']';

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
}

void Set_Spindown_StopPedaling(){

    memset(ucBtmTxBuf,0x00,20);
      
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB3;
    ucBtmTxBuf[2] = 0x13;   //Spindown OP code 
    
    ucBtmTxBuf[3] = 0x04;   // Parameter:  0x01 spindown speed   0x02 Succes   0x03:error  0x04:stop pedaling

    ucBtmTxBuf[19] = ']';

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
}


//--------------------------------------- 0xB3 End --------------------------------------------------------


//----------------------------------------0xB4 Start --------------------------------------------------------



Training_status_Def  Training_status = IDLE;
unsigned short minimum_speed;
unsigned short maximum_speed;	
unsigned short mimimum_increment_speed = 20;	

unsigned short maximum_inclination;		
unsigned short minimum_inclination;
unsigned short minimum_increment_inclination = 5;	

unsigned char  minimum_heart_rate  = 40;	
unsigned char  maximum_heart_rate  = 240;	
unsigned char  minimum_heart_rate_increment =1;

unsigned short minimum_resistance_level = 0;		
unsigned short maximum_resistance_level = 20;	
unsigned short minimum_resistance_level_increment = 5;	
unsigned short minimum_power = 0;	
unsigned short maximum_power = 500;		
unsigned short minimum_power_increment = 5;	

unsigned char  console_status = 1;	//1 : stop  2：pause  3: stop by safeKey   4: start 
unsigned char  cycle_length_for_FEC = 0;


//--------0xB4 設定運動資料Range--------------------------
void F_Btm_FEC_B4_SET_Data(unsigned char page){


    minimum_speed = Machine_Data.System_SPEED_Min;
    maximum_speed = Machine_Data.System_SPEED_Max;		
    maximum_inclination = Machine_Data.System_INCLINE_Max;
    minimum_inclination = Machine_Data.System_INCLINE_Min;
    
    
    FEC_State_Def FEC_State = READY; // IN_USE  READY

    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB4;
    ucBtmTxBuf[2] = page;
    
    if(page == 0){
        
        ucBtmTxBuf[3] = (unsigned char)Training_status;
        
        ucBtmTxBuf[4] = (unsigned char)minimum_speed;
        ucBtmTxBuf[5] = (unsigned char)(minimum_speed >> 8);
        
        ucBtmTxBuf[6] = (unsigned char)maximum_speed;
        ucBtmTxBuf[7] = (unsigned char)(maximum_speed >> 8);
        
        ucBtmTxBuf[8]  = (unsigned char)mimimum_increment_speed;
        ucBtmTxBuf[9]  = (unsigned char)(mimimum_increment_speed >> 8);
        
        ucBtmTxBuf[10] = (unsigned char)minimum_inclination ;
        ucBtmTxBuf[11] = (unsigned char)(minimum_inclination >> 8);
        
        ucBtmTxBuf[12] = (unsigned char)maximum_inclination;
        ucBtmTxBuf[13] = (unsigned char)(maximum_inclination >> 8);
        
        ucBtmTxBuf[14] = (unsigned char)minimum_increment_inclination;
        ucBtmTxBuf[15] = (unsigned char)(minimum_increment_inclination >> 8);
        
        ucBtmTxBuf[16] = minimum_heart_rate;
        ucBtmTxBuf[17] = maximum_heart_rate;
        ucBtmTxBuf[18] = minimum_heart_rate_increment;
        
    }else if(page == 1){
   
        ucBtmTxBuf[3] = (unsigned char)minimum_resistance_level;
        ucBtmTxBuf[4] = (unsigned char)(minimum_resistance_level >> 8);
            
        ucBtmTxBuf[5] = (unsigned char)maximum_resistance_level;
        ucBtmTxBuf[6] = (unsigned char)(maximum_resistance_level >> 8);
            
        ucBtmTxBuf[7] = (unsigned char)minimum_resistance_level_increment;
        ucBtmTxBuf[8] = (unsigned char)(minimum_resistance_level_increment >> 8);
            
        ucBtmTxBuf[9]  = (unsigned char)minimum_power;
        ucBtmTxBuf[10] = (unsigned char)(minimum_power >> 8);
            
        ucBtmTxBuf[11] = (unsigned char)maximum_power;
        ucBtmTxBuf[12] = (unsigned char)(maximum_power >> 8);
            
        ucBtmTxBuf[13] = (unsigned char)minimum_power_increment;
        ucBtmTxBuf[14] = (unsigned char)(minimum_power_increment >> 8);
            
        ucBtmTxBuf[15] = console_status;
        
        ucBtmTxBuf[16] = cycle_length_for_FEC;
        ucBtmTxBuf[17] = (unsigned char)FEC_State;
        ucBtmTxBuf[18] = 0;
              
    }
    
    ucBtmTxBuf[19] = ']';

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}

//-----
void F_SetFEC_State(FEC_State_Def FEC_State){

    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB4;
    ucBtmTxBuf[2] = 1;
    
    ucBtmTxBuf[3] = (unsigned char)minimum_resistance_level;
    ucBtmTxBuf[4] = (unsigned char)(minimum_resistance_level >> 8);
    
    ucBtmTxBuf[5] = (unsigned char)maximum_resistance_level;
    ucBtmTxBuf[6] = (unsigned char)(maximum_resistance_level >> 8);
    
    ucBtmTxBuf[7] = (unsigned char)minimum_resistance_level_increment;
    ucBtmTxBuf[8] = (unsigned char)(minimum_resistance_level_increment >> 8);
    
    ucBtmTxBuf[9]  = (unsigned char)minimum_power;
    ucBtmTxBuf[10] = (unsigned char)(minimum_power >> 8);
    
    ucBtmTxBuf[11] = (unsigned char)maximum_power;
    ucBtmTxBuf[12] = (unsigned char)(maximum_power >> 8);
    
    ucBtmTxBuf[13] = (unsigned char)minimum_power_increment;
    ucBtmTxBuf[14] = (unsigned char)(minimum_power_increment >> 8);
    
    ucBtmTxBuf[15] = console_status;
    
    ucBtmTxBuf[16] = cycle_length_for_FEC;
    ucBtmTxBuf[17] = (unsigned char)FEC_State;
    ucBtmTxBuf[18] = 0;  
    
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
}

//----------------------------------------0xB4 END --------------------------------------------------------


//----------------------------------------0xB5 Start --------------------------------------------------------
void F_Btm_FEC_B5_SET_ANT_ID(void){
    
    unsigned char check_sum;
    
    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0xB5;
    
    ucBtmTxBuf[2] =  ucProductionSerialNumber[0];
    ucBtmTxBuf[3] =  ucProductionSerialNumber[1];
    ucBtmTxBuf[4] =  ucProductionSerialNumber[2];     
    ucBtmTxBuf[5] =  ucProductionSerialNumber[3];
    ucBtmTxBuf[6] =  ucProductionSerialNumber[4];
    ucBtmTxBuf[7] =  ucProductionSerialNumber[5];
    ucBtmTxBuf[8] =  ucProductionSerialNumber[6];
    ucBtmTxBuf[9] =  ucProductionSerialNumber[7];
    ucBtmTxBuf[10] = ucProductionSerialNumber[8];
    ucBtmTxBuf[11] = ucProductionSerialNumber[9];     
    ucBtmTxBuf[12] = ucProductionSerialNumber[10];    
    ucBtmTxBuf[13] = ucProductionSerialNumber[11];    
    ucBtmTxBuf[14] = ucProductionSerialNumber[12];    
    ucBtmTxBuf[15] = ucProductionSerialNumber[13];    
    ucBtmTxBuf[16] = 0;
    ucBtmTxBuf[17] = 0;
    
    check_sum = ucBtmTxBuf[1];
    for(unsigned char i = 2;  i<18; i++){
        check_sum = check_sum ^ ucBtmTxBuf[i];
    }

    ucBtmTxBuf[18] = check_sum;
    
    ucBtmTxBuf[19] = ']';
    
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
 
}


//----------------------------------------0xB5 End --------------------------------------------------------



unsigned char btmReceiveArrayRecord[50][20];
unsigned char btmRAR_Cnt = 0;
uint32_t Uart2Srflags;

void Btm_Recive(){
    
    if(HAL_UART_Receive_IT(&huart2,&ucBtmRxTemp,1) == HAL_OK){
        ucBtmRxBuf[ucRxAddrs] = ucBtmRxTemp;
        
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);    //  TXE = 0   (如果TXE = 1 都沒清掉 會卡在中斷裡面
        
        if( (huart2.Instance->SR)&USART_SR_ORE == USART_SR_ORE){
            __HAL_UART_CLEAR_OREFLAG(&huart2);
        }
        
        //Uart2Srflags = huart2.Instance->SR | USART_SR_ORE;
        //huart2.Instance->SR = Uart2Srflags;
       
        
        if(ucBtmRxBuf[0] == 0x5B ){
            
            btm_Rx_is_busy = 1;
            ucRxAddrs++;
        }
    }
    
    if(ucRxAddrs >= BtmData){
        
        if(ucRxAddrs > BtmData){
            __asm("NOP");
        }
       
        if(ucBtmRxBuf[19] == 0x5D){
        
            //btmRAR_Cnt++;
            //btmRAR_Cnt = btmRAR_Cnt%50;
            
            //memcpy(btmReceiveArrayRecord[btmRAR_Cnt] ,  ucBtmRxBuf ,20);
            
            memcpy(ucBtmRxData ,  ucBtmRxBuf ,20);
            memset(ucBtmRxBuf,0x00,20);
            ucRxAddrs = 0;
            
        
        }else if(ucBtmRxBuf[19] != 0x5D){
           
            if((ucBtmRxBuf[0] == 0x5B) && (ucBtmRxBuf[1] == 0x5B) ){
            
                for(unsigned char i = 1; i < 20; i++){
                    ucBtmRxData[i-1] = ucBtmRxBuf[i];
                }
                ucBtmRxData[19] =0x5D;
                
            }
            
            memset(ucBtmRxBuf,0x00,20);
            ucRxAddrs = 0;
        }
        

        
        if(ucBtmRxData[1] ==0xF1){  //boot ok 
            btm_is_ready = 1;      
            ucRFVersion[0] = ucBtmRxData[2];       // RF L    BLE version L
            ucRFVersion[1] = ucBtmRxData[3];       // RF M    BLE version M
            ucRFVersion[2] = ucBtmRxData[4];       // RF H    BLE version H
        }
        
        //-----------------------------------------------------心跳裝置------------------------
        
        if(ucBtmRxData[1] ==0xE0){  // 裝置掃描解碼
            Scan_Re_E0();        
        }
        
        if(ucBtmRxData[1] ==0xE1){  // 裝置配對解碼
            BLE_Pairing_Re_E1();        
        }
        
        if(ucBtmRxData[1] == 0xE2){  // 裝置連線解碼
            Link_Sensor_Re_E2();        
        }
        
        if(ucBtmRxData[1] == 0xCB){  // BLE裝置數值回傳解碼
            SensorReceive_CB();        
        }
        
        if(ucBtmRxData[1] == 0xCE){  // BLE連線狀態回傳解碼
            LinkStateReceive_CE();  
            
        }
        
        if(ucBtmRxData[1] == 0xCC){  // ANT裝置數值回傳解碼
            SensorReceive_CC();        
        } 
        
        if(ucBtmRxData[1] == 0xCF){  // ANT 連線狀態回傳解碼
            LinkStateReceive_CF();        
        }
        
        if(ucBtmRxData[1] == 0xE4){  // 解除連線
            disconnect_Sensor_Re_E4();     
        }
        
        //-----------------------------------------------------------雲跑------------
        
         /* 0x02 :  Fw  Update 更新 訊息 -------- */
        if(ucBtmRxData[1] == 0x02){
            F_BtmRead02Cmd();
        }
        
        /* 0x03 :  Fw  Update  傳輸 資料 -------- */
        if(ucBtmRxData[1] == 0x03){
            F_BtmRead03Cmd();
        }
        
         /* 0x04 :  Fw  Update  傳輸 完成-------- */
        if(ucBtmRxData[1] == 0x04){
            F_BtmRead04Cmd();
        }
        
        /* 0x35 : status   ----app 啟動---  */            
        if((ucBtmRxData[1] == 0x35) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead35Cmd();
        }
        
        /*  0x37 : prs    -----speed 取得  ----*/
        if((ucBtmRxData[1] == 0x37) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead37Cmd();
        }
        
        /* 0x38 : pri    -----incline 取得 */
        if((ucBtmRxData[1] == 0x38) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead38Cmd();
        }
        
        /* 0x36 : Set    --------321 go  --- SystemMode  Progarm TIME   AGE   WEIGHT_L  WEIGHT_H     操場大小 */
        if((ucBtmRxData[1] == 0x36) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead36Cmd();
        }
      
        if(FW_Transmiting_03_Flag != 1){   // 0x03要專心收資料 來亂的都過濾掉
            
            /* 0x47 :   Ver  -------- */
            if((ucBtmRxData[1] == 0x47)&&(ucBtmRxData[2] <= 0x40)){
                F_BtmRead47Cmd();
            }
            
            /* 0x44 :   寫序號 名稱  -------- */
            if((ucBtmRxData[1] == 0x44)&&(ucBtmRxData[2] <= 0x40)){
                F_BtmRead44Cmd();
            }
            
            /* 0x46 :  回復 ODO -------- */
            if((ucBtmRxData[1] == 0x46)&&(ucBtmRxData[2] <= 0x40)){
                F_BtmRead46Cmd();
            }
            
        }
         /* 0x50 :  OTA  Update 更新 訊息 -------- */
        if(ucBtmRxData[1] == 0x50){
            F_BtmRead50Cmd();
        }
        //---------------------------------------------------------  FTMS  -----------
        if((ucBtmRxData[1] == 0xB0)&&(ucBtmRxData[2] <= 0x40)){
            F_Btm_FTMS_B0_Read();
        }
        if( ucBtmRxData[1] == 0xB2 ){
            F_Btm_FTMS_B2_Read();
        }
        btm_Rx_is_busy = 0;
    }
    
}

/*
BLE_RST  --> PA1
BLE_TX   --> PA2
BLE_RX   --> PA3
*/

void BLE_Init(){
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    
    /* -----         PA4 --> GFIT_CTRL    開啟RF_PWR      ------------- */
    GPIO_InitStruct.Pin =  GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_4 , GPIO_PIN_RESET);
    
    /* -----         PA1 --> BLE_RST      ------------- */
    GPIO_InitStruct.Pin =  GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_1 , GPIO_PIN_SET);
    
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;  
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_UART_MspDeInit(&huart2); 
    HAL_UART_MspInit(&huart2);    
    BtmRst();
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
   
    while(btm_is_ready!=1){}
    HAL_Delay(10);
    //ScanSensorE0(ANT_HR);
    HAL_Delay(10);
    __asm("NOP");
    
}



void BLE_DeInit(){
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    
    btm_is_ready =0;
    
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE); 
    
    
    HAL_UART_MspInit(&huart2); 
    
    if(HAL_UART_DeInit(&huart2) != HAL_OK ){
        Error_Handler();    
    }

    /* -----         PA1 -->   BLE_RST ------------- */
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_1 , GPIO_PIN_RESET);
    GPIO_InitStruct.Pin =  GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    
    /* -----         PA4 --> GFIT_CTRL    關掉RF_PWR      ------------- */
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_4 , GPIO_PIN_SET);
    GPIO_InitStruct.Pin =  GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   
    
}

void ble_disconnect_wait_Process(){

    if(Ble_wait_disconnect_Time_out_Cnt > 0){
        if(Linked_HR_info.Link_state == wait_disconnect ){
            wait_HR_disconnect_Flag = 1;
            Ble_wait_disconnect_Time_out_Cnt--;
        }
    }
    
    if(Linked_HR_info.Link_state == wait_disconnect ){
        
        if(Ble_wait_disconnect_Time_out_Cnt == 0){  //0xFF  丟到 沒有訊號了  執行原本要做的事情
            
            wait_HR_disconnect_Flag = 0;
            
            
            //Btm_Task_Adder(Scan_BLE_HRC_Sensor);
            if(btm_HRC_disconnect_Task_Temp == No_Task){    //心跳帶關掉 斷線
                Linked_HR_info.Link_state = disconnect;
            }else{
                Btm_Task_Adder(btm_HRC_disconnect_Task_Temp);  //按鈕強制斷線 並重新搜尋
                btm_HRC_disconnect_Task_Temp = No_Task;
                Linked_HR_info.Link_state = ble_researching;
            }
                   
        }
        
    }
    
}



void ble_Wait_First_CB_Value(){
    
    if(Ble_wait_HR_value_First_IN_Flag == 1){
        FirstCB_Time_Cnt++;
    }else{
        FirstCB_Time_Cnt = 0;
    }
    
    if(FirstCB_Time_Cnt >=300){  //經過30秒都沒有CB資料傳進來
        FirstCB_Time_Cnt = 0;
        Ble_wait_HR_value_First_IN_Flag = 0;
        wait_HR_disconnect_Flag = 0;
        
        Btm_Task_Adder(BLE_HRC_Disconnect);
        __asm("NOP");
    }

}

unsigned short ReSearch_Time_Out_Cnt;
void ble_ReSearching_TimeOut(){

    if(Linked_HR_info.Link_state == ble_researching){
        ReSearch_Time_Out_Cnt++;
    }else{
        ReSearch_Time_Out_Cnt = 0;
    }
    
    if(ReSearch_Time_Out_Cnt >= 50){
        ReSearch_Time_Out_Cnt = 0;
        Linked_HR_info.Link_state = disconnect;
    }
    
}
