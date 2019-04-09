#include "stm32f4xx_hal.h"
#include "structDef.h"
#include "SystemStick.h"

#if Use_BTSPK 

#define SPK_DET_GPIO  GPIOE
#define SPK_DET_PIN   GPIO_PIN_2

typedef enum{
    SPK_OFF =0,
    SPK_ON =1,
    
}BT_SPK_State_Def;

void BT_SPK_DET_PIN_Init(){

    GPIO_InitTypeDef GPIO_InitStruct;
    //-------  SPK  DET   ---------------------
    GPIO_InitStruct.Pin =  SPK_DET_PIN ;        
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SPK_DET_GPIO, &GPIO_InitStruct);

}

GPIO_PinState SPK_DET;
BT_SPK_State_Def BT_SPK_State;
unsigned char SPK_DET_Hold_High_Cnt;

void BT_SPK_Detect(){

    SPK_DET = HAL_GPIO_ReadPin( SPK_DET_GPIO  , SPK_DET_PIN);

    if(BT_SPK_State == SPK_OFF){
        if( SPK_DET == GPIO_PIN_SET){   //SPK_DET PIN 要持續拉high 2秒才承認連線
            if(T500ms_BTSPK_Det_Flag){
                T500ms_BTSPK_Det_Flag = 0;
                SPK_DET_Hold_High_Cnt++;
                
                if(SPK_DET_Hold_High_Cnt >=4){  //500ms * 4  = 2 sec
                    BT_SPK_State = SPK_ON;
                    BTSPK_Icon_Display_Cnt = 10;
                    SPK_DET_Hold_High_Cnt = 0;
                }
            }       
        }else{
            SPK_DET_Hold_High_Cnt = 0;
        } 
    }
    
    if(BT_SPK_State == SPK_ON){
        if( SPK_DET == GPIO_PIN_RESET){
            BT_SPK_State = SPK_OFF;
            BTSPK_OFF_Icon_Display_Cnt = 10;
        } 
    }

}
#endif