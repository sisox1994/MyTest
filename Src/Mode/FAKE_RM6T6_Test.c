#include "stm32f4xx_hal.h"
#include "system.h"

void RS485_Test_Func(){
    
    if(T_Marquee_Flag){
        T_Marquee_Flag = 0; 
         F_String_buffer_Auto_Middle( Left, "485  TEST" ,50 ,0);
        
         writeLEDMatrix();
        
    }   
}