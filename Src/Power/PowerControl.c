#include "stm32f4xx_hal.h"

#define EUP_POWER_GPIO     GPIOA
#define EUP_POWER_PIN      GPIO_PIN_8

void Power_5V_ON(){

    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Configure GPIO pins :  PA8  EUP_POWER  */
    GPIO_InitStruct.Pin   = EUP_POWER_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(EUP_POWER_GPIO, &GPIO_InitStruct);
    HAL_GPIO_WritePin(EUP_POWER_GPIO, EUP_POWER_PIN , GPIO_PIN_SET);
    
}

void Power_5V_OFF(){
    
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    HAL_GPIO_WritePin(EUP_POWER_GPIO, EUP_POWER_PIN , GPIO_PIN_RESET); 
    
    /* Configure GPIO pins :  PA8  EUP_POWER  */
    GPIO_InitStruct.Pin   = EUP_POWER_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    HAL_GPIO_Init(EUP_POWER_GPIO, &GPIO_InitStruct);
    //HAL_GPIO_WritePin(EUP_POWER_GPIO, EUP_POWER_PIN , GPIO_PIN_SET);
    
    
}