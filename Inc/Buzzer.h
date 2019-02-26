#ifndef __BUZZER_H__
#define __BUZZER_H__



#define Buzzer_GPIO GPIOB
#define Buzzer_Pin  GPIO_PIN_5
#define BuzzerTimeValue  50;

extern unsigned char BuzzerON_Flag;
extern unsigned short BuzzerCnt;

void Buzzer_Init();
void Buzzer_DeInit();
void BuzzerCheck();
void Buzzer_Btn();
void Buzzer_Set(unsigned short setValue);
void Buzzer_BeeBee(unsigned short setValue, unsigned char BeeCnt);
#endif