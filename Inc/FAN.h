#ifndef FAN_H_
#define FAN_H_

extern TIM_HandleTypeDef htim2;


void FAN_PWM_Init();

void FAN_SET_PWM_DUTY(unsigned char duty);

#endif