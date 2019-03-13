#include "stm32f4xx_hal.h"
#include "system.h"
#include "Buzzer.h"

//------------------------------Debug專用 只能從Live Watch關掉
unsigned char Debug_BuzzerOFf;

unsigned short BuzzerCnt;
unsigned char BuzzerON_Flag;

unsigned char ContuineBeepFlag;  //連續 bb叫
unsigned short Buzzer_BB_Period;

unsigned char Buzzer_Interval_Time;  //讓buzzer 響完有緩衝時間


void SetBuzzer_Interval(){
    Buzzer_Interval_Time = 100;
}

void Buzzer_Init();
void BuzzerCheck();
void Buzzer_Btn();

//------------------按鍵回饋音
void Buzzer_Btn(){
    
    
    if(Buzzer_Interval_Time == 0){
    //----------------------------------------------------  
        ContuineBeepFlag = 0;
        if(BuzzerON_Flag == 0){
            
#if Use_Buzzer
if(Debug_BuzzerOFf == 0){
            BuzzerON_Flag = 1;
            
            if(Press_Key == Start){
                if( (System_Mode == Idle) || (System_Mode == Prog_Set)){
                    BuzzerCnt = 0;
                    BuzzerON_Flag = 0;
                }else{
                    BuzzerCnt = BuzzerTimeValue;
                    HAL_GPIO_WritePin(Buzzer_GPIO, Buzzer_Pin, GPIO_PIN_SET);
                }
            }else{
                
                BuzzerCnt = BuzzerTimeValue; 
                HAL_GPIO_WritePin(Buzzer_GPIO, Buzzer_Pin, GPIO_PIN_SET);
                
            }
            __asm("NOP");
}
#endif
        } 
       
    //------------------------------------------    
    }
    

    
}

void Buzzer_Set(unsigned short setValue){
    
    if(BuzzerON_Flag == 0){
        
#if Use_Buzzer
if(Debug_BuzzerOFf == 0){
    
        BuzzerON_Flag = 1;
        HAL_GPIO_WritePin(Buzzer_GPIO, Buzzer_Pin, GPIO_PIN_SET);
        
        if(BuzzerCnt > 0){
            BuzzerCnt = setValue - BuzzerCnt;
        }else{
            BuzzerCnt = setValue;  
        }
         __asm("NOP");
}
#endif
    }
}

void Buzzer_BeeBee(unsigned short setValue, unsigned char BeeCnt){
    
    if(BuzzerON_Flag == 0){
       
#if Use_Buzzer 
if(Debug_BuzzerOFf == 0){
    
        ContuineBeepFlag = 1;
        Buzzer_BB_Period  = setValue / (BeeCnt * 2);
        BuzzerON_Flag = 1;
        HAL_GPIO_WritePin(Buzzer_GPIO, Buzzer_Pin, GPIO_PIN_SET);
        BuzzerCnt = setValue;
        __asm("NOP");
}
#endif
    }
    
}
void Buzzer_OFF(){
    HAL_GPIO_WritePin(Buzzer_GPIO, Buzzer_Pin, GPIO_PIN_RESET);
    BuzzerON_Flag = 0;
}

//----------- BuzzerCheck ----放在 SystemStick 裡面 偵測要叫多久-------------------------
void BuzzerCheck(){
    
    if(BuzzerON_Flag == 1){  
        if(ContuineBeepFlag ==0){
            if(BuzzerCnt !=0){
                BuzzerCnt--;
            }else{
                Buzzer_OFF();                
                SetBuzzer_Interval();
            }   
        }else if(ContuineBeepFlag == 1 ){
            if(BuzzerCnt !=0){
                BuzzerCnt--;
                if(BuzzerCnt % Buzzer_BB_Period == 0){
                    
                    if(HAL_GPIO_ReadPin(Buzzer_GPIO, Buzzer_Pin) == 1){
                        HAL_GPIO_WritePin(Buzzer_GPIO, Buzzer_Pin, GPIO_PIN_RESET);
                    }else if(HAL_GPIO_ReadPin(Buzzer_GPIO, Buzzer_Pin) == 0){
                        HAL_GPIO_WritePin(Buzzer_GPIO, Buzzer_Pin, GPIO_PIN_SET);
                    }  
                }
            }else if(BuzzerCnt == 0){
                Buzzer_OFF();
                ContuineBeepFlag = 0;
                SetBuzzer_Interval();                
            }  
        }
    }
    
    if(Buzzer_Interval_Time>0){
        Buzzer_Interval_Time--;
    }
    
    
}
void Buzzer_Init(){
    
    GPIO_InitTypeDef GPIO_InitStruct;
    //-------------  PB5 ----  Buzzer  ---------------------- 
    GPIO_InitStruct.Pin = Buzzer_Pin;             
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(Buzzer_GPIO, &GPIO_InitStruct);
}

void Buzzer_DeInit(){
    
    GPIO_InitTypeDef GPIO_InitStruct;
    //-------------  PB5 ----  Buzzer  ---------------------- 
    GPIO_InitStruct.Pin = Buzzer_Pin;             
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(Buzzer_GPIO, &GPIO_InitStruct);
}