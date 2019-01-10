#include "system.h"
#define skip_Startup_Frame  0

unsigned char BarArray_Test[32] = {1,2,3,0,5,6,8,8,7,
                                   1,2,3,4,5,6,8,8,7,6,
                                   1,2,3,0,5,6,8,8,7,6,1,1};

unsigned char ucStartUpFlow = 0;
unsigned char ucSecondCnt = 0;

extern unsigned char EngineerTestItem;
//���������  ����P�_  �i�J�u�{�Ҧ�?
void EnterEngineerMode_Key(){
    if(KeyCatch(0 ,2, Stop , cool)){
        System_Mode = Engineer;
        EngineerTestItem = 0;
    }
}

void LUBE_Mode_Key(){
    if( KeyCatch(0,1 , Stop) ){ 
        ucStartUpFlow = 3;  
    } 
}

void StartUp_Func(){
    
    switch(ucStartUpFlow){
      case 0: //���� 2 sec
        if(T1s_Flag){
            T1s_Flag = 0;
            if(ucSecondCnt == 2){  
           #if skip_Startup_Frame 
                ucStartUpFlow = 4;    
           #else
                ucStartUpFlow = 1;    
           #endif
                ucSecondCnt   = 0;
                
                Turn_OFF_All_Segment();
                Turn_OFF_All_LEDMatrix();
              //-----    ��̤ܳj�t�״��� ����   ----//
                F_String_buffer_Auto_Middle( Stay, System_Version  ,40 ,0);
                SET_Seg_Display(INCLINE   , (unsigned short)Machine_Data.System_INCLINE_Max , D2 , DEC );
                SET_Seg_Display(SPEED     , (unsigned short)Machine_Data.System_SPEED_Max   , D2 , DEC );
              
                unsigned int ODO_Temp;
                
                if(System_Unit == Metric){  
                    ODO_Temp = Machine_Data.TotalDistance;
                }else if(System_Unit == Imperial){
                    ODO_Temp = (Machine_Data.TotalDistance * 10)/16;
                }
                if( (ODO_Temp/1000) >= 100){
                    SET_Seg_Display(DISTANCE  , ODO_Temp/1000 , ND , DEC );
                }else if((ODO_Temp/1000) < 100){
                    SET_Seg_Display(DISTANCE  , ODO_Temp/100 , D2 , DEC );
                }
                
                SET_Seg_Display( TIME , Machine_Data.Total_Times/3600 ,ND ,DEC);
                
                writeSegmentBuffer();
                writeLEDMatrix();    
                //------------------------------------
                ClearStd_1_Sec_Cnt(); 
            }else{
                ucSecondCnt++;
            }
        }
        break;
      case 1: // ���� ���� �t�� �`�Z��  �`�ɶ����2��
        if(T1s_Flag){
            T1s_Flag = 0;
            if(ucSecondCnt == 2){
                if((Machine_Data.Total_Times/3600) >=180){  //�]�B���ϥζW�L 180�p��  ���LUBE�[�o����
                    ucStartUpFlow = 2;
                }else{                                               
                    ucStartUpFlow = 3;           //ATTACUS  FITNESS  �]�@�^
                }
                Turn_OFF_All_Segment();
                ClearStd_1_Sec_Cnt();
                ucSecondCnt   = 0;
            }else{
                ucSecondCnt++;
            }
        }
        break;  
      case 2:  //LUBE   �@���`�� ������UStop Key
        if(T_Marquee_Flag){
            T_Marquee_Flag = 0;             
            F_String_buffer_Auto( Left, "LUBE" ,50 ,0);
            writeLEDMatrix();    
        }
        LUBE_Mode_Key();
        break;
      case 3:               //WELLCOME  �]�@�� 
        if(T_Marquee_Flag){
            T_Marquee_Flag = 0;      
            //if(F_String_buffer_Auto( Left, "ATTACUS       FITNESS  " ,50 ,0) == 1){
            if((F_String_buffer_Auto( Left, "WELLCOME" ,50 ,0) == 1 )|| (KeyCatch(0,1 , Start)) ){
                SET_Seg_Display(INCLINE  , 0 , D2 , DEC );
                SET_Seg_Display(SPEED    , 0 , D2 , DEC );
                SET_Seg_Display(DISTANCE , Program_Data.Distance , D2 , DEC );
                SET_Seg_Display(HEARTRATE  , 0 , ND , DEC );
                SET_Seg_Display(CALORIES  , Program_Data.Calories , ND , DEC );
                SET_Seg_Display(PACE      , Program_Data.Pace     , ND , DEC );
                SET_Seg_TIME_Display( TIME  , 0);
                writeSegmentBuffer();
                ucStartUpFlow = 4;
            }
            writeLEDMatrix();  
        }
        break;
        
      case 4:
        System_Mode = Idle;
        break;
    }
    
    EnterEngineerMode_Key();  //���� �զX���s�i�J�u�{�Ҧ�
 

}