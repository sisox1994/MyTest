#include "stm32f4xx_hal.h"
#include "system.h"
#include "HT1632_C52.h"

//basic command   

#define HT1632_GPIO  GPIOC
#define HT1632__CS2_GPIO  GPIOA

#define DA_Pin    GPIO_PIN_11
#define WR_Pin    GPIO_PIN_10
#define CS2_Pin   GPIO_PIN_15
#define CS1_Pin   GPIO_PIN_12

void HT1632C_GPIO_Init(){
    
    GPIO_InitTypeDef GPIO_InitStruct;
        
    //PB7 --> DA  
    //PB6 --> WR 
    //PB5 --> CS2
    //PB4 --> CS1   
    
    GPIO_InitStruct.Pin = DA_Pin | WR_Pin | CS1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HT1632_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = CS2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HT1632__CS2_GPIO, &GPIO_InitStruct);
    
}
   
void CS2_Low(){HAL_GPIO_WritePin(HT1632__CS2_GPIO,CS2_Pin,GPIO_PIN_RESET);}
void CS2_High(){HAL_GPIO_WritePin(HT1632__CS2_GPIO,CS2_Pin,GPIO_PIN_SET);}
  
void CS1_Low(){HAL_GPIO_WritePin(HT1632_GPIO,CS1_Pin,GPIO_PIN_RESET);}
void CS1_High(){HAL_GPIO_WritePin(HT1632_GPIO,CS1_Pin,GPIO_PIN_SET); }
    
void DA_Low(){HAL_GPIO_WritePin(HT1632_GPIO,DA_Pin,GPIO_PIN_RESET);}
void DA_High(){HAL_GPIO_WritePin(HT1632_GPIO,DA_Pin,GPIO_PIN_SET);}

void WR_Low(){HAL_GPIO_WritePin(HT1632_GPIO,WR_Pin,GPIO_PIN_RESET);}
void WR_High(){HAL_GPIO_WritePin(HT1632_GPIO,WR_Pin,GPIO_PIN_SET);}

void HT_SendByte(unsigned char cmdbyte,unsigned char sendbits){   
    
    while(sendbits!=0){   
        
        WR_Low();  //WR 
        __asm("nop");     
        
        if(cmdbyte&0x80)  DA_High();    
        else              DA_Low();   
        
        WR_High();      //WR 
        __asm("nop");   
        cmdbyte<<=1;        //        
        sendbits--;     
    }   
}

void SYS_DIS(unsigned  char sw){
    
    if      (sw==0)  CS1_Low();
    else if (sw==1)  CS2_Low();
    
    HT_SendByte(HT_CMD,3);    
    HT_SendByte(HT_SYS_DIS,9);   
    __asm("nop");    
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void SET_PMOS_COM(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);   
    HT_SendByte(HT_COM_8_PMOS,9);   
    __asm("nop");     
    
    if         (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}
void SET_NMOS_COM(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);    
    HT_SendByte(HT_COM_8_NMOS,9);   
    __asm("nop");     
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void SET_RC_Master_Mode(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);   
    HT_SendByte(HT_INT_RC,9);   
    __asm("nop");     
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void SYS_EN(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);   
    HT_SendByte(HT_SYS_EN,9);   
    __asm("nop");     
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void LED_On(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);   
    HT_SendByte(HT_LED_ON,9);   
    __asm("nop");   
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void Blink_On(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);   
    HT_SendByte(HT_BLINK_ON,9);   
    __asm("nop");   
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void Blink_Off(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);   
    HT_SendByte(HT_BLINK_OFF,9);   
    __asm("nop");
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void SET_PWM(unsigned  char sw){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low();
    
    HT_SendByte(HT_CMD,3);   
    HT_SendByte(HT_PWM_CONTROL_7,9);   //  HT_PWM_CONTROL_5
    __asm("nop");
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void HT_Write(unsigned char Address ,unsigned char Data,unsigned char sw ){
    
    if      (sw==0) CS1_Low();
    else if (sw==1) CS2_Low(); 
    
    HT_SendByte(HT_WR,3);   
    
    HT_SendByte(Address,7);  
    HT_SendByte(Data,8);       
    __asm("nop");    
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    
}

void HT1632_Init(unsigned char sw ){
    
    if      (sw==0) CS1_High();
    else if (sw==1) CS2_High();
    WR_High();
    DA_High();
    SYS_DIS(sw);
    SET_PMOS_COM(sw);
    //SET_NMOS_COM();
    SET_PWM(sw);
    SET_RC_Master_Mode(sw);
    SYS_EN(sw);
    LED_On(sw);
    Blink_Off(sw);
    
}

void LED_Matrix_Init(){
    
    HT1632C_GPIO_Init();
    
    HT1632_Init(0);
    HT1632_Init(1);
    
}


