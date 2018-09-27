#ifndef PROGRAM_H_
#define PROGRAM_H_

typedef enum{
    
    SET_Time     = 0,
    SET_Age      = 1,
    SET_Weight   = 2,
    SET_THR      = 3,
    SET_MAX_INC  = 4,
    SET_CAL_GOAL = 5,
    SET_Dif_LEVEL = 6,
    SET_WORK_Time = 7,
    SET_REST_Time = 8,
    SET_LIKE_Prog = 9,
    SET_END       = 10,
        

}Program_Setting_item_Def;

typedef enum{
  
    Time_Age_Weight         = 0,
    Time_Age_Weight_THR     = 1,
    Time_Age_Weight_MAX_INC = 2,
    Age_Weight              = 3,  
    Age_Weight_Cal          = 4,
    Age_Weight_Lvl          = 5,
    Time_Age_Weight_Work_Rest = 6,
    P_Like                    = 7
    
   
}Setting_Class_Def;

extern Program_Setting_item_Def  Setting_item_Index;

extern unsigned char INCLINE_HIGH_Limit_Table[20];
extern unsigned char INCLINE_LOW_Limit_Table[20]; 


#define INCLINE_MAX_Level_DEF  31
#define INCLINE_Change_Range   8

extern void User_Stored_Setting_Init();

extern unsigned char Interval_1_1_Template[4];

extern const unsigned char Index_To_Bar[];

unsigned char SysINC_To_BarCode(unsigned char Index);

extern unsigned char ClassCnt;

void Calorie_Calculate();
void Mets_Calculate();
void ALTI_Calculate();

unsigned char Quick_SPEED__Key();
unsigned char Quick_INCLINE__Key();

void UserData_Init();
void GetMaxHeartRate();
void GetTargetHeartRate();

void Time_Change_Process();
void Time_Change_Process_InWorkout(short TimeChangeValue);

void Time_Change_Process_HRC_Hill_Init();
void Time_Change_Process_HRC_Hill_In_WorkOut(short TimeChangeValue);

void Time_Change_Process_Ez_INCLINE();
void EZ_MAX_INC_Change_Process();



//   Manual    Initial   Function
extern unsigned int   NowPeriodLimitValue_Temp;
extern unsigned short RestOfPeriod_Temp;
extern unsigned char Manual_Profile_Template[4];

void Quick_Start_Init();
void Manual_Init();
void Random_Init();

void CrossCountry_Init();
void WeightLoss_Init();
void Hill_Init();
void Interval_1_1_Init();
void Interval_1_2_Init();

//    HRC   Initial   Function
void Target_HeartRate_Goal_Init();
void Fat_Burn_Init();
void Cardio_Init();
void Heart_Rate_Hill_Init();
void Heart_Rate_Interval_Init();
void Extreme_Heart_Rate_Init();


//   Advanced  Initial   Function
void Hill_Climb_Init();
void Aerobic_Init();
void Interval_1_4_Init();
void Interval_2_1_Init();
void EZ_INCLINE_Init();
void MARATHON_Init();


//   Goal  Initial   Function
void Calorie_Goal_Init();
void Distance_Goal_160M_Init();
void Distance_Goal_5K_Init();
void Distance_Goal_10K_Init();

//   Custom  Initial   Function
void Custom_1_Init();
void Custom_2_Init();
void User_1_Init();
void User_2_Init();

//    ¶³¶]  Initial   Function
void Cloud_Run_Program_Init();

#endif