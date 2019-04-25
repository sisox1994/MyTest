#include "stm32f4xx_hal.h"
#include "string.h"
#include "system.h"
#include "btm.h"

#define FTMS_Test 0
#define configUSE_SPDRamSize 32
#define BtmData 20

unsigned char FW_Transmiting_03_Flag;
unsigned char ucRFVersion[3];          //FTMS SetFeature B0 cmd ���oRF���� 
unsigned short EepromVer_3;       // V1.2  
unsigned short EepromVer_4;        // A01

UART_HandleTypeDef huart2;

unsigned char btm_is_ready;      //����BTM F1
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
Pairing_Meseseage_def Pairing_Msg;             //��U�t����o���a�}��T
BLE_Paired_Device_Addr_List_def  BLE_Paired_device_list; // ���\�t�� �N���T�[�J�M���

Now_Linked_HR_Sensor_Info_Def Linked_HR_info;

Btm_Task_Def btmTask_List[Task_Amount];

unsigned char btm_Task_Cnt = 0;


unsigned char wait_HR_disconnect_Flag;
unsigned short disconnect_buffer_0xFF_Cnt;

Btm_Task_Def   btm_HRC_disconnect_Task_Temp;

unsigned char Ble_wait_HR_value_First_IN_Flag;

void BTM_UART_Transmit(){
    
    if(btm_Rx_is_busy == 0){
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);           //�T��Uart ���_�����
        HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);      // Tx ��Ƶo�e
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);            //�Ұ�Uart ���_�����
    }
    
    btm_Rx_is_busy = 0;

}

//---------------------0x02  Firmware Update ��s ���\�T�{-------------------------
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

    
    if((ucBtmRxData[2] != 0x40 ) && (ucBtmRxData[6] > 0)){ //�ܤ֤@��page   0x40���n�� 
        
        //���o �ɮפj�p    Page�ƶq
        FW.total_Size = (unsigned int)(ucBtmRxData[5]<<24)
                      + (unsigned int)(ucBtmRxData[4]<<16)
                      + (unsigned int)(ucBtmRxData[3]<<8)  
                      + (unsigned int)(ucBtmRxData[2]);
        
        FW.total_Page = (unsigned short)(ucBtmRxData[7]<<8)
                      + (unsigned short)(ucBtmRxData[6]);

        FW.Byte_Cnt = 0;
        FW.Error_Cnt  = 0;
        
#if Config_WDG	
        HAL_IWDG_Refresh(&hiwdg);    // ���o�`�e�q �����@�U�� �K�o���Ӥ[�ε�
#endif
        
        if(FW_Transmiting_03_Flag != 1){  //0x03 �w�g�b���ƤF �N���ΦA�^�F'
            if(System_Mode != Sys_OTA_Mode){
                System_Mode = Sys_OTA_Mode; //�p�G0x50�S���� �٬O�i�H��s
                
                Turn_OFF_All_Segment();
                Turn_OFF_All_LEDMatrix();
            }
            
            Update_FW_Flash_Erase();
            F_BtmReply02Cmd();
            //Btm_Task_Adder(C_02Val);   // �^�_APP�i�H�}�l���ƤF
        }
        
        OTA_Mode_Flag = 1;

    }
    __asm("NOP");
    
    
}
//---------------------0x02 End-----------------------------------------



//---------------------0x03  Firmware Data �ǿ�  -------------------------
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
    
    
    if(FW.Transmit_Page > (FW.total_Page + 2) ){ //  iOS �|��2��page�����t
        while(1){}
          
    }
    
    if(FW.Transmit_Page != nextPageShould){
        
        if(FW.Transmit_Page == (nextPageShould - 1)){ //�p�G�e�@���S��@���X��
            nextPageShould = nextPageShould - 1;
            FW.Byte_Cnt = FW.Byte_Cnt - 15;
            FW.Error_Cnt ++;
        }else{
            Flash_Write_Applicantion_Mode();
            Turn_OFF_All_LEDMatrix();
            HAL_UART_DeInit(&huart2);
            FW.Error_Type = Data_Lost;               //Data Lost ���]�F
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
    //----�p�G�̫�size���O16������  ---�ɳ̫�@�]���-------------
    if((FW.Byte_Cnt>=FW.total_Size) && (FW.Byte_Cnt%16!=0)){
       Write_EE_Flash_FW_Data_16Byte( (FW.Byte_Cnt/16) , FW_Buff_16Byte);
    }
    
        
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    
    //F_BtmReply03Cmd(FW.Transmit_Page );
    
   
}
//---------------------0x03 End-----------------------------------------

//---------------------0x04  Firmware Data �ǰe���� -------------------------
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

   Fw_Write_Finish_Check();   //���Ҧ��S���g�iflash

   if(  (FW.Transmit_Page >= (FW.total_Page - 1))  && (FW.Byte_Cnt  >= FW.total_Size) && (FW.Error_Cnt == 0)){
       
       Flash_Write_BootLoader_Mode();      //Flash�g�J Bootloader_Mode �X��
       
       HAL_NVIC_SystemReset();
       
   }else{

       HAL_UART_DeInit(&huart2);
       Flash_Write_Applicantion_Mode();
       FW.Error_Type = Size_Error;                   //��ƶǰepage�ƶq  size�e�q �����D
   }

   // Btm_Task_Adder(C_04Val); //�i��APP  �]�B����s����
   
}
//---------------------0x04 End-----------------------------------------


//�]�w�ŪަW��
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


//----------------E0  --���ySensor--  Tx------------------------------

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
    //--------------------�Ūޱ��y�M���l��-----------------
    Clear_BLE_Scan_Device_List();
    //-------------------------------------------------------
    Clear_Scan_Msg(); 
    //-------------------------------------------------------------------------
    
    BTM_UART_Transmit();
   // __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    //HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);  

}

//----------------------------E0  --���ySensor--  Rx-----------------------------------



unsigned char NearestDevieNumber;
unsigned char RSSI_Compare;

void Scan_Re_E0(){


    //---------------------  Scan_Meseseage  ---------------------------
    //----Common

    Scan_Msg.TransDir   = Rxx;
    
    ////Tx �e�X�N�M�w�F
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
        
        //---���y��H�����OANT+    BLE��ƪ�l��      
        Scan_Msg.DeviceNumber = 0;
        Scan_Msg.RSSI = 0;
        Scan_Msg.UUID = 0;
        Scan_Msg.Type_Check = (Sensor_UUID_Type_Def)0;
        memset(Scan_Msg.DeviceName,0x00,13);
        
    }else if(Scan_Msg.ScanType == BLE_HR ){
    
        if(ucBtmRxData[2] == 0x4F || ucBtmRxData[2] == 0x40 || ucBtmRxData[2] == 0x4C){
            
            Scan_Msg.Scan_State = (Scan_State_Def)ucBtmRxData[2];
            
            if(Scan_Msg.Scan_State == Scan_Time_Out){ //���y 15 �� �s�T���̱j������
                //Ask_Link_State_CE();
                Btm_Task_Adder(BLE_HRC_Pairing);  //�sRSSI�̱j�����@��
            }
          
        }else{
              
           //--- BLE       
            Scan_Msg.DeviceNumber = ucBtmRxData[2];
            Scan_Msg.RSSI = ucBtmRxData[3];
            Scan_Msg.UUID = ((unsigned short)(ucBtmRxData[5]<< 8))  + ucBtmRxData[4] ;
            Scan_Msg.Type_Check = (Sensor_UUID_Type_Def)Scan_Msg.UUID;
            
            for(unsigned char i = 6;i < 6 + 13; i++){  //BLE Device Name���׬�13
                Scan_Msg.DeviceName[i-6] = ucBtmRxData[i];
            }    
            
            
            
            char RejectDeviceName[] = {"Chandler_HRM"};
            if(!charArrayEquals(Scan_Msg.DeviceName , RejectDeviceName )){
            
                //�M��T���̱j���˸m  Brian �� RSSI�V�j �T���U�j
                if(BLE_Scan_Device_List.Device_Cnt == 0){
                    NearestDevieNumber = Scan_Msg.DeviceNumber;
                    RSSI_Compare = Scan_Msg.RSSI;
                }else if(BLE_Scan_Device_List.Device_Cnt>0){
                    if(Scan_Msg.RSSI > RSSI_Compare){
                        RSSI_Compare = Scan_Msg.RSSI;
                        NearestDevieNumber = Scan_Msg.DeviceNumber;
                    }  
                }
                
                //-------------------- �ⱽ�쪺�˸m���s�i�M��� -----------------------
                BLE_Scan_Device_List.messeage_List[ BLE_Scan_Device_List.Device_Cnt] = Scan_Msg;
                BLE_Scan_Device_List.Device_Cnt++;
                //------------------------------------------------------- 
                
                
                
                //---���y��H�����OBLE+    ANT��ƪ�l��   
                Scan_Msg.ANT_ID = 0;
                
            
            }

        }         
        
    }
    
    if(Linked_HR_info.Link_state == Linked ){ //�Ūޤ߸� =>0xFF��  ble�|�^E0 4F
        Ask_Link_State_CE();
    }
 

    //-------------------------------------------------------------------------
    
}



//--------------------   E1  Tx �t��   �g�LE0���y Sensor �ǥѱ��y�쪺�˸m�y���� �h��ܰt��   ----------------------
void Pairing_BLE_Sensor_E1(unsigned char DeviceNumber){
   
    
    
    if((BLE_Scan_Device_List.Device_Cnt>=1)){ //���y���h�s�u??  �ܤ֭n����@��device �~��h�s�u
        
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


//---------------------  E1  Rx  �t�� ���\  ���o�a�}�æs�i�M��  ------------------------
void BLE_Pairing_Re_E1(){
    
    if(ucBtmRxData[2] == 0x40){  //if -- [OK]
        Pairing_Msg.Pairing_Sensor_Type = (Sensor_UUID_Type_Def)(((unsigned short)(ucBtmRxData[4]<< 8))  + ucBtmRxData[3]);
        for(unsigned char i = 0; i < 6; i++ ){
            Pairing_Msg.BLE_Addrs[i] = ucBtmRxData[i + 5];
        }
        Pairing_Msg.BLE_ADDR_TYPE =  ucBtmRxData[11];
        
        
        //-------�C�������M�� �t��M��  �T�w�h�s �̥����쪺���@��Sensor
        BLE_Paired_device_list.Paired_Device_Cnt = 0;
        
        
        //�S���t��L
        if(BLE_Paired_device_list.Paired_Device_Cnt == 0){
            BLE_Paired_device_list.BLE_Paired_Device_Addr_List[BLE_Paired_device_list.Paired_Device_Cnt] = Pairing_Msg;
            BLE_Paired_device_list.Paired_Device_Cnt++;
        }else{
            
            for(unsigned char i = 0; i < (BLE_Paired_device_list.Paired_Device_Cnt); i++){
                //�p�G�M��̭��S���~�s�W
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
        
        // Linked  info ��J�ثe�s�u���˸m�W��   �� �Ūަ�}       
        memcpy( Linked_HR_info.DeviceName,BLE_Scan_Device_List.messeage_List[NearestDevieNumber].DeviceName,13);
        memcpy( Linked_HR_info.BLE_Addrs,  Pairing_Msg.BLE_Addrs , 6); 
       
        
        //--------------------�Ūޱ��y�M���l��-----------------------
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
    
    
    Clear_BLE_Scan_Device_List();  // �M�����y��T


    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
}


unsigned char DisConDeviceName[13];

//unsigned char Rest_of_HR_Value_Mask_Cnt; // �UE4 ���� CB�ٷ|�U3���ͤ߸��ƭȪ� Ack�^�� 

void disconnect_Sensor_Re_E4(){

    if( (ucBtmRxData[2] == 0x40) || (ucBtmRxData[2]==0x4B) ){
        
        if(Linked_HR_info.SensorType == BLE_HR){
            
            memset( Pairing_Msg.BLE_Addrs,0x00,6);
            Linked_HR_info.Link_state = wait_disconnect;
            disconnect_buffer_0xFF_Cnt = 0;
            memcpy(DisConDeviceName ,Linked_HR_info.DeviceName , 13 ); //��ثe�_�u���ź٥��O�_��
            //Ask_Link_State_CE();
        
        }else if(Linked_HR_info.SensorType == ANT_HR){
            
            Linked_HR_info.Link_state = wait_disconnect;
            
            disconnect_buffer_0xFF_Cnt = 0;
            //Ask_Link_State_CF();
        }
        
        
        //Rest_of_HR_Value_Mask_Cnt = 5;
        
        //�M���ثe�s�u���߸��a��T
        Linked_HR_info.SensorType = (Sensor_UUID_Type_Def)0;  
        Linked_HR_info.ANT_ID = 0;
        Linked_HR_info.usHR_bpm = 0;
        memset(Linked_HR_info.DeviceName,0x00,13);
        memset(Linked_HR_info.BLE_Addrs,0x00,6);
        //Linked_HR_info.Link_state = disconnect;  
    }
}

void SensorReceive_CB(){
    
    if(ucBtmRxData[3]!=0xFF){                   //���߸�
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
        
        
        
           
    }else if(ucBtmRxData[3] == 0xFF){           //�߸�0xFF    �Ȯ����u���A
      
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
    
    //---------�j���_�u -------------
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
        
    }else if(ucBtmRxData[3] == 0xFF){                //�߸��S���ƭ� �Ȯ����u���A
        
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
    
    //�T�w���u�F �~��i��s�����y
    if(Linked_HR_info.Link_state == disconnect){
        
        wait_HR_disconnect_Flag = 0;
        //��쥻�٨S���u�ɦs�U�Ӫ����Ȯ��X�Ӱ���
        
        Btm_Task_Adder(btm_HRC_disconnect_Task_Temp);
        btm_HRC_disconnect_Task_Temp = No_Task;
    }
    
    
    
    //---------------------��� �s�u----------------------------------------
    if((ucBtmRxData[3] == 0x44) || (ucBtmRxData[3] == 0x46)){  //����t�令�\ ��@��39
        
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
    
    //�T�w���u�F �~��i��s�����y
    if(Linked_HR_info.Link_state == disconnect){
        wait_HR_disconnect_Flag = 0;
        //��쥻�٨S���u�ɦs�U�Ӫ����Ȯ��X�Ӱ���
        
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
    
    ucCmdPage = 0; //�u��page 0�����O
    
    ucBtmTxBuf[0] = '[';					
    ucBtmTxBuf[1] = 0x35;
    ucBtmTxBuf[2] = ucCmdPage;
    //Type SystemMode Max SPD	Min SPD	Max Inc	MinInc	Inc NO	stageNumbers	stageGrowMode	 UNIT
    ucBtmTxBuf[3] = 0x02;        // �]�B��������	
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
        ucBtmTxBuf[5] = Machine_Data.System_SPEED_Max;    //���� 
        ucBtmTxBuf[6] = Machine_Data.System_SPEED_Min;
    }else{
        ucBtmTxBuf[5] = Machine_Data.System_SPEED_Max;    //�^�� 
        ucBtmTxBuf[6] = Machine_Data.System_SPEED_Min;
    }
    ucBtmTxBuf[7] = (Machine_Data.System_INCLINE_Max / 5);        // 0.0 % ~ 15.0% �@30��
    ucBtmTxBuf[8] = 0;
    ucBtmTxBuf[9] = 5;          //�C������  0.5 or 1.0
    ucBtmTxBuf[10] = configUSE_SPDRamSize;
    ucBtmTxBuf[11] = 0;
    ucBtmTxBuf[12] = (unsigned char)System_Unit;
    //ucBtmTxBuf[13] =  0x00;  appProgrm(�ثe�S��)
    ucBtmTxBuf[19] = ']';
  
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
  HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
  
}
    

void F_BtmRead35Cmd(void){
    
    //unsigned char uci;
    ucCmdPage = ucBtmRxData[2];
    
    
    //���oAPP�Ҧ�     �q�l��0/App1/���](�v��)2 /��]3 �V�m�p�e4
    ucAppTypeMode               = (ucAppTypeModeDef)ucBtmRxData[3];   
    CloudRun_Init_INFO.TypeMode = (ucAppTypeModeDef)ucBtmRxData[3];  // TypeMode:�q�l��0/App1/���]2/��]3/�V�m�Z��)4/�V�m�ɶ�)5
    
    
    //���oStage Number
    usAppStageNumber         = ucBtmRxData[4]+(256*ucBtmRxData[5]);       // Stage:20 ~ 400
    CloudRun_Init_INFO.Stage = (unsigned short)(ucBtmRxData[5]<<8) + (unsigned short)ucBtmRxData[4];
    
    //--�M�� �t��  ���� TABLE
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
        //--- ���] ---
        uiAppTotalDist = (ucBtmRxData[11] + (256*ucBtmRxData[12]))*10;   // (10m)
        uiAppStageDist =  ucBtmRxData[13] + (256*ucBtmRxData[14]);        // (1m)
        
        CloudRun_Init_INFO.Total_Dist = ((unsigned short)ucBtmRxData[11] + (unsigned short)(ucBtmRxData[12]<<8)) *10; 
        CloudRun_Init_INFO.Stage_Dist =  (unsigned short)ucBtmRxData[13] + (unsigned short)(ucBtmRxData[14]<<8);  

        //------------
#if AppErrChk
        temp1 = (uiAppTotalDist*10)/uiAppStageDist;
        if(temp1!=usAppStageNumber && temp1!=(usAppStageNumber-1)){
            ucAppTypeMode = 0;     // TotalDistance�PStage���`�����]�w
            ucAppErrflag = 1;
        }else{
            ucAppErrflag = 0;
        }
#endif
        if(ucAppTypeMode>=1 && ucAppTypeMode<=5){  //1.App  2.�v��  3.��]  4.�V�m(�Z��) 5.�V�m(�ɶ�)
            
            if(ucAppTypeMode == 1){
                ucProgramIndex = C_App_UserVal;
            }
            if(ucAppTypeMode == 2){
                
                ucProgramIndex = C_App_Dist1Val;     // �v��
                Program_Select = APP_Cloud_Run;
                
            }
            if(ucAppTypeMode == 3){                // ��]
                ucProgramIndex = C_App_Dist2Val;
                
            }
            if(ucAppTypeMode == 4){                //�V�m�p�e(�Z��)
                ucProgramIndex = C_App_Training_Val;
                Program_Select = APP_Train_Dist_Run;
            }
            if(ucAppTypeMode == 5){                //�V�m�p�e(�ɶ�)
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
    
    //00 =��NONE�C
    //01 =�ֳB��Workout�]�w�Ҧ��C
    //02=�ֳB��321 �˼ơC
    //03=�ֳB��Workout Warm Up�Ҧ��C
    //04 =�ֳB��Workout Running�Ҧ��C
    //05 =�ֳB��Workout Cool Down�Ҧ��C
    //06=�ֳB��Pause�Ҧ��C
    //07=�ֳB��summary�Ҧ��C
    //
    
    ucBtmTxBuf[3] = System_Mode;
    ucBtmTxBuf[4] = ucProgramIndex; 
    
    
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);

    
    IntoReadyMode_Process(); //r�i�J�B��
    
    Btm_Task_Adder(C_39Val);
    
    Cloud_Run_Initial_Busy_Flag = 0;
}

// -----------------------------  0x36 End  ----------------------------------------------------------






//-------------------------------------------0x37  �s�t�� table  Start-----------------------------------------------------------

unsigned char ucNextPage;
void F_BtmRead37Cmd(void){
    
    unsigned char  uci;
    unsigned short temp1;
    
    ucAppErrflag = 0;
    
    ucCmdPage = ucBtmRxData[2];
    
    if(ucCmdPage == 0){
        ucNextPage = 1;
    }else if(ucCmdPage != ucNextPage){
        ucAppErrflag = 1;          //����] �άO���]�����p�o�� �^0x41
        ucCmdPage = ucNextPage;    //�i�DAPP�n����     �^�����ӭn�Ǫ�page
    }else{
        ucNextPage = ucCmdPage + 1;
    }
    
    if(ucAppErrflag != 1){
        switch(ucAppTypeMode){
            
          case Digital_Watch:    //�q�l��
            // �@��Ҧ�������APP��Profile
            break;
          case App:     //ALA Fitness
            //�Ȯɤ��䴩
            break;
          case Cloud_Run:   //���]
            break;
            
          case void_Run:   //��]
          case Train_Run_Dist:  //�V�m(�Z��)
          case Train_Run_Time:  //�V�m(�ɶ�)
            if(ucCmdPage < ucAppDataPage){        // ����<432�����(420)
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

//-------------------------------------------0x38  �s����  table   Start-----------------------------------------------------------

void F_BtmRead38Cmd(void){
    
  unsigned char uci;
  unsigned short temp1;
  
  ucAppErrflag = 0;
  ucCmdPage = ucBtmRxData[2];

  if(ucCmdPage == 0){
      ucNextPage = 1;
  }else if(ucCmdPage != ucNextPage){
      ucAppErrflag = 1;          //����] �άO���]�����p�o�� �^0x41
      ucCmdPage = ucNextPage;    //�i�DAPP�n����     �^�����ӭn�Ǫ�page
  }else{
      ucNextPage = ucCmdPage + 1;
  }
  
  if(ucAppErrflag != 1){
      switch(ucAppTypeMode){
          
        case Digital_Watch:   //�q�l��
          // �@��Ҧ�������APP��Profile
          break;
        case App:   //ALA Fitness
          //�Ȯɤ��䴩
          break;
        case Cloud_Run:   //���]
        case void_Run:    //��]
        case Train_Run_Dist:  //�V�m(�Z��)
        case Train_Run_Time:  //�V�m(�ɶ�)
          if(ucCmdPage < ucAppDataPage){        // ����<432�����
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
    
    //--------------- �ɶ� ------------------------
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
    
    
    
    //-----------------------�߸� ------------------------------------
    if((usNowHeartRate > 0)){
        ucBtmTxBuf[6] = usNowHeartRate;        
    }else{
        ucBtmTxBuf[6] = 0;
    }
    
    Cloud_0x39_Info.c_HeartRate = usNowHeartRate;
    
    //----------------------------------------------------------------
    
    
    
    //------------------- ���q ---------------------------------------
    
    usSegmentNumber = (Cloud_0x39_Info.c_Distance / uiAppStageDist);
    
    /*if(usSegmentNumber> 40){     // �Ѯi���o�˳B�z(�u�B�z40��H�����)  190329 ��������
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
        
        /* 0 :�Z��H �Z�� �Z��L �d����H �d���� �d����L */
        
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



//----------------------------------------0x44 Start -----�Ͳ��Ǹ�-----------------------------
void F_BtmReply44Cmd(void){
    
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    
    //�qFlash   ŪSerial Number�^��  -- ucProductionSerialNumber --
      
    
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
    
    //-----------------�s���Ǹ��w�q--------------------------------
    
    ucBtmTxBuf[11] = ucProductionSerialNumber[8];   // �t��ID
    ucBtmTxBuf[12] = ucProductionSerialNumber[9];   // �t��ID
    
    ucBtmTxBuf[13] = ucProductionSerialNumber[10];  // Serial Number
    ucBtmTxBuf[14] = ucProductionSerialNumber[11];  // Serial Number
    ucBtmTxBuf[15] = ucProductionSerialNumber[12];  // Serial Number
    ucBtmTxBuf[16] = ucProductionSerialNumber[13];  // Serial Number
    
    //SN  Tool�s�u�~��b Serial �ݪ���o��X    �o��X�å� IOS ���]�s�u�|�{�h
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
      
      // 2017/6/21�Ͳ��Ĥ@�x => A25T030000001
      ucProductionSerialNumber[0] = ucBtmRxData[3];      // Year
      ucProductionSerialNumber[1] = ucBtmRxData[4];      // week
      ucProductionSerialNumber[2] = ucBtmRxData[5];      // week
      ucProductionSerialNumber[3] = ucBtmRxData[6];      // Type
      ucProductionSerialNumber[4] = ucBtmRxData[7];      // Type
      ucProductionSerialNumber[5] = ucBtmRxData[8];      // Type
      ucProductionSerialNumber[6] = ucBtmRxData[9];      // Type
      ucProductionSerialNumber[7] = ucBtmRxData[10];     // Type
      
      //---------�sSN2 DEF---------2�X �t��ID   4�X�y���X-----------------
      ucProductionSerialNumber[8] = ucBtmRxData[11];     //   �t�ӿ��ѽX  Serial Number
      ucProductionSerialNumber[9] = ucBtmRxData[12];     //   �t�ӿ��ѽX  Serial Number
      
      ucProductionSerialNumber[10] = ucBtmRxData[13];    // Serial Number
      ucProductionSerialNumber[11] = ucBtmRxData[14];    // Serial Number
      ucProductionSerialNumber[12] = ucBtmRxData[15];    // Serial Number
      ucProductionSerialNumber[13] = ucBtmRxData[16];    // Serial Number
      
    
      //��W�r Btm �ݭnreset   ����F1  ���m����   btm_is_ready --> 1  �Ūޥ��Ȧ^�_���`����
      Btm_SetDeviceName(ucProductionSerialNumber,14);  // BTM Device �ק�0x44���쪺SerialNumber�W�r
      btm_is_ready = 0; 
      
      //-----------------------Flash------------------------------------
      Write_SerialNumber_To_Flash(ucProductionSerialNumber);  //�N�Ǹ��g�JFlash
      Read_SerialNumber_From_Flash(ucProductionSerialNumber);  //Ū�XFlash
        
      
      
      __asm("NOP");

  }else if(ucCmdPage == 1){
      //-----------------  Read  --------------------------------------------
      F_BtmReply44Cmd();
      
  }

}

//----------------------------------------0x44 End --------------------------------------------------------

//----------------------------------------0x46 Start ---------�`���{ ODO  -----------------------------



unsigned int Machine_Total_Time_tmp;
unsigned int Machine_Total_Distance_tmp;

void F_BtmReply46Cmd(void){
    
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);       //������ ��ƥ��@�b RX���_�Ӷ�
    
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
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //-----------��ƥ᧹�A�}�Ҥ��_-----
    
}

void F_BtmRead46Cmd(void){
    
    ucCmdPage = ucBtmRxBuf[2];
    F_BtmReply46Cmd();
}

//----------------------------------------0x46 End --------------------------------------------------------

//----------------------------------------0x47 Start--------�^�� Console���� + RF����  �{�b�OOTA Mode?  -------------------------------

void F_BtmReply47Cmd(void){
    
     __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);       //������ ��ƥ��@�b RX���_�Ӷ�

    ucBtmTxBuf[0] = '[';
    ucBtmTxBuf[1] = 0x47;
    ucBtmTxBuf[2] = ucCmdPage;
    
    if(System_Mode == Sys_OTA_Mode)
        ucBtmTxBuf[3] = 0x01;         // OTA Mode
    else
        ucBtmTxBuf[3] = 0x00;         // Not OTA
    
    //Console����
    ucBtmTxBuf[4] = System_Version[2]-'0';   //EepromVer_3%10;       // Ver Low        Console version L
    ucBtmTxBuf[5] = System_Version[1]-'0';   //EepromVer_3/10;       // Ver High        Console version H

    ucBtmTxBuf[6] = (Modify_Version[1]-'0')*10 + (Modify_Version[2]-'0');  //EepromVer_4 // Axx   Console modify
        
    //RF����
    ucBtmTxBuf[7] = ucRFVersion[0];       // RF L    BLE version L
    ucBtmTxBuf[8] = ucRFVersion[1];       // RF M    BLE version M
    ucBtmTxBuf[9] = ucRFVersion[2];       // RF H     BLE version H
    

    //������� 
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
    

    if(System_Mode == Idle ){ //�u���b Idle Idle Mode�~��i�J��s
        
        //�e���M�� ���OTA
        Turn_OFF_All_Segment();
        Turn_OFF_All_LEDMatrix();
        
        Flash_Write_OTA_Mode();
        HAL_NVIC_SystemReset();
        
    }else{ 
        //��L�Ҧ� warm up   pause  workout ... ���ڵ���s
        Btm_Task_Adder(C_50Val);
    }
 
}

void F_BtmReply50Cmd(void){
    
    
    if(System_Mode != Sys_OTA_Mode){
        memset(ucBtmTxBuf,0x00,20);
        
        ucBtmTxBuf[0] = '[';
        ucBtmTxBuf[1] = 0x50;
        ucBtmTxBuf[2] = 0;
          
        ucBtmTxBuf[3] = 0x41;         //  0x41 �i���L�k��s
        
        ucBtmTxBuf[19] = ']';
        
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
        HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    }
    
}


//--------------------------------------0x50  End-------------------------------------------------------


//----------------------------------------0xB0 Start --------------------------------------------------------




unsigned int  FTMS_Data_Flag;          //��ڤW�u��3��byte
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
           //-----------------------------------�]�w �n�s����  �B�ʸ��-------------------------------------------------------------
        FTMS_Data_Flag =  heart_rate_present | treadmill_instantaneouse_pace_present | treadmill_average_pace_present | 
                          elevation_gain_present | inclination_and_ramp_angle_present | metalbolic_equivalent_present |
                          force_belt_power_output_present | total_distance_present | elapsed_time_present | remaining_time_present;
                   
#else
           //-----------------------------------�]�w �n�s����  �B�ʸ��-------------------------------------------------------------
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
        
        //-----------------------------------�]�w �n�s����  �B�ʸ��-------------------------------------------------------------
        /* FTMS_Data_Flag = average_speed_present   | average_power_present | average_cadence_present
                           total_distance_present 
                           metalbolic_equivalent_present 
                          elapsed_time_present | remaining_time_present;*/
        //-----------------------------------------------------------------------------------------------------------------------
        
         //-----------------------------------�]�w �n�s����  �B�ʸ��-------------------------------------------------------------
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
        //---------------------------- FTMS  �]�w Treadmill  Feature -------------------------------------------------- 
        FTMS_Feature = heart_rate_support | pace_support | elevation_gain_support | inclination_support  |
                       metalbolic_equivalent_support | forceOnbelt_and_powerOutput_support | total_distance_support |
                       elapsed_time_support | remaining_time_support | expended_energy_support;
            
        //  expended_energy_support (�u�n��d������ƥX��  Kinomap�N������߸�= =)
        //average_speed_support                     
         
        //---------------------------- FTMS  �]�w Treadmill Control Feature-------------------------------------------------- 
        FTMS_Control = inclination_target_setting_support | heart_rate_target_setting_support |
             target_expended_energy_configuration_support | target_step_number_configuartion_support |
             target_distance_configuration_support        | target_training_time_configuration_support;    
        
#else
        
        //---------------------------- FTMS  �]�w Treadmill  Feature -------------------------------------------------- 
        FTMS_Feature = heart_rate_support | pace_support | elevation_gain_support | inclination_support  |
                       metalbolic_equivalent_support | forceOnbelt_and_powerOutput_support | total_distance_support |
                       elapsed_time_support | remaining_time_support | expended_energy_support | expended_energy_support;
            
        //  expended_energy_support (�u�n��d������ƥX��  Kinomap�N������߸�= =)
        //average_speed_support                     
         
        //---------------------------- FTMS  �]�w Treadmill Control Feature-------------------------------------------------- 
        FTMS_Control = inclination_target_setting_support | heart_rate_target_setting_support |
             target_expended_energy_configuration_support | target_step_number_configuartion_support |
             target_distance_configuration_support        | target_training_time_configuration_support;      
#endif

        
        
    }else if(Type == indoor_bike){
    
        //---------------------------- FTMS  �]�w Indoor Bike  Feature -------------------------------------------------- 
        FTMS_Feature = resistance_level_support | cadence_support | power_measurement_support | heart_rate_support | expended_energy_support ;
        
        
        // total_distance_support |  average_speed_support | elapsed_time_support | remaining_time_support; 
        
        //---------------------------- FTMS  �]�wIndoor Bike Control Feature-------------------------------------------------- 
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

    
    if(ucPage == 0){     //  �]�w Feature   fitness machine feature�]uuid�G0x2ACC)
        
        FTMS_Feature_Config(Machine_Type);
           
        ucBtmTxBuf[3] = FTMS_Feature_4byte[0];   // 0~7
        ucBtmTxBuf[4] = FTMS_Feature_4byte[1];   // 8~15
        ucBtmTxBuf[5] = FTMS_Feature_4byte[2];   // 16~23
        ucBtmTxBuf[6] = FTMS_Feature_4byte[3];   // 24~31
        
        ucBtmTxBuf[7]  = FTMS_Control_4byte[0];   // 0~7
        ucBtmTxBuf[8]  = FTMS_Control_4byte[1];   // 8~15
        ucBtmTxBuf[9]  = FTMS_Control_4byte[2];   // 16~23
        ucBtmTxBuf[10] = FTMS_Control_4byte[3];   // 24~31
          
        btm_is_ready = 0; //�p�G���ק�  BTM �|���s Reset  ���U�@��F1
        
    }else if(ucPage == 1){
        
        // �]�w    Machine   Type
        //treadmill       0x0001              v �]�w���]�B��
        //cross trainer   0x0002
        //step climber    0x0004
        //stair climber   0x0008
        //rower           0x0010
        //indoor bike     0x0020

        ucBtmTxBuf[3] = (unsigned char)Machine_Type;    //Type Low byte
        ucBtmTxBuf[4] = 0x00;                            //Type High byte high byte �ȮɩT�w0x00  
        
        
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
        ucBtmTxBuf[4] = 0x0F;   //0x01 + 0x02 + 0x04 + 0x08;     //0x0F;   //  ���߸�
    }else if(SW == 0){
        ucBtmTxBuf[4] = 0x0E;   // 0x02 + 0x04 + 0x08;           //0x0F;   //  �S�߸�
    }
    ucBtmTxBuf[5] = 0x00;           //     
    ucBtmTxBuf[19] = ']';
    
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
    HAL_UART_Transmit(&huart2,ucBtmTxBuf, BtmData,10);
    
    
}*/


void F_Btm_FTMS_B0_Read(void){
    
    btm_is_ready = 1;  //�p�G������ B0 �� Response �N��Feature �S���ק� ���ݭnReset ����  ��btm_is_ready = 1
    
    ucRFVersion[0] = ucBtmRxData[3];
    ucRFVersion[1] = ucBtmRxData[4];
    ucRFVersion[2] = ucBtmRxData[5];  
}


//----------------------------------------0xB0 End --------------------------------------------------------


//----------------------------------------0xB1 Start --------------------------------------------------------

unsigned short i_RPM_sim = 0;
unsigned short a_RPM_sim = 0;

unsigned char PageSwitch = 1;  //�]�B����Ʀbpage 0 �M 1

unsigned int  uiDistance_1m;   //�� Program_Data.Distance �ন�@����
unsigned int  uiCalories_1kcal; //�� Program_Data.Calories �ন1 kcal

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
        //Kinomap  Indoor Bike  �Y�o�� i-speed ------------------------------------
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
        //�Ҳե�X�h�| "���H 10"
        ucBtmTxBuf[9]  =  (unsigned char)(simLevel*10);       // resistance level(sign)  L     ��� 0.1    
        ucBtmTxBuf[10] =  (unsigned char)((simLevel*10)>>8);  // resistance level(sign)  H
        //------------------------------------------------------------------------------
        
        //-------Power----------------------------------------------------------------------------------
        //Kinomap �Y I-power   // Zwift �Y�o�� Power
        ucBtmTxBuf[11] = (unsigned char)sim_i_Power;         // instantaneous power(sign) L    
        ucBtmTxBuf[12] = (unsigned char)(sim_i_Power>>8);    // instantaneous power(sign) H
        //-----------------------------------------------------------------------------------------------   
 
    }else if(PageSwitch == 3){
        
        //-----   RPM  ---------------------------------------------------------------
        //kinomap �Y  i_RPM     
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
        //Kinomap  Indoor Bike  �Y�o�� i-speed ------------------------------------
        ucBtmTxBuf[3] =  0XC1;       // instantaeous speed L      
        ucBtmTxBuf[4] =  0XC1;  // instantaeous speed H      
        //-------------------------------------------------------------------------
        
        //------INCLINE---------------------------------------------------
        //current inclination(��e���ɱר���) (0.1 percent)
        ucBtmTxBuf[10] =  0XC4;  // sim_incline;       // inclination (sign)L
        ucBtmTxBuf[11] =  0XC4;  // inclination (sign)H
        //current setting of the ramp angle (�שY���׷�e�]�m) (0.1 degree)
        
        //-------RAMP-----------------------------------------
        ucBtmTxBuf[12] = 0XC4;          // ramp and angle set (sign)L
        ucBtmTxBuf[13] = 0XC4;     // ramp and angle set (sign)H
        //-----------------------------------------------------------
        
        //-----Elevation Gain------------------------------------------
        //(�����׼W�q)
        ucBtmTxBuf[14] = 0XC5;     // positive elevation gain L 
        ucBtmTxBuf[15] = 0XC5;     // positive elevation gain H
        //(�t�����W�q)
        ucBtmTxBuf[16] =  0XC5;          // negative elevation gain L 
        ucBtmTxBuf[17] =  0XC5;     // negative elevation gain H        
        //--------------------------------------------------------
       
        //----Instantaneous Pace-----------------------------------------------------------
        ucBtmTxBuf[18] =  0XC6;  //Pace_Spd/60;  // treadmill instantaneous pace  // (�����B��) (0.1km per minute)
        //-------------------------------------------------------------------------------

        //---Total Distance---------------------------------------------------------------------------------
        uiDistance_1m = Program_Data.Distance/100;                  //  ��� 1����
        ucBtmTxBuf[7] = 0XC3;          //(unsigned char)(uiDistance_1m);          // total distance L     
        ucBtmTxBuf[8] = 0XC3 ;    //(unsigned char)(uiDistance_1m >> 8) ;    // total distance M     
        ucBtmTxBuf[9] = 0XC3;    //(unsigned char)(uiDistance_1m >> 16);    // total distance H  
        //--------------------------------------------------------------------------------------------------
   
    }else if(PageSwitch == 1){
        
        //----Average Pace--------------------------------------------------------------------
        ucBtmTxBuf[3] = 0XC7;  ///(600*usAvgCount)/uiAvgSpd;  // treadmill average pace // (�����B��) (0.1km per minute)
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
        ucBtmTxBuf[15] = 0XCD;   //0xFF;   // force on belt(sign)L   // (�ϥΪ̨B���]�a�[��t�y�����O)(1 newton)
        ucBtmTxBuf[16] = 0XCD;   //0x7F;   // force on belt(sign)H
       //-----------------------------------------------------------------------------------------------------------
        
        //---Power Output-------------------------------------------------------------------------------------------
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[17] = 0XCD;  //0xFF;   // power output(sign)L     // (�ϥΪ̨B���]�B���[��t�y�����O)(1 watt)
        ucBtmTxBuf[18] = 0XCD;  //0x7F;   // power output(sign)H
        //-------------------------------------------------------------------------------------------------------------
        
        
        //----------------------------------------------�ɶ����---------------------------------------------

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
        //Kinomap  Indoor Bike  �Y�o�� i-speed ------------------------------------
        ucBtmTxBuf[3] =   (unsigned char)(System_SPEED*10);       // instantaeous speed L      
        ucBtmTxBuf[4] =   (unsigned char)((System_SPEED*10)>>8);  // instantaeous speed H      
        //-------------------------------------------------------------------------
        
        //------INCLINE---------------------------------------------------
        //current inclination(��e���ɱר���) (0.1 percent)
        ucBtmTxBuf[10] =  (unsigned char)System_INCLINE;  // sim_incline;       // inclination (sign)L
        ucBtmTxBuf[11] =  (unsigned char)(System_INCLINE >> 8);                 // inclination (sign)H
        //current setting of the ramp angle (�שY���׷�e�]�m) (0.1 degree)
        
        //-------RAMP-----------------------------------------
        ucBtmTxBuf[12] = (unsigned char)sim_Ramp;          // ramp and angle set (sign)L
        ucBtmTxBuf[13] = (unsigned char)(sim_Ramp>>8);     // ramp and angle set (sign)H
        //-----------------------------------------------------------
        
        //-----Elevation Gain------------------------------------------
        //(�����׼W�q)
        ucBtmTxBuf[14] = (unsigned char)sim_Pos_Ele_Gain;          // positive elevation gain L 
        ucBtmTxBuf[15] = (unsigned char)(sim_Pos_Ele_Gain>>8);     // positive elevation gain H
        //(�t�����W�q)
        ucBtmTxBuf[16] = (unsigned char)sim_Neg_Ele_Gain;          // negative elevation gain L 
        ucBtmTxBuf[17] = (unsigned char)(sim_Neg_Ele_Gain>>8);     // negative elevation gain H        
        //--------------------------------------------------------
       
        //----Instantaneous Pace-----------------------------------------------------------
        ucBtmTxBuf[18] =  System_SPEED/60;  //sim_I_Pace;  // treadmill instantaneous pace  // (�����B��) (0.1km per minute)
        //-------------------------------------------------------------------------------

        //---Total Distance---------------------------------------------------------------------------------
        uiDistance_1m = Program_Data.Distance/100;             //  ��� 1����                
        ucBtmTxBuf[7] = (unsigned char)(uiDistance_1m);        // total distance L    (unsigned char)(sim_Distance_1m );     
        ucBtmTxBuf[8] = (unsigned char)(uiDistance_1m >> 8) ;  // total distance M     (unsigned char)(sim_Distance_1m >> 8) ;
        ucBtmTxBuf[9] = (unsigned char)(uiDistance_1m >> 16);  // total distance H     (unsigned char)(sim_Distance_1m >> 16);
        //--------------------------------------------------------------------------------------------------
   
    }else if(PageSwitch == 1){
        
        //----Average Pace--------------------------------------------------------------------
        ucBtmTxBuf[3] = System_SPEED/60;  ///(600*usAvgCount)/uiAvgSpd;  // treadmill average pace // (�����B��) (0.1km per minute)
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
        
        ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;  // elapsed time L  �y�u���ɶ�
        ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;  // elapsed time H
        //--------------------------------------------------------------------------

        
        
        
        //---Remaining Time------------------------------------------------------------ 
        ucBtmTxBuf[13] = (unsigned char)sim_Remain_Time;       //remaining time L     sim_Remain_Time
        ucBtmTxBuf[14] = (unsigned char)(sim_Remain_Time>>8);   //remaining time H    sim_Remain_Time>>8
        //--------------------------------------------------------------------------
        
        //----Force on Belt-------------------------------------------------------------------------------------------------
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[15] = (unsigned char)sim_Force_Belt;         //0xFF;   // force on belt(sign)L   // (�ϥΪ̨B���]�a�[��t�y�����O)(1 newton)
        ucBtmTxBuf[16] = (unsigned char)(sim_Force_Belt>>8);    //0x7F;   // force on belt(sign)H
       //-----------------------------------------------------------------------------------------------------------
        
        //---Power Output-------------------------------------------------------------------------------------------
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[17] = (unsigned char)sim_Power_Output;       //0xFF;   // power output(sign)L     // (�ϥΪ̨B���]�B���[��t�y�����O)(1 watt)
        ucBtmTxBuf[18] = (unsigned char)(sim_Power_Output>>8);  //0x7F;   // power output(sign)H
        //-------------------------------------------------------------------------------------------------------------
        
        
        //----------------------------------------------�ɶ����---------------------------------------------
        /*
        usSetTimeMin = Program_Data.Goal_Time / 60;
        if(usSetTimeMin == 0){
            ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;  // elapsed time L  �y�u���ɶ�
            ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;  // elapsed time H
            //Cloud_0x39_Info.c_Times = (Program_Data.Goal_Time - Program_Data.Goal_Counter);
            
        }else{
            ucBtmTxBuf[13] = Program_Data.Goal_Counter%256;  // remaining time L   �ѤU���ɶ�
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
    
    ucBtmTxBuf[2] =   PageSwitch ;           //  ���T�w�� Page 3  ��RPM��ƥX��
    
    if(PageSwitch == 0){
        
        //---------------
        //Kinomap  Indoor Bike  �Y�o�� i-speed
        ucBtmTxBuf[3] =   (unsigned char)sim_speed;        //(unsigned char)( System_SPEED * 10);         // instantaeous speed L         (unsigned char)( sim_speed * 10);    
        ucBtmTxBuf[4] =   (unsigned char)(sim_speed>>8);  //(unsigned char)((System_SPEED * 10)>>8);    // instantaeous speed H          (unsigned char)((sim_speed * 10)>>8);
        //---------------
        ucBtmTxBuf[5] = (unsigned char)sim_Aspeed;        //(unsigned char)uiAvgSpeed;          // average speed L   //����0
        ucBtmTxBuf[6] = (unsigned char)(sim_Aspeed >>8);  //(unsigned char)(uiAvgSpeed >> 8);   // average speed H
        //---------------
        uiDistance_1m = Program_Data.Distance/100;                  //  ��� 1����
        
        ucBtmTxBuf[7] =  (unsigned char)(sim_Distance_1m);          //(unsigned char)(uiDistance_1m);          // total distance L     
        ucBtmTxBuf[8] =  (unsigned char)(sim_Distance_1m >> 8) ;    //(unsigned char)(uiDistance_1m >> 8) ;    // total distance M     
        ucBtmTxBuf[9] =  (unsigned char)(sim_Distance_1m >> 16);    //(unsigned char)(uiDistance_1m >> 16);    // total distance H     
        
        //---------------
        //current inclination(��e���ɱר���) (0.1 percent)
        ucBtmTxBuf[10] =  (System_INCLINE);  // sim_incline;       // inclination (sign)L
        ucBtmTxBuf[11] = 0x00;                                     // inclination (sign)H
        //current setting of the ramp angle (�שY���׷�e�]�m) (0.1 degree)
        
        //(0x7FFF => Data Not Available)
        ucBtmTxBuf[12] = 0x00;     // ramp and angle set (sign)L
        ucBtmTxBuf[13] = 0x00;     // ramp and angle set (sign)H
        //(�����׼W�q)
        ucBtmTxBuf[14] = 0x00;     // positive elevation gain L 
        ucBtmTxBuf[15] = 0x00;     // positive elevation gain H
        //(�t�����W�q)
        ucBtmTxBuf[16] = 0x00;     // negative elevation gain L 
        ucBtmTxBuf[17] = 0x00;     // negative elevation gain H
        // (�����B��) (0.1km per minute)
        ucBtmTxBuf[18] =  0; //Pace_Spd/60;        // treadmill instantaneous pace
        
        
    }else if(PageSwitch == 1){
        
        //---------------------
        // (�����B��) (0.1km per minute)
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
        
        //----------------------------------------------�ɶ����---------------------------------------------
        /*
        usSetTimeMin = Program_Data.Goal_Time / 60;
        
        if(usSetTimeMin == 0){
        ucBtmTxBuf[11] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)%256;  // elapsed time L  �y�u���ɶ�
        ucBtmTxBuf[12] = (Program_Data.Goal_Time - Program_Data.Goal_Counter)/256;  // elapsed time H
        
        //Cloud_0x39_Info.c_Times = (Program_Data.Goal_Time - Program_Data.Goal_Counter);
        
    }else{
        ucBtmTxBuf[13] = Program_Data.Goal_Counter%256;  // remaining time L   �ѤU���ɶ�
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
        
        //----------------------------------------------�ɶ����---------------------------------------------
        
        // (�ϥΪ̨B���]�a�[��t�y�����O)(1 newton)
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[15] = 0;//0xFF;   // force on belt(sign)L
        ucBtmTxBuf[16] = 0;//0x7F;   // force on belt(sign)H
        // (�ϥΪ̨B���]�B���[��t�y�����O)(1 watt)
        // (0x7FFF => Data Not Available)
        ucBtmTxBuf[17] = 0;//0xFF;   // power output(sign)L
        ucBtmTxBuf[18] = 0;//0x7F;   // power output(sign)H
        //---------------------------
        
    }else if(PageSwitch == 2){
        
        //---------------
        ucBtmTxBuf[3] =    0;    // step per minute L       (unsigned char)( sim_speed * 10);    
        ucBtmTxBuf[4] =    0;    // step per minute H          (unsigned char)((sim_speed * 10)>>8);
        //---------------
        ucBtmTxBuf[5] = 0;   // average step rate L   //����0
        ucBtmTxBuf[6] = 0;   // average step rate H
        //---------------
        
        ucBtmTxBuf[7] =  0;    //stride count L        
        ucBtmTxBuf[8] =  0;   // stride count H    
        
        //-----------------------------------------------------------------------------
        //�Ҳե�X�h�| "���H 10"
        ucBtmTxBuf[9]  =  (unsigned char)(simLevel*10);       // resistance level(sign)  L     ��� 0.1    ��1 �����
        ucBtmTxBuf[10] =  (unsigned char)((simLevel*10)>>8);  // resistance level(sign)  H
        //------------------------------------------------------------------------------
        
        //------------------------------------------------------------------------------------------------
        //Kinomap �Y I-power
        ucBtmTxBuf[11] = (unsigned char)sim_i_Power;       // instantaneous power(sign) L    ��100 ����
        ucBtmTxBuf[12] = (unsigned char)(sim_i_Power>>8);  // instantaneous power(sign) H
        //-----------------------------------------------------------------------------------------------
        
        //---------------------------------------------------------------------------------------------
        ucBtmTxBuf[13] = (unsigned char)sim_a_Power;       // average power(sign)  L        ��100 ����
        ucBtmTxBuf[14] = (unsigned char)(sim_a_Power>>8);  // average power(sign)  H
        //-----------------------------------------------------------------------------------------------
        
        ucBtmTxBuf[15] = 0x00;     // floors L
        ucBtmTxBuf[16] = 0x00;     // floors H 
        
        ucBtmTxBuf[17] = 0x00;     // step count L
        ucBtmTxBuf[18] = 0x00;     // step count H
        
        
    }else if(PageSwitch == 3){
        
        //---------------
        //kinomap �Y  i_RPM     //��W��  Zwift �Y�o��
        ucBtmTxBuf[3] = (unsigned char)i_RPM_sim;       // instantaneous cadence L   //����87*2 0xAE
        ucBtmTxBuf[4] = (unsigned char)(i_RPM_sim>>8);  // instantaneous cadence H     
        //---------------
        ucBtmTxBuf[5] = (unsigned char)a_RPM_sim;       // average cadence  L   ////����87*2 0xAE
        ucBtmTxBuf[6] = (unsigned char)(a_RPM_sim>>8);  // average cadence  H
        //---------------
        
        ucBtmTxBuf[7] =  0;    //average stroke rate
        
        ucBtmTxBuf[8] =  0;   // stroke rate �]or cadence of FEC Rower page )   
        
        
        ucBtmTxBuf[9]  = 0;    // stroke count  L
        ucBtmTxBuf[10] = 0;    // stroke count  H
        
        ucBtmTxBuf[11] = 0x00;     // rower instantaneous pace L    
        ucBtmTxBuf[12] = 0x00;     // rower instantaneous pace H
        
        ucBtmTxBuf[13] = 0x00;     // rower average pace  L        ��100 ����
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
short sGrade_FEC;     //�u����Ƴ���
float fGrade_FEC;     //���p���I    
unsigned char RR_FEC; //rolling resistance  �����O   //��� 5*10 exp-5

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

short usGrade_Temp_FTMS;   //FTMS �ǹL�Ӫ���l���
short sGrade_FTMS;      //�u����Ƴ���
unsigned char ucCRR_FTMS; //coefficient of rolling resistance  ���L�i���Y��   ��� 0.0001
unsigned short usWindSpeed_FTMS; // ���t   ���  0.001 meter per second

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
    
    
     //------------���� FEC Page 51 �Y��----------------------------
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
    
#if FTMS_Activated_Permission   //����QZwift�H���Ұ�
    if(FTMS_OP_Code == start_or_resume){
        if(System_Mode == Idle){
            Quick_Start_Init();       //Zwift �S�ƴN�|�@���U�o��cmd (�]�A���y�˸m��)
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
                
               
                IntoSummaryMode_Process(); //190115 �����i�Jsummary
                
                Btm_Task_Adder(FTMS_Data_Broadcast); //Page 0 //�s���B�ʸ�� �k�s
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

unsigned char  console_status = 1;	//1 : stop  2�Gpause  3: stop by safeKey   4: start 
unsigned char  cycle_length_for_FEC = 0;


//--------0xB4 �]�w�B�ʸ��Range--------------------------
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
        
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);    //  TXE = 0   (�p�GTXE = 1 ���S�M�� �|�d�b���_�̭�
        
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
        
        //-----------------------------------------------------�߸��˸m------------------------
        
        if(ucBtmRxData[1] ==0xE0){  // �˸m���y�ѽX
            Scan_Re_E0();        
        }
        
        if(ucBtmRxData[1] ==0xE1){  // �˸m�t��ѽX
            BLE_Pairing_Re_E1();        
        }
        
        if(ucBtmRxData[1] == 0xE2){  // �˸m�s�u�ѽX
            Link_Sensor_Re_E2();        
        }
        
        if(ucBtmRxData[1] == 0xCB){  // BLE�˸m�ƭȦ^�ǸѽX
            SensorReceive_CB();        
        }
        
        if(ucBtmRxData[1] == 0xCE){  // BLE�s�u���A�^�ǸѽX
            LinkStateReceive_CE();  
            
        }
        
        if(ucBtmRxData[1] == 0xCC){  // ANT�˸m�ƭȦ^�ǸѽX
            SensorReceive_CC();        
        } 
        
        if(ucBtmRxData[1] == 0xCF){  // ANT �s�u���A�^�ǸѽX
            LinkStateReceive_CF();        
        }
        
        if(ucBtmRxData[1] == 0xE4){  // �Ѱ��s�u
            disconnect_Sensor_Re_E4();     
        }
        
        //-----------------------------------------------------------���]------------
        
         /* 0x02 :  Fw  Update ��s �T�� -------- */
        if(ucBtmRxData[1] == 0x02){
            F_BtmRead02Cmd();
        }
        
        /* 0x03 :  Fw  Update  �ǿ� ��� -------- */
        if(ucBtmRxData[1] == 0x03){
            F_BtmRead03Cmd();
        }
        
         /* 0x04 :  Fw  Update  �ǿ� ����-------- */
        if(ucBtmRxData[1] == 0x04){
            F_BtmRead04Cmd();
        }
        
        /* 0x35 : status   ----app �Ұ�---  */            
        if((ucBtmRxData[1] == 0x35) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead35Cmd();
        }
        
        /*  0x37 : prs    -----speed ���o  ----*/
        if((ucBtmRxData[1] == 0x37) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead37Cmd();
        }
        
        /* 0x38 : pri    -----incline ���o */
        if((ucBtmRxData[1] == 0x38) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead38Cmd();
        }
        
        /* 0x36 : Set    --------321 go  --- SystemMode  Progarm TIME   AGE   WEIGHT_L  WEIGHT_H     �޳��j�p */
        if((ucBtmRxData[1] == 0x36) && (ucBtmRxData[2] <= 0x40)){
            F_BtmRead36Cmd();
        }
      
        if(FW_Transmiting_03_Flag != 1){   // 0x03�n�M�ߦ���� �Ӷê����L�o��
            
            /* 0x47 :   Ver  -------- */
            if((ucBtmRxData[1] == 0x47)&&(ucBtmRxData[2] <= 0x40)){
                F_BtmRead47Cmd();
            }
            
            /* 0x44 :   �g�Ǹ� �W��  -------- */
            if((ucBtmRxData[1] == 0x44)&&(ucBtmRxData[2] <= 0x40)){
                F_BtmRead44Cmd();
            }
            
            /* 0x46 :  �^�_ ODO -------- */
            if((ucBtmRxData[1] == 0x46)&&(ucBtmRxData[2] <= 0x40)){
                F_BtmRead46Cmd();
            }
            
        }
         /* 0x50 :  OTA  Update ��s �T�� -------- */
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
    
    
    /* -----         PA4 --> GFIT_CTRL    �}��RF_PWR      ------------- */
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
    
    
    /* -----         PA4 --> GFIT_CTRL    ����RF_PWR      ------------- */
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
        
        if(Ble_wait_disconnect_Time_out_Cnt == 0){  //0xFF  ��� �S���T���F  ����쥻�n�����Ʊ�
            
            wait_HR_disconnect_Flag = 0;
            
            
            //Btm_Task_Adder(Scan_BLE_HRC_Sensor);
            if(btm_HRC_disconnect_Task_Temp == No_Task){    //�߸��a���� �_�u
                Linked_HR_info.Link_state = disconnect;
            }else{
                Btm_Task_Adder(btm_HRC_disconnect_Task_Temp);  //���s�j���_�u �í��s�j�M
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
    
    if(FirstCB_Time_Cnt >=300){  //�g�L30���S��CB��ƶǶi��
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
