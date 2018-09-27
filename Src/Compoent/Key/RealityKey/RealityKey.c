#include "stm32f4xx_hal.h"
#include "Buzzer.h"
#include "RealityKey.h"
#include "structDef.h"
#include "SystemStick.h"

GPIO_PinState   PB_1_spdUP;
GPIO_PinState   PB_2_spdDown;

GPIO_PinState   PE_7_incUP;
GPIO_PinState   PE_8_inc_Down;
unsigned short Debounce_Time;
unsigned short Debounce_Time_ADJ = 50;  //除彈跳時間
unsigned char Holding_Cnt;    //偵測按住多久用的Counter

void Reality_Key_Init(){

    GPIO_InitTypeDef GPIO_InitStruct;

    //---   SPD+   SPD-    --------------------------------
    GPIO_InitStruct.Pin =  GPIO_PIN_1 | GPIO_PIN_2 ;        
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    //---   INC+   INC-    --------------------------
    GPIO_InitStruct.Pin =  GPIO_PIN_7 | GPIO_PIN_8 ;        
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}




KeyName_Def  Catch_Key_Name;    //第一次按下 捕捉到的按鍵名稱    因為是偵測用 馬上會被清掉
KeyName_Def  Catch_Key_Save;      //按下去之後 直到放開前   (記錄"目前"按住) 按鈕名稱

void Key_Release_Check(){   //Debounce_time == 1 時  執行
    
    
    //只讀取當初按下的按鈕放開了沒
    if(Catch_Key_Save == Spd_Up){
        PB_1_spdUP = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_1);
    }else if(Catch_Key_Save == Spd_Down){
        PB_2_spdDown = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_2);
    }else if(Catch_Key_Save == Inc_Up){
        PE_7_incUP = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_7);
    }else if(Catch_Key_Save == Inc_Down){
        PE_8_inc_Down = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_8);
    }
     
    if(PB_1_spdUP | PB_2_spdDown | PE_7_incUP | PE_8_inc_Down){  //如果還按著  重置除彈跳時間
          Debounce_Time = Debounce_Time_ADJ;
          __asm("NOP");
    }else{
        Debounce_Time = 0;                 
        Holding_Cnt = 0;      //按鈕放開重置按住時間
        Catch_Key_Name = (KeyName_Def)0;   
        Catch_Key_Save = (KeyName_Def)0;    //按鈕放開  清掉暫存
    }
    
}


unsigned char R_KeyCatch(KeyName_Def KeyName){  //普通按一下  

    if(Catch_Key_Name == KeyName){
        Catch_Key_Name = (KeyName_Def)0;
        return 1;
    }
    return 0;
}

//按住超過 0.8秒  每隔0.1秒  return 1
unsigned char R_KeyContinueProcess(KeyName_Def KeyName){  
    if(Catch_Key_Save == KeyName){
        if( T100ms_R_KeyHold_Flag){
            T100ms_R_KeyHold_Flag = 0;
            
            Holding_Cnt++;
            if(Holding_Cnt>8){   
                return 1;
            }
        }
    }else{
        __asm("NOP");
    }
    return 0;
}


//Step 1     在其他按鍵沒有在動作的時候 去讀取按鍵GPIO
void RealityKey_PressDetect(){
    
    if(Debounce_Time == 0){
        PB_1_spdUP        = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_1);
        PB_2_spdDown  = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_2);
        PE_7_incUP          = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_7);
        PE_8_inc_Down  = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_8);
        
        if(PB_1_spdUP | PB_2_spdDown | PE_7_incUP | PE_8_inc_Down){
            
            Buzzer_ON();        //4個按鍵有一個按鍵被觸發 ,蜂鳴器就先B一聲
            
            //紀錄 按下的 按鍵 名稱  儲存到   Catch_Key_Name  
            if(PB_1_spdUP == GPIO_PIN_SET){
                Catch_Key_Name = Spd_Up;    
            }else if(PB_2_spdDown ==GPIO_PIN_SET ){
                Catch_Key_Name = Spd_Down;
            }else if(PE_7_incUP ==GPIO_PIN_SET ){
                Catch_Key_Name = Inc_Up;
            }else if(PE_8_inc_Down ==GPIO_PIN_SET ){
                Catch_Key_Name = Inc_Down;
            }  
            
            Catch_Key_Save = Catch_Key_Name;   //先把目前按的按鍵存起來 
            
            //清掉 GPIO狀態      當有按鍵按下時   Debounce_Time ! =  0 不會再去確認HAL_GPIO_ReadPin  所以要先清掉狀態
            PB_1_spdUP       = GPIO_PIN_RESET; 
            PB_2_spdDown = GPIO_PIN_RESET;
            PE_7_incUP         = GPIO_PIN_RESET;  
            PE_8_inc_Down = GPIO_PIN_RESET;
            
            //設定除彈跳時間
            Debounce_Time = Debounce_Time_ADJ;
            __asm("NOP");
        }
        
    }
      
}

void Debounce(){
    
    if(Debounce_Time == 1){
        Key_Release_Check();        //除彈跳 cnt 減到 ==1 時 去確認按鍵有沒有放開
    }else{
        if(Debounce_Time>1){
            Debounce_Time--;
        }
       
    }
}