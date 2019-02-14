#include "stm32f4xx_hal.h"
#include "system.h"
#if Use_FAN

TIM_HandleTypeDef htim2;
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
     
void FAN_PWM_Init(){
    

    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;
    
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    HAL_TIM_MspPostInit(&htim2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

}

void FAN_SET_PWM_DUTY(unsigned char duty){
    

    unsigned int PWM_Value;
    
    if(duty>100){
        duty = 100;
    }
    
    PWM_Value =  duty * 10;
    
    TIM2->CCR3 = PWM_Value;

}
#endif