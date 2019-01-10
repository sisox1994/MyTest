#include "stm32f4xx_hal.h"
#include "system.h"
#include "stdlib.h"
#include "MX25_DEF.h"
#include "MX25_CMD.h"

unsigned char Read_Buffer_256Byte[256];
#define Sector_0    0x000000
#define Sector_1    0x001000
#define Sector_2    0x002000
#define Sector_3    0x003000
#define Sector_4    0x004000
#define Sector_5    0x005000
#define Sector_6    0x006000
#define Sector_7    0x007000
#define Sector_8    0x008000  

#define Sector_979  0x3D3000  
#define Sector_1023 0x3FF000  
//總共1024 個


//---Custom Data 規劃在Sector_0   Page 1
#define OTA_Mode_Flag_Address      0x000000
#define Update_FW_Size_Address     0x000018

#define Flash_EngineerTest_Address  0x0000F0     //工程模式 Flash 讀寫功能測試
#define Custom_Save_Address         0x000100     
#define Machine_Save_Address        0x000200
#define Serial_Number_Address       0x000300

//--------------Sector 1 ~     ---------------
//#define Update_Firmware_Address  0x010000

//--------------Sector 979      ---------------
#define Update_Firmware_Address  0x3D3000

unsigned char FlashBuffer4096[4096];

unsigned char Write_FlashBuffer256[256];

Custom_Program_Data_Def MyCustom_1;
Custom_Program_Data_Def MyCustom_2;

User_Program_Data_Def MyUser_1;
User_Program_Data_Def MyUser_2;

Machine_Data_Def  Machine_Data;

//---------------------------------------Flash  通用寫入 讀取 functions ----------------------------------
unsigned short start_Sector;
unsigned short Sector_use_amount;

//(在地址 start_addr )  (寫入 amount 個byte資料 )   (從*Data指向的地址)
void Write_EE_Flash( unsigned int start_addr , unsigned int amount , unsigned char *Data){
    
    unsigned char ucWrite256bytebuffer[256];
    start_Sector = start_addr / 4096;   //要寫入個起始Sector
    Sector_use_amount  =  1 + (start_addr + amount) / 4096;  //看看 總共要寫入幾個sector
    
    for(unsigned char i = start_Sector ; i < (start_Sector + Sector_use_amount) ; i++){
        CMD_READ( i * 4096 , FlashBuffer4096, 4096);  //先讀出 Setor 4096個資料
        CMD_SE(i * 4096);  //讀出 之後存在 mcu 就可以先洗掉了
        
        for(unsigned int page = 0 ; page < 4096 ; page += 256){  //一個Setor 總共16 個Page 一個page 256 byte
            memset(ucWrite256bytebuffer,0x00,256);         //先清掉 寫入一個Page 256byte 的buffer   
            for(unsigned int j = 0; j  < 256; j++ ){
                if((i * 4096 + (page + j) >= start_addr ) && ( i * 4096 + (page + j) < (start_addr + amount) )){    
                    ucWrite256bytebuffer[  (i * 4096 + (page + j)) % 256 ] = Data[ ( i * 4096 + (page + j) )  -  start_addr   ];
                }else{
                    ucWrite256bytebuffer[j] = FlashBuffer4096[ page + j ]; //先把原本的值寫進buffer
                }                
            }
            CMD_PP( i * 4096 + page, ucWrite256bytebuffer , 256);   //寫一個page 進去 
        }
    }
    //-------------寫進去完   讀出來確認---------------
    CMD_READ( Sector_0 , FlashBuffer4096, 4096);  //先讀出 Setor 4096個資料
    __asm("NOP");
}

unsigned short start_Index;
//(從 地址 start_addr) (讀取 amount 個byte 的資料) (存放到 *Data指向的地址)
void Read_EE_Flash( unsigned int start_addr , unsigned int amount , unsigned char *Data){
    
    start_Sector = start_addr / 4096;   //要讀出的個起始Sector
    start_Index  = start_addr % 4096;
    CMD_READ( start_Sector * 4096 , FlashBuffer4096, 4096);  //先讀出 Setor 4096個資料
    for(unsigned short i = start_Index; i < (start_Index + amount) ; i  ++ ){
        Data[i-start_Index] = FlashBuffer4096[i];
    }
}
//---------------------------------------Flash  通用寫入 讀取 functions ----------------------------------

//===================    OTA  Firmware Data Flash  Functions    ===============================

// 把 APP收到的 Firmware Size 寫入 Flash  (給 Bootloader 讀取用) 
void Flash_Write_Firmware_Total_Size(){
    
     unsigned char Fw_size_tmp[4];
     Fw_size_tmp[0] = (unsigned char)FW.total_Size;
     Fw_size_tmp[1] = (unsigned char)(FW.total_Size>>8);
     Fw_size_tmp[2] = (unsigned char)(FW.total_Size>>16);
     Fw_size_tmp[3] = (unsigned char)(FW.total_Size>>24);
     Write_EE_Flash(Update_FW_Size_Address ,4 , Fw_size_tmp);
}

unsigned char Erase_Setor_Num;
extern UART_HandleTypeDef huart2;
//抹除 存放 APP  收到的 Firmware  Flash 儲存空間
void Update_FW_Flash_Erase(){
    
    //抹除需要一段時間 先關掉Btm中斷 預防掛掉
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);   
    Erase_Setor_Num = (FW.total_Size / 4096) + 1;
    
    for(unsigned char i = 0; i < Erase_Setor_Num; i++   ){   //2991
        CMD_SE( Update_Firmware_Address + (i * 4096));  
    }
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
}

//OTA模式 從APP收來的資料 一次寫入16byte
extern unsigned char FW_Buff_16Byte[16];
void Write_EE_Flash_FW_Data_16Byte( unsigned int page , unsigned char *Data){
    CMD_PP( Update_Firmware_Address + (page * 16)  , Data , 16);   //寫一個page(16byte) 進去   
}

//Firmware 寫入Flash完成 讀出來確認
void Fw_Write_Finish_Check(){
    for(unsigned char i = 0; i < Erase_Setor_Num; i++   ){
        CMD_READ( Update_Firmware_Address + (i * 4096) , FlashBuffer4096, 4096);
        __asm("NOP");
    }
}
//===================    OTA  Firmware Data Flash  Functions    ===============================

//比較兩個字串是否相同
unsigned char str_check(unsigned char *Str1 , unsigned char *Str2 ,unsigned char Num){

    for(unsigned char i = 0; i < Num ; i++){
        if( Str1[i] != Str2[i]){
            return 0;
        } 
    }
    return 1;
}

//==========================   寫入系統 Reboot 進入的模式   ===============================================

unsigned char OTA_Mode_Flag = 0;
unsigned char OTA_Flag[]         = {"OTA_Mode"};
unsigned char Application_Flag[] = {"Application_Mode"};
unsigned char Bootloader_Flag[]  = {"Bootloader_Mode"};
unsigned char FlagCheck[16];

void OTA_Mode_Check(){

    Read_EE_Flash( OTA_Mode_Flag_Address , 16 , FlagCheck);
    
    if( str_check( OTA_Flag , FlagCheck , sizeof(OTA_Flag) ) ){
        OTA_Mode_Flag = 1;    
    }
    if(str_check( Bootloader_Flag , FlagCheck , sizeof(Bootloader_Flag))){
        while(1){
            HAL_NVIC_SystemReset();
        }  //bootloader Mode 暫時先卡在這裡   
    }
    
    if(FlagCheck[0] == 0xFF){  // 第一次燒錄   沒有初始化過
        Flash_Write_Applicantion_Mode();
    }
    __asm("NOP");
}

//------------------------- 寫入系統旗標 ------------------------------------------
void Flash_Write_OTA_Mode(){
    Write_EE_Flash( OTA_Mode_Flag_Address , sizeof(OTA_Flag) ,OTA_Flag);
}
void Flash_Write_Applicantion_Mode(){
    Write_EE_Flash( OTA_Mode_Flag_Address , sizeof(Application_Flag) ,Application_Flag);
}
void Flash_Write_BootLoader_Mode(){
    Write_EE_Flash( OTA_Mode_Flag_Address , sizeof(Bootloader_Flag) ,Bootloader_Flag);
    Flash_Write_Firmware_Total_Size();  // 寫入更新檔 size
}

//======================================================================================================

//*******************   Program __ Custom_1,Custom_2  User_1,User_2  資料存取   ************************

void Custom_Data_Decode(){

    //------------------------- 把FLASH 裡面的Custom 資料解出來 ------------------------------------

    MyCustom_1.Custom_Time = (unsigned int)(FlashBuffer4096[Custom_Save_Address + 3] << 24)
                           + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 2] << 16) 
                           + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 1] <<  8)
                           + (unsigned int) FlashBuffer4096[Custom_Save_Address + 0];  
    
    MyCustom_1.Custom_Age    = FlashBuffer4096[Custom_Save_Address + 4];
    
    MyCustom_1.Custom_Weight = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 6] << 8)
                             + (unsigned short) FlashBuffer4096[Custom_Save_Address + 5];
    
    MyCustom_1.Custom_WorkTime = FlashBuffer4096[Custom_Save_Address + 7];
    MyCustom_1.Custom_RestTime = FlashBuffer4096[Custom_Save_Address + 8];
    
    //----------------------------------------------------------
    
    MyCustom_2.Custom_Time = (unsigned int)(FlashBuffer4096[Custom_Save_Address + 12] << 24)
                           + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 11] << 16)
                           + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 10] <<  8)
                           + (unsigned int) FlashBuffer4096[Custom_Save_Address + 9 ];   
    
    MyCustom_2.Custom_Age    = FlashBuffer4096[Custom_Save_Address + 13];
    
    MyCustom_2.Custom_Weight = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 15] << 8)
                             + (unsigned short) FlashBuffer4096[Custom_Save_Address + 14];
    
    MyCustom_2.Custom_WorkTime = FlashBuffer4096[Custom_Save_Address + 16];
    MyCustom_2.Custom_RestTime = FlashBuffer4096[Custom_Save_Address + 17];
    
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    
    
    MyUser_1.User_Time = (unsigned int)(FlashBuffer4096[Custom_Save_Address + 21] << 24)
                       + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 20] << 16)
                       + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 19] <<  8)
                       + (unsigned int) FlashBuffer4096[Custom_Save_Address + 18];   
        
        
    MyUser_1.User_Age = FlashBuffer4096[Custom_Save_Address + 22];
    
    MyUser_1.User_Weight = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 24] << 8)
                         + (unsigned short) FlashBuffer4096[Custom_Save_Address + 23];
        
    MyUser_1.User_Program_Like = (Program_Type_Def)FlashBuffer4096[Custom_Save_Address + 25];
    
    MyUser_1.User_THR = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 27] << 8)
                      + (unsigned short) FlashBuffer4096[Custom_Save_Address + 26];
                        
    MyUser_1.User_Ez_MaxINCLINE = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 29] << 8)
                                + (unsigned short) FlashBuffer4096[Custom_Save_Address + 28];
    
    MyUser_1.User_Calories_Goal = (unsigned int)(FlashBuffer4096[Custom_Save_Address + 33] << 24)
                                + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 32] << 16)
                                + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 31] <<  8)
                                + (unsigned int) FlashBuffer4096[Custom_Save_Address + 30];   
    
    MyUser_1.Diffculty_Level = FlashBuffer4096[Custom_Save_Address + 34];
    
    
    //---------------------------------------------------------------------------
    MyUser_2.User_Time = (unsigned int)(FlashBuffer4096[Custom_Save_Address + 38] << 24)
                       + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 37] << 16)
                       + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 36] <<  8)
                       + (unsigned int) FlashBuffer4096[Custom_Save_Address + 35];   
        
        
    MyUser_2.User_Age = FlashBuffer4096[Custom_Save_Address + 39];
    
    MyUser_2.User_Weight = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 41] << 8)
                         + (unsigned short) FlashBuffer4096[Custom_Save_Address + 40];
        
    MyUser_2.User_Program_Like = (Program_Type_Def)FlashBuffer4096[Custom_Save_Address + 42];
    
    
    MyUser_2.User_THR = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 44] << 8)
                      + (unsigned short) FlashBuffer4096[Custom_Save_Address + 43];
                        
    MyUser_2.User_Ez_MaxINCLINE = (unsigned short)(FlashBuffer4096[Custom_Save_Address + 46] << 8)
                                + (unsigned short) FlashBuffer4096[Custom_Save_Address + 45];
    
    MyUser_2.User_Calories_Goal = (unsigned int)(FlashBuffer4096[Custom_Save_Address + 50] << 24)
                                + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 49] << 16)
                                + (unsigned int)(FlashBuffer4096[Custom_Save_Address + 48] <<  8)
                                + (unsigned int) FlashBuffer4096[Custom_Save_Address + 47];   
    
    MyUser_2.Diffculty_Level = FlashBuffer4096[Custom_Save_Address + 51];
      
    //---------------------------------------------------------------------------
 
}


void Custom_Data_To_Flash_Buffer256(){
    
    Write_FlashBuffer256[0] = (unsigned char) MyCustom_1.Custom_Time;
    Write_FlashBuffer256[1] = (unsigned char)(MyCustom_1.Custom_Time >> 8);
    Write_FlashBuffer256[2] = (unsigned char)(MyCustom_1.Custom_Time >> 16);
    Write_FlashBuffer256[3] = (unsigned char)(MyCustom_1.Custom_Time >> 24);
    
    Write_FlashBuffer256[4] = MyCustom_1.Custom_Age;
    
    Write_FlashBuffer256[5] = (unsigned char) MyCustom_1.Custom_Weight; 
    Write_FlashBuffer256[6] = (unsigned char)(MyCustom_1.Custom_Weight >> 8);
    
    Write_FlashBuffer256[7] = MyCustom_1.Custom_WorkTime;
    Write_FlashBuffer256[8] = MyCustom_1.Custom_RestTime;
    //---------------------------------------------------------------------------
    Write_FlashBuffer256[9]  = (unsigned char) MyCustom_2.Custom_Time;
    Write_FlashBuffer256[10] = (unsigned char)(MyCustom_2.Custom_Time >> 8);
    Write_FlashBuffer256[11] = (unsigned char)(MyCustom_2.Custom_Time >> 16);
    Write_FlashBuffer256[12] = (unsigned char)(MyCustom_2.Custom_Time >> 24);
    
    Write_FlashBuffer256[13] = MyCustom_2.Custom_Age;
    
    Write_FlashBuffer256[14] = MyCustom_2.Custom_Weight; 
    Write_FlashBuffer256[15] = (unsigned char)(MyCustom_2.Custom_Weight >> 8);
    
    Write_FlashBuffer256[16] = MyCustom_2.Custom_WorkTime;
    Write_FlashBuffer256[17] = MyCustom_2.Custom_RestTime;
    //-----------------------------------------------------------------------------
    Write_FlashBuffer256[18] = (unsigned char) MyUser_1.User_Time;
    Write_FlashBuffer256[19] = (unsigned char)(MyUser_1.User_Time >> 8);
    Write_FlashBuffer256[20] = (unsigned char)(MyUser_1.User_Time >> 16);
    Write_FlashBuffer256[21] = (unsigned char)(MyUser_1.User_Time >> 24);
    
    Write_FlashBuffer256[22] = MyUser_1.User_Age;
    
    Write_FlashBuffer256[23] = MyUser_1.User_Weight; 
    Write_FlashBuffer256[24] = (unsigned char)(MyUser_1.User_Weight >> 8);
    
    Write_FlashBuffer256[25] = (unsigned char)MyUser_1.User_Program_Like;
    
    Write_FlashBuffer256[26] = MyUser_1.User_THR ; 
    Write_FlashBuffer256[27] = (unsigned char)(MyUser_1.User_THR >> 8);
    
    Write_FlashBuffer256[28] = MyUser_1.User_Ez_MaxINCLINE ; 
    Write_FlashBuffer256[29] = (unsigned char)(MyUser_1.User_Ez_MaxINCLINE >> 8);
    
    Write_FlashBuffer256[30] = (unsigned char)MyUser_1.User_Calories_Goal;
    Write_FlashBuffer256[31] = (unsigned char)(MyUser_1.User_Calories_Goal >> 8);
    Write_FlashBuffer256[32] = (unsigned char)(MyUser_1.User_Calories_Goal >> 16);
    Write_FlashBuffer256[33] = (unsigned char)(MyUser_1.User_Calories_Goal >> 24);
    
    Write_FlashBuffer256[34] = MyUser_1.Diffculty_Level;
    
    //---------------------------------------------------------------------------
    Write_FlashBuffer256[35] = (unsigned char) MyUser_2.User_Time;
    Write_FlashBuffer256[36] = (unsigned char)(MyUser_2.User_Time >> 8);
    Write_FlashBuffer256[37] = (unsigned char)(MyUser_2.User_Time >> 16);
    Write_FlashBuffer256[38] = (unsigned char)(MyUser_2.User_Time >> 24);
    
    Write_FlashBuffer256[39] = MyUser_2.User_Age;
    
    Write_FlashBuffer256[40] = MyUser_2.User_Weight; 
    Write_FlashBuffer256[41] = (unsigned char)(MyUser_2.User_Weight >> 8);
    
    Write_FlashBuffer256[42] = (unsigned char)MyUser_2.User_Program_Like;
    
    Write_FlashBuffer256[43] = MyUser_2.User_THR ; 
    Write_FlashBuffer256[44] = (unsigned char)(MyUser_1.User_THR >> 8);
    
    Write_FlashBuffer256[45] = MyUser_2.User_Ez_MaxINCLINE ; 
    Write_FlashBuffer256[46] = (unsigned char)(MyUser_1.User_Ez_MaxINCLINE >> 8);
    
    Write_FlashBuffer256[47]  = (unsigned char)MyUser_2.User_Calories_Goal;
    Write_FlashBuffer256[48] = (unsigned char)(MyUser_2.User_Calories_Goal >> 8);
    Write_FlashBuffer256[49] = (unsigned char)(MyUser_2.User_Calories_Goal >> 16);
    Write_FlashBuffer256[50] = (unsigned char)(MyUser_2.User_Calories_Goal >> 24);
    
    Write_FlashBuffer256[51] = MyUser_2.Diffculty_Level;
}



void Flash_Custom_Data_Saving(){
    
    if(Program_Select == Custom_1){
        MyCustom_1.Custom_Time     = Program_Data.Goal_Time;
        MyCustom_1.Custom_Age      = Program_Data.Age;
        MyCustom_1.Custom_Weight   = Program_Data.Weight;
        MyCustom_1.Custom_WorkTime = Program_Data.WorkTime;
        MyCustom_1.Custom_RestTime = Program_Data.RestTime;
    }else if(Program_Select == Custom_2){
        MyCustom_2.Custom_Time     = Program_Data.Goal_Time;
        MyCustom_2.Custom_Age      = Program_Data.Age;
        MyCustom_2.Custom_Weight   = Program_Data.Weight;
        MyCustom_2.Custom_WorkTime = Program_Data.WorkTime;
        MyCustom_2.Custom_RestTime = Program_Data.RestTime;  
    }else if(Program_Select == User_1){
        MyUser_1.User_Time         = Program_Data.Goal_Time;
        MyUser_1.User_Age          = Program_Data.Age;
        MyUser_1.User_Weight       = Program_Data.Weight;
        MyUser_1.User_Program_Like = Program_Data.Like_Program;
        
        MyUser_1.User_THR           = Program_Data.TargetHeartRate;
        MyUser_1.User_Ez_MaxINCLINE = Program_Data.Ez_MaxINCLINE;
        MyUser_1.User_Calories_Goal = Program_Data.Calories_Goal;
        MyUser_1.Diffculty_Level    = Program_Data.Diffculty_Level;
        
    }else if(Program_Select == User_2){
        MyUser_2.User_Time         = Program_Data.Goal_Time;
        MyUser_2.User_Age          = Program_Data.Age;
        MyUser_2.User_Weight       = Program_Data.Weight;
        MyUser_2.User_Program_Like = Program_Data.Like_Program;
        
        MyUser_2.User_THR           = Program_Data.TargetHeartRate;
        MyUser_2.User_Ez_MaxINCLINE = Program_Data.Ez_MaxINCLINE;
        MyUser_2.User_Calories_Goal = Program_Data.Calories_Goal;
        MyUser_2.Diffculty_Level    = Program_Data.Diffculty_Level;  
    }
    
    memset(Write_FlashBuffer256,0x00,256);
    
    //填暫存值   把要儲存的 Custom Data 先寫入 Flash 暫存 256 buffer
    Custom_Data_To_Flash_Buffer256();
    Write_FlashBuffer256[255] = 0xAA; //有寫入動作 就寫0xAA
 
    //寫進去
    Write_EE_Flash( Custom_Save_Address , 256 , Write_FlashBuffer256);
    
    //讀出來   並把FLASH 裡面的Custom 資料解出來 ------------------------------------
    CMD_READ(Sector_0 , FlashBuffer4096, Custom_Save_Address + 256);
    Custom_Data_Decode();
      
}


void Write_Default_Custom_Program_Data(){
    //載入 預設值
    unsigned short WeightTmp;
    
    if(Machine_Data.System_UNIT == Metric){
        WeightTmp = 70;
    }else if(Machine_Data.System_UNIT == Imperial){
        WeightTmp = 155;
    }
    
    MyCustom_1.Custom_Time     = 1800;
    MyCustom_1.Custom_Age      = 30;
    
    MyCustom_1.Custom_Weight   = WeightTmp;
    MyCustom_1.Custom_WorkTime = 4;
    MyCustom_1.Custom_RestTime = 4;
    //--------------------------------------
    MyCustom_2.Custom_Time     = 1800;
    MyCustom_2.Custom_Age      = 30;
    MyCustom_2.Custom_Weight   = WeightTmp;
    MyCustom_2.Custom_WorkTime = 4;
    MyCustom_2.Custom_RestTime = 4;
    //--------------------------------------
    MyUser_1.User_Time         = 1800;
    MyUser_1.User_Age          = 30;
    MyUser_1.User_Weight       = WeightTmp;
    MyUser_1.User_Program_Like = Manual;
    MyUser_1.User_THR          = 140;
    MyUser_1.User_Ez_MaxINCLINE = 75; 
    MyUser_1.User_Calories_Goal = 500;
    MyUser_1.Diffculty_Level    = 7;
    //--------------------------------------
    MyUser_2.User_Time         = 1800;
    MyUser_2.User_Age          = 30;
    MyUser_2.User_Weight       = WeightTmp;
    MyUser_2.User_Program_Like = Manual;
    MyUser_2.User_THR          = 140;
    MyUser_2.User_Ez_MaxINCLINE = 75; 
    MyUser_2.User_Calories_Goal = 500;
    MyUser_2.Diffculty_Level    = 7;
    //--------------------------------------
    
    memset(Write_FlashBuffer256,0x00,256);
    
    //填暫存值   把要儲存的 Custom Data 先寫入 Flash 暫存 256 buffer
    Custom_Data_To_Flash_Buffer256();
    Write_FlashBuffer256[255] = 0xAA; //有寫入動作 就寫0xAA
    
    //寫進去
    Write_EE_Flash( Custom_Save_Address , 256 , Write_FlashBuffer256);
}


void Flash_Custom_Data_Loading(){
    
    unsigned char DefaultValueChangeFlag;
    
    CMD_READ(Sector_0 , FlashBuffer4096, 4096);
    DefaultValueChangeFlag = FlashBuffer4096[Custom_Save_Address + 255];
    
    //***********************************************************************************************************************//
    //***************************||     沒有初始化過才會做      ||************************************************************// 
    //***********************************************************************************************************************//
    //如果DefaultValueChangeFlag 不等於0xAA 代表Flash 還沒初始化過
    if(DefaultValueChangeFlag != 0xAA){  
        Write_Default_Custom_Program_Data();   //寫入預設的Custom Data
    }
    //***********************************************************************************************************************//
    //********************************************************************************************************// 
    //***********************************************************************************************************************//
    
    //讀出來   並把FLASH 裡面的Custom 資料解出來 ------------------------------------
    CMD_READ(Sector_0 , FlashBuffer4096, 4096);
    Custom_Data_Decode();
    __asm("NOP");
    
}


//讀出   Flash Machine_Data
void Flash_Machine_Data_Loading(){
    
    
    CMD_READ(Sector_0 , FlashBuffer4096, 4096);
    
    //------------ 把FLASH ODO  最大最小 速度  揚升   資料解出來 ------------------------------------
  
   //代表沒有初始化過
   if(FlashBuffer4096[Machine_Save_Address + 255] == 0xFF){
       
       Write_Machine_Data_Init(Metric);  //初始化  : 公制
       
   }
   
    Machine_Data.TotalDistance = (unsigned int) FlashBuffer4096[Machine_Save_Address + 0]
                               + (unsigned int)(FlashBuffer4096[Machine_Save_Address + 1] <<  8)
                               + (unsigned int)(FlashBuffer4096[Machine_Save_Address + 2] << 16) 
                               + (unsigned int)(FlashBuffer4096[Machine_Save_Address + 3] << 24);
                               
    
    Machine_Data.Total_Times = (unsigned int)FlashBuffer4096[Machine_Save_Address + 4]
                             + (unsigned int)(FlashBuffer4096[Machine_Save_Address + 5] << 8)
                             + (unsigned int)(FlashBuffer4096[Machine_Save_Address + 6] << 16)
                             + (unsigned int)(FlashBuffer4096[Machine_Save_Address + 7] << 24); 
    
    
    
    Machine_Data.System_INCLINE_Max = (unsigned short) FlashBuffer4096[Machine_Save_Address + 8]
                                    + (unsigned short)(FlashBuffer4096[Machine_Save_Address + 9] << 8);
                                    
    Machine_Data.System_SPEED_Max   = (unsigned short) FlashBuffer4096[Machine_Save_Address + 10] 
                                    + (unsigned short)(FlashBuffer4096[Machine_Save_Address + 11] << 8);
                                     
   
    Machine_Data.System_INCLINE_Min = (unsigned short) FlashBuffer4096[Machine_Save_Address + 12]
                                    + (unsigned short)(FlashBuffer4096[Machine_Save_Address + 13] << 8);
                                     
    
    Machine_Data.System_SPEED_Min   = (unsigned short) FlashBuffer4096[Machine_Save_Address + 14]
                                    + (unsigned short)(FlashBuffer4096[Machine_Save_Address + 15] << 8);
    
                                        
    Machine_Data.System_UNIT = (System_Unit_Def)FlashBuffer4096[Machine_Save_Address + 16];
    
    System_Unit = Machine_Data.System_UNIT;

//----------------------------------------------------------
    
    __asm("NOP");
}


//將 Machine_Data 資料寫入暫存陣列..
unsigned char MachineDataArray[17];
void MachineData_To_Array(){
    
    MachineDataArray[0] = (unsigned char)Machine_Data.TotalDistance;
    MachineDataArray[1] = (unsigned char)(Machine_Data.TotalDistance >> 8);
    MachineDataArray[2] = (unsigned char)(Machine_Data.TotalDistance >> 16);
    MachineDataArray[3] = (unsigned char)(Machine_Data.TotalDistance >> 24);
    
    MachineDataArray[4] = (unsigned char)Machine_Data.Total_Times;
    MachineDataArray[5] = (unsigned char)(Machine_Data.Total_Times >> 8);
    MachineDataArray[6] = (unsigned char)(Machine_Data.Total_Times >> 16);
    MachineDataArray[7] = (unsigned char)(Machine_Data.Total_Times >> 24);
    
    MachineDataArray[8] = (unsigned char)Machine_Data.System_INCLINE_Max;
    MachineDataArray[9] = (unsigned char)(Machine_Data.System_INCLINE_Max >> 8);
    
    MachineDataArray[10] = (unsigned char)Machine_Data.System_SPEED_Max;
    MachineDataArray[11] = (unsigned char)(Machine_Data.System_SPEED_Max >> 8);
    
    MachineDataArray[12] = (unsigned char)Machine_Data.System_INCLINE_Min;
    MachineDataArray[13] = (unsigned char)(Machine_Data.System_INCLINE_Min >> 8);
    
    MachineDataArray[14] = (unsigned char)Machine_Data.System_SPEED_Min;
    MachineDataArray[15] = (unsigned char)(Machine_Data.System_SPEED_Min >> 8);
    
    MachineDataArray[16] = (unsigned char)(Machine_Data.System_UNIT);
    
}


//機器資料初始化  清掉總時間 總里程ODO 最大速度最小速度 最大揚升最小揚升  Unit為設定單位: 公制 or 英制
void Write_Machine_Data_Init(System_Unit_Def Unit){
    
    //-----------數值初始化-----------------------------
    Machine_Data.TotalDistance = 0;
    Machine_Data.Total_Times   = 0;
        
    Machine_Data.System_INCLINE_Max = 150;
    Machine_Data.System_INCLINE_Min = 0;
    
    if(Unit == Metric ){
	Machine_Data.System_UNIT = Metric;
        Machine_Data.System_SPEED_Max = 240;
        Machine_Data.System_SPEED_Min = 5;
    }else if(Unit == Imperial){
        Machine_Data.System_UNIT = Imperial;
        Machine_Data.System_SPEED_Max = 150;
        Machine_Data.System_SPEED_Min = 3;
    }           
    
    MachineData_To_Array();   
    unsigned char WriteBuffer256[256];
    memset(WriteBuffer256,0x00,256);
    WriteBuffer256[255] = 0x01;              //把0xFF清掉    -->表示初始化過了
    memcpy(WriteBuffer256, MachineDataArray,17);
    Write_EE_Flash( Machine_Save_Address , 256 , WriteBuffer256);  //寫入Flash
    Flash_Machine_Data_Loading();                                    //讀出Flash (確認寫入    
    //機器資料重置  Custom Program也一起重置  防止 公制模式  存的資料卻是英制
    Write_Default_Custom_Program_Data();
}



//跑步機  距離ODO  運動結束 總時間 更新寫入
void Machine_Data_Update(){
  
    if(System_Unit == Metric){  
        Machine_Data.TotalDistance += (Program_Data.Distance/100);  //單位:公尺  Machine_Data.TotalDistance讀出來/1000 =公里       
    }else if(System_Unit == Imperial){
        Machine_Data.TotalDistance += (Program_Data.Distance/100) * 1.61;  //單位:公尺  Machine_Data.TotalDistance讀出來/1000 =公里
    }
  
    Machine_Data.Total_Times += (Program_Data.Goal_Time - Program_Data.Goal_Counter); //
    MachineData_To_Array();
    Write_EE_Flash( Machine_Save_Address , 17 , MachineDataArray);   
    Flash_Machine_Data_Loading();
    
}

void Write_SerialNumber_To_Flash(char *Data){
    Write_EE_Flash( Serial_Number_Address , 14 , (unsigned char*) Data); 
}

void Read_SerialNumber_From_Flash(char *Data){
    Read_EE_Flash( Serial_Number_Address , 14 , (unsigned char*) Data);
}

unsigned char TestWriteData[1] ={0xAB};
unsigned char TestReadData[1];
unsigned char Flash_Test_Engineer(){
    
    Write_EE_Flash( Flash_EngineerTest_Address , 1, TestWriteData );    //寫入測試
    HAL_Delay(50);
    Read_EE_Flash( Flash_EngineerTest_Address ,1 , TestReadData);      //讀取測試
    
    if(TestReadData[0] == TestWriteData[0]){
        return 1;
    }else{
        return 0;
    }
}

//洗掉指定Sector
void FlashErase(unsigned int Sector){
    CMD_SE(Sector);   //洗掉Sector_0   4096 byte
    CMD_READ(Sector_0 , Read_Buffer_256Byte, 256);
    __asm("NOP");
}

void Flash_Read_Test(unsigned int Sector_Addr){
    CMD_READ( Sector_Addr , FlashBuffer4096, 4096);
    __asm("NOP");  
}
