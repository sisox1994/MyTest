#include "system.h"

void ClearAllOPTION_KeyDisplayCnt();
void HR_SENSOR_LINK_Key(){
    
    if( KeyCatch(0,1 , ANT) ){
        ClearAllOPTION_KeyDisplayCnt();
        Btm_Task_Adder(Scan_ANT_HRC_Sensor);
        __asm("NOP");
    }   
    
    if( KeyCatch(0,1 , BLE) ){
        if(Ble_wait_HR_value_First_IN_Flag == 0){
            ClearAllOPTION_KeyDisplayCnt();
            Btm_Task_Adder(Scan_BLE_HRC_Sensor);
        }
        __asm("NOP");  
    }   
    
}


unsigned char DisplayTime = 8;

void ClearAllOPTION_KeyDisplayCnt(){
    
    //�M���Ȯ���ܪ��U��Cnt
    
    Ble_Icon_Display_Cnt = 0;
    ANT_Icon_Display_Cnt = 0;
    
    APP_Connected_Display_Cnt = 0;
    
    DIST_Icon_Display_Cnt = 0;
    ALTI_Icon_Display_Cnt = 0;
    
    CAL_Icon_Display_Cnt = 0;
    CALH_Icon_Display_Cnt = 0;
    
    ELAPSED_Icon_Display_Cnt = 0;
    REMAINNING_Icon_Display_Cnt = 0;
    
    PACE_Icon_Display_Cnt = 0;
    STEP_Icon_Display_Cnt = 0;
    
    MET_Icon_Display_Cnt = 0;
    HR_Icon_Display_Cnt = 0;
    
}


void SCREEN_OPTION_Key(){
    
    //-----------------��ܤ���-----------------------------------------------
    if( KeyCatch(0,1 , Key_Cal) ){
        ClearAllOPTION_KeyDisplayCnt();
        
        //--���ӥd����    �C�p�ɮ��ӥd���� ��ܤ���
        if(Calories_Display_Type == Cal_){
            Calories_Display_Type = Cal_HR;
            CALH_Icon_Display_Cnt = DisplayTime;
        }else if(Calories_Display_Type == Cal_HR){
            Calories_Display_Type = Cal_;
            CAL_Icon_Display_Cnt = DisplayTime;
        }
        __asm("NOP");
    } 
     
    if( KeyCatch(0,1 , Key_Pace)){   
        
        ClearAllOPTION_KeyDisplayCnt();
        
        //--���� �B�� �B�T �B�W �B�Z-----
        if(Pace_Display_Switch == 0){   //"�B��"��"�B�t"�����N�n�F
            Pace_Display_Switch = 3;
            PACE_Icon_Display_Cnt = DisplayTime;
        }else{
            Pace_Display_Switch = 0;
            STEP_Icon_Display_Cnt = DisplayTime;
        }  
    }
    
    if( KeyCatch(0,1 , Key_Dist) ){
        ClearAllOPTION_KeyDisplayCnt();
        //-------- (�Z��)(���ް���) �ɶ���ܤ��� ------        
        if(Dist_Display_Type == DIST){
            Dist_Display_Type = ALTI;
            ALTI_Icon_Display_Cnt =DisplayTime;
        }else if(Dist_Display_Type == ALTI){
            Dist_Display_Type = DIST;
            DIST_Icon_Display_Cnt = DisplayTime;
        }
        __asm("NOP");
    } 
    
    if(System_Mode != Summary){
        
        if(KeyCatch(0,1 , Key_Met)){
            ClearAllOPTION_KeyDisplayCnt();
            if(HeartRate_Display_Type == HR){
                HeartRate_Display_Type = METS;
                MET_Icon_Display_Cnt = DisplayTime;
            }else if(HeartRate_Display_Type == METS){
                HeartRate_Display_Type = HR;
                HR_Icon_Display_Cnt = DisplayTime;
            }
        }
        
        if( KeyCatch(0,1 , Key_Time) ){
            
            ClearAllOPTION_KeyDisplayCnt();
            
            switch(Program_Select){
              case MARATHON_Mode:      //�ɶ��W�����
              case Quick_start:
              case Calorie_Goal:
              case Distance_Goal_160M:
              case Distance_Goal_5K:
              case Distance_Goal_10K:
              case APP_Cloud_Run:
              case APP_Train_Dist_Run:
              case APP_Train_Time_Run:
              case FIT_ARMY:
              case FIT_NAVY:
              case FIT_AIRFORCE: 
              case FIT_USMC:
              case FIT_WFI: 
                break;
                
              default:   //�w�]�U�����
                //-------- (�g�L)(�ѤU) �ɶ���ܤ��� ------        
                if(Time_Display_Type == Remaining){
                    Time_Display_Type = Elspsed;
                    ELAPSED_Icon_Display_Cnt = DisplayTime;
                }else if(Time_Display_Type == Elspsed){
                    Time_Display_Type = Remaining;
                    REMAINNING_Icon_Display_Cnt = DisplayTime;
                }
                break;
                
            }
            
            
            __asm("NOP");
        }  
        
        
    }

    
    //---------------------------------------------------------------------------
    
}

void IntoReadyMode_Process(){

    
    console_status = 4;      // 1 : stop  2�Gpause  3: stop by safeKey   4: start 
    Btm_Task_Adder(FEC_Data_Config_Page_1);
    
    
    Training_status = Manual_mode;
    Btm_Task_Adder(FEC_Data_Config_Page_0);
    
    
    //-----���m  �����߸�  �����t��  ��������---------
    ResetWorkoutAvgParam(); 
    
    //-------------------���]APP Program ��l��-----------------------
    if(Program_Select == APP_Cloud_Run){
        Cloud_Run_Program_Init();
    }
    
   //-----------------------------------------------------
    if(Program_Select == APP_Train_Dist_Run){
        Train_Dist_Run_Program_Init();
    }
    
    if(Program_Select == APP_Train_Time_Run){
        Train_Time_Run_Program_Init();
    }
    
    
    
    Time_Display_Type     = Remaining;    // �w�]�� �U�����
    Calories_Display_Type = Cal_;        // �w�]�� �d�������
    Pace_Display_Switch   = 3;           //�w�]��  PACE
    
    
    //Program_Select = (Program_Type_Def)0;
    str_cNt = 0;
    
    if(System_Mode != Ready){
        Buzzer_Set(150);
    }
   
    System_Mode = Ready;
   
    ClearStd_1_Sec_Cnt();
    
    
    
    
    
}
extern unsigned char Hint_Disp_Flag;
void Next_Mode_Process(){
    
    if(Program_Select == FIT_WFI){  //�������@�}�l �����N�O4.8 km
        
        if(System_Unit == Metric ){
            Set_SPEED_Value(48);
        }else if(System_Unit == Imperial){
            Set_SPEED_Value(30);
        }  
   
    }else{
        Set_SPEED_Value(Machine_Data.System_SPEED_Min);
    }
    
    
    if(Program_Select == Distance_Goal_160M || Program_Select == Distance_Goal_5K ||
       Program_Select == Distance_Goal_10K ){
           
       System_INCLINE = INCLINE_LOW_Limit_Table[Program_Data.Diffculty_Level];
       RM6_Task_Adder(Set_INCLINE);
       Update_BarArray_Data_Ex();
    }
    
   
    RM6_Task_Adder(Set_SPEED);
    RM6_Task_Adder(Motor_FR);
    

    ClassCnt = 0;
    
    switch(Program_Select){  
      case APP_Cloud_Run:
        //-------------���]   ��l���ɳ]�m--------------------------//
        System_INCLINE = Program_Data.INCLINE_Table_96[0] * 5;
        RM6_Task_Adder(Set_INCLINE);
       //-----------------------------------------------------------//
        IntoWorkoutModeProcess();
        break;
      case APP_Train_Dist_Run:
      case APP_Train_Time_Run:
        //-------------�V�m�p�e ��l���ɳt�׳]�m--------------------------//
        System_INCLINE = Program_Data.INCLINE_Table_96[0] * 5;
        System_SPEED   = CloudRun_Init_INFO.CloudRun_Spd_Buffer[0];    
        RM6_Task_Adder(Set_INCLINE);
        RM6_Task_Adder(Set_SPEED);    
        //---------------------------------------------------------------//
        IntoWorkoutModeProcess();
        break;
      case Quick_start:
      case FIT_ARMY:
      case FIT_NAVY:
      case FIT_AIRFORCE: 
      case FIT_USMC:
      case FIT_WFI: 
        IntoWorkoutModeProcess();   //Quick Start Mode ���LWarm Up
        break;
      default:
        IntoWarmUpModeProcess();
        break;
    }
    NoStep_Cnt = 0;  //�L�H���� cnt�M0
     
    if(Program_Select == FIT_WFI){
        Hint_Disp_Flag = 1;
    }
    
}

void Ready_Func(){


    if(T_Marquee_Flag){
        T_Marquee_Flag = 0;
        
        switch(str_cNt){
          case 0:
            if(F_Number_buffer_Auto( Down_and_Stay, 3 , 35 ,DEC ,0)){
                str_cNt += 1;  
                Buzzer_Set(150);
            }           
            break;
          case 1:
            if(F_Number_buffer_Auto( Down_and_Stay, 2 , 35 ,DEC ,0)){
                str_cNt += 1;  
                Buzzer_Set(150);
            }    
            break;    
          case 2:
            if(F_Number_buffer_Auto( Down_and_Stay, 1 , 35 ,DEC ,0)){
                str_cNt += 1;
                Buzzer_Set(500);
            } 
            break;
          case 3:
            if(F_String_buffer_Auto_Middle( Up_and_Stay, "GO" ,40 ,0)){
                str_cNt += 1;
                //Buzzer_Set(200);
            }      
            break; 
          case 4:
            Next_Mode_Process();
            str_cNt = 0;
            break;   
        }

        switch(Program_Select){
          case Quick_start:
          case MARATHON_Mode:
          case Calorie_Goal:
          case Distance_Goal_160M:
          case Distance_Goal_5K:
          case Distance_Goal_10K:
          case APP_Cloud_Run:
          case APP_Train_Dist_Run:
          case APP_Train_Time_Run:    //�W��
          case FIT_ARMY:
          case FIT_NAVY:
          case FIT_AIRFORCE: 
          case FIT_USMC:
          case FIT_WFI: 
            SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
            break;
            
          case User_1:
          case User_2:
            switch(Program_Data.Like_Program){
                
              case Quick_start:
              case MARATHON_Mode:
              case Calorie_Goal:
              case Distance_Goal_160M:
              case Distance_Goal_5K:
              case Distance_Goal_10K:
                SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time - Program_Data.Goal_Counter);
                break;
                
              default:
                SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time);
                break;
            }
            break;
            
            
          default:
            SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Time);
            break;
        }

        writeLEDMatrix();
        writeSegmentBuffer();
        
    }

}