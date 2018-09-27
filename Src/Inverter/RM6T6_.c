#include "stm32f0xx_hal.h"
#include "system.h"
#include "RM6T6.h"


UART_HandleTypeDef huart1;


//---Tx
unsigned char TXData_Length;
unsigned char ucUART_TxBuf[UART_TxBufDataLength];

//---Rx
unsigned char ucUART_RxBuf[UART_RxBufDataLength] ,  ucUART_RxData [UART_RxBufDataLength];
unsigned char ucUART_RxTemp;
unsigned char RX_ACKDataLength ;
unsigned char ucRxAdderss = 0;


Response_Message_Def  Response_Message;



//------------------------------------------------------------------------------------------------------
/*
#define REQ 0xF6                //－ Console 下達命令給Inverter時的訊息開頭。
#define ANS 0xF1                //－ 當Inverter 需要回覆資料給Console時的訊息開頭。
#define ACK 0xF2               //－ 當Inverter 不需要回覆資料給Console時的訊息開頭。
#define NAK 0xF3               //－ 當Inverter 接收到不合法的命令時的訊息開頭。 
#define END 0xF4               //－ 通訊訊息結尾。
*/
//----------------------------------------------------------------------------------------------------------


//--DATA

const uint16_t SpeedHz_Tab[311] =
{     0,
    100,  200,  250,  300,  350,  400,  450,  500,  550,  600,  // 0.1~ 1.0()
    660,  720,  780,  840,  900,  960, 1020, 1080, 1140, 1200,  // 1.1~ 2.0(60)
   1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1760,  // 2.1~ 3.0(56)
   1760, 1760, 1760, 1760, 1760, 1760, 1760, 1760, 1760, 2340,  // 3.1~ 4.0(58)
   2340, 2340, 2340, 2340, 2340, 2340, 2340, 2340, 2340, 2950,  // 4.1~ 5.0(61)
   2950, 2950, 2950, 2950, 2950, 2950, 2950, 2950, 2950, 3510,  // 5.1~ 6.0(56)
   3510, 3510, 3510, 3510, 3510, 3510, 3510, 3510, 3510, 4110,  // 6.1~ 7.0(60)
   4110, 4110, 4110, 4110, 4110, 4110, 4110, 4110, 4110, 4680,  // 7.1~ 8.0(57)
   4680, 4680, 4680, 4680, 4680, 4680, 4680, 4680, 4680, 5270,  // 8.1~ 9.0(59)
   5270, 5270, 5270, 5270, 5270, 5270, 5270, 5270, 5270, 5840,  // 9.1~10.0(57)
   5840, 5840, 5840, 5840, 5840, 5840, 5840, 5840, 5840, 6420,  //10.1~11.0(58)
   6420, 6420, 6420, 6420, 6420, 6420, 6420, 6420, 6420, 7000,  //11.1~12.0(58)
   7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7580,  //12.1~13.0(58)
   7580, 7580, 7580, 7580, 7580, 7580, 7580, 7580, 7580, 8160,  //13.1~14.0(58)
   8160, 8160, 8160, 8160, 8160, 8160, 8160, 8160, 8160, 8750,  //14.1~15.0(59)
   8750, 8750, 8750, 8750, 8750, 8750, 8750, 8750, 8750, 9330,  //15.1~16.0(58)
   9330, 9330, 9330, 9330, 9330, 9330, 9330, 9330, 9330, 9930,  //16.1~17.0(60)
   9930, 9930, 9930, 9930, 9930, 9930, 9930, 9930, 9930,10600,  //17.1~18.0(67)
  10600,10600,10600,10600,10600,10600,10600,10600,10600,11170,  //18.1~19.0(57)
  11170,11170,11170,11170,11170,11170,11170,11170,11170,11740,  //19.1~20.0(57)
  11740,11740,11740,11740,11740,11740,11740,11740,11740,12330,  //20.1~21.0(59)
  12330,12330,12330,12330,12330,12330,12330,12330,12330,12890,  //21.1~22.0(56)
  12890,12890,12890,12890,12890,12890,12890,12890,12890,13510,  //22.1~23.0(62)
  13510,13510,13510,13510,13510,13510,13510,13510,13510,14030,  //23.1~24.0(52)
  14030,14030,14030,14030,14030,14030,14030,14030,14030,14800,  //24.1~25.0(77)
  14800,14800,14800,14800,14800,14800,14800,14800,14800,15400,   //25.1~26.0(43)
  15400,15400,15400,15400,15400,15400,15400,15400,15400,16000,   //26.1~27.0(40)
  16000,16000,16000,16000,16000,16000,16000,16000,16000,16600,   //27.1~28.0(40)
  16600,16600,16600,16600,16600,16600,16600,16600,16600,17200,   //28.1~29.0(40)
  17200,17200,17200,17200,17200,17200,17200,17200,17200,17800,   //29.1~30.0(40)
  17800,17800,17800,17800,17800,17800,17800,17800,17800,18400   //30.1~31.0(40)
};




uint16_t F_SpeedToHz(uint8_t Speed)  // dsp_spd change to speed_Hz
{
  unsigned  short  SpdTemp_KM ,  data1,data2,data3;
  uint16_t R_Speed_Hz;   //要輸出給變頻器的頻率


  if(System_Unit == Imperial ){ 
    SpdTemp_KM = ((Speed*8)/5);    // speed : ml ==> km    
  }else if(System_Unit ==   Metric){
    SpdTemp_KM = Speed;                //speed :  km
  }
  
  data1 = SpdTemp_KM%10;               //
  data2 = SpeedHz_Tab[SpdTemp_KM];    //
  
  if(SpdTemp_KM < 10 || data1 == 0){      //0.1~0.9
    R_Speed_Hz = data2;
    return R_Speed_Hz;
  }else{
    data3 = (SpeedHz_Tab[SpdTemp_KM+10]-data2)/10;
    R_Speed_Hz = (data2+(data3*data1));
    return  R_Speed_Hz;
  }
}


const uint16_t InclineADTable[31] = {
    
    214 , 233,     //0.0     0.5  %
    252 , 271,     //1.0     1.5  %            
    290 , 309,     //2.0     2.5  %         
    335 , 355,     //3.0     3.5  %
    375 , 394,     //4.0     4.5  %
    415 , 440,     //5.0     5.5  %
    460 , 481,     //6.0     6.5  %
    500 , 520,     //7.0     7.5  %
    538 , 559,     //8.0     8.5  %
    580 , 619,     //9.0     9.5  %
    638 , 676,     //10.0   10.5  %
    695 , 729,     //11.0   11.5  %
    748 , 767,     //12.0   12.5  %
    805 , 830,     //13.0   13.5  %
    884,  923,     //14.0   14.5  %
    923            //15%   
};



//------  參數 命令
void PAR_CMD(CMD_Type_Def cmd_Type,unsigned char Parameter_No,  uint32_t Data );
 

//-----------------------  RM6T6_初始化      -------------


void UART_485_GPIO_Init(){
  
  GPIO_InitTypeDef GPIO_InitStruct;
 
  
  //ADM485 Control Pin   PA8-->Control
  
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin( GPIOA, GPIO_PIN_8 ,   GPIO_PIN_RESET  );
  
  
  //--PA9 Tx          -- PA10 Rx
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
      Error_Handler();
  }
  
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);  
  
  
}

void RM6T6_Init(){
    
  
    UART_485_GPIO_Init();
    
    
    // 問下控現在馬達有沒有再轉
    F_OUT();
    HAL_Delay(500);
    F_OUT();
    HAL_Delay(500);
    
    if(Response_Message.Status.Run == YES){
        OPT_CMD(Write , STOP);   
        HAL_Delay(500);
        F_OUT();
        HAL_Delay(500);
    }
    
    INCL_POS(); 
    HAL_Delay(500);
    
    unsigned short InclineTemp;
    
    InclineTemp = Response_Message.Data;
    
    
    if(InclineTemp > (InclineADTable[0] + 6)){
        INCL_CMD( Write , InclineADTable[0] );   
        HAL_Delay(20);
    }
 
    __asm("NOP");
    
    //Set_Display_Incline( ucNowIncline ); 
    //取得現在 Speed Hz 值 
    HAL_Delay(20);
    

  

}



//-----------------------------          RX          ------------------------------------------------------------------------ 

void Response_Message_Clear(){
    
    Response_Message.Control_code     = Null;
    Response_Message.Data             = 0;
    Response_Message.Error_Code       = No_error;
    Response_Message.Instruction_code = 0x00;
    
    Response_Message.Status.ESP       = Unknow;
    Response_Message.Status.FWD       = Unknow;
    Response_Message.Status.STALL     = Unknow;
    Response_Message.Status.READY     = Unknow;
    Response_Message.Status.IF        = Unknow;
    Response_Message.Status.Status_Byte = 0x00;
    
    Response_Message.Parameter_Number   = 0x00;
}


void StatusDecode(unsigned char StatusByte){

    Response_Message.Status.Status_Byte = StatusByte;
    
    if((StatusByte>>7)&&0x01){
        Response_Message.Status.ESP = YES;
    }else{
        Response_Message.Status.ESP = NO;
    }
    
    if((StatusByte>>6)&&0x01){
        Response_Message.Status.Run = YES;
    }else{
        Response_Message.Status.Run = NO;
    }
    
    if((StatusByte>>5)&&0x01){
        Response_Message.Status.FWD = CW;
    }else{
        Response_Message.Status.FWD = CCW;
    }
    
    if((StatusByte>>4)&&0x01){
        Response_Message.Status.STALL = YES;
    }else{
        Response_Message.Status.STALL = NO;
    }
    
    if((StatusByte>>3)&&0x01){
        Response_Message.Status.READY = YES;
    }else{
        Response_Message.Status.READY = NO;
    } 
    
    if((StatusByte>>2)&&0x01){
        Response_Message.Status.IF = YES;
    }else{
        Response_Message.Status.IF = NO;
    } 
       

}

void GetResponseInfo(){
    
    Response_Message_Clear();  //先清掉前一個回復的 Messeage
    
 
    if(ucUART_RxData[0] ==0xF2){              
        //Response(6)  ACK  INS  STU  
        if(ucUART_RxData[1] ==0x90 || ucUART_RxData[1] ==0x98 || ucUART_RxData[1] ==0xA0 || ucUART_RxData[1] ==0xA8 || ucUART_RxData[1] ==0xA9 || ucUART_RxData[1] ==0xB3){         
           
           Response_Message.Control_code       = (Control_code_Def)ucUART_RxData[0];
           Response_Message.Instruction_code   = ucUART_RxData[1];
           StatusDecode(ucUART_RxData[2]);
           
        }   
        
    }else if(ucUART_RxData[0] == 0xF3){    
        //Response(7) (9) NAK  INS  Error  STU 
        Response_Message.Control_code       = (Control_code_Def)ucUART_RxData[0];
        Response_Message.Instruction_code   = ucUART_RxData[1];
        
        if(ucUART_RxData[1] != 0xFF){
            
            Response_Message.Error_Code = (Error_code_Def)ucUART_RxData[2];
            StatusDecode(ucUART_RxData[3]);
            
        }else if(ucUART_RxData[1] == 0xFF){
            
            Response_Message.Parameter_Number = ucUART_RxData[2];
            Response_Message.Error_Code       = (Error_code_Def)ucUART_RxData[3];
            StatusDecode(ucUART_RxData[4]); 
            
        }
        
        
    }else if(ucUART_RxData[0] ==0xF1){       
        //Response(10)  ANS  INS  Data(H)   Data(L)  STU 
        if(ucUART_RxData[1] ==0x10 || ucUART_RxData[1] ==0x11 || ucUART_RxData[1] ==0x12 || ucUART_RxData[1] ==0x13 || ucUART_RxData[1] ==0x16 ||      
           ucUART_RxData[1] ==0x17 || ucUART_RxData[1] ==0x18 || ucUART_RxData[1] ==0x19 || ucUART_RxData[1] ==0x30 || ucUART_RxData[1] ==0x31  ){         
               
               Response_Message.Control_code     = (Control_code_Def)ucUART_RxData[0];
               Response_Message.Instruction_code = ucUART_RxData[1];
               
               //------    Split Code 處理  ---------
               unsigned char LByteTemp;
               unsigned char HByteTemp;
               
               if(ucUART_RxData[2] == 0xF7){
                   HByteTemp =  0xF0 + ucUART_RxData[3];
                   
                   if(ucUART_RxData[4] == 0xF7 ){
                       LByteTemp = 0xF0 + ucUART_RxData[5];
                       StatusDecode(ucUART_RxData[6]);
                   }else{
                       LByteTemp = ucUART_RxData[4];
                       StatusDecode(ucUART_RxData[5]);
                   }
                   
               }else{
                   HByteTemp = ucUART_RxData[2];  //------------------
                   
                   if(ucUART_RxData[3] == 0xF7 ){
                       LByteTemp = 0xF0 + ucUART_RxData[4];
                       StatusDecode(ucUART_RxData[5]);
                   }else{
                       LByteTemp = ucUART_RxData[3];//--------------------
                       StatusDecode(ucUART_RxData[4]);
                   }
               }
               
               Response_Message.Data = (HByteTemp<<8) +  LByteTemp;
               
               
               
    
               //Response(11)  ANS  INS  Data(L)  STU
        }else if(ucUART_RxData[1] ==0x1A || ucUART_RxData[1] ==0x21 || ucUART_RxData[1] ==0x22 || ucUART_RxData[1] ==0x23 || ucUART_RxData[1] ==0x28 ||    
                    ucUART_RxData[1] ==0x29 || ucUART_RxData[1] ==0x33 ){
                     
                        Response_Message.Control_code     = (Control_code_Def)ucUART_RxData[0];
                        Response_Message.Instruction_code = ucUART_RxData[1];
                        
                        if(ucUART_RxData[2] == 0xF7){
                            Response_Message.Data = 0xF0 + ucUART_RxData[3];
                            StatusDecode(ucUART_RxData[4]);
                        }else{                        
                            Response_Message.Data = ucUART_RxData[2];
                            StatusDecode(ucUART_RxData[3]);
                        }
  
             //Response(12)  ANS  7F   Par  Data(H)   Data(M)   Data(L)  STU
        }else if(ucUART_RxData[1] ==0x7F){           //總共右 HML 共3byte 
            
            Response_Message.Control_code     = (Control_code_Def)ucUART_RxData[0];
            Response_Message.Instruction_code = ucUART_RxData[1];
            Response_Message.Parameter_Number = ucUART_RxData[2];
            
            
            //------    Split Code 處理  ---------
            unsigned char L_ByteTemp;
            unsigned char M_ByteTemp;
            unsigned char H_ByteTemp;
            
            if(ucUART_RxData[3] == 0xF7 ){      
                if(ucUART_RxData[5] == 0xF7 ){
                    if(ucUART_RxData[7] == 0xF7 ){
                        //3個Byte皆>=0xF0                          1  1   1
                        H_ByteTemp = 0xF0 + ucUART_RxData[4];
                        M_ByteTemp = 0xF0 + ucUART_RxData[6];
                        L_ByteTemp = 0xF0 + ucUART_RxData[8];
                        StatusDecode(ucUART_RxData[9]);
                    }else{ 
                        //前2個 >=0xF0   L_Byte < 0xF0              1   1   0
                        H_ByteTemp = 0xF0 + ucUART_RxData[4];
                        M_ByteTemp = 0xF0 + ucUART_RxData[6];
                        L_ByteTemp =        ucUART_RxData[7];
                        StatusDecode(ucUART_RxData[8]);
                    }
                }else{
                
                    if(ucUART_RxData[6] == 0xF7 ){
                        //                                           1  0   1
                        H_ByteTemp = 0xF0 + ucUART_RxData[4];
                        M_ByteTemp =        ucUART_RxData[5];
                        L_ByteTemp = 0xF0 + ucUART_RxData[7];
                        StatusDecode(ucUART_RxData[8]);
                    }else{ 
                        //                                           1   0   0
                        H_ByteTemp = 0xF0 + ucUART_RxData[4];
                        M_ByteTemp =        ucUART_RxData[5];
                        L_ByteTemp =        ucUART_RxData[6];
                        StatusDecode(ucUART_RxData[7]);
                    }
                }
                
            }else{
                if(ucUART_RxData[4] == 0xF7 ){
                    if(ucUART_RxData[6] == 0xF7 ){
                        //                                        0  1   1
                        H_ByteTemp = ucUART_RxData[3];
                        M_ByteTemp = 0xF0 + ucUART_RxData[5];
                        L_ByteTemp = 0xF0 + ucUART_RxData[7];
                        StatusDecode(ucUART_RxData[8]);
                    }else{ 
                        //                                        0   1   0
                        H_ByteTemp = ucUART_RxData[3];
                        M_ByteTemp = 0xF0 + ucUART_RxData[5];
                        L_ByteTemp =        ucUART_RxData[6];
                        StatusDecode(ucUART_RxData[7]);
                    }
                }else{
                    if(ucUART_RxData[5] == 0xF7 ){
                        //                                           0  0   1
                        H_ByteTemp = ucUART_RxData[3];
                        M_ByteTemp = ucUART_RxData[4];
                        L_ByteTemp = 0xF0 + ucUART_RxData[6];
                        StatusDecode(ucUART_RxData[7]);
                    }else{ 
                        //                                           0   0   0
                        H_ByteTemp = ucUART_RxData[3];
                        M_ByteTemp = ucUART_RxData[4];
                        L_ByteTemp = ucUART_RxData[5];
                        StatusDecode(ucUART_RxData[6]);
                    }
                }
            }
            
            Response_Message.Data = (H_ByteTemp<<16) + (M_ByteTemp<<8) + L_ByteTemp;
                
        }    
        
    }    
    
}


unsigned char Inverter_UART_Busy_Flag;


void Inveter_UART_IT_Recive(){
    
    if(HAL_UART_Receive_IT(&huart1,&ucUART_RxTemp,1) == HAL_OK){
        
        ucUART_RxBuf[ucRxAdderss] = ucUART_RxTemp;   
        
        __asm("NOP");
        
        if( ucUART_RxBuf[ucRxAdderss] == 0xF4  ){
            
            RX_ACKDataLength = ucRxAdderss + 1;
            
            __asm("NOP");
            
            memset(ucUART_TxBuf , 0x00 , UART_RxBufDataLength);
            
            Inverter_UART_Busy_Flag = 0;
            
            for(unsigned int i = 0;   i  < RX_ACKDataLength ;  i++){
                ucUART_RxData[i] =  ucUART_RxBuf[i];
                ucUART_RxBuf[i] = 0;
            }
            
            GetResponseInfo();   
            
            ucRxAdderss = 0;
            
            __asm("NOP");
      
        }else{
            
            if( (ucUART_RxBuf[0] !=0xF1 ) && (ucUART_RxBuf[0] !=0xF2)  && (ucUART_RxBuf[0] !=0xF3) ){
                ucRxAdderss=0;
            }else{
                Inverter_UART_Busy_Flag = 1;
                ucRxAdderss++;
            }
            
        }
    }
}






//---------------------------------     TX              -----------------------------------------

//--------------------------------- Tx base func  ---------------------------

void DEControl(unsigned char state){      //RS485 Control Pin
    
    if(state == 1){
        HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_8  ,GPIO_PIN_SET);
    }else if(state == 0){
        HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_8  ,GPIO_PIN_RESET);
    }
}

void UART_TX_Transform(){
    
    DEControl(High);
    __asm("NOP");
    HAL_UART_Transmit(&huart1,ucUART_TxBuf, TXData_Length,10);
    __asm("NOP");
    DEControl(Low);  

}

//-------------------------Tx  Asistance  Func   ----------------------------

unsigned char *data;		//通訊訊息指標
unsigned char length;		//通訊訊息長度


unsigned int CRC_Check(unsigned char *data,unsigned char length){
    
    int  i;
    unsigned int reg_crc = 0xffff;
    
    while(length--)
    {
        reg_crc^=*data++;
        for(i = 0;i < 8;i++)
            if(reg_crc&0x01)
                reg_crc = (reg_crc>>1)^0xa001;
            else
                reg_crc = reg_crc>>1;
    }
    
    return reg_crc;
}


unsigned char Split(unsigned char*array ,unsigned char Length){     //Split code Func
    
    for(unsigned char i =2 ; i < Length-1 ; i++ ){     //    if   length = 8      i  = > 1~6     length =8
        
        if((array[i] & 0xF0) == 0xF0){
            
            for(unsigned char j = Length ; j > i+1  ; j --){
                array[j] = array[j -1];
            } 
            
            array[i+1] = array[i] &0x0F;
            array[i] = 0xF7;       
            Length++;
            i+=1;         
            
        }
    } 
    return Length;   // 回傳傳送長度
    
}




unsigned int CRC_Temp;
unsigned char CrcBuffer[10];



//--------------------------   Tx  指令碼庫 (Instruction Code)   -------------------------------------------



//-- InsCode No.1
void F_CMD(CMD_Type_Def cmd_Type , uint16_t  Data){                //設定頻率
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    //---------------------------------------------控制 主馬達頻率(轉速)  
    if(cmd_Type == Write){
        
        CrcBuffer[0] = 0x90;
        CrcBuffer[1] = (unsigned char)(Data>>8);
        CrcBuffer[2] = (unsigned char)Data;
        CRC_Temp = CRC_Check(CrcBuffer,3);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x90;  
        ucUART_TxBuf[2] = CrcBuffer[1]; 
        ucUART_TxBuf[3] = CrcBuffer[2];
        ucUART_TxBuf[4] = CRC_Temp>>8;    
        ucUART_TxBuf[5] = CRC_Temp;   
        ucUART_TxBuf[6] = 0xF4;  
        
        TXData_Length = Split(ucUART_TxBuf, 7);
        //--------------------------------------------------------------------------------------
    }else if (cmd_Type == Read){
        
        CrcBuffer[0]=0x10;
        CRC_Temp = CRC_Check(CrcBuffer,1); 
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x10;   
        ucUART_TxBuf[2] = CRC_Temp>>8;   
        ucUART_TxBuf[3] = CRC_Temp;         
        ucUART_TxBuf[4] = 0xF4;   
        
        TXData_Length = Split(ucUART_TxBuf, 5);
    }
    //--------------------------------------------------------------------------------------
    UART_TX_Transform();
    
}

//-- InsCode No.2
void F_OUT(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x11;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x11;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();  
    
}   


//-- InsCode No.3                                                                                
void V_OUT(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x12;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x12;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split( ucUART_TxBuf , 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}  


//-- InsCode No.4
void C_OUT(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x13;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x13;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}                  

//-- InsCode No.7
void VBUS(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x16;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x16;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}                       


//-- InsCode No.8
void INV_TEMP(){
    
    ucRxAdderss=0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x17;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x17;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
} 


//-- InsCode No.9
void INCL_CMD( CMD_Type_Def cmd_Type , uint16_t  Data ){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    //---------------------------------------------------- 
    if(cmd_Type == Write){
        
        CrcBuffer[0] = 0x98;
        CrcBuffer[1] = (unsigned char)(Data>>8);
        CrcBuffer[2] = (unsigned char)Data;
        CRC_Temp = CRC_Check(CrcBuffer,3);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x98;  
        ucUART_TxBuf[2] = CrcBuffer[1];
        ucUART_TxBuf[3] = CrcBuffer[2];   
        ucUART_TxBuf[4] = CRC_Temp>>8;    
        ucUART_TxBuf[5] = CRC_Temp;   
        ucUART_TxBuf[6] = 0xF4;  
        
        TXData_Length = Split(ucUART_TxBuf, 7);
        //--------------------------------------------------------------------------------------
    }else if (cmd_Type == Read){
        CrcBuffer[0] = 0x18;
        CRC_Temp = CRC_Check(CrcBuffer,1);   
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x18;   
        ucUART_TxBuf[2] = CRC_Temp>>8;   
        ucUART_TxBuf[3] = CRC_Temp;         
        ucUART_TxBuf[4] = 0xF4;   
        
        TXData_Length = Split(ucUART_TxBuf, 5);
        
    }
    //--------------------------------------------------------------------------------------
     UART_TX_Transform();

}


//-- InsCode No.10
void INCL_POS(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x19;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x19;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}       


//-- InsCode No.11
void TROBLE_MSG(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x1A;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x1A;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length= Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}


//-- InsCode No.12
void OPT_CMD(CMD_Type_Def cmd_Type ,Motor_State_Def motor_State){           //設定正轉反轉停止
    
    ucRxAdderss = 0;    //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    if(cmd_Type == Write){
        
        CrcBuffer[0] = 0xA0;
        CrcBuffer[1] = (unsigned char)motor_State;
        CRC_Temp = CRC_Check(CrcBuffer,2);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0xA0;  
        ucUART_TxBuf[2] = (unsigned char)motor_State;   
        ucUART_TxBuf[3] = CRC_Temp>>8;    
        ucUART_TxBuf[4] = CRC_Temp;    
        ucUART_TxBuf[5] = 0xF4;   
        
        TXData_Length = Split(ucUART_TxBuf, 6);
        
    }else if (cmd_Type == Read){
        
        CrcBuffer[0] = 0x20;
        CRC_Temp = CRC_Check(CrcBuffer,1);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x20;   
        ucUART_TxBuf[2] = CRC_Temp>>8;    
        ucUART_TxBuf[3] = CRC_Temp;   
        ucUART_TxBuf[4] = 0xF4;    
        
        TXData_Length = Split(ucUART_TxBuf, 5);
    }
    
    UART_TX_Transform();
    
}


//-- InsCode No.13
void C_EFF(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x21;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x21;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}      


//-- InsCode No.14
void M_INDEX(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x22;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x22;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}



//-- InsCode No.15
void DB_DUTY(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x23;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x23;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}


//-- InsCode No.16
void INCL_OPT(unsigned char  cmd_Type, unsigned char Data  ){
    
    ucRxAdderss = 0;    //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    if(cmd_Type == Write){
        
        CrcBuffer[0] = 0xA8;
        CrcBuffer[1] = Data;
        CRC_Temp = CRC_Check(CrcBuffer,2);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0xA8;  
        ucUART_TxBuf[2] = Data;   
        ucUART_TxBuf[3] = CRC_Temp>>8;    
        ucUART_TxBuf[4] = CRC_Temp;    
        ucUART_TxBuf[5] = 0xF4;   
        
        TXData_Length = Split(ucUART_TxBuf, 6);
        
    }else if (cmd_Type == Read){
        
        CrcBuffer[0] = 0x28;
        CRC_Temp = CRC_Check(CrcBuffer,1);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x28;   
        ucUART_TxBuf[2] = CRC_Temp>>8;    
        ucUART_TxBuf[3] = CRC_Temp;   
        ucUART_TxBuf[4] = 0xF4;    
        
        TXData_Length = Split(ucUART_TxBuf, 5);
        
    }
    
    UART_TX_Transform();
    
}    


//-- InsCode No.17
void INCL_FORCE(unsigned char  cmd_Type, unsigned char Data){
    
    ucRxAdderss = 0;    //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    if(cmd_Type == Write){
        
        CrcBuffer[0] = 0xA9;
        CrcBuffer[1] = Data;
        CRC_Temp = CRC_Check(CrcBuffer,2);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0xA9;  
        ucUART_TxBuf[2] = Data;   
        ucUART_TxBuf[3] = CRC_Temp>>8;    
        ucUART_TxBuf[4] = CRC_Temp;    
        ucUART_TxBuf[5] = 0xF4;   
        
        TXData_Length = Split(ucUART_TxBuf, 6);
        
    }else if (cmd_Type == Read){
        
        CrcBuffer[0] = 0x29;
        CRC_Temp = CRC_Check(CrcBuffer,1);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x29;   
        ucUART_TxBuf[2] = CRC_Temp>>8;    
        ucUART_TxBuf[3] = CRC_Temp;   
        ucUART_TxBuf[4] = 0xF4;    
        
        TXData_Length = Split(ucUART_TxBuf, 5);
        
    }
    
    UART_TX_Transform();
    
}


//-- InsCode No.18
void FootFall_10s(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x30;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x30;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}                
//-- InsCode No.19
void FootFall_Total(){
    
    ucRxAdderss = 0;   //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    CrcBuffer[0] = 0x31;
    CRC_Temp = CRC_Check(CrcBuffer,1);
    
    ucUART_TxBuf[0] = 0xF6;
    ucUART_TxBuf[1] = 0x31;   
    ucUART_TxBuf[2] = CRC_Temp>>8;    
    ucUART_TxBuf[3] = CRC_Temp;          
    ucUART_TxBuf[4] = 0xF4;   
    
    TXData_Length = Split(ucUART_TxBuf, 5);
    //----------------------------------------------------------------------------------
    UART_TX_Transform();
    
}
//-- InsCode No.20
void SaveToFlash(CMD_Type_Def cmd_Type,unsigned char Data){
    
    ucRxAdderss = 0;    //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    if(cmd_Type == Write){
        
        CrcBuffer[0] = 0xB3;
        CrcBuffer[1] = Data;
        CRC_Temp = CRC_Check(CrcBuffer,2);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0xB3;  
        ucUART_TxBuf[2] = Data;   
        ucUART_TxBuf[3] = CRC_Temp>>8;    
        ucUART_TxBuf[4] = CRC_Temp;    
        ucUART_TxBuf[5] = 0xF4;   
        
        TXData_Length = Split(ucUART_TxBuf, 6);
        
    }else if (cmd_Type == Read){
        
        CrcBuffer[0] = 0x33;
        CRC_Temp = CRC_Check(CrcBuffer,1);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x33;   
        ucUART_TxBuf[2] = CRC_Temp>>8;    
        ucUART_TxBuf[3] = CRC_Temp;   
        ucUART_TxBuf[4] = 0xF4;    
        
        TXData_Length = Split(ucUART_TxBuf, 5);
    }
    
    UART_TX_Transform();
    
}


///-----------------------  參數    Cmd-------------------------//
void PAR_CMD(CMD_Type_Def cmd_Type,unsigned char Parameter_No,  uint32_t Data ){
   
    ucRxAdderss = 0;    //收ACK  ANS   NAK  用
    
    memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
    
    if(cmd_Type == Write){
        
        CrcBuffer[0] = 0xFF;
        CrcBuffer[1] = Parameter_No;
        CrcBuffer[2] = (uint8_t)(Data>>16);
        CrcBuffer[3] = (uint8_t)(Data>>8);
        CrcBuffer[4] = (uint8_t)Data;
        
        CRC_Temp = CRC_Check(CrcBuffer,5);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0xFF;
        ucUART_TxBuf[2] = Parameter_No;
        ucUART_TxBuf[3] = (uint8_t)(Data>>16); 
        ucUART_TxBuf[4] = (uint8_t)(Data>>8);
        ucUART_TxBuf[5] = (uint8_t)Data;
        ucUART_TxBuf[6] = CRC_Temp>>8;    
        ucUART_TxBuf[7] = CRC_Temp;    
        ucUART_TxBuf[8] = 0xF4;    
       
        TXData_Length= Split(ucUART_TxBuf, 9);
        
    }else if (cmd_Type == Read){
        
        CrcBuffer[0] = 0x7F;
        CrcBuffer[1] = Parameter_No ;
        CrcBuffer[2] = (uint8_t)(Data>>16);
        CrcBuffer[3] = (uint8_t)(Data>>8);
        CrcBuffer[4] = (uint8_t)Data;
        CRC_Temp = CRC_Check(CrcBuffer,5);
        
        ucUART_TxBuf[0] = 0xF6;
        ucUART_TxBuf[1] = 0x7F;
        ucUART_TxBuf[2] =  Parameter_No ;
        ucUART_TxBuf[3] = (uint8_t)(Data>>16);
        ucUART_TxBuf[4] = (uint8_t)(Data>>8);
        ucUART_TxBuf[5] = (uint8_t)Data;
        ucUART_TxBuf[6] = CRC_Temp>>8;    
        ucUART_TxBuf[7] = CRC_Temp;    
        ucUART_TxBuf[8] = 0xF4;    
       
        TXData_Length = Split(ucUART_TxBuf, 9);
        
    }
    
    UART_TX_Transform();
    
}




