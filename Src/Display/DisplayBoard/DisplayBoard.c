#include "stm32f4xx_hal.h"

I2C_HandleTypeDef hi2c1;

#define I2C_Slave_Address   0x72
unsigned char DataBuffer[8];
HAL_StatusTypeDef I2C_status;

void I2C_RESET_CR2(I2C_HandleTypeDef *hi2c){
   // hi2c->Instance->CR2 = 0;
   hi2c->Instance->CR1 = 0x80;
   HAL_Delay(1);
   hi2c->Instance->CR1 = 0x00;
   hi2c->Instance->SR1 = 0x00;
   hi2c->Instance->SR2 = 0x00;
}

unsigned int Tr_cnt;
unsigned int ERR_cnt;
unsigned char I2C_Error_Flag;

void SET_DisplayBoard_Data(unsigned char L_mode,unsigned char L_dot,unsigned short L_value,unsigned char R_mode,unsigned char R_dot,unsigned short R_value){

    DataBuffer[0] = L_mode;   
    DataBuffer[1] = L_dot;   
    DataBuffer[2] = (unsigned char)L_value;  DataBuffer[3] = (unsigned char)(L_value>>8); 
    
    DataBuffer[4] = R_mode;  
    DataBuffer[5] = R_dot;   
    DataBuffer[6] = (unsigned char)R_value;  DataBuffer[7] = (unsigned char)(R_value>>8); 
    
    if(I2C_Error_Flag == 0){
        I2C_status =  HAL_I2C_Master_Transmit(&hi2c1, I2C_Slave_Address , DataBuffer, 8, 10);
        Tr_cnt++;
    }
    
    if(I2C_status != HAL_OK){
        
        I2C_Error_Flag = 1;
        ERR_cnt++;
        //HAL_Delay(100);
         //Buzzer_BeeBee(50, 2);
        
        HAL_I2C_MspDeInit(&hi2c1);
        HAL_I2C_MspInit(&hi2c1);
        __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_FLAG_STOPF); /* Clear STOP Flag */
        //I2C_RESET_CR2(&hi2c1);                                                   /* Clear Configuration Register 2 */
        hi2c1.State = HAL_I2C_STATE_READY;
        hi2c1.Mode  = HAL_I2C_MODE_NONE;
        __HAL_UNLOCK(&hi2c1);                                                  /* Process Unlocked */
          //HAL_Delay(100);
          I2C_status =  HAL_I2C_Master_Transmit(&hi2c1, I2C_Slave_Address , DataBuffer, 8, 10);
        
        //while(1);
    }else{
        I2C_Error_Flag = 0;
    }
    
}

void DisplayBoard_I2C_Init(){

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    SET_DisplayBoard_Data('X',0,0,'X',0,0);

}