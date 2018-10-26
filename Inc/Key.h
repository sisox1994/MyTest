#ifndef    __KEY_H__
#define    __KEY_H__

typedef enum{

  Inc_15 = 8,
  Inc_10 = 9,
  Inc_5  = 10,
  Inc_0  = 11,
  
  Start  = 42,
  Stop   = 41,
  
  Spd_3  = 4,
  Spd_6  = 3,
  Spd_9  = 2,
  Spd_12 = 1,
  
  Key_Manual  = 14,
  Key_HRC     = 13,
  Key_Advance = 12,

  BLE     = 7,
  Enter     = 6,
  
}Key_Name_Def;
extern unsigned char KeyDelaying_Flag;
extern Key_Name_Def Press_Key;
void Key_GPIO_Init();
void KeySweep();
void KeyCheck();
void KeyTest();

unsigned char KeyForContinueProcess(Key_Name_Def KeyCheck);
unsigned char KeyCatch(unsigned char Sec ,uint8_t Num , ...);

void ClearKey();
void KeyDelay();
#endif