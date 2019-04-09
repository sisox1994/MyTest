#ifndef  REALITYKEY_H_
#define REALITYKEY_H_

typedef enum{
    
    Spd_Up   = 1,
    Spd_Down = 2,
    
    Inc_Up   = 3,
    Inc_Down = 4,
    
}KeyName_Def;

void Debounce();
void Reality_Key_Init();
void RealityKey_PressDetect();
unsigned char R_KeyCatch(KeyName_Def KeyName);
unsigned char R_KeyContinueProcess(KeyName_Def KeyName);

#endif