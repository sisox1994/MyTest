/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "system.h"

/* External variables --------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{

  while (1)
  {
   
  }
 
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  while (1)
  {
    
  }
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  time();
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}

void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi2);
}

void USART1_IRQHandler(void)
{
    
    HAL_UART_IRQHandler(&huart1);
    Inveter_UART_IT_Recive();
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
  Btm_Recive();
}

void USART6_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart6);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
