#include "stm32f4xx_hal.h"
#include "system.h"
#include "RM6T6.h"

UART_HandleTypeDef huart1;

//---Tx
unsigned char Tx_Transform_flag;
unsigned int Tx_No_Response_Cnt;
Error_code_Def  Erroe_Disp_Once;
unsigned char IF_Occur_flag;

unsigned char TXData_Length;
unsigned char ucUART_TxBuf[UART_TxBufDataLength];

//---Rx
typedef struct{
    unsigned char ucUART_RxBuff[UART_RxBufDataLength];
}ucUART_RxBuf_def;

unsigned char ucUART_RxBuf_Cnt;

ucUART_RxBuf_def ucUART_RxBuf_List[20];

unsigned char ucUART_RxBuf[UART_RxBufDataLength] ,  ucUART_RxData [UART_RxBufDataLength];
unsigned char ucUART_RxTemp;
unsigned char RX_ACKDataLength ;
unsigned char ucRxAdderss = 0;

unsigned char Inverter_UART_Busy_Flag;  

unsigned short INCL_Config_Value;
unsigned char  INCL_Moveing_Flag;
unsigned char  INCL_Moveing_Time_out_Cnt;

unsigned short SPEED_Config_Value;
unsigned char  SPEED_Changing_Flag;
unsigned char  SPEED_Change_Time_out_Cnt;

Response_Message_Def  Response_Message;
RM6T6_State_Def       RM6T6_state;

void DEControl(unsigned char state);
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

const uint16_t SpeedHzTab_CreateByBruce[236] = {

  249,313,371,425,476,530,583,639,690,744,
  797,849,900,954,1008,1065,1117,1169,1221,1273,
  1325,1376,1428,1480,1532,1584,1637,1689,1742,1794,
  1847,1900,1952,2005,2057,2110,2162,2215,2267,2320,
  2372,2424,2477,2529,2582,2634,2686,2738,2790,2842,
  2895,2947,2999,3051,3103,3155,3208,3260,3313,3365,
  3418,3470,3523,3575,3628,3680,3733,3786,3839,3892,
  3945,3998,4051,4104,4157,4210,4262,4314,4366,4418,
  4470,4522,4574,4626,4678,4730,4782,4834,4886,4938,
  4990,5042,5094,5146,5198,5250,5303,5356,5409,5462,
  5515,5568,5621,5674,5727,5780,5832,5884,5936,5988,
  6040,6092,6144,6196,6248,6300,6353,6406,6459,6512,
  6565,6618,6671,6724,6777,6830,6882,6934,6986,7038,
  7090,7142,7194,7246,7298,7350,7403,7455,7508,7560,
  7613,7665,7718,7770,7823,7875,7928,7980,8033,8085,
  8138,8190,8243,8295,8348,8400,8452,8504,8556,8608,
  8660,8712,8764,8816,8868,8920,8974,9028,9082,9136,
  9190,9244,9298,9352,9406,9460,9512,9564,9616,9668,
  9720,9772,9824,9876,9928,9980,10032,10084,10136,10188,
  10240,10292,10344,10396,10448,10500,10555,10610,10665,10720,
  10775,10830,10885,10940,10995,11050,11102,11154,11206,11258,
  11310,11362,11414,11466,11518,11570,11623,11676,11729,11782,
  11835,11888,11941,11994,12047,12100,12153,12206,12259,12312,
  12365,12418,12471,12524,12577,12630 };



/*
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
};*/

/*uint16_t F_SpeedToHz(uint16_t Speed)  // dsp_spd change to speed_Hz
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
}*/


uint16_t F_SpeedToHz(uint16_t Speed)  // dsp_spd change to speed_Hz
{
    unsigned  short  SpdTemp_KM;
    uint16_t R_Speed_Hz;   //要輸出給變頻器的頻率
   
    if(System_Unit == Imperial ){ 
        SpdTemp_KM = ((Speed*8)/5);    // speed : ml ==> km    
    }else if(System_Unit ==   Metric){
        SpdTemp_KM = Speed;                //speed :  km
    }
    
    if(SpdTemp_KM<4){
        R_Speed_Hz = 0;
    }else if(SpdTemp_KM >=4 && SpdTemp_KM <5){
       R_Speed_Hz = SpeedHzTab_CreateByBruce[SpdTemp_KM - 4];
        
    }else if(SpdTemp_KM >=5 &&  SpdTemp_KM <= 240){
        
        R_Speed_Hz = SpeedHzTab_CreateByBruce[SpdTemp_KM - 5];
        
    }else if(SpdTemp_KM>240){
        R_Speed_Hz = SpeedHzTab_CreateByBruce[235];
    }
    
    return  R_Speed_Hz;
   
}

const uint16_t InclineADTable[31] = {
    
    65 , 85,     //0.0     0.5  %
    113 , 138,     //1.0     1.5  %            
    167 , 189,     //2.0     2.5  %         
    224 , 248,     //3.0     3.5  %
    274 , 296,     //4.0     4.5  %
    325 , 349,     //5.0     5.5  %   0k
    376 , 402,     //6.0     6.5  %   ok
    428 , 458,     //7.0     7.5  %  //ok
    478 , 502,     //8.0     8.5  %    //ok
    529 , 554,     //9.0     9.5  %    //OK
    586 , 608,     //10.0   10.5  %   //ok
    635 , 660,     //11.0   11.5  %   //ok
    687 , 715,     //12.0   12.5  %  //ok
    742 , 767,     //13.0   13.5  %  //OK
    800,  827,     //14.0   14.5  %
    859            //15%   
};

// 系統參數設置
void Set_SPEED_Value(unsigned short speedValue){ 
    System_SPEED = speedValue;
}

void Set_INCLINE_Value(unsigned short inclineValue){
    System_INCLINE = inclineValue;
}
//------------------------RM6T6 任務 -------------------------------------------------
void Set_System_SPEED(){
    F_CMD( Write , F_SpeedToHz(System_SPEED)); 
    SPEED_Config_Value =  F_SpeedToHz(System_SPEED);
    RM6_Task_Adder(Read_SPEED);
}

void Set_System_INCLINE(){   //System_INCLINE   0~150     =>   InclineADTable index  0 ~ 30
    INCL_CMD( Write , InclineADTable[ System_INCLINE / 5  ] );
    INCL_Config_Value = InclineADTable[ System_INCLINE / 5  ];
    RM6_Task_Adder(Read_INCLINE);
}

void Set_Motor_FR(){
    OPT_CMD( Write , FR);  
}
void Set_Motor_STOP(){
    OPT_CMD( Write , STOP); 
}
void read_INCLINE(){
    INCL_POS(); 
}
void read_SPEED(){
    F_OUT();
}
void read_Foot_Fall(){
    FootFall_Total();
}
void read_Foot_Fall_10s(){
    FootFall_10s();
}
//--------------------------RM6T6 - 任務管理器----------------------------------------
#define RM6_Task_Amount 10
unsigned char RM6_Task_Cnt;
RM6T6_Task_Def RM6_Task_List[RM6_Task_Amount];  

void RM6T6_Task_ClearAll(){

    for(unsigned char i =0;i<RM6_Task_Amount;i++){
        RM6_Task_List[i] = NO_Task;
    }
}

void RM6_Task_Adder(RM6T6_Task_Def  RM6_Task){
    //目前沒有任務的話就不去確認 直接新增任務到清單
    if( RM6_Task_Cnt == 0 ){
            RM6_Task_List[RM6_Task_Cnt] = RM6_Task;
            RM6_Task_Cnt++;
    }else{
        //----如果任務 沒有在清單裡 才新增任務 避免任務重複
        unsigned char Task_Exit_Flag = 0;
        
        for(unsigned char i = 0; i < RM6_Task_Cnt; i++){
            if(RM6_Task == RM6_Task_List[i]){
                Task_Exit_Flag = 1;
                break;
            }
        }
        if(Task_Exit_Flag == 0){
            if(RM6_Task_Cnt < RM6_Task_Amount){  //最多N個任務在駐列清單
                
                /*if( RM6_Task == Set_SPEED ){
                    //------------------速度命令優先-----------------------
                    for(unsigned char i = (RM6_Task_Amount-1); i > 1; i--){
                        RM6_Task_List[i] = RM6_Task_List[i-1];                        
                    }                    
                    RM6_Task_List[0] = RM6_Task;
                    RM6_Task_Cnt++;
                }else{*/
                    RM6_Task_List[RM6_Task_Cnt] = RM6_Task;
                    RM6_Task_Cnt++;  
                //}
 
            }  
        }
    }  
}


unsigned char Task_execute_Flag = 1;
unsigned char Incline_Check_Flag = 1;
unsigned char Speed_Check_Flag = 1;
unsigned char footfall_Check_Flag = 1;

void RM6_background_Task(){

#if RM6T6_IN_USE
    
    if(T100ms_RM6T6_Task_Flag){
        T100ms_RM6T6_Task_Flag = 0;
        
        if(Tx_Transform_flag == 1){
            Tx_No_Response_Cnt++;
        }else{
            Tx_No_Response_Cnt = 0;
        }
        
        
        if(Task_execute_Flag == 1){
        //--------------------------------------------------------------------------------  
            if(Inverter_UART_Busy_Flag != 1){  //在RM6T6沒有收資料的時候才傳送 任務命令 
                if(RM6_Task_Cnt >=1){  //至少要有一個任務  
                    
                    
                    switch(RM6_Task_List[0]){  
                      case Set_SPEED:
                        Set_System_SPEED();
                        break;
                      case Set_INCLINE:
                        Set_System_INCLINE();
                        break;
                      case Motor_FR:
                        Set_Motor_FR();
                        break;
                      case Motor_RR:
                        break;
                      case Motor_STOP:
                        Set_Motor_STOP();
                        break;
                      case Read_SPEED:
                        read_SPEED();
                        break; 
                      case Read_INCLINE:
                        read_INCLINE();
                        break; 
                      case Read_Foot_Fall:
                        read_Foot_Fall();
                        break;
                      case  Read_Foot_Fall_10s:
                        read_Foot_Fall_10s();
                        break;
                      case NO_Task:
                        //not do anything
                        break;      
                    }
                    
                    //-----------------做完之後 刪掉[0] 的任務     [0]<--[1]    [1]<--[2] ....
                    
                    for(unsigned char i = 0; i < RM6_Task_Cnt; i++){
                        if(i == (RM6_Task_Amount - 1)){
                            RM6_Task_List[(RM6_Task_Amount - 1)] = NO_Task; 
                        }else{
                            RM6_Task_List[i] = RM6_Task_List[i+1];
                        }
                    }
                    RM6_Task_Cnt = RM6_Task_Cnt - 1;    //執行完 就 -1  
                    
                    //--------------------------------------------------------------------
                }   
            }
        //------------------------------------------------------------------------------
        }
        

          
        if(Incline_Check_Flag == 1){
            //-----------------------------------------------------------------------
            //檢查  揚升有沒有到達該到的位置
            if( (RM6T6_state.INCLINE_ivt >= (INCL_Config_Value + 6)) || (RM6T6_state.INCLINE_ivt <= (INCL_Config_Value - 6))){
                
                RM6_Task_Adder(Read_INCLINE);
                INCL_Moveing_Flag = 1;
                
                if(INCL_Moveing_Time_out_Cnt >=10){ //超過時間沒有到達目標速度 在下一次速度cmd
                    Set_INCLINE_Value(System_INCLINE);
                    RM6_Task_Adder(Set_INCLINE);
                    INCL_Moveing_Time_out_Cnt = 0;
                }
                INCL_Moveing_Time_out_Cnt++;
                
            }else{
                INCL_Moveing_Flag = 0;
                INCL_Moveing_Time_out_Cnt = 0;
            }
            //-----------------------------------------------------------------------
        }
        

        
        if(Speed_Check_Flag == 1){
            //-----------------------------------------------------------------------
            if((RM6T6_state.SPEED_ivt >= (SPEED_Config_Value + 6)) || (RM6T6_state.SPEED_ivt <= (SPEED_Config_Value - 6)) ){
                
                RM6_Task_Adder(Read_SPEED);
                SPEED_Changing_Flag = 1;
                if(SPEED_Change_Time_out_Cnt >=10){ //超過時間沒有到達目標速度 在下一次速度cmd
                    Set_SPEED_Value(System_SPEED);
  
                    
                    RM6_Task_Adder(Set_SPEED);
                    SPEED_Change_Time_out_Cnt = 0;
                }
                SPEED_Change_Time_out_Cnt++;
            }else{
                SPEED_Changing_Flag = 0;
                SPEED_Change_Time_out_Cnt = 0;
            }
            //-----------------------------------------------------------------------   
        }

        
        if(footfall_Check_Flag == 1){
        //----------------------------------------------------------------------------
            if( (System_Mode == Workout) || (System_Mode == WarmUp) || (System_Mode == CooolDown ) ){
                RM6_Task_Adder(Read_Foot_Fall);
                RM6_Task_Adder(Read_Foot_Fall_10s);
            }  
        //----------------------------------------------------------------------------    
        }
        

    }
  
#endif
}

//--------------------------------------------------------------------------------------
//------  參數 命令
void PAR_CMD(CMD_Type_Def cmd_Type,unsigned char Parameter_No,  uint32_t Data );
 
//-----------------------  RM6T6_初始化      -------------
#define Control_GPIO  GPIOA
#define Control_PIN     GPIO_PIN_11

void UART_485_GPIO_Init(){

  GPIO_InitTypeDef GPIO_InitStruct;
 //ADM485 Control Pin   PA11-->Control
  
  GPIO_InitStruct.Pin = Control_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Control_GPIO, &GPIO_InitStruct);
  HAL_GPIO_WritePin( Control_GPIO, Control_PIN ,   GPIO_PIN_RESET  );
  
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
    HAL_UART_Receive_IT(&huart1,&ucUART_RxTemp,1);
    // 問下控現在馬達有沒有再轉
    F_OUT();
    HAL_Delay(20);
    if(Response_Message.Status.Run == YES){
        OPT_CMD(Write,STOP);   
        HAL_Delay(20);
        F_OUT();
        HAL_Delay(20);
    }
    INCL_POS(); 
    HAL_Delay(20);
    
    unsigned short InclineTemp;
    InclineTemp = Response_Message.Data;
    if(InclineTemp > (InclineADTable[0] + 6)){
        INCL_CMD( Write , InclineADTable[0] );   
        HAL_Delay(20);
    }
    INCL_Config_Value  = InclineADTable[0];           //初始化目標 揚升直
    SPEED_Config_Value = F_SpeedToHz(System_SPEED);   //初始化目標 速度頻率
    F_CMD(Write,0);
    
    HAL_Delay(20);
    PAR_CMD(Read,90,0); //讀出VR 最小值
    HAL_Delay(20);
    if(Response_Message.Data != 60){
        PAR_CMD(Write,90,60);  //寫入 60 VR 最小值
        HAL_Delay(20);
    }  
    
    HAL_Delay(20);
}


void RM6T6_DeInit(){
    
    GPIO_InitTypeDef GPIO_InitStruct;
   
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE); 
    
        //--PA9 Tx    -- PA10 Rx    
    if (HAL_UART_DeInit(&huart1) != HAL_OK){
        Error_Handler();
    }
    
    //ADM485 Control Pin   PA11-->Control
    HAL_GPIO_WritePin( Control_GPIO, Control_PIN ,   GPIO_PIN_RESET  );
    GPIO_InitStruct.Pin = Control_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(Control_GPIO, &GPIO_InitStruct);

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

unsigned char status_shift_temp;
void StatusDecode(unsigned char StatusByte){

    Response_Message.Status.Status_Byte = StatusByte;
    
    status_shift_temp = StatusByte>>7;
    if((status_shift_temp)&0x01 == 0x01){
        Response_Message.Status.ESP = YES;
    }else{
        Response_Message.Status.ESP = NO;
    }
    
    status_shift_temp = StatusByte>>6;
    if((status_shift_temp)&0x01 == 0x01){
        Response_Message.Status.Run = YES;
    }else{
        Response_Message.Status.Run = NO;
    }
    
     
    status_shift_temp = StatusByte>>5;
    if((status_shift_temp)&0x01 == 0x01){
        Response_Message.Status.FWD = CW;
    }else{
        Response_Message.Status.FWD = CCW;
    }
    
    status_shift_temp = StatusByte>>4;
    if((status_shift_temp)&0x01 == 0x01){
        Response_Message.Status.STALL = YES;
    }else{
        Response_Message.Status.STALL = NO;
    }
    
    
    status_shift_temp = StatusByte>>3;
    if((status_shift_temp)&0x01 == 0x01){
        Response_Message.Status.READY = YES;
    }else{
        Response_Message.Status.READY = NO;
    } 
    
    status_shift_temp = StatusByte>>2;
    if((status_shift_temp)&0x01 == 0x01){
        Response_Message.Status.IF = YES;
    }else{
        Response_Message.Status.IF = NO;
    } 
       
}


unsigned int foot_Cnt_Temp;

void GetRM6T6_state_status(){
    RM6T6_state.status.ESP = Response_Message.Status.ESP;
    RM6T6_state.status.FWD = Response_Message.Status.FWD;
    RM6T6_state.status.IF  = Response_Message.Status.IF;
    RM6T6_state.status.READY = Response_Message.Status.READY;
    RM6T6_state.status.Run   = Response_Message.Status.Run;
    RM6T6_state.status.STALL = Response_Message.Status.STALL;
    RM6T6_state.status.Status_Byte = Response_Message.Status.Status_Byte; 
}


unsigned char pace_cnt_diff;

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
    
    if(Response_Message.Instruction_code == 0x11){
        
        RM6T6_state.SPEED_ivt = Response_Message.Data;
        //RM6T6_state.status      = Response_Message.Status;
        GetRM6T6_state_status();
    }
    if(Response_Message.Instruction_code == 0x19){
        
        RM6T6_state.INCLINE_ivt = Response_Message.Data;
        //RM6T6_state.status      = Response_Message.Status;
        GetRM6T6_state_status();
    }
    if(Response_Message.Instruction_code == 0x31){
   
        RM6T6_state.Foot_cnt = Response_Message.Data;
        if(RM6T6_state.Foot_cnt != foot_Cnt_Temp){
            
           
            pace_cnt_diff = RM6T6_state.Foot_cnt - foot_Cnt_Temp;    
            
            foot_Cnt_Temp = RM6T6_state.Foot_cnt;
    
            Program_Data.Pace += pace_cnt_diff; 
        }
        //RM6T6_state.status   = Response_Message.Status;
        GetRM6T6_state_status();
    }
    if(Response_Message.Instruction_code == 0x30){
    
        RM6T6_state.Foot_cnt_10s = Response_Message.Data;
        //RM6T6_state.status       = Response_Message.Status;
        GetRM6T6_state_status();    
    }    
}

void Inveter_UART_IT_Recive(){
    
     ucLog_data[0]='6';
     tx_flag = 1;
    //char str[] = "R\n";        
    //memcpy(ucLog_data,str,sizeof(str));       
    //LOG_UART6_Transmit();
    
    
    if(HAL_UART_Receive_IT(&huart1,&ucUART_RxTemp,1) == HAL_OK){
        ucUART_RxBuf[ucRxAdderss] = ucUART_RxTemp;   
        __asm("NOP");
        
        if( ucUART_RxBuf[ucRxAdderss] == 0xF4  ){
            RX_ACKDataLength = ucRxAdderss + 1;
            __asm("NOP");
            if(ucUART_RxBuf[1] == 0x90){
                __asm("NOP");
            }
            if(ucUART_RxBuf[1] == 0x19){
                //-----------------------------------------------------------------------------
                memcpy(ucUART_RxBuf_List[ucUART_RxBuf_Cnt].ucUART_RxBuff , ucUART_RxBuf   ,20 );
                ucUART_RxBuf_Cnt++;
                ucUART_RxBuf_Cnt = ucUART_RxBuf_Cnt%20;
                //-------------------------------------------------------------------------------
            }
            memset(ucUART_TxBuf , 0x00 , UART_RxBufDataLength);
            Inverter_UART_Busy_Flag = 0;
            for(unsigned int i = 0;   i  < RX_ACKDataLength ;  i++){
                ucUART_RxData[i] =  ucUART_RxBuf[i];
                ucUART_RxBuf[i] = 0;
            }
            
            
 
#if FAKE_RM6T6_Mode
            //---------------485 測試模式----------------------------------  
            if(System_Mode == RS485_Test_Mode){
                if(ucUART_RxData[0] == 0xF6){
                    if(ucUART_RxData[1] == 0x90){
                        
                        memset(ucUART_TxBuf , 0x00 , UART_TxBufDataLength);
                        TXData_Length = 6;
                        ucUART_TxBuf[0] = 0xF2;
                        ucUART_TxBuf[1] = 0x90;  
                        ucUART_TxBuf[2] = 0x28;   
                        ucUART_TxBuf[3] = 0xAE;    
                        ucUART_TxBuf[4] = 0x6D;    
                        ucUART_TxBuf[5] = 0xF4;   
                        DEControl(High);
                        HAL_UART_Transmit(&huart1,ucUART_TxBuf, TXData_Length,10);
                        DEControl(Low);
                    } 
                }
            }
            //--------------------------------------------------------   
#endif
        
            GetResponseInfo();   
            memset(ucUART_RxData , 0x00 , UART_RxBufDataLength); 
            ucRxAdderss = 0;
            __asm("NOP");
      
        }else{
            if( (ucUART_RxBuf[0] !=0xF1 ) && (ucUART_RxBuf[0] !=0xF2)  && (ucUART_RxBuf[0] !=0xF3)  && (ucUART_RxBuf[0] !=0xF6)){
                ucRxAdderss=0;
            }else{
                Inverter_UART_Busy_Flag = 1;
                ucRxAdderss++;
            }
        }
        Tx_Transform_flag = 0;
    }
    

    
}

//---------------------------------     TX              -----------------------------------------
//--------------------------------- Tx base func  ---------------------------

void DEControl(unsigned char state){      //RS485 Control Pin
    
    if(state == 1){
        HAL_GPIO_WritePin(Control_GPIO,  Control_PIN  ,GPIO_PIN_SET);
    }else if(state == 0){
        HAL_GPIO_WritePin(Control_GPIO,  Control_PIN  ,GPIO_PIN_RESET);
    }
}

void UART_TX_Transform(){
    
    DEControl(High);
    __asm("NOP");
    HAL_UART_Transmit(&huart1,ucUART_TxBuf, TXData_Length,10);
    Tx_Transform_flag = 1;
    //HAL_UART_Transmit_IT(&huart1, ucUART_TxBuf, TXData_Length);
    __asm("NOP");
    DEControl(Low);  
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);  //防止自己停下來 不中斷了
   
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