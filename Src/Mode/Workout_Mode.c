#include "system.h"

#define CANCEL_TIME_SEC 10

unsigned int Shift_Times;
unsigned char No_HR_Value_Cnt; //�P�_�B�ʤ�  60��S���߸� �N�i�Jsummary ��cnt
unsigned char HR_BiggerThan85Percent_Cnt;  // �������Ҧ�  �P�_ 10�� �߸��O�_�W�L�̤j�߸�85%

void BarArray_Shift_Process();

unsigned char TimePeroid_Process();

unsigned char Time_Set_Flag;
unsigned int  Set_TimeVal_Temp; //�B�ʤ��]�w�ɶ����Ȯɦs���
int           TimeVal_Diff;     //�ɶ��W��ƭ�
unsigned char Cancel_SetTime_Cnt;

//---------WFI ������  ��ܴ��ܥ� Flag
unsigned char Hint_Disp_Flag = 0;

extern unsigned char  Number_Digit_Tmp;
extern unsigned char  Time_KeyPad_Iput_Flag;  //-------�ɶ��]�w  keypad��J�Ҧ�
extern unsigned char  NumberKeyProcess();
extern void NumberInsert_Time(unsigned int* Time_Modify);

void IntoWorkoutModeProcess(){
    
    console_status = 4;      // 1 : stop  2�Gpause  3: stop by safeKey   4: start 
    Btm_Task_Adder(FEC_Data_Config_Page_1);
    
  
    WarmUp_3_Minute_Cnt = 0;
    System_Mode = Workout;
    F_SetFEC_State(IN_USE);
    ClearStd_1_Sec_Cnt(); 

}

short Now_INCLINE_Oringin_Value;
short Now_SPEED_Oringin_Value;

void Update_BarArray_Data_Ex(){
    
    //�Z���ؼмҦ� ��Diffculty_Level �h����i�վ㪺���n�d���
    if( Program_Select == Distance_Goal_160M || 
          Program_Select == Distance_Goal_5K || 
          Program_Select == Distance_Goal_10K)
    {
        if(System_INCLINE >= INCLINE_HIGH_Limit_Table[Program_Data.Diffculty_Level] ){
            System_INCLINE = INCLINE_HIGH_Limit_Table[Program_Data.Diffculty_Level];
        }else if(System_INCLINE <= INCLINE_LOW_Limit_Table[Program_Data.Diffculty_Level]){
            System_INCLINE = INCLINE_LOW_Limit_Table[Program_Data.Diffculty_Level];
        }
    
    }

    Now_INCLINE_Oringin_Value = Program_Data.INCLINE_Table_96[(Program_Data.NowPeriodIndex % 96)]; 
    Program_Data.INCLINE_ValueDiff = (System_INCLINE/5) - Now_INCLINE_Oringin_Value;

    Now_SPEED_Oringin_Value  = Program_Data.SPEED_Table_96[(Program_Data.NowPeriodIndex % 96)]; 
    Program_Data.SPEED_ValueDiff = System_SPEED - Now_SPEED_Oringin_Value;
     
    //--�˥��]�n�����----
    for(unsigned char i = 0; i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] += Program_Data.INCLINE_ValueDiff;
    }
    
    if(Program_Data.MasterPage == 0){
        for(unsigned char i = 0 ; i < 96 ; i++){
            
            if(i >= Program_Data.NowPeriodIndex % 96 ){
                if(i < Program_Data.PeriodNumber){
                    Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;
                    Program_Data.SPEED_Table_96[i]   += Program_Data.SPEED_ValueDiff;
                }
            }
        }
        
    }else if(Program_Data.MasterPage == 1){
        for(unsigned char i = 32 ; i < 96 ; i++){
            if(i >= Program_Data.NowPeriodIndex % 96 ){
                Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;   
                Program_Data.SPEED_Table_96[i]   += Program_Data.SPEED_ValueDiff;
            } 
        }
        for(unsigned char i = 0 ; i < 32 ; i++){
            Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;
            Program_Data.SPEED_Table_96[i]   += Program_Data.SPEED_ValueDiff;
        }
        
    }else if(Program_Data.MasterPage == 2){
        if( (Program_Data.NowPeriodIndex % 96 >= 64) && (Program_Data.NowPeriodIndex % 96 < 96) ){
            for(unsigned char i = 64 ; i < 128 ; i++){
                if(i >= Program_Data.NowPeriodIndex % 96 ){
                    Program_Data.INCLINE_Table_96[i%96] += Program_Data.INCLINE_ValueDiff;  
                    Program_Data.SPEED_Table_96[i%96]   += Program_Data.SPEED_ValueDiff;
                }
            }
        }
        if( (Program_Data.NowPeriodIndex % 96 >= 0) && (Program_Data.NowPeriodIndex % 96 < 32) ){
            for(unsigned char i = 0 ; i < 32 ; i++){
                if(i >= Program_Data.NowPeriodIndex % 96 ){
                    Program_Data.INCLINE_Table_96[i%96] += Program_Data.INCLINE_ValueDiff; 
                    Program_Data.SPEED_Table_96[i%96]   += Program_Data.SPEED_ValueDiff;
                }
            }
        }
        for(unsigned char i = 32 ; i < 64 ; i++){
            Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;
            Program_Data.SPEED_Table_96[i]   += Program_Data.SPEED_ValueDiff;
        }
        
    }
    
    BarArray_Shift_Process();
    //DrawBarArray_Workout(Program_Data.BarArray_Display , Program_Data.PeriodIndex_After_Shift , 0 );
    
    //writeLEDMatrix();
    //ClearBlinkCnt_BarArray();
    
}

unsigned char Quick_INCLINE__Key(){

    switch(Program_Select){  //�괭�� �����^��0
        
      case Target_HeartRate_Goal: 
      case Fat_Burn:            
      case Cardio:              
      case Heart_Rate_Hill:     
      case Heart_Rate_Interval: 
      case Extreme_Heart_Rate:  
      case EZ_INCLINE: 
      case APP_Cloud_Run:
      //case APP_Train_Dist_Run:
      //case APP_Train_Time_Run:  
        //---�괭��--//
        return 0;
        break;
        
    }
 
    if(R_KeyCatch(Inc_Up)      || R_KeyContinueProcess(Inc_Up) ||
       KeyCatch(0,1,Key_IncUp) || KeyForContinueProcess(Key_IncUp) ){ 
           
        if(System_INCLINE<Machine_Data.System_INCLINE_Max) System_INCLINE+=5;
        __asm("NOP");
         return 1;
    }
    
    if(R_KeyCatch( Inc_Down)   || R_KeyContinueProcess(Inc_Down) ||
       KeyCatch(0,1,Key_IncDwn) || KeyForContinueProcess(Key_IncDwn) ){ 
           
        if(System_INCLINE>Machine_Data.System_INCLINE_Min)System_INCLINE-=5;
        __asm("NOP");
         return 1;
    }
    
    
    if( KeyCatch(0,1 , Inc_15) ){
        System_INCLINE = 150;
        return 1;
    }
    if( KeyCatch(0,1 , Inc_12) ){
        System_INCLINE = 120;
        return 1;
    } 
    if( KeyCatch(0,1 , Inc_9) ){
        System_INCLINE = 90;
        return 1;
    }
    if( KeyCatch(0,1 , Inc_6) ){
        System_INCLINE = 60;
        return 1;
    }
    if( KeyCatch(0,1 , Inc_3) ){
        System_INCLINE = 30;
        return 1;
    } 
    if( KeyCatch(0,1 , Inc_0) ){
        System_INCLINE = 0;
        return 1;
    }     
    
    return 0;
}

unsigned char Quick_SPEED__Key(){

    
    if( R_KeyCatch( Spd_Up)       || R_KeyContinueProcess(Spd_Up) ||
        KeyCatch(0,1 , Key_SpdUp) || KeyForContinueProcess(Key_SpdUp) ){
            
        if(System_SPEED < Machine_Data.System_SPEED_Max){
            System_SPEED += 1;
        }
          return 1;
    }
    
    if(R_KeyCatch( Spd_Down)      || R_KeyContinueProcess(Spd_Down) ||
       KeyCatch(0,1 , Key_SpdDwn) || KeyForContinueProcess(Key_SpdDwn) ){
           
        if(System_SPEED > Machine_Data.System_SPEED_Min){
            System_SPEED -=1;
        }
        return 1;
    }
    
    
    if( KeyCatch(0,1 , Spd_18) ){
        
        if(System_Unit == Imperial){
            System_SPEED = 120;
        }else{
            System_SPEED = 180;
        }
        return 1;
    }
    
    if( KeyCatch(0,1 , Spd_15) ){
        if(System_Unit == Imperial){
            System_SPEED = 100;
        }else{            
            System_SPEED = 150;
        }
        return 1;
    } 
    if( KeyCatch(0,1 , Spd_12) ){
        if(System_Unit == Imperial){
            System_SPEED = 80;
        }else{ 
            System_SPEED = 120;
        }
        return 1;
    }
    if( KeyCatch(0,1 , Spd_9) ){        
        if(System_Unit == Imperial){
            System_SPEED = 60;
        }else{ 
            System_SPEED = 90;
        }
        return 1;
    }
    if( KeyCatch(0,1 , Spd_6) ){
        if(System_Unit == Imperial){
            System_SPEED = 40;
        }else{ 
            System_SPEED = 60;
        }
        return 1;
    } 
    if( KeyCatch(0,1 , Spd_3) ){
        if(System_Unit == Imperial){
            System_SPEED = 20;
        }else{ 
            System_SPEED = 30;
        }
        return 1;
    }  
  
    return 0;
}

unsigned int Time_Chage_InWorkout_Temp;

void Workout_Key(){

    
    if(Time_Set_Flag == 0){
        
        //-----------------��ܤ���-----------------------------------------------
        SCREEN_OPTION_Key();
        
        if(KeyCatch(0,1 , cool)){  //FitTest �S��cool down �Ҧ�
            
            if( (Program_Select == FIT_ARMY)     || (Program_Select == FIT_NAVY) ||
                (Program_Select == FIT_AIRFORCE) || (Program_Select == FIT_USMC) ||  (Program_Select == FIT_WFI) ){
            
            }else{
                
                System_INCLINE = 0;
                RM6_Task_Adder(Set_INCLINE);
                
                Set_SPEED_Value((System_SPEED * 3)/10);
                
                if( System_SPEED < Machine_Data.System_SPEED_Min){
                    System_SPEED = Machine_Data.System_SPEED_Min;
                }

                RM6_Task_Adder(Set_SPEED);
                IntoCoolDownModeProcess(); 
            }
        }
        //----------------------------------------------------------
        if( KeyCatch(0,1 , Stop) || PauseKey()){
            
            
            IntoPauseMode_Process(); 
            
            
            Btm_Task_Adder(FTMS_Data_Broadcast); //Page 0 //�s���B�ʸ�� �k�s
            Btm_Task_Adder(FTMS_Data_Broadcast); //Page 1
            Set_SPEED_Value(0);
            RM6_Task_Adder(Set_SPEED);
            RM6_Task_Adder(Motor_STOP);
            Buzzer_BeeBee(Time_Set, Cnt_Set);
        }

        //-------------------------------------------------------
        
        if((Program_Select == FIT_WFI)&&(Program_Data.NowPeriodIndex < 28)){
            /*�p�G�O   ����������   �i�Jcool  down �e  �T����ܳt��*/
        }else{
            
            if(Quick_SPEED__Key() == 1){
                
                switch(Program_Select){
                    
                  case APP_Train_Dist_Run:
                  case APP_Train_Time_Run: 
                    Update_BarArray_Data_Ex();
                    break;
                  case FIT_WFI: 
                    if(Program_Data.NowPeriodIndex >= 28){
                        Update_BarArray_Data_Ex();
                    }
                    break;
                  default:
                    //---�ϧΤ�������--//
                    break;
                }
                
                Rst_Speed_Blink();
                RM6_Task_Adder(Set_SPEED);
            }
        }

        if(Program_Select == FIT_WFI){
           /*����������  �T����ܴ���*/
        }else{
            if(Quick_INCLINE__Key() == 1){
                
                RM6_Task_Adder(Set_INCLINE);
                
                switch(Program_Select){
                  case Quick_start:
                  case Manual:
                  case Random: 
                  case CrossCountry: 
                  case WeightLoss:   
                  case Interval_1_1: 
                  case Interval_1_2:
                  case Hill:   
                  case Hill_Climb: 
                  case Aerobic:     
                  case Interval_1_4: 
                  case Interval_2_1:  
                  case MARATHON_Mode:
                  case Calorie_Goal:
                  case Distance_Goal_160M:
                  case Distance_Goal_5K:
                  case Distance_Goal_10K:
                  case Custom_1: 
                  case Custom_2:
                  case APP_Train_Dist_Run:
                  case APP_Train_Time_Run: 
                  case FIT_ARMY:
                  case FIT_NAVY:
                  case FIT_AIRFORCE: 
                  case FIT_USMC:
                    Update_BarArray_Data_Ex();
                    break;
                    
                  case Target_HeartRate_Goal: 
                  case Fat_Burn:            
                  case Cardio:    
                  case Heart_Rate_Hill:     
                  case Heart_Rate_Interval: 
                  case Extreme_Heart_Rate: 
                  case EZ_INCLINE: 
                  case APP_Cloud_Run:
                  case FIT_WFI:
                    //---�괭��--//
                    break; 
                }
            }
        }

     
    }

    //---------------------BLE ANT+ �߸�  �s��---------------
    HR_SENSOR_LINK_Key(); 
    

    if(System_Mode == Workout){
        if( KeyCatch(0,1 , Enter)){
            Cancel_SetTime_Cnt = CANCEL_TIME_SEC;
            //-----------------------------�D�ɶ��]�w�Ҧ�
            if(Time_Set_Flag == 0){
                
                //----------�Y�ǤW�ƼҦ�����վ�ɶ�------
                switch(Program_Select){
                  case Heart_Rate_Hill:     
                  case Heart_Rate_Interval: 
                  case Extreme_Heart_Rate:  
                    Time_Set_Flag = 1;
                    break;
                  case Quick_start:   
                  case EZ_INCLINE:  
                  case MARATHON_Mode:
                  case WeightLoss: 
                  case APP_Cloud_Run:
                  case APP_Train_Dist_Run:
                  case APP_Train_Time_Run:  
                    //---��ɶ�--//
                    Time_Set_Flag = 0;
                    break;
                  default:
                    Time_Set_Flag = 1;
                    break;
                }
                
                 
                Time_Display_Type = Remaining; //���令��ܳѾl�ɶ�
                
            }else if(Time_Set_Flag == 1){  ///------------�ɶ��]�w����  �׽T�w

                if(Time_KeyPad_Iput_Flag == 0){   ///--
                    //----------------------------------------------------
                    if( ((Set_TimeVal_Temp + TimeVal_Diff) >60)  &&  (Program_Data.Goal_Counter > 60) ){
                        
                        switch(Program_Select){
                          case Heart_Rate_Hill:     
                          case Heart_Rate_Interval: 
                          case Extreme_Heart_Rate:  
                            Time_Change_Process_HRC_Hill_In_WorkOut(TimeVal_Diff);
                            break;
                          case EZ_INCLINE:  
                          case MARATHON_Mode:
                          case WeightLoss: 
                            //---��ɶ�--//
                            break;
                          default:
                            Time_Change_Process_InWorkout(TimeVal_Diff);
                            break;
                        }
                        BarArray_Shift_Process();   
                        Time_Set_Flag = 0;
                        TimeVal_Diff  = 0;
                        
                    }else{  //��J�ƭȤ��ŦX�d��
                        TimeVal_Diff = 0;
                        Time_Set_Flag = 1;
                    }
                    //---------------------------------------------------------
                }else if(Time_KeyPad_Iput_Flag == 1){  //-----KeyPad ��J�׽T�w
                
                    
                    //��J�Ѿl�ɶ��ܤ֭n1����
                    if(Time_Chage_InWorkout_Temp>60){
                        TimeVal_Diff = Time_Chage_InWorkout_Temp - (Program_Data.Goal_Counter/60)*60;
                    }else{
                        TimeVal_Diff = 0;
                    }
                    
                    
                    /*if(Time_Chage_InWorkout_Temp < (Program_Data.Goal_Time-Program_Data.Goal_Counter) +240){
                        TimeVal_Diff = 0;
                    }else{
                        TimeVal_Diff = Time_Chage_InWorkout_Temp - Program_Data.Goal_Time; //����X�t��
                    }*/
       
                    Number_Digit_Tmp = 0;
                    Time_KeyPad_Iput_Flag = 0;
                }
            }
            
            
        }
        
        if(Time_Set_Flag == 1){
                        
            if( KeyCatch(0,1 , Stop) ){
                Time_Set_Flag = 0;
                Time_KeyPad_Iput_Flag = 0;
                Number_Digit_Tmp =0;
            }
            
            
            if(Time_KeyPad_Iput_Flag == 0){
                    //--------------  (+)  (-) ����----------------------------------------
                    if( R_KeyCatch( Spd_Up)    || R_KeyContinueProcess(Spd_Up)    ||
                    KeyCatch(0,1 , Key_SpdUp)  || KeyForContinueProcess(Key_SpdUp)|| 
                    R_KeyCatch(Inc_Up)         || R_KeyContinueProcess(Inc_Up)    ||
                    KeyCatch(0,1,Key_IncUp)    || KeyForContinueProcess(Key_IncUp) ){
                        
                        Cancel_SetTime_Cnt = CANCEL_TIME_SEC;
                        //----------------------------------------
                        //if(Set_TimeVal_Temp + TimeVal_Diff >240){
                            //if(Program_Data.Goal_Counter > 60){
                                TimeVal_Diff+=60;   
                            //}   
                        //}//----------------------------------------
                    }
                        
                      
                    if(R_KeyCatch( Spd_Down)   || R_KeyContinueProcess(Spd_Down)   ||
                    KeyCatch(0,1 , Key_SpdDwn) || KeyForContinueProcess(Key_SpdDwn)|| 
                    R_KeyCatch( Inc_Down)      || R_KeyContinueProcess(Inc_Down)   ||    
                    KeyCatch(0,1,Key_IncDwn)   || KeyForContinueProcess(Key_IncDwn) ){
                        
                        Cancel_SetTime_Cnt = CANCEL_TIME_SEC;
                        //----------------------------------------
                        if(Set_TimeVal_Temp + TimeVal_Diff >120){
                            if(Program_Data.Goal_Counter > 60){
                                TimeVal_Diff-=60; 
                            }   
                        }//----------------------------------------
                    }
                    
                    if( KeyCatch(0,1 , Cancel)){

                        Cancel_SetTime_Cnt = CANCEL_TIME_SEC;
                        
                          TimeVal_Diff = 0;
                          Time_Set_Flag = 0;
                    }
                    //------------------------------------------------------------------------
            }
            
            NumberInsert_Time(&Time_Chage_InWorkout_Temp);
               

            
 
        }
        
    }

    

    /*
    if( KeyCatch(0,1,Key_Goal) ){  //(++   �ɶ�)

        switch(Program_Select){
          case Heart_Rate_Hill:     
          case Heart_Rate_Interval: 
          case Extreme_Heart_Rate:  
            Time_Change_Process_HRC_Hill_In_WorkOut(60);
            break;
          case EZ_INCLINE:  
          case MARATHON_Mode:
          case WeightLoss: 
          case APP_Cloud_Run:
          case APP_Train_Dist_Run:
          case APP_Train_Time_Run:  
            //---��ɶ�--//
            break;
          default:
            Time_Change_Process_InWorkout(60);
            break;
        }
        BarArray_Shift_Process();
    }
    
    if( KeyCatch(0,1 ,Key_Manual)){   // (-- �ɶ�)
        
        if(Program_Data.Goal_Time >240){
            if(Program_Data.Goal_Counter > 60){
     
                switch(Program_Select){
                    
                  case Heart_Rate_Hill:     
                  case Heart_Rate_Interval: 
                  case Extreme_Heart_Rate:  
                    Time_Change_Process_HRC_Hill_In_WorkOut(-60);
                    break;
                  case EZ_INCLINE:  
                  case MARATHON_Mode:
                  case WeightLoss: 
                    //---��ɶ�--//
                    break;
                  default:
                    Time_Change_Process_InWorkout(-60);
                    break;
                }
                BarArray_Shift_Process();    
            }   
        }
    }*/
    

}


unsigned short Time_Set = 600;
unsigned char Cnt_Set = 2;


unsigned int uiAvgSpeed_Tmp;
unsigned int uiAvgSpeed;

unsigned int uiAvgIncline_Tmp;
unsigned int uiAvgIncline;

unsigned int uiAvgHeartRate_Tmp;
unsigned int uiAvgHeartRate;

unsigned int uiAvg_Cnt;
unsigned int uiAvgHR_Cnt;

void ResetWorkoutAvgParam(){
    
    uiAvgSpeed_Tmp = 0;
    uiAvgSpeed     = 0;
    
    uiAvgIncline_Tmp = 0;
    uiAvgIncline     = 0;
    
    uiAvgHeartRate_Tmp = 0;
    uiAvgHeartRate     = 0; 
    
    uiAvg_Cnt    = 0;
    uiAvgHR_Cnt  = 0;
}

//�C�j  "�@�� " ����
unsigned int step_old;
unsigned char NoStep_Cnt;

unsigned short MaxHeart85Percent;//��85%�߸�

unsigned int ODO_RecordCnt;      //�����ɶ�
unsigned int ODO_RecordDistance; //�����Z��

void TimeProcess(){

    
    //-----------------5�����g�@��ODO----------------------
    if(ONE_SEC_Cnt_Value == 1000){
        
        if(ODO_RecordCnt%300==0){            
            Machine_Data_Update();          
        }
        
        ODO_RecordDistance += (System_SPEED *10000) / 3600 ;
        ODO_RecordCnt++;
    }
    //--------------------------------------------------------------------------------------
    
     
    
#if AutoPause
    
    //---------�]�a�W�L�H�P�_ 30s �S���W�[�B��   �i�J�Ȱ� ----------
    
    if(AutoPause_Flag == 1){  //���å\��   ���ե�
    
        if(step_old != Program_Data.Pace){
            step_old = Program_Data.Pace;
            NoStep_Cnt = 0;
        }
        if(step_old == Program_Data.Pace){
            NoStep_Cnt++;
        }
        
        if(NoStep_Cnt == 30){   //
            IntoPauseMode_Process(); 
            Btm_Task_Adder(FTMS_Data_Broadcast); //Page 0 //�s���B�ʸ�� �k�s
            Btm_Task_Adder(FTMS_Data_Broadcast); //Page 1
            Set_SPEED_Value(0);
            RM6_Task_Adder(Set_SPEED);
            RM6_Task_Adder(Motor_STOP);
            Buzzer_BeeBee(Time_Set, Cnt_Set);
            NoStep_Cnt = 0;
        }
        
    }

    //-------------------------------------
#endif
    
    
    //----------��B�ʥ����t�t-----
    if(System_SPEED != 0){
        uiAvg_Cnt++; 
        uiAvgSpeed_Tmp += System_SPEED;
        uiAvgSpeed = uiAvgSpeed_Tmp / uiAvg_Cnt;
    }

    
    //----------��B�ʥ�������-----
    uiAvgIncline_Tmp += System_INCLINE;
    uiAvgIncline = uiAvgIncline_Tmp / uiAvg_Cnt;
    
    
    
    
    //------�߸������p�� -----------
    if(HeartRate_Is_Exist_Flag==1){
        if(usNowHeartRate >0){
            
            uiAvgHR_Cnt++;
            
            uiAvgHeartRate_Tmp += usNowHeartRate;
            uiAvgHeartRate = uiAvgHeartRate_Tmp / uiAvgHR_Cnt;
            
            
        }
    }
    //----------------------------------------------
    

    //�p��B�ʱj��
    Mets_Calculate();
    //�p�Ⱚ��
    ALTI_Calculate();
    //�p����ӥd����
    Calorie_Calculate(); 
    
    if(Program_Select == APP_Cloud_Run){
        
         if((Program_Data.Distance + (System_SPEED *10000) / 3600 )/100  >= Program_Data.Distance_Goal){
           Program_Data.Distance = Program_Data.Distance_Goal*100;
          
        }else{
           Program_Data.Distance += (System_SPEED *10000) / 3600 ;
        }
        
    }else{
        Program_Data.Distance += (System_SPEED *10000) / 3600 ;   //�C+1��
    
    }
    

    //---------------------------�B�ʵ�������P�_-----------------------------------------//
    
    if(Program_Select == APP_Cloud_Run){   //���]�Ҧ�
    
        unsigned int uiAppTotalDistTmp;
        
        if(System_Unit == Metric){  
           uiAppTotalDistTmp = uiAppTotalDist;
        }else if(System_Unit == Imperial){
           uiAppTotalDistTmp = (uiAppTotalDist * 10)/16;
        }
       
        if(Cloud_0x39_Info.c_Distance >= uiAppTotalDistTmp){  //�Z���F�ФF
            
            IntoSummaryMode_Process();
            
            
            Set_SPEED_Value(0);
            RM6_Task_Adder(Set_SPEED);
            RM6_Task_Adder(Motor_STOP);
            
            System_INCLINE = 0;
            RM6_Task_Adder(Set_INCLINE);
            
            Buzzer_BeeBee(Time_Set, Cnt_Set);
        }
    }
    
    if(Program_Select == APP_Train_Dist_Run){   //�V�m�p�e  (�Z��)
    
        unsigned int uiAppTotalDistTmp;
        
        if(System_Unit == Metric){  
           uiAppTotalDistTmp = uiAppTotalDist;
        }else if(System_Unit == Imperial){
           uiAppTotalDistTmp = (uiAppTotalDist * 10)/16;
        }
       
        if(Cloud_0x39_Info.c_Distance >= uiAppTotalDistTmp){  //�Z���F�ФF
            
            IntoSummaryMode_Process();
             
            
            Set_SPEED_Value(0);
            RM6_Task_Adder(Set_SPEED);
            RM6_Task_Adder(Motor_STOP);
            
            System_INCLINE = 0;
            RM6_Task_Adder(Set_INCLINE);
            
            Buzzer_BeeBee(Time_Set, Cnt_Set);
        }
    }   
    
    //------�p�G�O�߸��Ҧ�   �߸���M�_�u�F  �W�L 60��S�����s�s�u  �N�i�Jworkout summary
    switch(Program_Select){
      case Target_HeartRate_Goal:
      case Fat_Burn:
      case Cardio:
      case Heart_Rate_Hill:   
      case Heart_Rate_Interval: 
      case Extreme_Heart_Rate: 
        if(usNowHeartRate == 0){
            
            if(No_HR_Value_Cnt >=60){
                
                IntoSummaryMode_Process();
                
                Set_SPEED_Value(0);
                RM6_Task_Adder(Set_SPEED);
                RM6_Task_Adder(Motor_STOP);
                
                System_INCLINE = 0;
                RM6_Task_Adder(Set_INCLINE);
                
                Buzzer_BeeBee(Time_Set, Cnt_Set);
                
            }
            
            No_HR_Value_Cnt++;
        }else{
            No_HR_Value_Cnt = 0;
        }
        break;
      case FIT_WFI:
        //-------------------------------------
        if(usNowHeartRate == 0){
            
            if(No_HR_Value_Cnt >=10){
                
                IntoSummaryMode_Process();
                
                Set_SPEED_Value(0);
                RM6_Task_Adder(Set_SPEED);
                RM6_Task_Adder(Motor_STOP);
                
                System_INCLINE = 0;
                RM6_Task_Adder(Set_INCLINE);
                Buzzer_BeeBee(Time_Set, Cnt_Set);
            }
            
            No_HR_Value_Cnt++;
        }else{
            No_HR_Value_Cnt = 0;
        }
        
        //--------------�߸� > �̤j�߸�85% �֭p10��j�������------
        MaxHeart85Percent = (Program_Data.MaxHeartRate * 85)/100;
        if(usNowHeartRate > MaxHeart85Percent ){

            if(HR_BiggerThan85Percent_Cnt >= 10){
                
                IntoSummaryMode_Process();
                
                Set_SPEED_Value(0);
                RM6_Task_Adder(Set_SPEED);
                RM6_Task_Adder(Motor_STOP);
                
                System_INCLINE = 0;
                RM6_Task_Adder(Set_INCLINE);
                
                Buzzer_BeeBee(Time_Set, Cnt_Set);
            }
            
            HR_BiggerThan85Percent_Cnt++;
        }else{
            HR_BiggerThan85Percent_Cnt = 0;
        }
        
        //---------------------------------------
        break;
      default:
        break;
        
    }
    
    //---------------------- �ؼХd�����Ҧ�--------------------------------
    if(Program_Select == Calorie_Goal){
        if((Program_Data.Calories / 100000) == Program_Data.Calories_Goal){
            
            //�B�ʵ���  �����k0
            System_INCLINE = 0;
            RM6_Task_Adder(Set_INCLINE);
            
            Set_SPEED_Value((System_SPEED * 3)/10 );
            if( System_SPEED < Machine_Data.System_SPEED_Min){
                System_SPEED = Machine_Data.System_SPEED_Min;
            }
            
            RM6_Task_Adder(Set_SPEED);
            IntoCoolDownModeProcess();
            Buzzer_BeeBee(Time_Set, Cnt_Set);
        }
    }
    
    //------------�ؼжZ���Ҧ�-------------------------------------------------------
    if((Program_Select == Distance_Goal_160M) || (Program_Select == Distance_Goal_5K) || 
       (Program_Select == Distance_Goal_10K) ){
           
        if((Program_Data.Distance / 100) >= Program_Data.Distance_Goal){
            
            Program_Data.Distance = Program_Data.Distance_Goal *100; //�����z��
            
            //�B�ʵ���  �����k0
            System_INCLINE = 0;
            RM6_Task_Adder(Set_INCLINE);
            
            Set_SPEED_Value((System_SPEED * 3)/10 );
            if( System_SPEED < Machine_Data.System_SPEED_Min){
                System_SPEED = Machine_Data.System_SPEED_Min;
            }
            
            RM6_Task_Adder(Set_SPEED);
            IntoCoolDownModeProcess();  
             Buzzer_BeeBee(Time_Set, Cnt_Set);
        }
    }
    
    //------------Fit Test �Ҧ�-----------------------------------------------------
    if(  (Program_Select == FIT_ARMY)     || (Program_Select == FIT_NAVY)  ||   
         (Program_Select == FIT_AIRFORCE) || (Program_Select == FIT_USMC)  ){
           
           if((Program_Data.Distance / 100) >= Program_Data.Distance_Goal){
               
               GetFitTest_Score();
               
               //�B�ʵ���  �����k0
               System_INCLINE = 0;
               RM6_Task_Adder(Set_INCLINE);
               
               Set_SPEED_Value(0);
               RM6_Task_Adder(Set_SPEED);
               IntoSummaryMode_Process();  
               Buzzer_BeeBee(Time_Set, Cnt_Set);
           }
       }
    
    
    //-----------------    �ɶ��P�_�Ҧ�   -----------------------------------
    if(Program_Data.Goal_Counter > 0){
        Program_Data.Goal_Counter--;
    }else if(Program_Data.Goal_Counter == 0){
        //�B�ʵ���  �����k0
        System_INCLINE = 0;
        RM6_Task_Adder(Set_INCLINE);
        
        switch(Program_Select){            
          case Quick_start:
          case APP_Train_Time_Run:       //�V�m�p�e  (�ɶ�)  
          case FIT_WFI:
            //-----------Nick �o�{��Bug  �ɶ��V�m�Ҧ��F���ؼ� �]�a�S�����U��------------
            Set_SPEED_Value(0);
            RM6_Task_Adder(Set_SPEED);
            //------------------------------
            IntoSummaryMode_Process();
            break;
          default:
            Set_SPEED_Value((System_SPEED * 3)/10 );
            if( System_SPEED < Machine_Data.System_SPEED_Min){
                System_SPEED = Machine_Data.System_SPEED_Min;
            }
            RM6_Task_Adder(Set_SPEED);
            IntoCoolDownModeProcess();
            break;
        } 
        Buzzer_BeeBee(Time_Set, Cnt_Set);
    }
    //-------------------------------------------------------------------------------------

}

//---------------  �i�J�U�@��period������P�_   -----------------------
unsigned int GenNewBarDataIndexofStart; 

unsigned char TimePeroid_Process(){

    if(Program_Data.Goal_Counter == Program_Data.NextPeriodValue){
        
        Program_Data.NowPeriodIndex++;
        
        if( Program_Data.NextPeriodValue - Program_Data.PeriodWidth > 4290000000 ){
            Program_Data.NextPeriodValue = 0;
        }else{
            Program_Data.NextPeriodValue -= Program_Data.PeriodWidth;
        }
        
        
        if(Program_Select == APP_Train_Time_Run){
    
            if( (Program_Data.NowPeriodIndex - 30) % 32 == 0 && (Program_Data.NowPeriodIndex > 32)){
            
                Program_Data.MasterPage = ((Program_Data.NowPeriodIndex - 30) /32) % 3;
                GenNewBarDataIndexofStart  = (Program_Data.NowPeriodIndex - 30) + 64;
                
                //--------------------------------------------------------------
                
                short Incline_Diff;
                short Speed_Diff;
                
                Speed_Diff   = Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96]   - CloudRun_Init_INFO.CloudRun_Spd_Buffer[Program_Data.NowPeriodIndex];
                Incline_Diff = Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] - CloudRun_Init_INFO.CloudRun_Inc_Buffer[Program_Data.NowPeriodIndex];
                
                
                //----------����------------Table96 ���s����
                for(unsigned int i = GenNewBarDataIndexofStart; i < GenNewBarDataIndexofStart + 32 ; i++){
                    Program_Data.INCLINE_Table_96[i%96] = CloudRun_Init_INFO.CloudRun_Inc_Buffer[i] + Incline_Diff; 
                }
                //----------�t��------------Table96 ���s����
                for(unsigned int i = GenNewBarDataIndexofStart; i < GenNewBarDataIndexofStart + 32 ; i++){
                    Program_Data.SPEED_Table_96[i%96] = CloudRun_Init_INFO.CloudRun_Spd_Buffer[i] + Speed_Diff;
                } 
                
            }
            
            
            
            
        }else{
        
            if( (Program_Data.NowPeriodIndex - 30) % 32 == 0 && (Program_Data.NowPeriodIndex > 32)){
            
                Program_Data.MasterPage = ((Program_Data.NowPeriodIndex - 30) /32) % 3;
                GenNewBarDataIndexofStart  = (Program_Data.NowPeriodIndex - 30) + 64;
            
                for(unsigned int i = GenNewBarDataIndexofStart; i < GenNewBarDataIndexofStart + 32 ; i++){
                    Program_Data.INCLINE_Table_96[i%96] = Program_Data.INCLINE_Template_Table[(i - Program_Data.Template_Loop_Start_Index) % Program_Data.Template_Table_Num]; 
                }
            } 
        }
    

        switch(Program_Select){
          case Target_HeartRate_Goal:
          case Fat_Burn: 
          case Cardio:   
          case Heart_Rate_Hill: 
          case Heart_Rate_Interval: 
          case Extreme_Heart_Rate: 
            break;
            
          default:
            //--------------------------------------------------------------------------------//
            System_INCLINE = Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] * 5;
       
            if(Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] < 0){
                System_INCLINE = 0;
            }else if(System_INCLINE > 150){
                System_INCLINE = 150;
            }
            
            RM6_Task_Adder(Set_INCLINE);
            //--------------------------------------------------------------------------------//
            
            //----(�t�פ���)�ϥγo��----�]�w�t��----------�V�m�Ҧ�(�ɶ�)----------------------
            /*if(Program_Select == APP_Train_Time_Run){
                System_SPEED  = CloudRun_Init_INFO.CloudRun_Spd_Buffer[Program_Data.NowPeriodIndex];
                if(System_SPEED>Machine_Data.System_SPEED_Max){
                    System_SPEED = Machine_Data.System_SPEED_Max;
                }else if(System_SPEED<Machine_Data.System_SPEED_Min){
                    System_SPEED = Machine_Data.System_SPEED_Min;
                }
                RM6_Task_Adder(Set_SPEED);
            }*/
            
            //--(�t�׷|��)-------�]�w�t��----------�V�m�Ҧ�(�ɶ�)---------------------
            if(Program_Select == APP_Train_Time_Run){
                
                Buzzer_BeeBee(300, 3);
                System_SPEED = Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96];
             
                //-------�p�G�O�t���N�վ㬰�̤p�t��
                if(Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96]<0){
                    System_SPEED = Machine_Data.System_SPEED_Min;
                } 
                
                if(System_SPEED < Machine_Data.System_SPEED_Min){
                    System_SPEED = Machine_Data.System_SPEED_Min;
                }else if(System_SPEED > Machine_Data.System_SPEED_Max){
                    System_SPEED = Machine_Data.System_SPEED_Max;
                }
                RM6_Task_Adder(Set_SPEED);
            }
            
            //--  FIT Test  WFI  ������   �t�ץѵ{������   �ϥΪ̤���վ�---------------------
            if(Program_Select == FIT_WFI){
                
                if(Program_Data.NowPeriodIndex == 6){
                    Hint_Disp_Flag = 2;   //��� BEGINING
                }
                
                if(Program_Data.NowPeriodIndex == 28){
                    Hint_Disp_Flag = 3;   //��� Cool Down
                }
                
                if(Program_Data.NowPeriodIndex < 29){ //�i�J cool down �H�e�ѵ{������
                    
                    System_SPEED = Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96];
                    
                    if( (Program_Data.NowPeriodIndex == 6) ||(Program_Data.NowPeriodIndex == 10) ||
                        (Program_Data.NowPeriodIndex == 14)||(Program_Data.NowPeriodIndex == 18) ||
                        (Program_Data.NowPeriodIndex == 22)||(Program_Data.NowPeriodIndex == 26) ||
                        (Program_Data.NowPeriodIndex == 28) ){
                            
                            Buzzer_BeeBee(300, 3);
                    }
                    
                    
                    //-------�p�G�O�t���N�վ㬰�̤p�t��
                    if(Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96]<0){
                        System_SPEED = Machine_Data.System_SPEED_Min;
                    } 
                    if(System_SPEED < Machine_Data.System_SPEED_Min){
                        System_SPEED = Machine_Data.System_SPEED_Min;
                    }else if(System_SPEED > Machine_Data.System_SPEED_Max){
                        System_SPEED = Machine_Data.System_SPEED_Max;
                    }
                    RM6_Task_Adder(Set_SPEED);
                    
                }

            }
            
            
            
            break;
        }
        return 1; 
    }
    return 0;  
}

//----------------------�ثe�u�� Cloud Run�Ψ�-----------------------------------------------//
unsigned char DistancePeroid_Process(){

    //---------------  �i�J�U�@��period������P�_   -----------------------
    
    if(Program_Data.Distance/100 >= (Program_Data.Distance_Goal - Program_Data.NextPeriodValue) ){
        
        Program_Data.NowPeriodIndex++;
        
        //�����
        if( Program_Data.NextPeriodValue - Program_Data.PeriodWidth > 4290000000 ){
            Program_Data.NextPeriodValue = 0;
        }else{
            Program_Data.NextPeriodValue -= Program_Data.PeriodWidth;   //�]�w�U�@�Ӫ��e
        }
     
        //---------------------INC Table Shift �B�z--------------------------------
        if( (Program_Data.NowPeriodIndex - 30) % 32 == 0 && (Program_Data.NowPeriodIndex > 32)){
            
            Program_Data.MasterPage = ((Program_Data.NowPeriodIndex - 30) /32) % 3;
            GenNewBarDataIndexofStart  = (Program_Data.NowPeriodIndex - 30) + 64;
           
            short Incline_Diff;
            short Speed_Diff;
            
            Speed_Diff   = Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96]   - CloudRun_Init_INFO.CloudRun_Spd_Buffer[Program_Data.NowPeriodIndex];
            Incline_Diff = Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] - CloudRun_Init_INFO.CloudRun_Inc_Buffer[Program_Data.NowPeriodIndex];
            
            
            //----------����------------Table96 ���s����
            for(unsigned int i = GenNewBarDataIndexofStart; i < GenNewBarDataIndexofStart + 32 ; i++){
                Program_Data.INCLINE_Table_96[i%96] = CloudRun_Init_INFO.CloudRun_Inc_Buffer[i] + Incline_Diff;
            }
            //----------�t��------------Table96 ���s����
            for(unsigned int i = GenNewBarDataIndexofStart; i < GenNewBarDataIndexofStart + 32 ; i++){
                Program_Data.SPEED_Table_96[i%96] = CloudRun_Init_INFO.CloudRun_Spd_Buffer[i] + Speed_Diff; 
            } 
            
        }

        switch(Program_Select){
          case Target_HeartRate_Goal:
          case Fat_Burn: 
          case Cardio:   
          case Heart_Rate_Hill: 
          case Heart_Rate_Interval: 
          case Extreme_Heart_Rate: 
            break;
            
          default:
            //-------------�]�w����--------------------------------------
            
            
            System_INCLINE = Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] * 5;
            
            if(Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] < 0){
                System_INCLINE = 0;
            }else if(System_INCLINE > 150){
                System_INCLINE = 150;
            }
            
            /*
            if(System_INCLINE > 150){
                System_INCLINE = 150;
            }else if(Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] < 0){
                System_INCLINE = 0;
            } */
            
            RM6_Task_Adder(Set_INCLINE);
            
            //-----(�t�פ���)�ϥγo��---�]�w�t��----------�V�m�Ҧ�(�Z��) Only-------------------------
            /*if(Program_Select == APP_Train_Dist_Run){
                System_SPEED  = CloudRun_Init_INFO.CloudRun_Spd_Buffer[Program_Data.NowPeriodIndex];
                if(System_SPEED>Machine_Data.System_SPEED_Max){
                    System_SPEED = Machine_Data.System_SPEED_Max;
                }else if(System_SPEED<Machine_Data.System_SPEED_Min){
                    System_SPEED = Machine_Data.System_SPEED_Min;
                }
                RM6_Task_Adder(Set_SPEED);
            }*/
            //--(�t�׷|��)-------�]�w�t��----------�V�m�Ҧ�(�ɶ�)---------------------
            if(Program_Select == APP_Train_Dist_Run){
            
                Buzzer_BeeBee(300, 3);
                System_SPEED = Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96];
                 
                //-------�p�G�O�t���N�վ㬰�̤p�t��
                if(Program_Data.SPEED_Table_96[Program_Data.NowPeriodIndex%96]<0){
                    System_SPEED = Machine_Data.System_SPEED_Min;
                }                
                if(System_SPEED < Machine_Data.System_SPEED_Min){
                    System_SPEED = Machine_Data.System_SPEED_Min;
                }else if(System_SPEED > Machine_Data.System_SPEED_Max){
                    System_SPEED = Machine_Data.System_SPEED_Max;
                }
                RM6_Task_Adder(Set_SPEED);
            } 
            
            
 
            break;
        }
        return 1;   
    }
    return 0;

}

//���ɭȶW�L�W�� �ΤU��    �N�ƭȰ��վ�
//�� 0~~ 30 �ন  1~8 
short Fix_INCLINE_Limit(short INCLINE_Value){

    if(INCLINE_Value>= 0x00 ){
        
        if(INCLINE_Value > 30){
            return 30;
        }else{
            return INCLINE_Value;
        }
        
    }else if(INCLINE_Value < 0){
         return 0;          //�p��0 �N���1��
    } 
    
    return INCLINE_Value;
}



unsigned char Fix_SPEED_Limit(short Speed_Value){

    if(Speed_Value < Machine_Data.System_SPEED_Min){
         return 1;          //�p��0 �N���1��
    }else if(Speed_Value >= Machine_Data.System_SPEED_Min &&  Speed_Value < Machine_Data.System_SPEED_Max ){
        return Speed_Value/30 + 1;
    }else if(Speed_Value >= Machine_Data.System_SPEED_Max){
        return 8;
    }else{
        return 1;
    }
       
}


unsigned short offsetValue;
void BarArray_Shift_Process(){
    
    if(Program_Data.PeriodNumber>32){
        Shift_Times = Program_Data.PeriodNumber - 32; //��ݭn�첾�����
    }else{
        Shift_Times = 0;
    }
    

    if(Shift_Times == 0){  //�p�G �ɶ��p�� 32����  ��ܤ��ݭn�첾  inddex �]���γB�z
        
        //Index (1)
        Program_Data.PeriodIndex_After_Shift = Program_Data.NowPeriodIndex; //������쥻��
        //BarArray  (1)
        for(unsigned char i = 0;  i < 32; i++){
            //�� 0~~ 30 �ন  1~8 
            Program_Data.BarArray_Display[i] =  Index_To_Bar[ Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])] ;  
            //��0.5~~ 24.0 �ন  1~8 
            Program_Data.BarArray_Display_Speed[i] = Fix_SPEED_Limit(Program_Data.SPEED_Table_96[i%96]); 
        }
          
    }else if(Shift_Times > 0){     //�ݭn�첾������
        
        if( Program_Data.NowPeriodIndex < 16 ){ //��lindex < 16    0~15  ���index�d��  0 ~ 31
             
            //Index (1)
            Program_Data.PeriodIndex_After_Shift = Program_Data.NowPeriodIndex; //������쥻��
            
            //BarArray (1)
            for(unsigned char i = 0;  i < 32; i++){
                //�� 0~~ 30 �ন  1~8 
                Program_Data.BarArray_Display[i] =  Index_To_Bar[ Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])] ;  
                //��0.5~~ 24.0 �ন  1~8 
                Program_Data.BarArray_Display_Speed[i] = Fix_SPEED_Limit(Program_Data.SPEED_Table_96[i%96]); 
                
            }
            
        }else if( (Program_Data.NowPeriodIndex > 15)&&(Program_Data.NowPeriodIndex < (16 + Shift_Times) ) ){  //�ϰ� index ����
            
            //Index (2)
            Program_Data.PeriodIndex_After_Shift = 15; //index �d�b15  ���ϧΩ��e�첾�N�n
            
            //BarArray (2)
            offsetValue = (Program_Data.NowPeriodIndex - 15);  //������m

            //**********************************************************************

            for(unsigned int i = offsetValue;  i < (32 + offsetValue) ; i++){
                //�� 0~~ 30 �ন  1~8 
                Program_Data.BarArray_Display[i - offsetValue] =Index_To_Bar[Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])];
                //��0.5~~ 24.0 �ন  1~8 
                Program_Data.BarArray_Display_Speed[i - offsetValue] = Fix_SPEED_Limit(Program_Data.SPEED_Table_96[i%96]); 
                
            }
            
        }else if( Program_Data.NowPeriodIndex > (15 + Shift_Times) ){  //�ϧΩT�w����  index �}�l���ᨫ
            
            //Index (3)
            Program_Data.PeriodIndex_After_Shift = Program_Data.NowPeriodIndex - Shift_Times;  
            
            //BarArray (3)
            for(unsigned int i = Shift_Times;  i < (32 + Shift_Times) ; i++){
                //�� 0~~ 30 �ন  1~8 
                Program_Data.BarArray_Display[i - Shift_Times]       = Index_To_Bar[Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])];
                //��0.5~~ 24.0 �ন  1~8 
                Program_Data.BarArray_Display_Speed[i - Shift_Times] = Fix_SPEED_Limit(Program_Data.SPEED_Table_96[i%96]); 
            }
        } 
    } 
}

void THR_INCLINE_Process(){
 
        if( (usNowHeartRate >= Program_Data.TargetHeartRate - 2) && (usNowHeartRate <= Program_Data.TargetHeartRate + 2) ){
            Program_Data.HeartRatePeroidCnt = 0;
        }
        if( (usNowHeartRate > Program_Data.TargetHeartRate - 10) && (usNowHeartRate < Program_Data.TargetHeartRate - 3) ){
            if( Program_Data.HeartRatePeroidCnt == 30){
                Program_Data.HeartRatePeroidCnt = 0;
                if(System_INCLINE <150){
                    System_INCLINE += 10;
                    RM6_Task_Adder(Set_INCLINE);
                    //Update_BarArray_Data_Ex();
                } 
            }  
        }
        if( (usNowHeartRate > Program_Data.TargetHeartRate - 25) && (usNowHeartRate < Program_Data.TargetHeartRate - 11) ){
            
            if( Program_Data.HeartRatePeroidCnt >= 15){
                Program_Data.HeartRatePeroidCnt = 0;
                
                if(System_INCLINE <150){
                    System_INCLINE += 10;
                    RM6_Task_Adder(Set_INCLINE);
                    //Update_BarArray_Data_Ex();
                } 
            }  
        }
        if( usNowHeartRate < Program_Data.TargetHeartRate - 26 ){
            
            if( Program_Data.HeartRatePeroidCnt >= 10){
                Program_Data.HeartRatePeroidCnt = 0;
                
                if(System_INCLINE <150){
                    System_INCLINE += 10;
                    RM6_Task_Adder(Set_INCLINE);
                    //Update_BarArray_Data_Ex();
                } 
            }  
        }
        if((usNowHeartRate >= Program_Data.TargetHeartRate + 3) ){
        
            if( Program_Data.HeartRatePeroidCnt >= 15){
                Program_Data.HeartRatePeroidCnt = 0;
                
                if(System_INCLINE > 0){
                    System_INCLINE -= 10;
                    RM6_Task_Adder(Set_INCLINE);
                    //Update_BarArray_Data_Ex();
                } 
            }  
        }
        Program_Data.HeartRatePeroidCnt ++;
}

void FatBurn_Carido_INCLINE_Process(){
        
        if( (usNowHeartRate >= Program_Data.HR_Range_Low) && (usNowHeartRate <= Program_Data.HR_Range_High) ){
            Program_Data.HeartRatePeroidCnt = 0;
        }
        if( usNowHeartRate <  Program_Data.HR_Range_Low ){
            
            if( Program_Data.HeartRatePeroidCnt == 30){
                Program_Data.HeartRatePeroidCnt = 0;
                if(System_INCLINE <150){
                    System_INCLINE += 10;
                    RM6_Task_Adder(Set_INCLINE);
                    //Update_BarArray_Data_Ex();
                } 
            }  
        }
        if( usNowHeartRate > Program_Data.HR_Range_High){
            
            if( Program_Data.HeartRatePeroidCnt >= 30){
                Program_Data.HeartRatePeroidCnt = 0;
                if(System_INCLINE > 0){
                    System_INCLINE -= 10;
                    RM6_Task_Adder(Set_INCLINE);
                    //Update_BarArray_Data_Ex();
                } 
            }  
        }
       
        Program_Data.HeartRatePeroidCnt ++;
    
}

unsigned short High_Limit;
unsigned short Low_Limit;
void HRC_Rate_INCLINE_Process(){

    Low_Limit =  ( (Program_Data.TargetHeartRate * Program_Data.HRC_INCLINE_Rate_Table[Program_Data.NowPeriodIndex] )/100)- 3;
    High_Limit = ( (Program_Data.TargetHeartRate * Program_Data.HRC_INCLINE_Rate_Table[Program_Data.NowPeriodIndex] )/100) + 3;
    
    if( (usNowHeartRate >= Low_Limit) && (usNowHeartRate <= High_Limit) ){
        Program_Data.HeartRatePeroidCnt = 0;
    }
    
    if( usNowHeartRate <  Low_Limit ){
        
        if( Program_Data.HeartRatePeroidCnt == 15){
            Program_Data.HeartRatePeroidCnt = 0;
            
            if(System_INCLINE <150){
                System_INCLINE += 10;
                RM6_Task_Adder(Set_INCLINE);
                //Update_BarArray_Data_Ex();
            } 
        }  
    }
    
    if( usNowHeartRate > High_Limit){
        
        if( Program_Data.HeartRatePeroidCnt >= 15){
            Program_Data.HeartRatePeroidCnt = 0;
            
            if(System_INCLINE > 0){
                System_INCLINE -= 10;
                RM6_Task_Adder(Set_INCLINE);
                //Update_BarArray_Data_Ex();
            } 
        }  
    }
    Program_Data.HeartRatePeroidCnt++;
}

unsigned char Incline_Speed_BarArrayDisplay_Switch;



void Workout_Func(){
    

    Workout_Key();
    
    if(T5ms_Workout_Display_Flag){
        T5ms_Workout_Display_Flag = 0;
        //bar ��� + �{�{

        switch(Program_Select){
          case Quick_start:
          case Manual:
          case Random: 
          case CrossCountry: 
          case WeightLoss:   
          case Interval_1_1: 
          case Interval_1_2:
          case Hill: 
          case Heart_Rate_Hill:  
          case Heart_Rate_Interval: 
          case Extreme_Heart_Rate:   
          case Hill_Climb:   
          case Aerobic:      
          case Interval_1_4: 
          case Interval_2_1:
          case EZ_INCLINE:  
          case MARATHON_Mode:
          case Calorie_Goal:
          case Distance_Goal_160M:
          case Distance_Goal_5K:
          case Distance_Goal_10K:
          case Custom_1: 
          case Custom_2:
          case APP_Cloud_Run:
          case FIT_ARMY:
          case FIT_NAVY:
          case FIT_AIRFORCE: 
          case FIT_USMC:
            DrawBarArray_Workout(Program_Data.BarArray_Display ,Program_Data.PeriodIndex_After_Shift , 1 );
            break;
          case FIT_WFI: 
            if(Hint_Disp_Flag == 0){
                 DrawBarArray_Workout(Program_Data.BarArray_Display ,Program_Data.PeriodIndex_After_Shift , 1 );
            }
            break;
          case APP_Train_Dist_Run:
          case APP_Train_Time_Run:
            if(Incline_Speed_BarArrayDisplay_Switch == 0){
                DrawBarArray_Workout(Program_Data.BarArray_Display_Speed ,Program_Data.PeriodIndex_After_Shift , 1 );
            }else{
                DrawBarArray_Workout(Program_Data.BarArray_Display ,Program_Data.PeriodIndex_After_Shift , 1 );
            }
            break;
            
          case Target_HeartRate_Goal:
          case Fat_Burn:
          case Cardio:    
            Basic_HRC_Display();
            break;   
        }
  
        if(Program_Select == FIT_WFI){
            
            if(Hint_Disp_Flag == 0){
                writeLEDMatrix();
            }
          
        }else{
            writeLEDMatrix();
        }
        
        //Display
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
            SET_Seg_TIME_Display( TIME  ,Program_Data.Goal_Time - Program_Data.Goal_Counter);
            break;
            
          default:   //�w�]�U�����
            if(Time_Display_Type == Remaining){
                
                if(Time_Set_Flag == 1){
                    Set_TimeVal_Temp = Program_Data.Goal_Counter;
                      
                    if(Time_KeyPad_Iput_Flag == 0){ //�@��+- ���
                        if((Set_TimeVal_Temp + TimeVal_Diff) < 3600){        //�p��1�p��
                            TIME_SET_Display( TIME  , Set_TimeVal_Temp + TimeVal_Diff ,0x0C , 0);
                        }else if((Set_TimeVal_Temp + TimeVal_Diff) >= 3600){ //�j��1�p��
                            TIME_SET_Display( TIME  , Set_TimeVal_Temp + TimeVal_Diff,0x03 , 0); 
                        }
                    }else if(Time_KeyPad_Iput_Flag == 1){  //--KeyPad��J���
                        SET_Seg_Display(TIME , Time_Chage_InWorkout_Temp/60 , ND , DEC );  
                    }
                    
                }else{
                    SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Counter);    //�ѤU�h�֮ɶ�  �U��
                }
                                        
            }else if(Time_Display_Type == Elspsed){
                SET_Seg_TIME_Display( TIME  ,Program_Data.Goal_Time - Program_Data.Goal_Counter);    //�g�L�h�֮ɶ� �W��
            }
            break;
            
        }
        writeSegmentBuffer();
    }
    
    if(Program_Select == FIT_WFI){
        
        if(Hint_Disp_Flag == 1){
            if(T_Marquee_Flag){
                T_Marquee_Flag = 0;
                
                if(F_String_buffer_Auto_Middle( Left, "WARM    UP" ,55 ,0) == 1){
                    Hint_Disp_Flag = 0;
                }
                writeLEDMatrix();
            }
        }
            
        if(Hint_Disp_Flag == 2){
            if(T_Marquee_Flag){
                T_Marquee_Flag = 0;
                
                if( F_String_buffer_Auto_Middle( Left, "BEGINING" ,55 ,0) == 1){
                    Hint_Disp_Flag = 0;
                }
                writeLEDMatrix();
            }
        }
        
        if(Hint_Disp_Flag == 3){
            if(T_Marquee_Flag){
                T_Marquee_Flag = 0;
                
                if( F_String_buffer_Auto_Middle( Left, "COOL    DOWN" ,55 ,0) == 1){
                    Hint_Disp_Flag = 0;
                }
                writeLEDMatrix();
            }
        }
   
    }

    

    if(T1s_Flag){     //�ɶ��p��  ���:��
        T1s_Flag = 0;

         Btm_Task_Adder(FTMS_Data_Broadcast);   
        
         
         if(Cancel_SetTime_Cnt>0){
             Cancel_SetTime_Cnt--;
         }
         
         if(Time_Set_Flag == 1){
             if(Cancel_SetTime_Cnt == 0){
                 Time_Set_Flag = 0;
                 Time_KeyPad_Iput_Flag = 0;
                 Number_Digit_Tmp =0;
             }
         }
         
         
        //Check �ثe��F���@�� Period     //�b��F�U�@�� period �ɥh�P�_�n���첾�}�C
         //�v�ɼҦ� �M   �V�m�p�e(�Z��)
        if(Program_Select == APP_Cloud_Run || Program_Select == APP_Train_Dist_Run || 
           Program_Select == FIT_ARMY      || Program_Select == FIT_NAVY ||
           Program_Select == FIT_AIRFORCE  || Program_Select == FIT_USMC   ){
               
            if(DistancePeroid_Process() == 1){  //�ζZ���Ŷq
                BarArray_Shift_Process();
            }
        }else{    //case APP_Train_Time_Run:
            
            if(TimePeroid_Process() == 1){      //�ήɶ��Ŷq
                BarArray_Shift_Process();
            }  
        }
           
        if(usNowHeartRate != 0){
            No_HR_Value_Cnt = 0;
            
            switch(Program_Select){
              case Target_HeartRate_Goal:
                THR_INCLINE_Process();
                break;
                
              case Fat_Burn:
              case Cardio:
                FatBurn_Carido_INCLINE_Process();
                break;
                
              case Heart_Rate_Hill:   
              case Heart_Rate_Interval: 
              case Extreme_Heart_Rate: 
                HRC_Rate_INCLINE_Process();
                break;
            }
        }
        TimeProcess();
    }
    
    CloudRun_Workout_Data_Broadcast(); //500ms ��
   
    
    
}
