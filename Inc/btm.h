#ifndef __BTM_H__
#define __BTM_H__


typedef enum{
  ASLEEP   = 1,
  READY    = 2,
  IN_USE   = 3,
  FINISHED = 4
}FEC_State_Def;

typedef enum{
    others = 0,
    IDLE   = 1, 
    Warming_up = 2, 
    Low_intensity_interval  = 3,
    High_intensity_interval = 4,
    Recovery_interval = 5,
    Isometric = 6,
    Heart_rate_control = 7,
    Fitness_test = 8,
    Speed_outside_of_control_region_Low  = 9,
    Speed_outside_of_control_region_High = 10,
    Cool_down    = 11,
    Watt_control = 12,
    Manual_mode  = 13,
    Pre_workout  = 14,
    Post_workout = 15,
}Training_status_Def;


typedef enum{
    Digital_Watch  = 0,
    App            = 1, 
    Cloud_Run      = 2,
    void_Run       = 3,
    Train_Run_Dist = 4,
    Train_Run_Time = 5,
    C_App_UserVal      = 9,
    C_App_Dist1Val     = 10,
    C_App_Dist2Val     = 11,
    C_App_Training_Val = 13,
}ucAppTypeModeDef;

typedef enum{
    No_Task  = 0,
    Scan_BLE_HRC_Sensor  = 1,    
    Scan_ANT_HRC_Sensor  = 2,
    BLE_HRC_Disconnect   = 3,
    ANT_HRC_Disconnect   = 4,
    BLE_HRC_Pairing      = 5,
    BLE_HRC_Link         = 6,
    
    C_02Val = 12,
    C_04Val = 14,
    
    C_09Val = 9,
    
    C_35Val = 35,
    C_37Val = 37,
    C_38Val = 38,
    C_36Val = 36,
    
    C_39Val = 39,
    
    C_47Val = 47,
    C_44Val = 44,
    C_46Val = 46,
    C_50Val = 50,
    
    Feature_FTMS = 70,
    Type_FTMS     = 71,
    Flag_FTMS     = 72,
  
    FTMS_Data_Broadcast = 73,
    //FTMS_HR_Connect    = 74,
    //FTMS_HR_Disconnect = 75,
    FEC_SET_SN = 76,
    FEC_Data_Config_Page_0 = 77,
    FEC_Data_Config_Page_1 = 78,

}Btm_Task_Def;

typedef enum{
    
    treadmill     = 0x01,
    cross_trainer = 0x02,
    step_climber  = 0x04,
    stair_climber = 0x08,
    rower         = 0x10,
    indoor_bike   = 0x20,  
   
}FTMS_Machine_Type_Def;

typedef struct{
    System_Mode_Def c_System_Mode;
    unsigned int    c_Times;
    unsigned short  c_HeartRate;
    unsigned short  c_Stage;
    unsigned short  c_Speed;
    unsigned short  c_Incline;
    unsigned int    c_Distance;
    unsigned int    c_Calories;
    unsigned int    c_Step;
    unsigned long   c_ALTI;
    unsigned char   c_SPFreq;    
}Cmd_39_outData_Def;

typedef enum{
    BLE_HR  = 0x180D,    
    ANT_HR  = 0xF80D,    
    BLE_CSC = 0x1816,  
    ANT_CSC = 0xF816,              
    BLE_RSC = 0x1814,  
    ANT_RSC = 0xF814,           
    ANT_CSC_SPEED       = 0xE816,  
    ANT_CSC_CADENCE     = 0xD816,   
    SCAN_ALL_BLE_SENSOR = 0xFFFF      
}Sensor_UUID_Type_Def;

typedef enum{
      Scaning   = 0x40,
      Scaning2  = 0x4C,
      Paired_OK = 0x41,
      No_Device = 0x42,
      Scan_Time_Out = 0x4F,
}Scan_State_Def;

typedef enum{
      Linked     = 0x40,
      Linked_2   = 0x42,
      connected  = 0x44,
      fail       = 0x45,
      disconnect = 0x46,
      receive    = 0x48,
      nodify_ok  = 0x49,
      reject     = 0x4A,
      Link_unavailabe = 0x4B,
      busy            = 0x4C,       
      bonding_succes  = 0x4D,
      bonding_fail    = 0x4E,
      time_out        = 0x4F,
      ble_researching = 0xFC,
      wait_disconnect = 0xFD,
      wait_hr_value   = 0xFE,
      hr_disconnect   = 0xFF            
}Linking_State_Def;

typedef enum{
    Txx = 0,
    Rxx = 1    
}Trans_Dir_Def;

typedef struct{    
    Trans_Dir_Def         TransDir;
    Sensor_UUID_Type_Def  ScanType;    
    Scan_State_Def        Scan_State;    
    //--------------------ANT+
    unsigned short ANT_ID;
    //-------------------BLE
    unsigned char  DeviceNumber;
    unsigned char  RSSI;
    unsigned short UUID;
    Sensor_UUID_Type_Def Type_Check;
    char DeviceName[13];       
}Scan_Meseseage_def;


typedef struct{    
    Sensor_UUID_Type_Def  Pairing_Sensor_Type;
    char BLE_Addrs[6];
    unsigned char BLE_ADDR_TYPE;       
}Pairing_Meseseage_def;


typedef struct{    
    unsigned char Paired_Device_Cnt;
    Pairing_Meseseage_def BLE_Paired_Device_Addr_List[10]; //儲存已經成功配對到的藍芽裝置地址

}BLE_Paired_Device_Addr_List_def;


typedef struct{
    unsigned char       Device_Cnt;
    Scan_Meseseage_def  messeage_List[10];
}BLE_Device_List_def;


typedef struct{    
    
    Sensor_UUID_Type_Def SensorType;    
    char DeviceName[13];
    unsigned short ANT_ID;
    unsigned short usHR_bpm;
    Linking_State_Def Link_state;    
    char BLE_Addrs[6];

}Now_Linked_HR_Sensor_Info_Def;

typedef struct{    
    ucAppTypeModeDef TypeMode;
    unsigned short  Stage;
    Switch_def       AutoPause;
    unsigned char CloudRun_Inc_Buffer[432];
    unsigned char CloudRun_Spd_Buffer[432];    
    unsigned short C_Time;
    unsigned char  C_Age;
    unsigned short C_Weight;
    unsigned char  PlayGround;
    unsigned short Total_Dist;
    unsigned short Stage_Dist;
    
}CloudRun_Init_INFO_Def;

typedef enum{
    No_Error   = 0,
    Data_Lost  = 1,
    Size_Error = 2,    
}Error_Type_Def;

typedef struct{    
    unsigned int  total_Size;
    unsigned int  Byte_Cnt;
    unsigned short total_Page;
    unsigned short Transmit_Page;
    unsigned short Error_Cnt;
    Error_Type_Def  Error_Type;
}OTA_FW_Transmit_Info_Def;

#define Task_Amount  6

extern CloudRun_Init_INFO_Def CloudRun_Init_INFO;
extern unsigned char FW_Transmiting_03_Flag;
extern unsigned int uiAppTotalDist;
extern unsigned int uiAppStageDist;
extern unsigned int uiAppTotalDist;
extern Cmd_39_outData_Def  Cloud_0x39_Info;
extern BLE_Device_List_def BLE_Scan_Device_List;
extern BLE_Paired_Device_Addr_List_def  BLE_Paired_device_list; // 成功配對 就把資訊加入清單裡
extern Now_Linked_HR_Sensor_Info_Def Linked_HR_info;
extern unsigned char btm_Task_Cnt;
extern Scan_Meseseage_def Scan_Msg;
extern unsigned char  Ble_wait_HR_value_First_IN_Flag;
extern unsigned char  wait_HR_disconnect_Flag;
extern unsigned short disconnect_buffer_0xFF_Cnt;
extern Btm_Task_Def   btmTask_List[Task_Amount];
extern Btm_Task_Def   btm_HRC_disconnect_Task_Temp;

extern unsigned char btm_is_ready;
extern unsigned char btm_Rx_is_busy;
extern unsigned char NearestDevieNumber;

void ble_disconnect_wait_Process();
void ble_ReSearching_TimeOut();
void Btm_Task_Adder(Btm_Task_Def btmTask);
void BTM_background_Task();

void BLE_Init();
void BLE_DeInit();

void Btm_Recive();
void BtmRst();
void Btm_SetDeviceName(char* name,unsigned char Len);
void ScanSensorE0(Sensor_UUID_Type_Def  Sensor_Type);
void Pairing_BLE_Sensor_E1(unsigned char DeviceNumber);

void Link_Sensor_E2_BLE(Pairing_Meseseage_def paired_msg);
void Link_Sensor_E2_ANT( unsigned short ANT_ID);
void Ask_Link_State_CE();
void Ask_Link_State_CF();
void disconnect_Sensor_E4(Sensor_UUID_Type_Def  Sensor_Type);
void ble_Wait_First_CB_Value();

// 雲跑 func
extern unsigned char Cloud_Run_Initial_Busy_Flag;

void APP_background_Broadcast();
void CloudRun_Workout_Data_Broadcast();
void F_BtmReplyCmd(unsigned char data);

void F_BtmReply02Cmd(void);
void F_BtmReply04Cmd(void);

void F_BtmReply35Cmd(void);
void F_BtmReply36Cmd(void);
void F_BtmReply39Cmd(void);
void F_BtmReply47Cmd(void);
void F_BtmReply44Cmd(void);
void F_BtmReply46Cmd(void);
void F_BtmReply50Cmd(void);

//FTMS
void F_Btm_FTMS_B0(unsigned char ucPage);
void F_Btm_FTMS_B1();
void F_Btm_FTMS_B0_HR_Switch(unsigned char SW);

//FEC
void F_Btm_FEC_B5_SET_ANT_ID(void);
void F_Btm_FEC_B4_SET_Data(unsigned char page);
void F_SetFEC_State(FEC_State_Def FEC_State);


//0xB3 SpinDown
void Set_Spindown_SPEED(unsigned short Speed_Low, unsigned short Speed_High );
void Set_Spindown_Success(unsigned short SpindownTime);
void Set_Spindown_Error();
void Set_Spindown_StopPedaling();

#endif