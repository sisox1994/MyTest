#include "stm32f4xx_hal.h"
#include "string.h"
#include "system.h"
#include "btm.h"

void Linked_Check_For_New_Scan(Btm_Task_Def btmTask){

    if( (Linked_HR_info.Link_state == Linked) ||  (Linked_HR_info.Link_state == Linked_2)){//-Linked ���A ��CB��Ʊ����T�{
        
        //-------------------------------------------------------------�s�u��  �ҥH�� --  �_�u
        if(Linked_HR_info.SensorType == BLE_HR){
            
            btmTask_List[btm_Task_Cnt] =   BLE_HRC_Disconnect;
            btm_Task_Cnt++;
            
        }else if(Linked_HR_info.SensorType == ANT_HR){
            
            btmTask_List[btm_Task_Cnt] = ANT_HRC_Disconnect;
            btm_Task_Cnt++;
        }  
        
        //-----------���_�u����n���檺  ���s�_��------------------
        wait_HR_disconnect_Flag = 1;  
        
        btm_HRC_disconnect_Task_Temp = btmTask;
        
    }else{
        btmTask_List[btm_Task_Cnt] = btmTask;
        btm_Task_Cnt++;
    }
}

void Btm_Task_Adder(Btm_Task_Def btmTask){
    
    //�ثe�S�����Ȫ��ܴN���h�T�{ �����s�W���Ȩ�M��
    if( btm_Task_Cnt == 0 ){
        
        //--�p�G�n���y�߸� Sensor ���T�{�{�b�s�u���A  �p�G�s�u���N����@���_�u���O--------
        //if((btmTask == Scan_ANT_HRC_Sensor) || (btmTask == Scan_BLE_HRC_Sensor)){  
            
            //��U��E4  �n��sensor ���u �~��b�j�M
            //if(wait_HR_disconnect_Flag == 0){
                //Linked_Check_For_New_Scan(btmTask);
            //}  
            
        //}else{
            btmTask_List[btm_Task_Cnt] = btmTask;
            btm_Task_Cnt++;
        //}
        
    }else{
        
        //----�p�G���� �S���b�M��� �~�s�W���� �קK���ȭ���
        unsigned char Task_Exit_Flag = 0;
        
        for(unsigned char i = 0; i < btm_Task_Cnt; i++){
            if(btmTask == btmTask_List[i]){
                Task_Exit_Flag = 1;
                break;
            }
        }
        Task_Exit_Flag = 0;  //���\����
        if(Task_Exit_Flag == 0){
            
            if(btm_Task_Cnt < Task_Amount){  //�̦h N�ӥ��Ȧb�n�C�M��
                
                //--�p�G�n���y�߸� Sensor ���T�{�{�b�s�u���A  �p�G�s�u���N����@���_�u���O--------
                //if((btmTask == Scan_ANT_HRC_Sensor) || (btmTask == Scan_BLE_HRC_Sensor)){   
                    
                    //��U��E4  �n��sensor ���u �~��b�j�M
                    //if(wait_HR_disconnect_Flag == 0){
                        //Linked_Check_For_New_Scan(btmTask);
                    //}   
                        
                //}else{
                    btmTask_List[btm_Task_Cnt] = btmTask;
                    btm_Task_Cnt++;   
                //}
            }   
        } 
    }
}


void APP_background_Broadcast(){

    if(T5s_cmd39_Flag){
        T5s_cmd39_Flag = 0;
        
        if(System_Mode == Idle){
            
            if(Cloud_Run_Initial_Busy_Flag != 1){   //�b�Ƕ��]�]�w��T��  0x39���Ȱ�
                Btm_Task_Adder(C_39Val);
            }
            
        }else{
            
            switch(System_Mode){
                
              case Prog_Set: 
              case WarmUp:
              case CooolDown:
              case Paused:
              case Summary:
              case Ready:
                Btm_Task_Adder(C_39Val);
                break;
            }
        }
    }
}

void CloudRun_Workout_Data_Broadcast(){

    if(T500ms_cmd39_Flag){
        T500ms_cmd39_Flag = 0;
        
        
        if (System_Mode == Workout){  //&& (Program_Select == APP_Cloud_Run)){   
            Btm_Task_Adder(C_39Val);
        }
        
        
    }
}


void BTM_background_Task(){
    
    if(T100ms_Btm_Task_Flag){
        T100ms_Btm_Task_Flag = 0;
        
        //---�Ū� �s�u�|����[ �_�u�]����[----
        //ble_disconnect_wait_Process();
        //ble_Wait_First_CB_Value();
        //ble_ReSearching_TimeOut();
        //-------------------------------------
        
        if(btm_is_ready == 1){  //�bBtm  Rest �����~�������
        
            if(btm_Task_Cnt >=1 ){  //�ܤ֭n���@�ӥ���
              
                //------------------------------------------------------------
                switch(btmTask_List[0]){
                  case Connect_Paired_ANT_HR_E2:  
                    Link_Sensor_E2_ANT( ANT_ID_Paired_legacy);
                    break;
                  case Connect_Paired_BLE_HR_E2:
                    Link_Sensor_E2_BLE(BLE_Paired_legacy_Info);
                    break;
                  case Connect_Paired_NFC_BLE_HR_E2:
                    Link_BLE_NFC_Pairing_E2();
                    break;
                  case Scan_BLE_HRC_Sensor:
                    ScanSensorE0(BLE_HR);
                    break;
                  case Scan_ANT_HRC_Sensor:
                    ScanSensorE0(ANT_HR);
                    break;
                  case BLE_HRC_Disconnect:
                    disconnect_Sensor_E4(BLE_HR);
                    break;
                  case ANT_HRC_Disconnect:
                    disconnect_Sensor_E4(ANT_HR);
                    break;
                    
                  case BLE_HRC_Pairing:
                    Pairing_BLE_Sensor_E1(BLE_Scan_Device_List.messeage_List[NearestDevieIndex].DeviceNumber);
                    break;
                    
                    //-----------------Fw Update---------------------------
                  case C_02Val:
                    F_BtmReply02Cmd();
                    break;
                  case C_04Val:   
                    F_BtmReply04Cmd();
                    break;
                    
                    //--------------���] --Cloud Run    -------------------------
                    
                  case C_35Val:
                    F_BtmReply35Cmd();
                    break;
                  case C_37Val:
                    F_BtmReplyCmd(0x37);
                    break;
                  case C_38Val:
                    F_BtmReplyCmd(0x38);
                    break;
                    
                  case C_36Val:
                    F_BtmReply36Cmd();
                    break;
                  case C_39Val:
                    F_BtmReply39Cmd();
                    break;
                  case C_47Val:
                    F_BtmReply47Cmd();
                    break;
                    
                  case C_44Val:
                    F_BtmReply44Cmd();
                    break;
                  case C_46Val:
                    F_BtmReply46Cmd();
                    break;
                  case C_50Val:
                    F_BtmReply50Cmd();
                    break;
                    
                    //---------------FTMS------FEC------------------------------
                  case Feature_FTMS:
                    F_Btm_FTMS_B0(0);           //�]�w profile ��� ��Xflag
                    break;
                  case  Type_FTMS:
                    F_Btm_FTMS_B0(1); 
                    break;
                  case Flag_FTMS:
                    F_Btm_FTMS_B0(2); 
                    break;     
                    
                  case FTMS_Data_Broadcast:
                    F_Btm_FTMS_B1();
                    break;
                     
                  case FEC_SET_SN:
                    F_Btm_FEC_B5_SET_ANT_ID();
                    break;
                  case FEC_Data_Config_Page_0:
                    F_Btm_FEC_B4_SET_Data(0);
                    break;
                  case FEC_Data_Config_Page_1:
                    F_Btm_FEC_B4_SET_Data(1);
                    break;  

                  case Connectuin_Configure_C8:
                    Btm_Set_Configure_C8();
                    break;
                  case No_Task:
                    break;
                }
                //-------------------------------------------------------------------------

                //-----------------�������� �R��[0] ������     [0]<--[1] , [1]<--[2] ....
                for(unsigned char i = 0; i < btm_Task_Cnt; i++){ 
                    if(i == (Task_Amount - 1)){
                        btmTask_List[(Task_Amount - 1)] = No_Task;
                    }else{
                        btmTask_List[i] = btmTask_List[i+1];
                    }
                }
                btm_Task_Cnt = btm_Task_Cnt - 1;
                
            }  
        }   
    }
}