#include "system.h"

unsigned int Shift_Times;
unsigned char No_HR_Value_Cnt; //�P�_�B�ʤ�  60��S���߸� �N�i�Jsummary ��cnt
void BarArray_Shift_Process();
unsigned char TimePeroid_Process();

void IntoWorkoutModeProcess(){
    
    if(Program_Select == APP_Cloud_Run){
        System_INCLINE = Program_Data.INCLINE_Table_96[0] * 5;
        RM6_Task_Adder(Set_INCLINE);
    }
    
    WarmUp_3_Minute_Cnt = 0;
    System_Mode = Workout;
    ClearStd_1_Sec_Cnt(); 

}

short Now_INCLINE_Oringin_Value;
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

    //--�˥��]�n�����----
    for(unsigned char i = 0; i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] += Program_Data.INCLINE_ValueDiff;
    }
    
    if(Program_Data.MasterPage == 0){
        for(unsigned char i = 0 ; i < 96 ; i++){
            
            if(i >= Program_Data.NowPeriodIndex % 96 ){
                Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;
            }
        }
        
    }else if(Program_Data.MasterPage == 1){
        for(unsigned char i = 32 ; i < 96 ; i++){
            if(i >= Program_Data.NowPeriodIndex % 96 ){
                Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;   
            } 
        }
        for(unsigned char i = 0 ; i < 32 ; i++){
            Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;
        }
        
    }else if(Program_Data.MasterPage == 2){
        if( (Program_Data.NowPeriodIndex % 96 >= 64) && (Program_Data.NowPeriodIndex % 96 < 96) ){
            for(unsigned char i = 64 ; i < 128 ; i++){
                if(i >= Program_Data.NowPeriodIndex % 96 ){
                    Program_Data.INCLINE_Table_96[i%96] += Program_Data.INCLINE_ValueDiff;   
                }
            }
        }
        if( (Program_Data.NowPeriodIndex % 96 >= 0) && (Program_Data.NowPeriodIndex % 96 < 32) ){
            for(unsigned char i = 0 ; i < 32 ; i++){
                if(i >= Program_Data.NowPeriodIndex % 96 ){
                    Program_Data.INCLINE_Table_96[i%96] += Program_Data.INCLINE_ValueDiff; 
                }
            }
        }
        for(unsigned char i = 32 ; i < 64 ; i++){
            Program_Data.INCLINE_Table_96[i] += Program_Data.INCLINE_ValueDiff;
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
        //---�괭��--//
        return 0;
        break;
        
    }
 
    if(R_KeyCatch( Inc_Up) || R_KeyContinueProcess(Inc_Up)){
        if(System_INCLINE<Machine_Data.System_INCLINE_Max) System_INCLINE+=5;
        __asm("NOP");
         return 1;
    }
    
    if(R_KeyCatch( Inc_Down) || R_KeyContinueProcess(Inc_Down)){
        if(System_INCLINE>Machine_Data.System_INCLINE_Min)System_INCLINE-=5;
        __asm("NOP");
         return 1;
    }
    
    
    if( KeyCatch(0,1 , Inc_15) ){
        System_INCLINE = 150;
        return 1;
    }
    if( KeyCatch(0,1 , Inc_10) ){
        System_INCLINE = 100;
        return 1;
    }
    if( KeyCatch(0,1 , Inc_5) ){
        System_INCLINE = 50;
        return 1;
    }
    if( KeyCatch(0,1 , Inc_0) ){
        System_INCLINE = 0;
        return 1;
    }     
    
    return 0;
}

unsigned char Quick_SPEED__Key(){

    
    if(R_KeyCatch( Spd_Up) || R_KeyContinueProcess(Spd_Up)){
        if(System_SPEED < Machine_Data.System_SPEED_Max){
            System_SPEED += 1;
        }
          return 1;
    }
    
    if(R_KeyCatch( Spd_Down) || R_KeyContinueProcess(Spd_Down)){
        if(System_SPEED > Machine_Data.System_SPEED_Min){
            System_SPEED -=1;
        }
        return 1;
    }
    
      /*f( KeyCatch(0,1 , Key_Manual) || KeyForContinueProcess(Key_Manual)){    //�̥���
        
        if(System_SPEED < Machine_Data.System_SPEED_Max){
            System_SPEED += 1;
        }
        return 1;
    }
    if( KeyCatch(0,1 , Key_HRC) ||  KeyForContinueProcess(Key_HRC)){      // ��2
        
        if(System_SPEED > Machine_Data.System_SPEED_Min){
            System_SPEED -=1;
        }
        return 1;
    }*/
  
    return 0;
}

void Workout_Key(){

    if( KeyCatch(0,1 , Key_Manual)){    //�̥���
        
        //-------------------- ���� �B�� �B�T �B�W �B�Z-----
        Pace_Display_Switch ++;
        Pace_Display_Switch = Pace_Display_Switch % 4;
        //-------------------------------------------------
    }
    
    if( KeyCatch(0,1 , Enter) ){
        Btm_Task_Adder(Scan_ANT_HRC_Sensor);
        __asm("NOP");
    }   
    
    if( KeyCatch(0,1 , BLE) ){
        Btm_Task_Adder(Scan_BLE_HRC_Sensor);
        __asm("NOP");
    }    

    if( KeyCatch(0,1 , Key_Advance) ){
   
        //���ӥd����    �C�p�ɮ��ӥd���� ��ܤ���
        if(Calories_Display_Type == Cal_){
            Calories_Display_Type = Cal_HR;
        }else if(Calories_Display_Type == Cal_HR){
            Calories_Display_Type = Cal_;
        }

        __asm("NOP");
    }    
    
    if( KeyCatch(0,1 , Start) ){
        
        //-------- (�g�L)(�ѤU) �ɶ���ܤ��� ------        
        if(Time_Display_Type == Remaining){
            Time_Display_Type = Elspsed;
        }else if(Time_Display_Type == Elspsed){
            Time_Display_Type = Remaining;
        }
        
        __asm("NOP");
    }  
    
    if( KeyCatch(0,1 , Stop) ||  PauseKey() ){
        
        IntoPauseMode_Process(); 
        
        Btm_Task_Adder(FTMS_Data_Broadcast); //Page 0 //�s���B�ʸ�� �k�s
        Btm_Task_Adder(FTMS_Data_Broadcast); //Page 1
     
        Set_SPEED_Value(0);
        RM6_Task_Adder(Set_SPEED);
        RM6_Task_Adder(Motor_STOP);
        
        Buzzer_BeeBee(Time_Set, Cnt_Set);
    }
    
    if( KeyCatch(0,1 , Spd_12) ){  //(++   �ɶ�)

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
            //---��ɶ�--//
            break;
          default:
            Time_Change_Process_InWorkout(60);
            break;
        }
        BarArray_Shift_Process();
    }
    
    if( KeyCatch(0,1 , Spd_3) ){   // (-- �ɶ�)
        
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
    }
    
    if(Quick_SPEED__Key() == 1){
    
        Rst_Speed_Blink();
        //SPEED_Changing_Flag = 0;
       
        RM6_Task_Adder(Set_SPEED);
    
    }
    
    if(Quick_INCLINE__Key() == 1){
        
        //Rst_Incline_Blink();
        //INCL_Moveing_Flag = 0;
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
            //---�괭��--//
            break; 
        }
    }
}


unsigned short Time_Set = 600;
unsigned char Cnt_Set = 2;


unsigned int uiAvgSpeed_Tmp;
unsigned int uiAvgSpeed;
unsigned int uiAvgSpd_Cnt;
//�C�j  "�@�� " ����
void TimeProcess(){

    uiAvgSpeed_Tmp += System_SPEED;
    uiAvgSpd_Cnt++; 
    uiAvgSpeed = uiAvgSpeed_Tmp/ uiAvgSpd_Cnt;
    
    //�p��B�ʱj��
    Mets_Calculate();
    //�p�Ⱚ��
    ALTI_Calculate();
    //�p����ӥd����
    Calorie_Calculate(); 
    Program_Data.Distance += (System_SPEED *10000) / 3600 ;   //�C+1��
    
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
    
    //------�p�G�O�߸��Ҧ�   �߸���M�_�u�F  �W�L 60��S�����s�s�u  �N�i�Jworkout summary
    switch(Program_Select){
      case Target_HeartRate_Goal:
      case Fat_Burn:
      case Cardio:
      case Heart_Rate_Hill:   
      case Heart_Rate_Interval: 
      case Extreme_Heart_Rate: 
        //-------------------------------------
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
            RM6_Task_Adder(Set_SPEED);
            IntoCoolDownModeProcess();
        }
    }
    
    if((Program_Select == Distance_Goal_160M) || (Program_Select == Distance_Goal_5K) || (Program_Select == Distance_Goal_10K)){
        if((Program_Data.Distance / 100) >= Program_Data.Distance_Goal){
            
            //�B�ʵ���  �����k0
            System_INCLINE = 0;
            RM6_Task_Adder(Set_INCLINE);
            
            Set_SPEED_Value((System_SPEED * 3)/10 );
            RM6_Task_Adder(Set_SPEED);
            IntoCoolDownModeProcess();  
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
            IntoSummaryMode_Process();
            break;
          default:
            Set_SPEED_Value((System_SPEED * 3)/10 );
            RM6_Task_Adder(Set_SPEED);
            IntoCoolDownModeProcess();
            break;
        } 
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
        
        if( (Program_Data.NowPeriodIndex - 30) % 32 == 0 && (Program_Data.NowPeriodIndex > 32)){
            
            Program_Data.MasterPage = ((Program_Data.NowPeriodIndex - 30) /32) % 3;
            GenNewBarDataIndexofStart  = (Program_Data.NowPeriodIndex - 30) + 64;
            
            for(unsigned int i = GenNewBarDataIndexofStart; i < GenNewBarDataIndexofStart + 32 ; i++){
                Program_Data.INCLINE_Table_96[i%96] = Program_Data.INCLINE_Template_Table[(i - Program_Data.Template_Loop_Start_Index) % Program_Data.Template_Table_Num]; 
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
            if(System_INCLINE > 150){
                System_INCLINE = 150;
            }else if(Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] < 0){
                System_INCLINE = 0;
            } 
            RM6_Task_Adder(Set_INCLINE);
            //--------------------------------------------------------------------------------//
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
     
        if( (Program_Data.NowPeriodIndex - 30) % 32 == 0 && (Program_Data.NowPeriodIndex > 32)){
            
            Program_Data.MasterPage = ((Program_Data.NowPeriodIndex - 30) /32) % 3;
            GenNewBarDataIndexofStart  = (Program_Data.NowPeriodIndex - 30) + 64;
           
            for(unsigned int i = GenNewBarDataIndexofStart; i < GenNewBarDataIndexofStart + 32 ; i++){
                Program_Data.INCLINE_Table_96[i%96] = ucAppINCBuffer[i]; 
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
            System_INCLINE = Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] * 5;
            if(System_INCLINE > 150){
                System_INCLINE = 150;
            }else if(Program_Data.INCLINE_Table_96[Program_Data.NowPeriodIndex%96] < 0){
                System_INCLINE = 0;
            } 
            RM6_Task_Adder(Set_INCLINE);
            break;
        }
        return 1;   
    }
    return 0;

}

//���ɭȶW�L�W�� �ΤU��    �N�ƭȰ��վ�
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


unsigned short offsetValue;
void BarArray_Shift_Process(){
    
    Shift_Times = Program_Data.PeriodNumber - 32; //��ݭn�첾�����

    if(Shift_Times == 0){  //�p�G �ɶ��p�� 32����  ��ܤ��ݭn�첾  inddex �]���γB�z
        
        //Index (1)
        Program_Data.PeriodIndex_After_Shift = Program_Data.NowPeriodIndex; //������쥻��
        //BarArray  (1)
        for(unsigned char i = 0;  i < 32; i++){
            Program_Data.BarArray_Display[i] =  Index_To_Bar[ Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])] ;  
        }
          
    }else if(Shift_Times > 0){     //�ݭn�첾������
        
        if( Program_Data.NowPeriodIndex < 16 ){ //��lindex < 16    0~15  ���index�d��  0 ~ 31
             
            //Index (1)
            Program_Data.PeriodIndex_After_Shift = Program_Data.NowPeriodIndex; //������쥻��
            
            //BarArray (1)
            for(unsigned char i = 0;  i < 32; i++){
                Program_Data.BarArray_Display[i] =  Index_To_Bar[ Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])] ;  
            }
            
        }else if( (Program_Data.NowPeriodIndex > 15)&&(Program_Data.NowPeriodIndex < (16 + Shift_Times) ) ){  //�ϰ� index ����
            
            //Index (2)
            Program_Data.PeriodIndex_After_Shift = 15; //index �d�b15  ���ϧΩ��e�첾�N�n
            
            //BarArray (2)
            offsetValue = (Program_Data.NowPeriodIndex - 15);  //������m

            //**********************************************************************

            for(unsigned int i = offsetValue;  i < (32 + offsetValue) ; i++){
                Program_Data.BarArray_Display[i - offsetValue] =Index_To_Bar[Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])];
            }
            
        }else if( Program_Data.NowPeriodIndex > (15 + Shift_Times) ){  //�ϧΩT�w����  index �}�l���ᨫ
            
            //Index (3)
            Program_Data.PeriodIndex_After_Shift = Program_Data.NowPeriodIndex - Shift_Times;  
            
            //BarArray (3)
            for(unsigned int i = Shift_Times;  i < (32 + Shift_Times) ; i++){
                Program_Data.BarArray_Display[i - Shift_Times] = Index_To_Bar[Fix_INCLINE_Limit(Program_Data.INCLINE_Table_96[i%96])];
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

void Workout_Func(){
    
    ucSubSystemMode = C_App_RunningVal;
    
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
            DrawBarArray_Workout(Program_Data.BarArray_Display ,Program_Data.PeriodIndex_After_Shift , 1 );
            break;
            
          case Target_HeartRate_Goal:
          case Fat_Burn:
          case Cardio:    
            Basic_HRC_Display();
            break;   
        }
  
        writeLEDMatrix();
        
        //Display
        switch(Program_Select){
          case MARATHON_Mode:      //�ɶ��W�����
          case Quick_start:
          case Calorie_Goal:
          case Distance_Goal_160M:
          case Distance_Goal_5K:
          case Distance_Goal_10K:
          case APP_Cloud_Run:
            SET_Seg_TIME_Display( TIME  ,Program_Data.Goal_Time - Program_Data.Goal_Counter);
            break;
            
          default:   //�w�]�U�����
            if(Time_Display_Type == Remaining){
                SET_Seg_TIME_Display( TIME  , Program_Data.Goal_Counter);                           //�ѤU�h�֮ɶ�  �U��
            }else if(Time_Display_Type == Elspsed){
                SET_Seg_TIME_Display( TIME  ,Program_Data.Goal_Time - Program_Data.Goal_Counter);    //�g�L�h�֮ɶ� �W��
            }
            break;
            
        }
        writeSegmentBuffer();
    }

    if(T1s_Flag){     //�ɶ��p��  ���:��
        T1s_Flag = 0;

         Btm_Task_Adder(FTMS_Data_Broadcast);   
        
         
         
        //Check �ثe��F���@�� Period     //�b��F�U�@�� period �ɥh�P�_�n���첾�}�C
        if(Program_Select == APP_Cloud_Run){
            if(DistancePeroid_Process() == 1){  //�ζZ���Ŷq
                BarArray_Shift_Process();
            }
        }else{
            
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
