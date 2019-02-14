#ifndef __BUZZER_H__
#define __BUZZER_H__



#define Buzzer_GPIO GPIOB
#define Buzzer_Pin  GPIO_PIN_5
#define BuzzerTimeValue  50;

extern unsigned char BuzzerON_Flag;

void Buzzer_Init();
void BuzzerCheck();
void Buzzer_ON();
void Buzzer_Set(unsigned short setValue);
void Buzzer_BeeBee(unsigned short setValue, unsigned char BeeCnt);
#endif