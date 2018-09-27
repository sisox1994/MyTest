#include "stm32f4xx_hal.h"

#define High 1
#define Low  0

typedef enum{

    Remaining  = 0,
    Elspsed    = 1
    
}Time_Display_Def;


typedef enum{

    Cal_    = 0,
    Cal_HR  = 1
    
}Calories_Display_Def;


typedef enum{

    Metric  = 0,
    Imperial = 1
    
}System_Unit_Def;

typedef enum{

    ON  = 1,
    OFF = 0

}Switch_def;

typedef enum{

    Plug_IN  = 0,
    Plug_Out = 1

}SafeKey_State_def;


typedef enum{

    Quick_start  = 0,
    
    Manual       = 1,
    Random       = 2,
    CrossCountry = 3,
    WeightLoss   = 4,   
    Interval_1_1 = 5,
    Interval_1_2 = 6,
    Hill         = 7,
     
    
    Target_HeartRate_Goal = 11,    
    Fat_Burn              = 12, 
    Cardio                = 13,
    Heart_Rate_Hill       = 14,
    Heart_Rate_Interval   = 15,
    Extreme_Heart_Rate    = 16,

    Hill_Climb    = 22,
    Aerobic       = 23,
    Interval_1_4  = 24,
    Interval_2_1  = 25,
    EZ_INCLINE    = 26,
    MARATHON_Mode = 27,

    Calorie_Goal        = 31,
    Distance_Goal_160M  = 32,
    Distance_Goal_5K    = 33,
    Distance_Goal_10K   = 34,

    Custom_1             = 35,
    Custom_2             = 36,
    User_1               = 37,
    User_2               = 38,
        
    APP_Cloud_Run        = 39


}Program_Type_Def;

typedef struct{
    
    unsigned int   Custom_Time;
    unsigned char  Custom_Age;
    unsigned short Custom_Weight;
    unsigned char  Custom_WorkTime;
    unsigned char  Custom_RestTime;
            
}Custom_Program_Data_Def;

typedef struct{
    
    
    
    unsigned int    TotalDistance;             //總里程ODO   0.001公里 = 公尺   存在flash
    unsigned int    Total_Times;               //運動時間   單位     1 s   
    
    unsigned short  System_INCLINE_Max;
    unsigned short  System_SPEED_Max;
    
    unsigned short  System_INCLINE_Min;
    unsigned short  System_SPEED_Min;
    
    System_Unit_Def  System_UNIT;
            
}Machine_Data_Def;




typedef struct{
    
    unsigned int   User_Time;
    unsigned char  User_Age;
    unsigned short User_Weight;

    Program_Type_Def User_Program_Like;
    
    unsigned short User_THR;
    unsigned short User_Ez_MaxINCLINE;
    unsigned int   User_Calories_Goal;
    unsigned char  Diffculty_Level;
    
    
    
}User_Program_Data_Def;



typedef struct{
    
    unsigned int   Goal_Time;
    unsigned int   Goal_Counter;
    
    unsigned int   Distance;
    unsigned int   Distance_Goal;
    
    unsigned char  Age;
    unsigned short Weight;
    
    Program_Type_Def Like_Program;
    unsigned short Ez_MaxINCLINE;       //Ez-INCLINE 模式最高揚升
    
    unsigned short TargetHeartRate;   //最大 * 75%
    
    uint32_t         Calories_Goal;
    uint32_t         Calories;
    uint32_t         Calories_HR;
     
    unsigned char  Diffculty_Level;
   
    unsigned int   Pace;

    unsigned short RestOfPeriod;
    unsigned int   NowPeriodLimitValue;
   
    
    unsigned short MaxHeartRate;     //用年齡算
   
    unsigned short HR_Range_High;     //目標 * 72%
    unsigned short HR_Range_Low;       //目標 *60%
  
    short INCLINE_Template_Table[32];      //儲存 Program 重複的圖形樣本      0 = 0.0% , 1 =0.5% , 2 = 1.0% ----> 30= 14.5%  , 31 =15.0%
    unsigned int Template_Table_Num;    //儲存 program 固定樣本圖形 數量
    
    short INCLINE_Start_Template_Table[10];      //儲存 一開始的樣本  Hill mode

    unsigned char HRC_INCLINE_Rate_Table[32]; 
    
    unsigned char  BarArray_Display[32];     //顯示出來的部分
    

    short INCLINE_Table_96[96];
    
    unsigned char  MasterPage;
    short          INCLINE_ValueDiff;
    
    unsigned short  PeriodNumber;     //總共有幾個區間
    unsigned char   PeriodWidth;     //區間寬度 單位:秒

    unsigned int   NextPeriodValue;
    unsigned char  Template_Loop_Start_Index;

    unsigned short  NowPeriodIndex;    //目前在哪個period
    unsigned char   PeriodIndex_After_Shift;  //顯示專用的 偏移index
    
    unsigned short  HeartRatePeroidCnt;   //30s   15s   10s   
    
    unsigned char  WorkTime;
    unsigned char  RestTime;
    
    

}Program_Data_Def;

typedef enum{

    StartUP  = 0,
    Menu     = 1,
    Program_Setting = 2,
    WarmUp   = 3,
    CooolDown = 4,
    Workout  = 5,
    Paused   = 6,
    Ready    = 7,
    Summary  = 8,
    Engineer = 9,
    Safe     = 10,
        

    C_SysIdleModeVal  = 11,
    C_APPModeVal      = 12,
    C_Sys_OTAModeVal  = 13,
    
    RS485_Test_Mode   = 14,

        
        
       
}System_Mode_Def;




typedef enum{
    
    Basic    = 0,
    HRC      = 1,
    Advanced = 2,
    Goal     = 3,
    Fit_Test = 4
    
}Program_Class_Def;


typedef enum{
    
    Up    = 0,
    Down  = 1,
    Left  = 2,
    Right = 3,
    
    Up_and_Stay    = 4,
    Down_and_Stay  = 5,
    Left_and_Stay  = 6,
    Right_and_Stay = 7,
    
    Stay = 8,
    
    
}Direction_Def;


typedef struct {

    unsigned int B0:1;
    unsigned int B1:1;
    unsigned int B2:1;
    unsigned int B3:1;

    unsigned int B4:1;
    unsigned int B5:1;
    unsigned int B6:1;
    unsigned int B7:1;

    unsigned int B8:1;
    unsigned int B9:1;
    unsigned int B10:1;
    unsigned int B11:1;

    unsigned int B12:1;
    unsigned int B13:1;
    unsigned int B14:1;
    unsigned int B15:1;
    
    unsigned int B16:1;
    unsigned int B17:1;
    unsigned int B18:1;
    unsigned int B19:1;  
    
    unsigned int B20:1;
    unsigned int B21:1;
    unsigned int B22:1;
    unsigned int B23:1;  
       
    unsigned int B24:1;
    unsigned int B25:1;
    unsigned int B26:1;
    unsigned int B27:1;  
       
    unsigned int B28:1;
    unsigned int B29:1;
    unsigned int B30:1;
    unsigned int B31:1;  
     
}Flags_32bit;






