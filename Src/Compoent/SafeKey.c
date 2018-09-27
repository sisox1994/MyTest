#include "stm32f4xx_hal.h"
#include "system.h"

/*
 PD4 --> PAUSE KEY
 PD5 --> SAFETY
*/

#define SAFETY_PIN    GPIO_PIN_5
#define SAFETY_GPIO   GPIOD

#define PAUSE_KEY_PIN    GPIO_PIN_4
#define PAUSE_KEY_GPIO   GPIOD

void SafeKey_Init();
void SafeKey_Detect();

#define    RealSafeKey   0

void SafeKey_Init(){
 
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*PD4  -->    PAUSE_KEY  IN  */ 
    GPIO_InitStruct.Pin = PAUSE_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     
    GPIO_InitStruct.Pull = GPIO_NOPULL ;         
    HAL_GPIO_Init(PAUSE_KEY_GPIO,&GPIO_InitStruct);
    
    /*PD5  -->   Safe Key  IN  */ 
    GPIO_InitStruct.Pin = SAFETY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     
    GPIO_InitStruct.Pull = GPIO_NOPULL ;         
    HAL_GPIO_Init(SAFETY_GPIO,&GPIO_InitStruct); 
}

void SafeKey_Detect(){
    
    if(HAL_GPIO_ReadPin(SAFETY_GPIO,SAFETY_PIN) == 1){ 
        safekey = Plug_Out;
#if RealSafeKey
        
        if( (System_Mode == CooolDown) || (System_Mode == Workout) || (System_Mode == WarmUp) ){
            Machine_Data_Update();
        }   
        System_Mode = Safe;
#endif
    }else{
        safekey = Plug_IN;
    }
}

void Safe_Func(){
    
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0; 
        
        F_String_buffer_Auto( Left, "PLUG      IN      SAFETY" ,50 ,0);
        
        writeLEDMatrix();
        Turn_OFF_All_Segment();
    }
    
    if(safekey == Plug_IN){
        Turn_OFF_All_LEDMatrix();
        HAL_NVIC_SystemReset();
    }
    
}