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
unsigned short Debounce_Time_ADJ = 50;  //���u���ɶ�
unsigned char Holding_Cnt;    //��������h�[�Ϊ�Counter

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




KeyName_Def  Catch_Key_Name;    //�Ĥ@�����U �����쪺����W��    �]���O������ ���W�|�Q�M��
KeyName_Def  Catch_Key_Save;      //���U�h���� �����}�e   (�O��"�ثe"����) ���s�W��

void Key_Release_Check(){   //Debounce_time == 1 ��  ����
    
    
    //�uŪ�������U�����s��}�F�S
    if(Catch_Key_Save == Spd_Up){
        PB_1_spdUP = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_1);
    }else if(Catch_Key_Save == Spd_Down){
        PB_2_spdDown = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_2);
    }else if(Catch_Key_Save == Inc_Up){
        PE_7_incUP = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_7);
    }else if(Catch_Key_Save == Inc_Down){
        PE_8_inc_Down = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_8);
    }
     
    if(PB_1_spdUP | PB_2_spdDown | PE_7_incUP | PE_8_inc_Down){  //�p�G�٫���  ���m���u���ɶ�
          Debounce_Time = Debounce_Time_ADJ;
          __asm("NOP");
    }else{
        Debounce_Time = 0;                 
        Holding_Cnt = 0;      //���s��}���m����ɶ�
        Catch_Key_Name = (KeyName_Def)0;   
        Catch_Key_Save = (KeyName_Def)0;    //���s��}  �M���Ȧs
    }
    
}


unsigned char R_KeyCatch(KeyName_Def KeyName){  //���q���@�U  

    if(Catch_Key_Name == KeyName){
        Catch_Key_Name = (KeyName_Def)0;
        return 1;
    }
    return 0;
}

//����W�L 0.8��  �C�j0.1��  return 1
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


//Step 1     �b��L����S���b�ʧ@���ɭ� �hŪ������GPIO
void RealityKey_PressDetect(){
    
    if(Debounce_Time == 0){
        PB_1_spdUP        = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_1);
        PB_2_spdDown  = HAL_GPIO_ReadPin( GPIOB  , GPIO_PIN_2);
        PE_7_incUP          = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_7);
        PE_8_inc_Down  = HAL_GPIO_ReadPin( GPIOE  , GPIO_PIN_8);
        
        if(PB_1_spdUP | PB_2_spdDown | PE_7_incUP | PE_8_inc_Down){
            
            Buzzer_ON();        //4�ӫ��䦳�@�ӫ���QĲ�o ,���ﾹ�N��B�@�n
            
            //���� ���U�� ���� �W��  �x�s��   Catch_Key_Name  
            if(PB_1_spdUP == GPIO_PIN_SET){
                Catch_Key_Name = Spd_Up;    
            }else if(PB_2_spdDown ==GPIO_PIN_SET ){
                Catch_Key_Name = Spd_Down;
            }else if(PE_7_incUP ==GPIO_PIN_SET ){
                Catch_Key_Name = Inc_Up;
            }else if(PE_8_inc_Down ==GPIO_PIN_SET ){
                Catch_Key_Name = Inc_Down;
            }  
            
            Catch_Key_Save = Catch_Key_Name;   //����ثe��������s�_�� 
            
            //�M�� GPIO���A      ��������U��   Debounce_Time ! =  0 ���|�A�h�T�{HAL_GPIO_ReadPin  �ҥH�n���M�����A
            PB_1_spdUP       = GPIO_PIN_RESET; 
            PB_2_spdDown = GPIO_PIN_RESET;
            PE_7_incUP         = GPIO_PIN_RESET;  
            PE_8_inc_Down = GPIO_PIN_RESET;
            
            //�]�w���u���ɶ�
            Debounce_Time = Debounce_Time_ADJ;
            __asm("NOP");
        }
        
    }
      
}

void Debounce(){
    
    if(Debounce_Time == 1){
        Key_Release_Check();        //���u�� cnt ��� ==1 �� �h�T�{���䦳�S����}
    }else{
        if(Debounce_Time>1){
            Debounce_Time--;
        }
       
    }
}