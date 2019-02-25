#ifndef    __KEY_H__
#define    __KEY_H__

typedef enum{

  
    Inc_15  = 44,
    Inc_12  = 43,
    Inc_9  = 42,
    Inc_6   = 41,
    Inc_3  = 40,
    Inc_0 =  39,
    
    Spd_3 = 1,
    Spd_6 = 2,
    Spd_9 = 3,
    Spd_12 = 4,
    Spd_15 = 8,
    Spd_18 = 9,
    
    Num_0 = 11,
    Num_1 = 19,
    Num_2 = 20,
    Num_3 = 21,
    Num_4 = 16,
    Num_5 = 17,
    Num_6 = 18,
    Num_7 = 13,
    Num_8 = 14,
    Num_9 = 15,
    
    Key_Goal      = 32,
    Key_FitTest   = 33,
    Key_Custom    = 34,
    Key_Manual    = 35,
    Key_HRC       = 36,
    Key_Advance   = 37,
    
    Key_Dist      = 28,
    Key_Cal       = 27,
    Key_Time      = 26,
    Key_Pace      = 25,
    Key_Met       = 24,
    
    Key_SpdUp  = 23,
    Key_SpdDwn = 22, //22  29
    
    Key_IncUp  = 30,
    Key_IncDwn = 31, //22  29
    
    
    ANT     = 29,//45
    BLE     = 38,
    Enter   = 12,
    Cancel  = 10,
    
    Stop    = 7,
    Start   = 6,
    cool    = 5,
  
  
}Key_Name_Def;
extern unsigned char KeyDelaying_Flag;
extern Key_Name_Def Press_Key;
void Key_GPIO_Init();
void Key_GPIO_DeInit();
void KeySweep();
void KeyCheck();
void KeyTest();

unsigned char KeyForContinueProcess(Key_Name_Def KeyCheck);
unsigned char KeyCatch(unsigned char Sec ,uint8_t Num , ...);

void ClearKey();
void KeyDelay();
#endif