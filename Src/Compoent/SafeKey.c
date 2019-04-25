#include "stm32f4xx_hal.h"
#include "system.h"

#define SAFETY_PIN    GPIO_PIN_5
#define SAFETY_GPIO   GPIOD

#define PAUSE_KEY_PIN    GPIO_PIN_4
#define PAUSE_KEY_GPIO   GPIOD

GPIO_PinState Pause_State;
void SafeKey_Init(){
 
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*PD4  -->    PAUSE_KEY  IN  */ 
    GPIO_InitStruct.Pin = PAUSE_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     
    GPIO_InitStruct.Pull = GPIO_NOPULL ;         
    HAL_GPIO_Init(PAUSE_KEY_GPIO,&GPIO_InitStruct);
    
    /*PD5  -->   Safe Key  IN  */ 
    GPIO_InitStruct.Pin = SAFETY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     
    GPIO_InitStruct.Pull = GPIO_NOPULL ;         
    HAL_GPIO_Init(SAFETY_GPIO,&GPIO_InitStruct); 
}
unsigned char In_SafeMode_Flag;
void SafeKey_Detect(){
    
    
    if(In_SafeMode_Flag == 0){
        if(safekey == Plug_Out){
            In_SafeMode_Flag = 1;
            
            console_status = 3;      // 1 : stop  2¡Gpause  3: stop by safeKey   4: start 
            Btm_Task_Adder(FEC_Data_Config_Page_1);
        }
    }
    

    
    Pause_State =  HAL_GPIO_ReadPin(PAUSE_KEY_GPIO,PAUSE_KEY_PIN);
    
    if((HAL_GPIO_ReadPin(SAFETY_GPIO,SAFETY_PIN) == 1)  ){    //|| (Pause_State == GPIO_PIN_RESET)
        safekey = Plug_Out;
#if Use_SafeKey
        if( (System_Mode == CooolDown) || (System_Mode == Workout) || (System_Mode == WarmUp) ){
            Machine_Data_Update();
            RM6T6_Task_ClearAll();
        }   
        System_Mode = Safe;
#endif
    }else{
        In_SafeMode_Flag = 0;
        safekey = Plug_IN;
    }
}
unsigned char Pause_press_Flag;
unsigned char PauseKey(){

    Pause_State =  HAL_GPIO_ReadPin(PAUSE_KEY_GPIO,PAUSE_KEY_PIN);
    
    if(T100ms_PauseKey_Flag){
       
        if(Pause_press_Flag == 0){
            if(Pause_State == GPIO_PIN_RESET){
                BuzzerON_Flag = 0;
                
                Pause_press_Flag = 1;
                if(System_Mode!= Workout){
                    Buzzer_Btn();
                }
                
                return 1;
            }
        }
    }
   
    if(Pause_press_Flag == 1){
        if(Pause_State == GPIO_PIN_SET){
            T100ms_PauseKey_Flag = 0;
            Pause_press_Flag = 0; 
        }
    }
    return 0;
}

void Safe_Func(){
    
    MarqueeCnt = 55;
    
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0; 
        //F_String_buffer_Auto( Left, "PLUG      IN      SAFETY" ,50 ,0);
        F_String_buffer_Auto( Left, "EMERGENCY       STOP" ,55 ,0);
        writeLEDMatrix();
        Turn_OFF_All_Segment();
        SET_DisplayBoard_Data('X',0,0,'X',0,0);
    }
    
    if(safekey == Plug_IN){
        Turn_OFF_All_LEDMatrix();
        HAL_NVIC_SystemReset();
    }
}