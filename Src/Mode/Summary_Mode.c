#include "system.h"

void IntoSummaryMode_Process(){

    SET_DisplayBoard_Data(L_Mode,1,0 ,R_Mode,1,0); 
    
    __asm("NOP");  //盢笲笆 禯瞒,丁  糶秈Flash 
    
    
    if(Program_Select == FIT_WFI){
        GetFitTest_Score();
    }
    
    Machine_Data_Update();
    
    System_Mode = Summary;
    
    console_status =  1;//1 : stop  2pause  3: stop by safeKey   4: start 
    FE_Status = FINISHED;
    Btm_Task_Adder(FEC_Data_Config_Page_1);
        
    ClearStd_1_Sec_Cnt(); 
    
    F_BtmReply39Cmd();//ミ皑禗APP 秈summary 
    
    __asm("NOP");
    
}


void Summary_Key(){

 
    if( KeyCatch(0,1 , Stop) || PauseKey() ){
        IntoIdleMode_Process();
        
        Training_status = IDLE;
        Btm_Task_Adder(FEC_Data_Config_Page_0);
        
        console_status = 1;      // 1 : stop  2pause  3: stop by safeKey   4: start 
        Btm_Task_Adder(FEC_Data_Config_Page_1);
        
        
    }
    SCREEN_OPTION_Key();
    HR_SENSOR_LINK_Key(); 
    
}



void Summary_Idel_detect(){
    
    if(T1s_Idle == 1){
        T1s_Idle = 0;
        
        if(ret_Idle_cnt == 120){
            
            IntoIdleMode_Process();
            
            ClassCnt = 0;
            ret_Idle_cnt = 0;
        }else{
            ret_Idle_cnt++;
        }
        
    }
    
    
}

unsigned char Summary_blink_flag;

unsigned char ARMY_SCORE_MSG[] = {'S','C','R','O','P',':',' ',' ',' ','1','0','0',' ','/',' ',' ','P','A','S','S','\0'}; 
unsigned char NAVY_POINT_MSG[] = {'P','O','I','N','T',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'}; 

unsigned char AIRF_POINT_MSG[] = {'P','O','I','N','T','S',':',' ',' ','6','0','.','0',' ','/',' ',' ','M','O','D','E','R','A','T','E','-','R','I','S','K','\0'}; 

unsigned char USMC_POINT_MSG[] = {'P','O','I','N','T','S',':',' ',' ','1','0','0','\0'}; 

unsigned char WFI_POINT_MSG[] = {'V','O','2','M','A','X',':',' ',' ','0','0','.','0','0','\0'}; 

void Summary_Func(){

    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;

        if(Program_Select == FIT_ARMY){
        //-----------------------ARMY------------------------------------------------------          
            if(Program_Data.FitTest_Score == 0){
                ARMY_SCORE_MSG[9]  = ' ';
                ARMY_SCORE_MSG[10] = ' ';
                ARMY_SCORE_MSG[11] = '0';
                
                ARMY_SCORE_MSG[16] = 'F';
                ARMY_SCORE_MSG[17] = 'A';
                ARMY_SCORE_MSG[18] = 'I';
                ARMY_SCORE_MSG[19] = 'L';
               
            }else if(Program_Data.FitTest_Score == 100){
                ARMY_SCORE_MSG[9]  = '1';
                ARMY_SCORE_MSG[10] = '0';
                ARMY_SCORE_MSG[11] = '0';
                
                ARMY_SCORE_MSG[16] = 'P';
                ARMY_SCORE_MSG[17] = 'A';
                ARMY_SCORE_MSG[18] = 'S';
                ARMY_SCORE_MSG[19] = 'S';
                
            }else if( (Program_Data.FitTest_Score>=10) &&(Program_Data.FitTest_Score<=99) ){
            
                ARMY_SCORE_MSG[9]  = ' ';
                ARMY_SCORE_MSG[10] = (Program_Data.FitTest_Score/10) +0x30;
                ARMY_SCORE_MSG[11] = (Program_Data.FitTest_Score%10) +0x30;
                
                if(Program_Data.FitTest_Score>=60){
                    ARMY_SCORE_MSG[16] = 'P';
                    ARMY_SCORE_MSG[17] = 'A';
                    ARMY_SCORE_MSG[18] = 'S';
                    ARMY_SCORE_MSG[19] = 'S';
                }else{
                    ARMY_SCORE_MSG[16] = 'F';
                    ARMY_SCORE_MSG[17] = 'A';
                    ARMY_SCORE_MSG[18] = 'I';
                    ARMY_SCORE_MSG[19] = 'L';
                }
                
                
            }else if(Program_Data.FitTest_Score<10){
                ARMY_SCORE_MSG[9]  = ' ';
                ARMY_SCORE_MSG[10] = ' ';
                ARMY_SCORE_MSG[11] = (Program_Data.FitTest_Score) +0x30;
                
                ARMY_SCORE_MSG[16] = 'F';
                ARMY_SCORE_MSG[17] = 'A';
                ARMY_SCORE_MSG[18] = 'I';
                ARMY_SCORE_MSG[19] = 'L';
                
            }
             F_String_buffer_Auto( Left, ARMY_SCORE_MSG ,55 ,0);
        
        }else if(Program_Select == FIT_NAVY){
        //-----------------------------NAVY--------------------------------------------------------------   
            if(Program_Data.FitTest_Score == 0){
                NAVY_POINT_MSG[9]  = ' ';
                NAVY_POINT_MSG[10] = ' ';
                NAVY_POINT_MSG[11] = '0';
                
                NAVY_POINT_MSG[16] = ' ';
                NAVY_POINT_MSG[17] = ' ';
                NAVY_POINT_MSG[18] = ' ';
                NAVY_POINT_MSG[19] = ' ';
               
            }else if(Program_Data.FitTest_Score == 100){
                NAVY_POINT_MSG[9]  = '1';
                NAVY_POINT_MSG[10] = '0';
                NAVY_POINT_MSG[11] = '0';
                
                NAVY_POINT_MSG[16] = ' ';
                NAVY_POINT_MSG[17] = ' ';
                NAVY_POINT_MSG[18] = ' ';
                NAVY_POINT_MSG[19] = ' ';
                
            }else if( (Program_Data.FitTest_Score>=10) &&(Program_Data.FitTest_Score<=99) ){
            
                NAVY_POINT_MSG[9]  = ' ';
                NAVY_POINT_MSG[10] = (Program_Data.FitTest_Score/10) +0x30;
                NAVY_POINT_MSG[11] = (Program_Data.FitTest_Score%10) +0x30;
                
                if(Program_Data.FitTest_Score>=60){
                    NAVY_POINT_MSG[16] = ' ';
                    NAVY_POINT_MSG[17] = ' ';
                    NAVY_POINT_MSG[18] = ' ';
                    NAVY_POINT_MSG[19] = ' ';
                }else{
                    NAVY_POINT_MSG[16] = ' ';
                    NAVY_POINT_MSG[17] = ' ';
                    NAVY_POINT_MSG[18] = ' ';
                    NAVY_POINT_MSG[19] = ' ';
                }
                
                
            }else if(Program_Data.FitTest_Score<10){
                NAVY_POINT_MSG[9]  = ' ';
                NAVY_POINT_MSG[10] = ' ';
                NAVY_POINT_MSG[11] = (Program_Data.FitTest_Score) +0x30;
                
                NAVY_POINT_MSG[16] = ' ';
                NAVY_POINT_MSG[17] = ' ';
                NAVY_POINT_MSG[18] = ' ';
                NAVY_POINT_MSG[19] = ' ';
                
            }
            F_String_buffer_Auto( Left, NAVY_POINT_MSG ,55 ,0);
        
        }else if(Program_Select == FIT_AIRFORCE){
        //-----------------------------AIRFORCE--------------------------------------------------------------
            
            if(Program_Data.FitTest_Score >=100){
                AIRF_POINT_MSG[9]  = (Program_Data.FitTest_Score/100)    + 0x30;
                AIRF_POINT_MSG[10] = (Program_Data.FitTest_Score%100/10) + 0x30;
                AIRF_POINT_MSG[12] = (Program_Data.FitTest_Score%100%10) + 0x30;
            }else if( (Program_Data.FitTest_Score < 100) && (Program_Data.FitTest_Score >= 10)){
                AIRF_POINT_MSG[9]  = ' ';
                AIRF_POINT_MSG[10] = (Program_Data.FitTest_Score/10) + 0x30;
                AIRF_POINT_MSG[12] = (Program_Data.FitTest_Score%10) + 0x30;
            }else if( Program_Data.FitTest_Score == 0){
                AIRF_POINT_MSG[9]  = ' ';
                AIRF_POINT_MSG[10] = '0';
                AIRF_POINT_MSG[12] = '0';
            }
  
            //------------------------Risk  陪ボ-----------------------------------
            if(Program_Data.FitTest_RISK == 0){
            
                AIRF_POINT_MSG[14] = '/';
                AIRF_POINT_MSG[15] = ' ';
                AIRF_POINT_MSG[16] = ' ';
                
                AIRF_POINT_MSG[17] = 'L';
                AIRF_POINT_MSG[18] = 'O';
                AIRF_POINT_MSG[19] = 'W';
                AIRF_POINT_MSG[20] = '-';
                AIRF_POINT_MSG[21] = 'R';
                AIRF_POINT_MSG[22] = 'I';
                AIRF_POINT_MSG[23] = 'S';
                AIRF_POINT_MSG[24] = 'K';
                
                for(unsigned char i = 25; i < 30;i++){
                    AIRF_POINT_MSG[i] = '\0';
                }
                
            }else if(Program_Data.FitTest_RISK == 1){
                AIRF_POINT_MSG[14] = '/';
                AIRF_POINT_MSG[15] = ' ';
                AIRF_POINT_MSG[16] = ' ';
                
                AIRF_POINT_MSG[17] = 'M';
                AIRF_POINT_MSG[18] = 'O';
                AIRF_POINT_MSG[19] = 'D';
                AIRF_POINT_MSG[20] = 'E';
                AIRF_POINT_MSG[21] = 'R';
                AIRF_POINT_MSG[22] = 'A';
                AIRF_POINT_MSG[23] = 'T';
                AIRF_POINT_MSG[24] = 'E';
                AIRF_POINT_MSG[25] = '-';
                AIRF_POINT_MSG[26] = 'R';
                AIRF_POINT_MSG[27] = 'I';
                AIRF_POINT_MSG[28] = 'S';
                AIRF_POINT_MSG[29] = 'K';
                
            }else if(Program_Data.FitTest_RISK == 2){  
                AIRF_POINT_MSG[14] = '/';
                AIRF_POINT_MSG[15] = ' ';
                AIRF_POINT_MSG[16] = ' ';
                
                AIRF_POINT_MSG[17] = 'H';
                AIRF_POINT_MSG[18] = 'I';
                AIRF_POINT_MSG[19] = 'G';
                AIRF_POINT_MSG[20] = 'H';
                AIRF_POINT_MSG[21] = '-';
                AIRF_POINT_MSG[22] = 'R';
                AIRF_POINT_MSG[23] = 'I';
                AIRF_POINT_MSG[24] = 'S';
                AIRF_POINT_MSG[25] = 'K';
                
                for(unsigned char i = 26; i < 30;i++){
                    AIRF_POINT_MSG[i] = '\0';
                }
            }else if(Program_Data.FitTest_RISK == 3){
                
                for(unsigned char i = 14; i < 30;i++){
                    AIRF_POINT_MSG[i] = '\0';
                }
            }
            //---------------------------------------------------------------------------
            
            
            
            F_String_buffer_Auto( Left, AIRF_POINT_MSG ,55 ,0);
  
        }else if(Program_Select == FIT_USMC){

            if(Program_Data.FitTest_Score == 100){
                USMC_POINT_MSG[9]  = '1';
                USMC_POINT_MSG[10] = '0';
                USMC_POINT_MSG[11] = '0';
            }else if( (Program_Data.FitTest_Score >= 10)&&(Program_Data.FitTest_Score < 100) ){
                USMC_POINT_MSG[9]  = ' ';
                USMC_POINT_MSG[10] = Program_Data.FitTest_Score/10 + 0x30;
                USMC_POINT_MSG[11] = Program_Data.FitTest_Score%10 + 0x30;
            }else if( Program_Data.FitTest_Score < 10 ){
                USMC_POINT_MSG[9]  = ' ';
                USMC_POINT_MSG[10] = ' ';
                USMC_POINT_MSG[11] = Program_Data.FitTest_Score + 0x30;
            }
            
            F_String_buffer_Auto( Left, USMC_POINT_MSG ,55 ,0);
            
        }else if(Program_Select == FIT_WFI){
            

            if( Program_Data.FitTest_Score == 0 ){
                
                WFI_POINT_MSG[9]  = '0';
                
                for(unsigned char i = 10;i<14;i++){
                    WFI_POINT_MSG[i] = '\0';
                }
                
            }else{
                unsigned char digit4 = 0;
                unsigned char digit3 = 0;
                unsigned char digit2 = 0;
                unsigned char digit1 = 0;
                
                digit4 = Program_Data.FitTest_Score/1000;
                digit3 = Program_Data.FitTest_Score%1000/100;
                digit2 = Program_Data.FitTest_Score%1000%100/10;
                digit1 = Program_Data.FitTest_Score%1000%100%10;
                
                if( (digit2 !=0) || (digit1 !=0)){
                    //----俱计场だ
                    WFI_POINT_MSG[9]  = digit4 + 0x30;
                    WFI_POINT_MSG[10] = digit3 + 0x30;
                    
                    WFI_POINT_MSG[11] = '.';
                    
                    //---计翴场だ
                    WFI_POINT_MSG[12] = digit2 + 0x30;
                    
                    if(digit1 == 0){
                        WFI_POINT_MSG[13] = ' ';   //计翴材2狦琌0  碞ぃ璶陪ボ
                    }else{
                        WFI_POINT_MSG[13] = digit1 + 0x30;    //计翴材2琌计碞陪ボê计
                    }
                }else if( (digit2 == 0)&&(digit1 == 0) ){  //Τ俱计⊿Τ计翴场だ
                
                     //----俱计场だ
                    WFI_POINT_MSG[9]  = digit4 + 0x30;
                    WFI_POINT_MSG[10] = digit3 + 0x30;
                    
                    //----计翴常ぃ陪ボ-------
                    for(unsigned char i = 11;i<14;i++){
                        WFI_POINT_MSG[i] = '\0';
                    }
                    
                }
                
                
            }
            
            
            F_String_buffer_Auto( Left, WFI_POINT_MSG ,55 ,0);
        }else{
            F_String_buffer_Auto( Left, "WORKOUT   SUMMARY" ,55 ,0);
        }
        
       
        
        SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
        
        SET_Seg_Display(SPEED    , uiAvgSpeed  , D2 , DEC );
        SET_Seg_Display(INCLINE  , uiAvgIncline, D2 , DEC );
        SET_Seg_Display(HEARTRATE, uiAvgHeartRate, ND , DEC );
        
        
        if(T500ms_Flag == 1){
            T500ms_Flag = 0;
            
            if(Summary_blink_flag == 1){
                Summary_blink_flag = 0;
            }else{
                Summary_blink_flag = 1;
            }
        }
        
        if(Summary_blink_flag == 1){
            writeSegmentBuffer();
        }else{
            Turn_OFF_All_Segment();
        }
       
        
        writeLEDMatrix();
        
    }
    
    

    Summary_Idel_detect();
    
    Summary_Key();
    
}