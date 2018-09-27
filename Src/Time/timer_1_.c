#include "stm32f0xx_hal.h"
#include "system.h"


TIM_HandleTypeDef htim1;




void timer_1(){
    
    
    
}




void Timer_1_Init(){
    
    //Timer1 Use APB CLK = 32Mhz
    TIM_ClockConfigTypeDef sClockSourceConfig;
    
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = (32000-1);            // (32Mhz / 32000) = 1KHz   base on 1ms    value range: 0~65535
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = (1000-1);                // 1000 * 1ms = 1s                         value range: 0~65535
    
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);      // 中斷啟動 important   沒開不會中斷
    HAL_TIM_Base_Start_IT(&htim1);                     //啟動 timer1 中斷
    
}



