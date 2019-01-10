#include "stm32f4xx_hal.h"

#ifndef    __RM6T6_H__
#define    __RM6T6_H__



#define UART_TxBufDataLength 15
#define UART_RxBufDataLength 20

void RM6T6_Init();
void Inveter_UART_IT_Recive();
uint16_t F_SpeedToHz(uint16_t Speed);
void UART_TX_Transform();


extern const uint16_t InclineADTable[31];
extern void Set_SPEED_Value(unsigned short speedValue);

extern unsigned char  INCL_Moveing_Flag;
extern unsigned char  SPEED_Changing_Flag;
typedef enum{

    NO_Task  = 0,
    
    Set_SPEED   = 1,    
    Set_INCLINE = 2,
    
    Motor_FR    = 3,
    Motor_RR    = 4,
    Motor_STOP  = 5,
    
    Read_SPEED   = 6,
    Read_INCLINE = 7,
        
    Read_Foot_Fall     = 8,
    Read_Foot_Fall_10s = 9    
        
}RM6T6_Task_Def;


typedef enum {
    
    Write = 0,
    Read  = 1
        
}CMD_Type_Def;


typedef enum{
    
    Null = 0x00,
    REQ = 0xF6,
    ANS = 0xF1,
    ACK = 0xF2,
    NAK = 0xF3,
    END = 0xF4
    
}Control_code_Def;



typedef enum{
    

    NO     = 0x00,
    YES    = 0x01,
    Unknow = 0x02,
    
    CW     = 0x03,
    CCW    = 0x04,
    
               
}Status_Item_Def;


typedef struct{
    
    unsigned char  Status_Byte;
    Status_Item_Def      ESP;
    Status_Item_Def      Run;
    Status_Item_Def      FWD;
    Status_Item_Def      STALL;
    Status_Item_Def      READY;  
    Status_Item_Def      IF;
        
}Status_Def;


typedef enum{
    
    No_error        = 0x00,
    Framing_error   = 0x01,
    Overrun_error   = 0x02,
    Format_error    = 0x03,
    CRC_error       = 0x04,
    Data_error      = 0x05,
    Instruction_error   = 0x06,
    Pr_process_error    = 0x11,
    Pr_write_protection = 0x12,
    Pr_data_error       = 0x13   

}Error_code_Def;


typedef struct{

    
  Control_code_Def  Control_code;
  unsigned char    Instruction_code;
  
  Status_Def        Status;
  
  Error_code_Def    Error_Code;
  uint32_t          Data;  
  uint8_t           Parameter_Number;

}Response_Message_Def;


typedef struct{


    
    Status_Def      status;
    
    unsigned short SPEED_ivt;
    unsigned short INCLINE_ivt;
    
    unsigned short   Foot_cnt;
    unsigned short   Foot_cnt_10s;

    

    
}RM6T6_State_Def;

extern RM6T6_State_Def   RM6T6_state;


typedef enum{
    
    FR   = 0x80,
    RR   = 0x40,
    STOP = 0x00,
    E_SP = 0x20,
    RST  = 0x04

}Motor_State_Def;


//--------------------Task Manager-----------------------
void RM6_Task_Adder(RM6T6_Task_Def  RM6_Task);
void RM6_background_Task();

extern Response_Message_Def  Response_Message;


//Cmd List


void F_CMD(CMD_Type_Def cmd_Type , uint16_t  Data);            //No.1
void F_OUT();                                                  //No.2
void V_OUT();                                                  //No.3
void C_OUT();                                                  //No.4
  
void VBUS();                                                   //No.7
void INV_TEMP();                                               //No.8
void INCL_CMD( CMD_Type_Def cmd_Type , uint16_t  Data );       //No.9
void INCL_POS();                                               //No.10
void TROBLE_MSG();                                             //No.11
void OPT_CMD(CMD_Type_Def cmd_Type ,Motor_State_Def motor_State);      //No.12

void C_EFF();                                                  //No.13
void M_INDEX();                                                //No.14
void DB_DUTY();                                                //No.15
void INCL_OPT(unsigned char  cmd_Type, unsigned char Data);  //No.16
void INCL_FORCE(unsigned char  cmd_Type, unsigned char Data);//No.17
void FootFall_10s();                                           //No.18
void FootFall_Total( );                                        //No.19
void SaveToFlash(CMD_Type_Def cmd_Type,unsigned char Data);   //No.20



#endif