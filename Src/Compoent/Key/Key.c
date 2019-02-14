#include "stm32f4xx_hal.h"
#include "system.h"
//--------------------------------KD --Input----------------
#define  KeyDetect_GPIO  GPIOD
uint16_t       Detect_Pin_Array[7] = {GPIO_PIN_14 , GPIO_PIN_13 ,GPIO_PIN_12 , GPIO_PIN_11 ,GPIO_PIN_10 ,GPIO_PIN_9 ,GPIO_PIN_8};

//--------------------------------Key  ---Output--------------
uint16_t       Key_Pin_Array[7] = {GPIO_PIN_15 , GPIO_PIN_14 ,GPIO_PIN_13 , GPIO_PIN_12 ,GPIO_PIN_11 ,GPIO_PIN_10 ,GPIO_PIN_9};
GPIO_TypeDef*  Key_GPIO_Array[7] = {GPIOE , GPIOE ,GPIOE , GPIOE ,GPIOE ,GPIOE ,GPIOE};
//------------------------------------------------------------

unsigned short KDC_Value = 150; //按鍵 除彈跳 緩衝時間

void Key_GPIO_Init(){
    //------------------        OUTPUT    -----------------------------------   
    //Key0 ~ Key6  --:     PE15 - PE14 - PE13 - PE12 - PE11 - PE10 - PE9
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13 | GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    //-----------------       INPUT     ---------------------------------------
    //KD0 ~ KD6  --:       PD14 - PD13 - PD12 - PD11 - PD10 - PD9 - PD8   
    GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_13 | GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_8;        
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    
}
//-------------------------------------
unsigned char KeyProcessingFlag;
unsigned char KeyPressingFlag;
unsigned char KeyDelaying_Flag;
unsigned short KeyDelaying_Cnt;
Key_Name_Def Press_Key;

uint8_t KeyNumber;
uint8_t KeyNumber_Temp;
uint8_t KeyStableCnt;
//-------------------------------------
uint32_t SweepCnt = 0;
void KeySweep(){

    if(T5ms_Flag){
        T5ms_Flag = 0;
            HAL_GPIO_WritePin( Key_GPIO_Array[SweepCnt]  , Key_Pin_Array[SweepCnt] , GPIO_PIN_RESET); 
            for(unsigned char j = 0; j < 7; j++ ){
                if(j != SweepCnt){
                    HAL_GPIO_WritePin( Key_GPIO_Array[j]  , Key_Pin_Array[j] , GPIO_PIN_SET); 
                }
            }
            //-------------------------------------------------- 
            for(unsigned char k = 0; k < 7 ; k++){
                if( HAL_GPIO_ReadPin( KeyDetect_GPIO , Detect_Pin_Array[k]) == 0 ){
                    KeyNumber = (SweepCnt*7) + (k+1);
                    Press_Key = (Key_Name_Def)KeyNumber;
                    KeyPressingFlag = 1;
                    __asm("NOP");
                }
            }   
            //-------------------------------------------------------------------
            SweepCnt++;
            SweepCnt = (SweepCnt % 7);
    }
}

unsigned char TestCnt;
unsigned char KeyHolding_Flag;
unsigned char KeyHolding_Cnt;

Key_Name_Def KeyPressDetect_Array[10];
Key_Name_Def KeyPressRelease_Array[10];
Key_Name_Def KeyPressCheck_Array[10];
Key_Name_Def KeyPressAddCheck_Array[10];

unsigned char KeyPressAddArray_Cnt;
unsigned char KeyConfirm_Cnt = 0;
unsigned char KeyConfirm_DoubleCnt = 0;
unsigned char KeyPressArray_Cnt = 0;

unsigned char KeyProcessFlag;
unsigned char KeyReleaseCheck();
unsigned char KeyHoldTime_Sec;

Key_Name_Def KeyCheck_Save;
Key_Name_Def Key_Detect;

unsigned char HoldingCnt;

//-----按住---------------------------
unsigned char KeyForContinueProcess(Key_Name_Def KeyCheck){
    
    Key_Detect= (Key_Name_Def)(0x00); 
    KeyCheck_Save = KeyCheck;
    
    if(KeyPressRelease_Array[0] == KeyCheck_Save){
        
        for(unsigned char i = 0 ; i < 7; i++){
             
            for(unsigned char j = 0; j < 7; j++ ){
                if(j != i){
                    HAL_GPIO_WritePin( Key_GPIO_Array[j]  , Key_Pin_Array[j] , GPIO_PIN_SET); 
                }
            }
            //------------------------------------------------------------------------
            for(int i = 0;i<50;i++){
                __asm("NOP");
            }
            HAL_GPIO_WritePin( Key_GPIO_Array[i]  , Key_Pin_Array[i] , GPIO_PIN_RESET);
            
            for(int i = 0;i<50;i++){
                __asm("NOP");
            }
            
            //--------------------------------------------------------------------
            for(unsigned char k = 0; k < 7 ; k++){
                if( HAL_GPIO_ReadPin( KeyDetect_GPIO , Detect_Pin_Array[k]) == 0 ){
                    Key_Detect = (Key_Name_Def)((i*7) + (k+1)) ;  
                }
            }             
        }
        if( T100ms_KeyHold_Flag){
            T100ms_KeyHold_Flag = 0;
            if(Key_Detect == KeyCheck_Save){
                HoldingCnt++;
                if(HoldingCnt > 8){
                    return 1;
                }    
            }
        }
    }else{
        __asm("NOP");    
    }
    return 0;            
}


unsigned char KeyCatch(unsigned char Sec ,uint8_t Num, ...){
    
    memset(KeyPressCheck_Array,0x00,10);
    //-------------------------------------------------------------------
    va_list Key_Name_List_ptr;
    va_start(Key_Name_List_ptr ,Num);
    
    for(unsigned char i = 0; i < Num ; i++){
        KeyPressCheck_Array[i] = (Key_Name_Def)va_arg(Key_Name_List_ptr,int);
    }
    
    va_end(Key_Name_List_ptr);
    //------------------------------------------------------------------------
    if(KeyDelaying_Flag == 1){
        if(Sec == 0){  //單純按下放開       普通點擊按鍵  
            //----------------------確認按鍵放開 ---------------------------------
            if(KeyProcessFlag == 1){  
                KeyConfirm_DoubleCnt = 0;
                for(unsigned char i = 0; i < Num ; i++ ){
                    for(unsigned char j =0 ;j < 10 ;j++){
                        if( KeyPressRelease_Array[j] == KeyPressCheck_Array[i] ){ //KeyPressAddCheck_Array
                            KeyConfirm_DoubleCnt++; 
                        }
                    }
                }
                if(KeyConfirm_DoubleCnt == Num){
                    memset(KeyPressRelease_Array, 0x00,10);
                    KeyDelaying_Flag = 0;
                    KeyProcessFlag = 0;
                    HoldingCnt = 0;
                    ret_Idle_cnt = 0;
                    return 1;
                }
            }
            //-----------------------------------------------------------------
        }else if( (Sec > 0) && (Sec <= 15) ){   //判斷  按鈕 hold住 1秒~15秒
            
            if(KeyHoldTime_Sec == Sec){
                if(KeyProcessFlag == 1){
                    KeyConfirm_DoubleCnt = 0;
                    for(unsigned char i = 0; i < Num ; i++ ){
                        for(unsigned char j =0 ;j < 10 ;j++){
                            if( KeyPressRelease_Array[j] == KeyPressCheck_Array[i] ){
                                KeyConfirm_DoubleCnt++; 
                            }
                        }
                    }
                    if(KeyConfirm_DoubleCnt == Num){
                        Buzzer_ON();
                        memset(KeyPressRelease_Array, 0x00,10);
                        KeyDelaying_Flag = 0;
                        KeyProcessFlag = 0;
                        ret_Idle_cnt = 0;
                        return 1;
                    }
                }
            }
            
            if(T1s_Key_Hold_Flag){
                T1s_Key_Hold_Flag = 0;
                if(KeyHoldTime_Sec != Sec){
                    KeyHoldTime_Sec++;
                }
            } 
        }
        
    }else{
        
       if(Sec>0)KeyHoldTime_Sec = 0;
        KeyPressArray_Cnt = 0;
        memset(KeyPressDetect_Array,0x00,10);   //清掉前一次 要偵測的按鍵列表
        KeyConfirm_Cnt = 0;
        //--------------------------按鍵掃描----------------------------------
        Press_Key = (Key_Name_Def)0x00;
        
        for(unsigned char i = 0 ; i < 7; i++){
            
            for(unsigned char j = 0; j < 7; j++ ){
                if(j != i){
                    HAL_GPIO_WritePin( Key_GPIO_Array[j]  , Key_Pin_Array[j] , GPIO_PIN_SET); 
                }
            }
            
            //----------------------------------------------------------------
            for(int i = 0;i<50;i++){
                __asm("NOP");
            }
            HAL_GPIO_WritePin( Key_GPIO_Array[i]  , Key_Pin_Array[i] , GPIO_PIN_RESET); 
            for(int i = 0;i<50;i++){
                __asm("NOP");
            }
            //---------------------------------------------------------------- 
            
            //------------------紀錄當下按了那些鍵 並存到--KeyPressDetect_Array-----------------------
            
            for(unsigned char k = 0; k < 7 ; k++){
                
                if( HAL_GPIO_ReadPin( KeyDetect_GPIO , Detect_Pin_Array[k]) == 0 ){
                    KeyNumber = (i*7) + (k+1);
                    Press_Key = (Key_Name_Def)KeyNumber;
                    KeyPressDetect_Array[KeyPressArray_Cnt] = Press_Key;
                    KeyPressArray_Cnt++;   
                }
            }   
            
        }
      //--------------------確認當下按的  和  欲偵測的是否相同--------------------------
        for(unsigned char i = 0; i < Num ; i++ ){
            for(unsigned char j =0 ;j < 10 ;j++){
                if( KeyPressDetect_Array[j] == KeyPressCheck_Array[i] ){
                    KeyConfirm_Cnt++; 
                }
            }
        }
        
        if(KeyConfirm_Cnt == Num){    //確認正確
            
            if(ContuineBeepFlag!=1){   //防止蜂鳴器錯誤動作
                if(KeyConfirm_Cnt==1){
                    
                    if( (System_Mode== Workout) && (Press_Key == Stop) ){
                      //---峰鳴器不要叫
                    }else{
                      Buzzer_ON();
                    }
                }
            }
    
            memcpy(KeyPressRelease_Array, KeyPressCheck_Array,10);
            KeyDelaying_Flag = 1;      //除彈跳
            KeyDelaying_Cnt  = KDC_Value;
            __asm("NOP");
        }
    }
    __asm("NOP");
    
    //--------------------------------------------------
    return 0;
}

void KeyOpenCheck(){
    
    unsigned char KeyStillHold = 0;
    KeyPressAddArray_Cnt = 0;
    
    for(unsigned char i = 0 ; i < 7; i++){
        
         
        for(unsigned char j = 0; j < 7; j++ ){
            if(j != i){
                HAL_GPIO_WritePin( Key_GPIO_Array[j]  , Key_Pin_Array[j] , GPIO_PIN_SET); 
            }
        }
       //--------------------------------------------------
        for(int i = 0;i<50;i++){
            __asm("NOP");
        }
        HAL_GPIO_WritePin( Key_GPIO_Array[i]  , Key_Pin_Array[i] , GPIO_PIN_RESET);
        for(int i = 0;i<50;i++){
            __asm("NOP");
        }
        //--------------------------------------------------
        for(unsigned char k = 0; k < 7 ; k++){
            if( HAL_GPIO_ReadPin( KeyDetect_GPIO , Detect_Pin_Array[k]) == 0 ){

                KeyPressAddCheck_Array[KeyPressAddArray_Cnt] = (Key_Name_Def)((i*7) + (k+1));
                KeyPressAddArray_Cnt++;
                KeyStillHold = 1;
                //break;
            }
        }   
    }

    if(KeyStillHold == 1){
        KeyDelaying_Cnt = KDC_Value;
        memcpy(KeyPressRelease_Array,KeyPressAddCheck_Array,10);
        
    }else{
        memset(KeyPressAddCheck_Array,0x00,10);
        KeyProcessFlag = 1; 
    }
}

void KeyDelay(){

    if(KeyDelaying_Flag == 1){
        
        if(KeyDelaying_Cnt == 0){
            KeyOpenCheck();
        }else{
            KeyDelaying_Cnt--;
        }  
    } 
}