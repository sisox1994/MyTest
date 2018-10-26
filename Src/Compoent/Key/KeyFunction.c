#include "system.h"

void KeyTest(){
    
    //----------------------------------
    if(KeyCatch(0,1 ,Inc_15) ){            
        INCL_CMD( Write , InclineADTable[30] );   
        HAL_Delay(20);
    }
    
    if(KeyCatch(0,1,Inc_10) ){
        INCL_CMD( Write , InclineADTable[20] );   
        HAL_Delay(20);
        
    }
    
    if(KeyCatch(0,1,Inc_5) ){
        INCL_CMD( Write , InclineADTable[10] );   
        HAL_Delay(20);
        
    }
    
    if(KeyCatch(0,1,Inc_0) ){
        INCL_CMD( Write , InclineADTable[0] );   
        HAL_Delay(20);
        
    }
    
    //---------------------------------------------
    
    if(KeyCatch(0,1 ,Stop) ){            
        OPT_CMD(Write , STOP);   
        HAL_Delay(20);
        F_OUT();
    }
    
    if(KeyCatch(0,1,Start) ){
        OPT_CMD(Write , FR);
        HAL_Delay(20);
        F_OUT();
    }
    //--------------------------------------------------
    
    if(KeyCatch(0,1,Spd_3) ){
        F_CMD(Write, 600);
        HAL_Delay(20);
    }
    
    if(KeyCatch(0,1,Spd_6) ){
        F_CMD(Write, 1200);
        HAL_Delay(20);
    }
    
    if(KeyCatch(0,1,Spd_9) ){
        F_CMD(Write, 1760);
        HAL_Delay(20);            
    }
    
    if(KeyCatch(0,1,Spd_12) ){
        F_CMD(Write, 2340);
        HAL_Delay(20);
    }
   
    //-------------------------------------
    if(KeyCatch(0,1,Enter) ){
        F_OUT();
    }
}
