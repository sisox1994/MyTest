#include "stm32f0xx_hal.h"

unsigned char flag = 0;

void TimePeriodTest(){
    
    if(flag){
        flag = 0;
        HAL_GPIO_WritePin( GPIOA , GPIO_PIN_7 , GPIO_PIN_RESET);
    }else if(!flag){
        flag = 1;
        HAL_GPIO_WritePin( GPIOA , GPIO_PIN_7 , GPIO_PIN_SET);
    }
    
}