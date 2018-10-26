#include "stm32f4xx_hal.h"
#include "string.h"
#include "system.h"

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
                Draw_Auto(  Stay , Download_Icon  ,50 ,0);  //顯示下載圖案
            }else{
                
                F_String_buffer_Auto_Middle(Stay,"OTA" ,30 ,0);
            }
        }
        writeLEDMatrix();  
    }
}