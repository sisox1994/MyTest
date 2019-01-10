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
//�`�@1024 ��


//---Custom Data �W���bSector_0   Page 1
#define OTA_Mode_Flag_Address      0x000000
#define Update_FW_Size_Address     0x000018

#define Flash_EngineerTest_Address  0x0000F0     //�u�{�Ҧ� Flash Ū�g�\�����
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

//---------------------------------------Flash  �q�μg�J Ū�� functions ----------------------------------
unsigned short start_Sector;
unsigned short Sector_use_amount;

//(�b�a�} start_addr )  (�g�J amount ��byte��� )   (�q*Data���V���a�})
void Write_EE_Flash( unsigned int start_addr , unsigned int amount , unsigned char *Data){
    
    unsigned char ucWrite256bytebuffer[256];
    start_Sector = start_addr / 4096;   //�n�g�J�Ӱ_�lSector
    Sector_use_amount  =  1 + (start_addr + amount) / 4096;  //�ݬ� �`�@�n�g�J�X��sector
    
    for(unsigned char i = start_Sector ; i < (start_Sector + Sector_use_amount) ; i++){
        CMD_READ( i * 4096 , FlashBuffer4096, 4096);  //��Ū�X Setor 4096�Ӹ��
        CMD_SE(i * 4096);  //Ū�X ����s�b mcu �N�i�H���~���F
        
        for(unsigned int page = 0 ; page < 4096 ; page += 256){  //�@��Setor �`�@16 ��Page �@��page 256 byte
            memset(ucWrite256bytebuffer,0x00,256);         //���M�� �g�J�@��Page 256byte ��buffer   
            for(unsigned int j = 0; j  < 256; j++ ){
                if((i * 4096 + (page + j) >= start_addr ) && ( i * 4096 + (page + j) < (start_addr + amount) )){    
                    ucWrite256bytebuffer[  (i * 4096 + (page + j)) % 256 ] = Data[ ( i * 4096 + (page + j) )  -  start_addr   ];
                }else{
                    ucWrite256bytebuffer[j] = FlashBuffer4096[ page + j ]; //����쥻���ȼg�ibuffer
                }                
            }
            CMD_PP( i * 4096 + page, ucWrite256bytebuffer , 256);   //�g�@��page �i�h 
        }
    }
    //-------------�g�i�h��   Ū�X�ӽT�{---------------
    CMD_READ( Sector_0 , FlashBuffer4096, 4096);  //��Ū�X Setor 4096�Ӹ��
    __asm("NOP");
}

unsigned short start_Index;
//(�q �a�} start_addr) (Ū�� amount ��byte �����) (�s��� *Data���V���a�})
void Read_EE_Flash( unsigned int start_addr , unsigned int amount , unsigned char *Data){
    
    start_Sector = start_addr / 4096;   //�nŪ�X���Ӱ_�lSector
    start_Index  = start_addr % 4096;
    CMD_READ( start_Sector * 4096 , FlashBuffer4096, 4096);  //��Ū�X Setor 4096�Ӹ��
    for(unsigned short i = start_Index; i < (start_Index + amount) ; i  ++ ){
        Data[i-start_Index] = FlashBuffer4096[i];
    }
}
//---------------------------------------Flash  �q�μg�J Ū�� functions ----------------------------------

//===================    OTA  Firmware Data Flash  Functions    ===============================

// �� APP���쪺 Firmware Size �g�J Flash  (�� Bootloader Ū����) 
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
//�ٰ� �s�� APP  ���쪺 Firmware  Flash �x�s�Ŷ�
void Update_FW_Flash_Erase(){
    
    //�ٰ��ݭn�@�q�ɶ� ������Btm���_ �w������
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);   
    Erase_Setor_Num = (FW.total_Size / 4096) + 1;
    
    for(unsigned char i = 0; i < Erase_Setor_Num; i++   ){   //2991
        CMD_SE( Update_Firmware_Address + (i * 4096));  
    }
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //----------------
}

//OTA�Ҧ� �qAPP���Ӫ���� �@���g�J16byte
extern unsigned char FW_Buff_16Byte[16];
void Write_EE_Flash_FW_Data_16Byte( unsigned int page , unsigned char *Data){
    CMD_PP( Update_Firmware_Address + (page * 16)  , Data , 16);   //�g�@��page(16byte) �i�h   
}

//Firmware �g�JFlash���� Ū�X�ӽT�{
void Fw_Write_Finish_Check(){
    for(unsigned char i = 0; i < Erase_Setor_Num; i++   ){
        CMD_READ( Update_Firmware_Address + (i * 4096) , FlashBuffer4096, 4096);
        __asm("NOP");
    }
}
//===================    OTA  Firmware Data Flash  Functions    ===============================

//�����Ӧr��O�_�ۦP
unsigned char str_check(unsigned char *Str1 , unsigned char *Str2 ,unsigned char Num){

    for(unsigned char i = 0; i < Num ; i++){
        if( Str1[i] != Str2[i]){
            return 0;
        } 
    }
    return 1;
}

//==========================   �g�J�t�� Reboot �i�J���Ҧ�   ===============================================

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
        }  //bootloader Mode �Ȯɥ��d�b�o��   
    }
    
    if(FlagCheck[0] == 0xFF){  // �Ĥ@���N��   �S����l�ƹL
        Flash_Write_Applicantion_Mode();
    }
    __asm("NOP");
}

//------------------------- �g�J�t�κX�� ------------------------------------------
void Flash_Write_OTA_Mode(){
    Write_EE_Flash( OTA_Mode_Flag_Address , sizeof(OTA_Flag) ,OTA_Flag);
}
void Flash_Write_Applicantion_Mode(){
    Write_EE_Flash( OTA_Mode_Flag_Address , sizeof(Application_Flag) ,Application_Flag);
}
void Flash_Write_BootLoader_Mode(){
    Write_EE_Flash( OTA_Mode_Flag_Address , sizeof(Bootloader_Flag) ,Bootloader_Flag);
    Flash_Write_Firmware_Total_Size();  // �g�J��s�� size
}

//======================================================================================================

//*******************   Program __ Custom_1,Custom_2  User_1,User_2  ��Ʀs��   ************************

void Custom_Data_Decode(){

    //------------------------- ��FLASH �̭���Custom ��ƸѥX�� ------------------------------------

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
    
    //��Ȧs��   ��n�x�s�� Custom Data ���g�J Flash �Ȧs 256 buffer
    Custom_Data_To_Flash_Buffer256();
    Write_FlashBuffer256[255] = 0xAA; //���g�J�ʧ@ �N�g0xAA
 
    //�g�i�h
    Write_EE_Flash( Custom_Save_Address , 256 , Write_FlashBuffer256);
    
    //Ū�X��   �ç�FLASH �̭���Custom ��ƸѥX�� ------------------------------------
    CMD_READ(Sector_0 , FlashBuffer4096, Custom_Save_Address + 256);
    Custom_Data_Decode();
      
}


void Write_Default_Custom_Program_Data(){
    //���J �w�]��
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
    
    //��Ȧs��   ��n�x�s�� Custom Data ���g�J Flash �Ȧs 256 buffer
    Custom_Data_To_Flash_Buffer256();
    Write_FlashBuffer256[255] = 0xAA; //���g�J�ʧ@ �N�g0xAA
    
    //�g�i�h
    Write_EE_Flash( Custom_Save_Address , 256 , Write_FlashBuffer256);
}


void Flash_Custom_Data_Loading(){
    
    unsigned char DefaultValueChangeFlag;
    
    CMD_READ(Sector_0 , FlashBuffer4096, 4096);
    DefaultValueChangeFlag = FlashBuffer4096[Custom_Save_Address + 255];
    
    //***********************************************************************************************************************//
    //***************************||     �S����l�ƹL�~�|��      ||************************************************************// 
    //***********************************************************************************************************************//
    //�p�GDefaultValueChangeFlag ������0xAA �N��Flash �٨S��l�ƹL
    if(DefaultValueChangeFlag != 0xAA){  
        Write_Default_Custom_Program_Data();   //�g�J�w�]��Custom Data
    }
    //***********************************************************************************************************************//
    //********************************************************************************************************// 
    //***********************************************************************************************************************//
    
    //Ū�X��   �ç�FLASH �̭���Custom ��ƸѥX�� ------------------------------------
    CMD_READ(Sector_0 , FlashBuffer4096, 4096);
    Custom_Data_Decode();
    __asm("NOP");
    
}


//Ū�X   Flash Machine_Data
void Flash_Machine_Data_Loading(){
    
    
    CMD_READ(Sector_0 , FlashBuffer4096, 4096);
    
    //------------ ��FLASH ODO  �̤j�̤p �t��  ����   ��ƸѥX�� ------------------------------------
  
   //�N��S����l�ƹL
   if(FlashBuffer4096[Machine_Save_Address + 255] == 0xFF){
       
       Write_Machine_Data_Init(Metric);  //��l��  : ����
       
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


//�N Machine_Data ��Ƽg�J�Ȧs�}�C..
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


//������ƪ�l��  �M���`�ɶ� �`���{ODO �̤j�t�׳̤p�t�� �̤j���ɳ̤p����  Unit���]�w���: ���� or �^��
void Write_Machine_Data_Init(System_Unit_Def Unit){
    
    //-----------�ƭȪ�l��-----------------------------
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
    WriteBuffer256[255] = 0x01;              //��0xFF�M��    -->��ܪ�l�ƹL�F
    memcpy(WriteBuffer256, MachineDataArray,17);
    Write_EE_Flash( Machine_Save_Address , 256 , WriteBuffer256);  //�g�JFlash
    Flash_Machine_Data_Loading();                                    //Ū�XFlash (�T�{�g�J    
    //������ƭ��m  Custom Program�]�@�_���m  ���� ����Ҧ�  �s����ƫo�O�^��
    Write_Default_Custom_Program_Data();
}



//�]�B��  �Z��ODO  �B�ʵ��� �`�ɶ� ��s�g�J
void Machine_Data_Update(){
  
    if(System_Unit == Metric){  
        Machine_Data.TotalDistance += (Program_Data.Distance/100);  //���:����  Machine_Data.TotalDistanceŪ�X��/1000 =����       
    }else if(System_Unit == Imperial){
        Machine_Data.TotalDistance += (Program_Data.Distance/100) * 1.61;  //���:����  Machine_Data.TotalDistanceŪ�X��/1000 =����
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
    
    Write_EE_Flash( Flash_EngineerTest_Address , 1, TestWriteData );    //�g�J����
    HAL_Delay(50);
    Read_EE_Flash( Flash_EngineerTest_Address ,1 , TestReadData);      //Ū������
    
    if(TestReadData[0] == TestWriteData[0]){
        return 1;
    }else{
        return 0;
    }
}

//�~�����wSector
void FlashErase(unsigned int Sector){
    CMD_SE(Sector);   //�~��Sector_0   4096 byte
    CMD_READ(Sector_0 , Read_Buffer_256Byte, 256);
    __asm("NOP");
}

void Flash_Read_Test(unsigned int Sector_Addr){
    CMD_READ( Sector_Addr , FlashBuffer4096, 4096);
    __asm("NOP");  
}
