#include "main.h"
#include "stm32f4xx_hal.h"
#include "system.h"

UART_HandleTypeDef huart6;
TIM_HandleTypeDef htim1;

/* TIM1 init function */
static void MX_TIM1_Init(void);
static void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 30;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1549;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_Base_Start_IT(&htim1);
  
}


void SystemClock_Config(void);
void GPIO_CLK_Enable();
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
             

unsigned char My_PWM_Value = 0;

void Other_Init(){
#if Use_FAN
    FAN_PWM_Init();
#endif    
    
#if Use_BTSPK 
    BT_SPK_DET_PIN_Init();
#endif
}

unsigned char RM_Task_Switch = 1;
unsigned char APP_Task_Switch = 1;


void Sleep_Func(){

    
}

void SystemConfigForIntSleepMode(){

    BLE_DeInit();
    RM6T6_DeInit();
    HT1632C_DeInit();
    Key_GPIO_DeInit();
    Buzzer_DeInit();
    Power_5V_OFF();
    
    HAL_TIM_Base_Stop_IT(&htim1);
    

    SysTick->CTRL = 0;
    __HAL_RCC_PWR_CLK_DISABLE();
    

    
    //HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    //HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    
    /* Clear all related wakeup flags */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    
    
    //HAL_PWR_EnterSTANDBYMode(); 
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
    
    
}

int main(void)
{
    
    HAL_Init();
    SystemClock_Config();
    
    GPIO_CLK_Enable();
    SafeKey_Init();
    Flash_Init();
    Power_5V_ON();
  
    MX_TIM1_Init();
    //FlashErase(0);
    
    Flash_Machine_Data_Loading();
    Read_SerialNumber_From_Flash(ucProductionSerialNumber);  //Read  BTM  Serial Number
    
    OTA_Mode_Check();
    BLE_Init();
    Buzzer_Init();
    LED_Matrix_Init();
    DisplayBoard_I2C_Init();
    
    Buzzer_BeeBee(500, 2);
    
  //---------------時序測試----------------------------
  /*Configure GPIO pin Output Level */
    GPIO_InitTypeDef GPIO_InitStruct;
    
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pins :PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  //--------------------------------------------------  
  
    if(OTA_Mode_Flag == 0){   
        
        System_Mode = StartUp; 
        Turn_ON_All_LEDMatrix();
        Turn_ON_All_Segment();  
        Key_GPIO_Init();
        Reality_Key_Init();
        Other_Init();       //風扇跟藍芽喇叭
        RM6T6_Init();
        HR_5K_Init();
        Hand_HR__Init();
        Flash_Custom_Data_Loading();
        Btm_Task_Adder(Feature_FTMS);
        Btm_Task_Adder(Type_FTMS);
        Btm_Task_Adder(Flag_FTMS);
        Btm_Task_Adder(FEC_SET_SN);               //0xB5
        Btm_Task_Adder(FEC_Data_Config_Page_0); // 0xB4 page 0
        Btm_Task_Adder(FEC_Data_Config_Page_1); // 0xB4 page 1
        
        
    }else if(OTA_Mode_Flag == 1){
        //Feed_Dog_Flag = 0;             
        System_Mode = Sys_OTA_Mode;
    }
    
#if FAKE_RM6T6_Mode
    System_Mode = RS485_Test_Mode;
#endif
    

    
    while (1)
    {
  
        if(System_Mode != System_Sleep){
            //------------------------------------------
            #if Use_FAN     
            FAN_SET_PWM_DUTY(My_PWM_Value);
            #endif

            #if Use_BTSPK 
            if(System_Mode != StartUp){
                BT_SPK_Detect();
            }
            #endif   
            
            RealityKey_PressDetect();
            SafeKey_Detect();
            BTM_background_Task(); // 藍芽 任務排程器
            
            if( OTA_Mode_Flag == 0){   //如果是OTA 模式就不做其他背景執行動作
                F_HeartRate_Supervisor(); 
                
                if(RM_Task_Switch == 1){
                    RM6_background_Task(); // 變頻器 任務排程器
                }
                if(APP_Task_Switch == 1){
                    APP_background_Broadcast();   //0x39  每隔5秒  丟系統狀態 
                }
                
            }
        //-------------------------------------------
        }
        
 
        switch(System_Mode){
          case StartUp:
            StartUp_Func();
            break;
          case Idle:
            Idle_Func();
            break;
          case Prog_Set:
            ProgSet_Func();
            break;
          case Ready:
            Ready_Func();
            break;   
          case WarmUp:
            WarmUp_Func();
            break; 
          case CooolDown:
            CoolDown_Func();
            break;  
          case Workout:
            Workout_Func();
            break;  
          case Paused:
            Pause_Func();
            break; 
          case Summary:
            Summary_Func();
            break; 
          case Engineer:
            Engineer_Func();
            break;
          case Safe:
            Safe_Func();
            break;
          case  Sys_OTA_Mode:
            OTA_Mode_Func();
            break;
            
          case System_Sleep:
            Sleep_Func();
            break;
            
#if FAKE_RM6T6_Mode
          case  RS485_Test_Mode:
            RS485_Test_Func();
            break;
#endif
           
        } 
        
        
        
    }

}

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }


  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}




/* USART1 init function */
void MX_USART1_UART_Init(void)
{
/*
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }*/

}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{
/*
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
*/
}


void GPIO_CLK_Enable(){
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
}

void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_11|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE7 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_7|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA4 PA5 PA6 
                           PA11 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_11|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE8 PE9 PE10 PE11 
                           PE12 PE13 PE14 PE15 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11 
                           PD12 PD13 PD14 PD3*/
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC9 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void _Error_Handler(char *file, int line)
{

  while(1)
  {
  }
 
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 

}
#endif /* USE_FULL_ASSERT */

