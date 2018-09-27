#include "stm32f4xx_hal.h"

I2C_HandleTypeDef hi2c1;


#define I2C_Slave_Address   0x72
unsigned char DataBuffer[8];

void SET_DisplayBoard_Data(unsigned char L_mode,unsigned char L_dot,unsigned short L_value,unsigned char R_mode,unsigned char R_dot,unsigned short R_value){

    DataBuffer[0] = L_mode;   
    DataBuffer[1] = L_dot;   
    DataBuffer[2] = (unsigned char)L_value;  DataBuffer[3] = (unsigned char)(L_value>>8); 
    
    DataBuffer[4] = R_mode;  
    DataBuffer[5] = R_dot;   
    DataBuffer[6] = (unsigned char)R_value;  DataBuffer[7] = (unsigned char)(R_value>>8); 
    
    HAL_I2C_Master_Transmit(&hi2c1, I2C_Slave_Address , DataBuffer, 8, 10);
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


