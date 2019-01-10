#include "system.h"

unsigned short stageNumber;

void Cloud_Run_Program_Init(){

    UserData_Init(); 
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = uiAppTotalDist;
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = (uiAppTotalDist * 10)/16;
    }
    
    
    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile �˥�  
    Program_Data.Template_Loop_Start_Index = 0;


    Program_Data.PeriodNumber = uiAppTotalDist/uiAppStageDist + 1;

    for(unsigned int i = 0 ; i < Program_Data.PeriodNumber; i++){
        if(i < 96){
            Program_Data.INCLINE_Table_96[i] = CloudRun_Init_INFO.CloudRun_Inc_Buffer[i]; 
        }
        
    }
    for(unsigned int i = Program_Data.PeriodNumber; i < 96;  i++){
        if(i < 96){
            Program_Data.INCLINE_Table_96[i] = 0; 
        }
       
    }

    Table_96_To_BarArray();
    /*for(unsigned char i = 0 ; i < 32; i++){
        
        if(Program_Data.INCLINE_Table_96[i] >= 0x00 ){
            
            if(Program_Data.INCLINE_Table_96[i] > 30){
                Program_Data.BarArray_Display[i] = Index_To_Bar[30];
            }else{
                Program_Data.BarArray_Display[i] = Index_To_Bar[ Program_Data.INCLINE_Table_96[i] ];
            }
            
        }else if(Program_Data.INCLINE_Table_96[i] < 0){
            
            Program_Data.BarArray_Display[i] = Index_To_Bar[0];          //�p��0 �N���1��
        } 
        
    }*/

    
    if(System_Unit == Metric){  
        Program_Data.PeriodWidth  = uiAppStageDist; 
    }else if(System_Unit == Imperial){
        Program_Data.PeriodWidth  = (uiAppStageDist * 10)/16; 
    }
    
    Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;


}


void Train_Dist_Run_Program_Init(){

    UserData_Init(); 
    
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = uiAppTotalDist;
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = (uiAppTotalDist * 10)/16;
    }
    
    
    Program_Data.Goal_Time    = 360000;  //15360
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile �˥�  
    Program_Data.Template_Loop_Start_Index = 0;


    //Program_Data.PeriodNumber = uiAppTotalDist/uiAppStageDist + 1;
    Program_Data.PeriodNumber = uiAppTotalDist/uiAppStageDist; 
    

    for(unsigned int i = 0 ; i < Program_Data.PeriodNumber; i++){
        if(i < 96){
            Program_Data.INCLINE_Table_96[i] = CloudRun_Init_INFO.CloudRun_Inc_Buffer[i]; 
            Program_Data.SPEED_Table_96[i]   = CloudRun_Init_INFO.CloudRun_Spd_Buffer[i];
        }
    }
    
    for(unsigned int i = Program_Data.PeriodNumber; i < 96;  i++){
        if(i < 96){
            Program_Data.INCLINE_Table_96[i] = 0; 
            Program_Data.SPEED_Table_96[i]   = 0;
        }   
    }

    
    Table_96_To_BarArray();
    /*
    for(unsigned char i = 0 ; i < 32; i++){
        if(Program_Data.INCLINE_Table_96[i] >= 0x00 ){
            if(Program_Data.INCLINE_Table_96[i] > 30){
                Program_Data.BarArray_Display[i] = Index_To_Bar[30];
            }else{
                Program_Data.BarArray_Display[i] = Index_To_Bar[ Program_Data.INCLINE_Table_96[i] ];
            }     
        }else if(Program_Data.INCLINE_Table_96[i] < 0){
            Program_Data.BarArray_Display[i] = Index_To_Bar[0];          //�p��0 �N���1��
        } 
    }*/
    
    //------------�N �V�m�p�e�ǹL�Ӫ��t��Table �ഫ��Bar
    for(unsigned char i = 0 ; i < 32; i++){
        if(Program_Data.SPEED_Table_96[i] >= 5 ){
            if(Program_Data.SPEED_Table_96[i] >= 240){
                Program_Data.BarArray_Display_Speed[i] = 8;
            }else{
                Program_Data.BarArray_Display_Speed[i] =  Program_Data.SPEED_Table_96[i] /30 + 1;
            }     
        }else if(Program_Data.INCLINE_Table_96[i] < 5){
            Program_Data.BarArray_Display_Speed[i] = 1;          //�t�פp��0.5 �N���1��
        } 
    }
    
    if(System_Unit == Metric){  
        Program_Data.PeriodWidth  = uiAppStageDist; 
    }else if(System_Unit == Imperial){
        Program_Data.PeriodWidth  = (uiAppStageDist * 10)/16; 
    }
    
    Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;


}

void Train_Time_Run_Program_Init(){
    
    UserData_Init(); 
    
    Program_Data.Goal_Time    = CloudRun_Init_INFO.Stage * 60; 
    Program_Data.Goal_Counter = CloudRun_Init_INFO.Stage * 60;
    

    //User_Stored_Setting_Init();
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    
    //---------Start Profile �˥�  
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 32;
    
    /*for(unsigned char i = 0;i < Program_Data.Template_Table_Num; i++){
        Program_Data.INCLINE_Template_Table[i] = Hill_Climb_Template[i];
    }*/
    
    for(unsigned char i = 0 ; i < 96; i++){
        Program_Data.INCLINE_Table_96[i] = CloudRun_Init_INFO.CloudRun_Inc_Buffer[i]; 
        Program_Data.SPEED_Table_96[i]   = CloudRun_Init_INFO.CloudRun_Spd_Buffer[i];
    }
    
    Table_96_To_BarArray();
    /*
    for(unsigned char i = 0 ; i < 32; i++){
        
        if(Program_Data.INCLINE_Table_96[i] >= 0x00 ){
            
            if(Program_Data.INCLINE_Table_96[i] > 30){
                Program_Data.BarArray_Display[i] = Index_To_Bar[30];
            }else{
                Program_Data.BarArray_Display[i] = Index_To_Bar[ Program_Data.INCLINE_Table_96[i] ];
            }
            
        }else if(Program_Data.INCLINE_Table_96[i] < 0){
            
            Program_Data.BarArray_Display[i] = Index_To_Bar[0];          //�p��0 �N���1��
        } 
        
    }*/


    //------------�N �V�m�p�e�ǹL�Ӫ��t��Table �ഫ��Bar
    for(unsigned char i = 0 ; i < 32; i++){
        if(Program_Data.SPEED_Table_96[i] >= 5 ){
            if(Program_Data.SPEED_Table_96[i] >= 240){
                Program_Data.BarArray_Display_Speed[i] = 8;
            }else{
                Program_Data.BarArray_Display_Speed[i] =  Program_Data.SPEED_Table_96[i] /30 + 1;
            }     
        }else if(Program_Data.INCLINE_Table_96[i] < 5){
            Program_Data.BarArray_Display_Speed[i] = 1;          //�t�פp��0.5 �N���1��
        } 
    }
    
    
    if(Program_Data.Goal_Time >= 60){     //�ܤ�1����   �V�m�p�e(�ɶ�)�T�w�@��O�@���� 
        Program_Data.PeriodWidth  = 60;                              //�@��϶��j��60  �N=60
        Program_Data.PeriodNumber = Program_Data.Goal_Time / 60;     //���`�@���X�Ӱ϶�
        Program_Data.NextPeriodValue = Program_Data.Goal_Time - Program_Data.PeriodWidth;
    }
    
}