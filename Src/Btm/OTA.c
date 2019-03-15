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
                Draw_Auto(  Stay , Download_Icon  ,50 ,0);  //顯示下載圖案
            }else{ 
                F_String_buffer_Auto_Middle(Stay,"OTA" ,30 ,0);
            }
        }
        
        if(FW.Transmit_Page < FW.total_Page){         
            
            if(TransmitPage_old != FW.Transmit_Page ){ //紀錄傳輸page有沒有進展
                TransmitPage_old = FW.Transmit_Page;
                Transmit_Time_out_Cnt = 0;
            }else if(TransmitPage_old == FW.Transmit_Page){
                //如果一直卡在同一個page代表 app關掉或是斷線了
                Transmit_Time_out_Cnt++;   //斷線cnt開始計數
            }
            
            if(Transmit_Time_out_Cnt == 100){ //當數到100都沒訊號就從開機 回到application
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