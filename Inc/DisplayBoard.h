#ifndef DISPLAYBOARD_H_
#define DISPLAYBOARD_H_

extern I2C_HandleTypeDef hi2c1;

void DisplayBoard_I2C_Init();
void SET_DisplayBoard_Data(unsigned char L_mode,unsigned char L_dot,unsigned short L_value,unsigned char R_mode,unsigned char R_dot,unsigned short R_value);

#endif
