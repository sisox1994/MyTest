#include "stm32f4xx_hal.h"
#include "system.h"


char ucProductionSerialNumber[30];

System_Mode_Def System_Mode;

SafeKey_State_def safekey;

Time_Display_Def       Time_Display_Type;
Calories_Display_Def   Calories_Display_Type;
unsigned char str_cNt = 0;

unsigned short System_INCLINE; //0~150   => 0.0~15.0 %   =>0~30
unsigned short System_SPEED;   //0 ~ 120 

System_Unit_Def System_Unit;

//unsigned int Total_Machine_Distants = 254;  //�����]���`���{  ��� ����/10 => 1:100����
//unsigned int Total_Machine_Times    = 10;     //�����]���`�ɶ�  ��� �p�� 


unsigned short usNowHeartRate;

unsigned char HeartRate_Is_Exist_Flag = 0;





///�D�n�\��   �P�_�߸��˸m�O�_���ƭ�   ���ƭ� FTMS �߸�Flag = 1  �ƭ���0  ���_�u  Flag = 0
// Zwift �S���p  FTMS �߸��e0bpm �αNFlag���� Zwift�߸��ƭȳ��|���d�b�̫�j��0����ƭ�


void F_HeartRate_Supervisor(){


    if(T1s_HR_Monitor_Flag){
        T1s_HR_Monitor_Flag = 0;      
    
        
        if(HeartRate_Is_Exist_Flag == 0){
            
            if(usNowHeartRate > 0){
                HeartRate_Is_Exist_Flag = 1;
                //Btm_Task_Adder(FTMS_HR_Connect);
            }
            
        }
        
        else if(HeartRate_Is_Exist_Flag == 1){
            
            if(usNowHeartRate == 0){
                HeartRate_Is_Exist_Flag = 0;
                //Btm_Task_Adder(FTMS_HR_Disconnect);
            } 
        }
 
        
    }
    

}



short Move_X = 52;


unsigned char charArrayEquals( char A1[], char A2[]){
    
    unsigned int A1_len = strlen(A1);
    unsigned int A2_len = strlen(A2);
    
    if(A1_len != A2_len){
        
       return 0;  
       
    }else if(A1_len == A2_len){
        
        for(int i = 0; i < A1_len;i++ ){
            if(A1[i] != A2[i]){
                return 0;  
            }
        }
        return 1;
    }

     return 0; 
}
