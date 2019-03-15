#include "stm32f4xx_hal.h"
#include "string.h"
#include "system.h"

unsigned int TransmitPage_old;
unsigned int Transmit_Time_out_Cnt;

void OTA_Mode_Func(){
    
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;
        
        if(FW.Error_Type == Data_Lost){
        
            F_String_buffer_Auto_Middle(Stay,"LOST" ,30 ,0);
            writeLEDMatrix();  
            HAL_Delay(3000);
            HAL_NVIC_SystemReset();
            
        }else if(FW.Error_Type == Size_Error){
            
            F_String_buffer_Auto_Middle(Stay,"ERR" ,30 ,0);
            writeLEDMatrix();  
            HAL_Delay(3000);
            HAL_NVIC_SystemReset();
            
        }else if(FW.Error_Type == No_Error){
            if(FW_Transmiting_03_Flag == 1){
                Draw_Auto(  Stay , Download_Icon  ,50 ,0);  //��ܤU���Ϯ�
            }else{ 
                F_String_buffer_Auto_Middle(Stay,"OTA" ,30 ,0);
            }
        }
        
        if(FW.Transmit_Page < FW.total_Page){         
            
            if(TransmitPage_old != FW.Transmit_Page ){ //�����ǿ�page���S���i�i
                TransmitPage_old = FW.Transmit_Page;
                Transmit_Time_out_Cnt = 0;
            }else if(TransmitPage_old == FW.Transmit_Page){
                //�p�G�@���d�b�P�@��page�N�� app�����άO�_�u�F
                Transmit_Time_out_Cnt++;   //�_�ucnt�}�l�p��
            }
            
            if(Transmit_Time_out_Cnt == 100){ //��ƨ�100���S�T���N�q�}�� �^��application
                Turn_OFF_All_LEDMatrix();                
                F_String_buffer_Auto_Middle(Stay,"FAIL" ,30 ,0);
                writeLEDMatrix();  
                HAL_Delay(3000);
                
                Flash_Write_Applicantion_Mode();
                HAL_Delay(15);
                HAL_NVIC_SystemReset();
            }
            
            
        }
        
        writeLEDMatrix();  
    }
}