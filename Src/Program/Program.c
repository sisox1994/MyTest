#include "system.h"


const unsigned char Index_To_Bar[] = {1,1,1,1,
                                        2,2,2,2,
                                        3,3,3,3,
                                        4,4,4,4,
                                        5,5,5,5,
                                        6,6,6,6,
                                        7,7,7,7,
                                        8,8,8};




void User_Stored_Setting_Init(){
    
    if( Program_Select == User_1 ){
        
        Program_Data.Goal_Time    = MyUser_1.User_Time; 
        Program_Data.Age          = MyUser_1.User_Age;  
        Program_Data.Weight       = MyUser_1.User_Weight; 
        
        Program_Data.Like_Program = MyUser_1.User_Program_Like; 
                 
        Program_Data.TargetHeartRate = MyUser_1.User_THR;
        
        Program_Data.Ez_MaxINCLINE   = MyUser_1.User_Ez_MaxINCLINE;
                
        Program_Data.Calories_Goal   = MyUser_1.User_Calories_Goal;
        Program_Data.Diffculty_Level = MyUser_1.Diffculty_Level; 
           
        
    }else if( Program_Select == User_2 ){
        
        Program_Data.Goal_Time    = MyUser_2.User_Time; 
        Program_Data.Age          = MyUser_2.User_Age;  
        Program_Data.Weight       = MyUser_2.User_Weight; 
        
        Program_Data.Like_Program = MyUser_2.User_Program_Like; 
        
        Program_Data.TargetHeartRate = MyUser_2.User_THR;
        
        Program_Data.Ez_MaxINCLINE   = MyUser_2.User_Ez_MaxINCLINE;
        
        Program_Data.Calories_Goal   = MyUser_2.User_Calories_Goal;
        Program_Data.Diffculty_Level = MyUser_2.Diffculty_Level; 

    }
    
}


void GetMaxHeartRate(){
    
    Program_Data.MaxHeartRate = 207 - ( (67 * Program_Data.Age)/100); 
                                       
}

void GetTargetHeartRate(){
    
    Program_Data.TargetHeartRate = (Program_Data.MaxHeartRate * 75) / 100; 
                                       
}

void UserData_Init(){
    
    Program_Data.Age = 30;
    
    
    if(System_Unit ==  Metric ){
	Program_Data.Weight = 70;
    }else if(System_Unit ==  Imperial){
        Program_Data.Weight = 155;
    }
  
    Program_Data.Distance = 0;
    Program_Data.Calories = 0;
    Program_Data.Pace     = 0;
    
    Program_Data.INCLINE_ValueDiff = 0;  //與樣本楊升的差值
    Program_Data.MasterPage = 0;
    
    ulAltitude = 0;
    
    GetMaxHeartRate();
    GetTargetHeartRate();
   
}


//--------------0 ~150   =>  棒子高度
unsigned char SysINC_To_BarCode(unsigned char Index){

    return Index_To_Bar[Index/5];
}



//卡路里 相關資料運算

unsigned int uiCalTemp;
unsigned short usCaloriesMin;
unsigned short usCaloriesHour;
void Calorie_Calculate(){

    unsigned int uii,uij;
    //unsigned int uiCalTemp;
    unsigned char ucgrade;
    
    
    ucgrade = System_INCLINE;
  
   if(System_Unit == Metric){
        /* 公制 */
        if(System_SPEED >= 59){
            uii = 153200;
            uij = 6850;
        }else{
            uii = 76800;
            uij = 13700;
        }
        uiCalTemp=(((1000+((uii*System_SPEED)/1609)+((uij*System_SPEED*ucgrade)/3218))*Program_Data.Weight)/3600);   
    }else if(System_Unit == Imperial){     
        /* 英制 */
        if(System_SPEED >= 37){
            uii = 15320;
            uij = 685;
        }else{
            uii = 7680;
            uij = 1370;
        }
        uiCalTemp=(((100000+(uii*System_SPEED)+(uij*System_SPEED*ucgrade/2))*Program_Data.Weight)/792000);
    }
   
  
    Program_Data.Calories += uiCalTemp;
   
    if(Program_Data.Calories > 9999999)
        Program_Data.Calories = 0;
    
    usCaloriesMin = (unsigned short)(uiCalTemp * 0.6);
    usCaloriesHour = (unsigned short)(uiCalTemp * 3.6);
    
    if(usCaloriesHour > 9999)
        usCaloriesHour = 9999;
    
    Program_Data.Calories_HR = (uiCalTemp * 3600)/1000; //單位 kcal/小時
    
     
}


unsigned short usMET;
unsigned short usDHMtemp;

void Mets_Calculate(){
    
    unsigned int uii;
    unsigned int uij;
    unsigned int uiMetTemp;
    
    
    //=====計算======
    if(System_SPEED > 45){
        uii = 2;
        uij = 9;
    }else{
        uii = 1;
        uij = 18;
    }
    
    if(System_Unit == Metric  ){ 
         uiMetTemp = ((uii*16646*System_SPEED)+(uij*83*System_SPEED*(System_INCLINE/5))+350000);      
    }else if(System_Unit == Imperial  ){
         uiMetTemp = ((uii*26800*System_SPEED)+(uij*134*System_SPEED*(System_INCLINE/5))+350000);  
    }
    
    usMET = uiMetTemp / 35000;
    
    //======顯示======
    usDHMtemp = usMET;
    
}


const unsigned short usInclinePercentage[31] = 
{  0,50, 105, 157, 209, 262, 297, 349, 402, 437, 472,
   524, 577, 612, 664, 717, 752, 805, 857, 910, 945,
   998,1033,1086,1122,1175,1210,1246,1299,1352,1388};

unsigned int AltitudeTemp;
unsigned int Altitudekm;
unsigned short Data16;


void ALTI_Calculate(){
   
    /* Altitude */
    if(System_Unit == Metric){     // 公制
        
        AltitudeTemp = Program_Data.Distance/100;

        if(AltitudeTemp!=Altitudekm){
            
            if(AltitudeTemp>(Altitudekm+16)){
                Data16 = ((AltitudeTemp-Altitudekm)/16);
                Altitudekm += (Data16*16);
                ulAltitude = ulAltitude+((Data16*usInclinePercentage[(System_INCLINE/5)]*1609)/100000);
                
            }else{
                if(AltitudeTemp<Altitudekm){
                    Data16 = (((10000+AltitudeTemp)-Altitudekm)/16);
                    Altitudekm += (Data16*16);
                    ulAltitude = ulAltitude+((Data16*usInclinePercentage[(System_INCLINE/5)]*1609)/100000);
                } 
            }
        }
        if((ulAltitude/10)>99999)
            ulAltitude-=1000000;
        
    }else if(System_Unit == Imperial  ){     // 英制 FT
        
        AltitudeTemp = Program_Data.Distance/10;

        if(AltitudeTemp != Altitudekm){
            
            if(AltitudeTemp>Altitudekm)
                ulAltitude = ulAltitude+(((AltitudeTemp-Altitudekm)*usInclinePercentage[(System_INCLINE/5)]*264)/10000);
            else
                ulAltitude = ulAltitude+((AltitudeTemp*usInclinePercentage[(System_INCLINE/5)]*264)/10000);
        }
        Altitudekm=AltitudeTemp;
        if((ulAltitude/10)>99999)
            ulAltitude-=1000000;
    }
}
