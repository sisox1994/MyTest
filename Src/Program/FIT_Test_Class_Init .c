#include "system.h"

//--------------  ARMY (Male)  --------------------------//
extern unsigned char ARMY_SCORE_MALE_17_21[73];
extern unsigned char ARMY_SCORE_MALE_22_26[91];
extern unsigned char ARMY_SCORE_MALE_27_31[93];
extern unsigned char ARMY_SCORE_MALE_32_36[108];
extern unsigned char ARMY_SCORE_MALE_37_41[115];
extern unsigned char ARMY_SCORE_MALE_42_46[116];
extern unsigned char ARMY_SCORE_MALE_47_51[121];
extern unsigned char ARMY_SCORE_MALE_52_56[118];
extern unsigned char ARMY_SCORE_MALE_57_61[112];
extern unsigned char ARMY_SCORE_MALE_62up[108];
//--------------  ARMY (Female)  --------------------------//
extern unsigned char ARMY_SCORE_Female_17_21[83];
extern unsigned char ARMY_SCORE_Female_22_26[100];
extern unsigned char ARMY_SCORE_Female_27_31[107];
extern unsigned char ARMY_SCORE_Female_32_36[106];
extern unsigned char ARMY_SCORE_Female_37_41[95];
extern unsigned char ARMY_SCORE_Female_42_46[91];
extern unsigned char ARMY_SCORE_Female_47_51[89];
extern unsigned char ARMY_SCORE_Female_52_56[75];
extern unsigned char ARMY_SCORE_Female_57_61[68];
extern unsigned char ARMY_SCORE_Female_62up[65];

//------------------- NAVY  ----------------------------------//
extern unsigned short NAVY_Point_List[5];
//--(Male)--
extern unsigned short NAVY_Time_Male_20_24[5];
extern unsigned short NAVY_Time_Male_25_29[5];
extern unsigned short NAVY_Time_Male_30_34[5];
extern unsigned short NAVY_Time_Male_35_39[5];
extern unsigned short NAVY_Time_Male_40_44[5];
extern unsigned short NAVY_Time_Male_45_49[5];
extern unsigned short NAVY_Time_Male_50_57[5];
extern unsigned short NAVY_Time_Male_55_59[5];
extern unsigned short NAVY_Time_Male_60_64[5];
extern unsigned short NAVY_Time_Male_65up[5];
//--(female)--
extern unsigned short NAVY_Time_Female_17_19[5];
extern unsigned short NAVY_Time_Female_20_24[5];
extern unsigned short NAVY_Time_Female_25_29[5];
extern unsigned short NAVY_Time_Female_30_34[5];
extern unsigned short NAVY_Time_Female_35_39[5];
extern unsigned short NAVY_Time_Female_40_44[5];
extern unsigned short NAVY_Time_Female_45_49[5];
extern unsigned short NAVY_Time_Female_50_57[5];
extern unsigned short NAVY_Time_Female_55_59[5];
extern unsigned short NAVY_Time_Female_60_64[5];
extern unsigned short NAVY_Time_Female_65up[5];

unsigned char getNAVYPointIndex(unsigned int usedTime ,unsigned short *array){
    
    if(usedTime <= array[0]){   
        return 0;
    }else if( (usedTime > array[0]) && (usedTime <= array[1])){
        return 1;
    }else if( (usedTime > array[1]) && (usedTime <= array[2])){
        return 2;
    }else if( (usedTime > array[2]) && (usedTime <= array[3])){
        return 3;
    }else if( (usedTime > array[3]) && (usedTime <= array[4])){
        return 4;
    }else{
        return 4;
    }
}

//-----------------------------AIR FORCE -----------------------------------------------------------------------------------------------
//---(Male)-----
extern unsigned short AIRF_Time_Male_30_Down[25];
extern unsigned short AIRF_Score_Male_30_Down[25];
extern unsigned char  AIRF_RISK_Male_30_Down[25];

extern unsigned short AIRF_Time_Male_30_39[23];
extern unsigned short AIRF_Score_Male_30_39[23];
extern unsigned char  AIRF_RISK_Male_30_39[23];

extern unsigned short AIRF_Time_Male_40_49[24];
extern unsigned short AIRF_Score_Male_40_49[24];
extern unsigned char  AIRF_RISK_Male_40_49[24];

extern unsigned short AIRF_Time_Male_50_59[23];
extern unsigned short AIRF_Score_Male_50_59[23];
extern unsigned char  AIRF_RISK_Male_50_59[23];

extern unsigned short AIRF_Time_Male_60_up[23];
extern unsigned short AIRF_Score_Male_60_up[23];
extern unsigned char  AIRF_RISK_Male_60_up[23];

//---(female)---
extern unsigned short AIRF_Time_Female_30_Down[24];
extern unsigned short AIRF_Score_Female_30_Down[24];
extern unsigned char AIRF_RISK_Female_30_Down[24];

extern unsigned short AIRF_Time_Female_30_39[22];
extern unsigned short AIRF_Score_Female_30_39[22];
extern unsigned char AIRF_RISK_Female_30_39[22];

extern unsigned short AIRF_Time_Female_40_49[22];
extern unsigned short AIRF_Score_Female_40_49[22];
extern unsigned char AIRF_RISK_Female_40_49[22];

extern unsigned short AIRF_Time_Female_50_59[18];
extern unsigned short AIRF_Score_Female_50_59[18];
extern unsigned char AIRF_RISK_Female_50_59[18];

extern unsigned short AIRF_Time_Female_60_up[17];
extern unsigned short AIRF_Score_Female_60_up[17];
extern unsigned char AIRF_RISK_Female_60_up[17];
//--------------------------------------------------------------------


extern unsigned short WFI_VO2MAX_Table[41];

void getAIRForce_Score_Risk(unsigned int useTime ,unsigned short *TimeArray ,unsigned short *ScoreArray , unsigned char *RiskArray  ,unsigned char size){
    
    unsigned char index;
    
    if( useTime < TimeArray[0]){              //最高分
        index = 0;
    }else if(useTime >= TimeArray[size-1]){    //最低分
        index = size-1;
    }else{                                    //區間算分
        
        for(unsigned char i = 1; i < (size - 1);i++ ){
            
            if( (useTime >= TimeArray[i])&&(useTime < TimeArray[i+1]) ){
                index = i;
            }
        }
    }
    
    Program_Data.FitTest_Score = ScoreArray[index];
    Program_Data.FitTest_RISK  = RiskArray[index];
    __asm("NOP");
}

void GetFitTest_Score(){
    
    unsigned int UsedTime;
    UsedTime = Program_Data.Goal_Time - Program_Data.Goal_Counter;
    
     //-----------------------ARMY-----------------------------------------------------------------
    if(Program_Select == FIT_ARMY){
        
        if(Program_Data.Gender == Male){
            
            if(Program_Data.Age <= 21){
            
                 unsigned int BaseTime = 780; //13:00 以內
                
                if(UsedTime <= BaseTime){   //13分鐘以內
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_17_21)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_17_21[ (UsedTime-1-BaseTime)/6 ];
                    }
                }
                   
            }else if((Program_Data.Age >= 22) && (Program_Data.Age <=26) ){
            
                unsigned int BaseTime = 780; //13:00 以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_22_26)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_22_26[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }else if((Program_Data.Age >= 27) && (Program_Data.Age <=31) ){
               
                unsigned int BaseTime = 798; //13:18以內
                
                if(UsedTime <= BaseTime){   
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_27_31)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_27_31[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }else if((Program_Data.Age >= 32) && (Program_Data.Age <=36) ){
            
                unsigned int BaseTime = 798; //13:18以內
                
                if(UsedTime <= BaseTime){   
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_32_36)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_32_36[ (UsedTime-1-BaseTime)/6 ];
                    }
                }  
                
            }else if((Program_Data.Age >= 37) && (Program_Data.Age <=41) ){
               
                unsigned int BaseTime = 816; //13:36以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_37_41)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_37_41[ (UsedTime-1-BaseTime)/6 ];
                    }
                }    
                
            }else if((Program_Data.Age >= 42) && (Program_Data.Age <=46) ){
                
                unsigned int BaseTime = 846; //14:06以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_42_46)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_42_46[ (UsedTime-1-BaseTime)/6 ];
                    }
                }    
                
            }else if((Program_Data.Age >= 47) && (Program_Data.Age <=51) ){
            
                unsigned int BaseTime = 864; //14:24以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_47_51)){
                        Program_Data.FitTest_Score = 5;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_47_51[ (UsedTime-1-BaseTime)/6 ];
                    }
                }  
                
            }else if((Program_Data.Age >= 52) && (Program_Data.Age <=56) ){
                
                unsigned int BaseTime = 882; //14:42以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_52_56)){
                        Program_Data.FitTest_Score = 7;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_52_56[ (UsedTime-1-BaseTime)/6 ];
                    }
                }   
                
            }else if((Program_Data.Age >= 57) && (Program_Data.Age <=61) ){
                
                unsigned int BaseTime = 918; //15:18以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_57_61)){
                        Program_Data.FitTest_Score = 3;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_57_61[ (UsedTime-1-BaseTime)/6 ];
                    }
                }        
                
            }else if( Program_Data.Age >= 62 ){
                
                unsigned int BaseTime = 942; //15:42以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_MALE_62up)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_MALE_62up[ (UsedTime-1-BaseTime)/6 ];
                    }
                }        
                
            }
            
        
        
        }else if(Program_Data.Gender == Female){
            
            if(Program_Data.Age <= 21){    
                
                unsigned int BaseTime = 936; //15:36以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_17_21)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_17_21[ (UsedTime-1-BaseTime)/6 ];
                    }
                }  
                      
            }else if((Program_Data.Age >= 22) && (Program_Data.Age <=26) ){
                
                unsigned int BaseTime = 936; //15:36以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_22_26)){
                        Program_Data.FitTest_Score = 0;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_22_26[ (UsedTime-1-BaseTime)/6 ];
                    }
                }  
                
            }else if((Program_Data.Age >= 27) && (Program_Data.Age <=31) ){
                
                unsigned int BaseTime = 948; //15:48以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_27_31)){
                        Program_Data.FitTest_Score = 9;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_27_31[ (UsedTime-1-BaseTime)/6 ];
                    }
                }  
                
            }else if((Program_Data.Age >= 32) && (Program_Data.Age <=36) ){
 
                unsigned int BaseTime = 954; //15:54以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_32_36)){
                        Program_Data.FitTest_Score = 27;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_32_36[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }else if((Program_Data.Age >= 37) && (Program_Data.Age <=41) ){
 
                unsigned int BaseTime = 1020; //17:00以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_37_41)){
                        Program_Data.FitTest_Score = 33;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_37_41[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }else if((Program_Data.Age >= 42) && (Program_Data.Age <=46) ){

                unsigned int BaseTime = 1044; //17:24以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_42_46)){
                        Program_Data.FitTest_Score = 42;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_42_46[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }else if((Program_Data.Age >= 47) && (Program_Data.Age <=51) ){
               
                unsigned int BaseTime = 1056; //17:36以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_47_51)){
                        Program_Data.FitTest_Score = 44;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_47_51[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }else if((Program_Data.Age >= 52) && (Program_Data.Age <=56) ){
                
                unsigned int BaseTime = 1140; //19:00以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_52_56)){
                        Program_Data.FitTest_Score = 44;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_52_56[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }else if((Program_Data.Age >= 57) && (Program_Data.Age <=61) ){
                
                unsigned int BaseTime = 1182; //19:42以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_57_61)){
                        Program_Data.FitTest_Score = 47;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_57_61[ (UsedTime-1-BaseTime)/6 ];
                    }
                }
                
            }else if( Program_Data.Age >= 62 ){
  
                unsigned int BaseTime = 1200; //20:00以內
                
                if(UsedTime <= BaseTime){  
                    Program_Data.FitTest_Score = 100;
                }else{
                    if( ((UsedTime-1-BaseTime)/6) >= sizeof(ARMY_SCORE_Female_62up)){
                        Program_Data.FitTest_Score = 48;
                    }else{
                        Program_Data.FitTest_Score = ARMY_SCORE_Female_62up[ (UsedTime-1-BaseTime)/6 ];
                    }
                } 
                
            }
            
        }
    
    }
    
    //-----------------------NAVY------------------------------------------------------------------
    if(Program_Select == FIT_NAVY){
       
        if(Program_Data.Gender == Male){
            
            if(Program_Data.Age <= 24){    
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_20_24)];
            }else if( (Program_Data.Age >= 25) && (Program_Data.Age <= 29) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_25_29)];
            }else if( (Program_Data.Age >= 30) && (Program_Data.Age <= 34) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_30_34)];
            }else if( (Program_Data.Age >= 35) && (Program_Data.Age <= 39) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_35_39)];
            }else if( (Program_Data.Age >= 40) && (Program_Data.Age <= 44) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_40_44)];
            }else if( (Program_Data.Age >= 45) && (Program_Data.Age <= 49) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_45_49)];
            }else if( (Program_Data.Age >= 50) && (Program_Data.Age <= 57) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_50_57)];
            }else if( (Program_Data.Age >= 55) && (Program_Data.Age <= 59) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_55_59)];
            }else if( (Program_Data.Age >= 60) && (Program_Data.Age <= 64) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_60_64)];
            }else if(Program_Data.Age >= 65){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Male_65up)];
            }
            
            
        }else if(Program_Data.Gender == Female){
            if(Program_Data.Age <= 19){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_17_19)];
            }else if((Program_Data.Age >= 20)&&(Program_Data.Age <= 24)){    
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_20_24)];
            }else if( (Program_Data.Age >= 25) && (Program_Data.Age <= 29) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_25_29)];
            }else if( (Program_Data.Age >= 30) && (Program_Data.Age <= 34) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_30_34)];
            }else if( (Program_Data.Age >= 35) && (Program_Data.Age <= 39) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_35_39)];
            }else if( (Program_Data.Age >= 40) && (Program_Data.Age <= 44) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_40_44)];
            }else if( (Program_Data.Age >= 45) && (Program_Data.Age <= 49) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_45_49)];
            }else if( (Program_Data.Age >= 50) && (Program_Data.Age <= 57) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_50_57)];
            }else if( (Program_Data.Age >= 55) && (Program_Data.Age <= 59) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_55_59)];
            }else if( (Program_Data.Age >= 60) && (Program_Data.Age <= 64) ){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_60_64)];
            }else if(Program_Data.Age >= 65){
                Program_Data.FitTest_Score = NAVY_Point_List[getNAVYPointIndex(UsedTime,NAVY_Time_Female_65up)];
            }
        }
        
    
    }
   
    //-----------------------AIR FORCE------------------------------------------------------------------
    if(Program_Select == FIT_AIRFORCE){
        
        if(Program_Data.Gender == Male){
        
            if(Program_Data.Age < 30){
                
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Male_30_Down,AIRF_Score_Male_30_Down,AIRF_RISK_Male_30_Down,25);
                
            }else if( (Program_Data.Age >= 30) && (Program_Data.Age <= 39) ){

                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Male_30_39,AIRF_Score_Male_30_39,AIRF_RISK_Male_30_39,23);
           
            }else if( (Program_Data.Age >= 40) && (Program_Data.Age <= 49) ){
                           
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Male_40_49,AIRF_Score_Male_40_49,AIRF_RISK_Male_40_49,24);
                
            }else if( (Program_Data.Age >= 50) && (Program_Data.Age <= 59) ){
            
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Male_50_59,AIRF_Score_Male_50_59,AIRF_RISK_Male_50_59,23);
                
            }else if( Program_Data.Age >= 60 ){
                
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Male_60_up,AIRF_Score_Male_60_up,AIRF_RISK_Male_60_up,23);

            }    
           
        }else if(Program_Data.Gender == Female){
            
            if(Program_Data.Age < 30){
                
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Female_30_Down,AIRF_Score_Female_30_Down,AIRF_RISK_Female_30_Down,24);
                
            }else if( (Program_Data.Age >= 30) && (Program_Data.Age <= 39) ){
                
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Female_30_39,AIRF_Score_Female_30_39,AIRF_RISK_Female_30_39,22);
                
            }else if( (Program_Data.Age >= 40) && (Program_Data.Age <= 49) ){
           
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Female_40_49,AIRF_Score_Female_40_49,AIRF_RISK_Female_40_49,22);
                
            }else if( (Program_Data.Age >= 50) && (Program_Data.Age <= 59) ){
                
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Female_50_59,AIRF_Score_Female_50_59,AIRF_RISK_Female_50_59,18);
                
            }else if( Program_Data.Age >= 60 ){
                
                getAIRForce_Score_Risk(UsedTime,AIRF_Time_Female_60_up,AIRF_Score_Female_60_up,AIRF_RISK_Female_60_up,17);
                            
            }
            
        }
        
    }
    
    //-----------------------USMC------------------------------------------------------------------
    if(Program_Select == FIT_USMC){
        
        unsigned int BaseTime;
        
        if(Program_Data.Gender == Male){
            BaseTime = 1080; //18:00 以內
        }else if(Program_Data.Gender == Female){
            BaseTime = 1260;  //21:00 以內
        }
        
        if(UsedTime <= BaseTime){   
            Program_Data.FitTest_Score = 100;
        }else{
            if( ( (UsedTime - BaseTime - 1)/10 + 1)  >= 100 ){
                Program_Data.FitTest_Score = 0;
            }else{
                Program_Data.FitTest_Score = 100 - ((UsedTime - BaseTime - 1)/10 + 1);
            }
        }

    }
   
    //-----------------------WFI------------------------------------------------------------------
    if(Program_Select == FIT_WFI){
    
        if(UsedTime < 240){
            Program_Data.FitTest_Score = 0;
        }else if(UsedTime >= 240){
                     
            for(unsigned char i = 0; i < 41 ;i++){
                if( (UsedTime-180) > (60+ i*15)){
                    Program_Data.FitTest_Score = WFI_VO2MAX_Table[i];
                }
            }
            
        }
        
    }
    
    
}







void FIT_ARMY_Init(){
  
    UserData_Init(); 
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = 3200;    //3.2 km
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = 2000;    //2.0 mile
    }

    Program_Data.Goal_Time    = 360000; 
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    //---------Start Profile 樣本  --------------
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
    
    
    Table_96_To_BarArray_Mapping();
    
    
    //---PeriodNumber固定32格 ---------PeriodWidth = 目標距離 /32等分-------------------------------
    Program_Data.PeriodNumber = 32;
    Program_Data.PeriodWidth = Program_Data.Distance_Goal/32;
    Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;
  
}


void FIT_NAVY_Init(){
    

    UserData_Init(); 
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = 2400;         //2.4 km
    }else if(System_Unit == Imperial){
        Program_Data.Distance_Goal   = 1500;         //1.5 mile
    }

    Program_Data.Goal_Time    = 360000; 
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    //---------Start Profile 樣本  ---------------
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
    
    
    Table_96_To_BarArray_Mapping();
    
    //---PeriodNumber固定32格 ---------PeriodWidth = 目標距離 /32等分-------------------------------
    Program_Data.PeriodNumber = 32;
    Program_Data.PeriodWidth = Program_Data.Distance_Goal/32;
    Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;
    
}


void  FIT_AIRFORCE_Init(){

    
    UserData_Init(); 
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = 2400;   //2.4 km
    }else if(System_Unit == Imperial){         
        Program_Data.Distance_Goal   = 1500;   //1.5 mile
    }

    Program_Data.Goal_Time    = 360000; 
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    //---------Start Profile 樣本  ---------------
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
     
    Table_96_To_BarArray_Mapping();
    
    //---PeriodNumber固定32格 ---------PeriodWidth = 目標距離 /32等分-------------------------------
    Program_Data.PeriodNumber = 32;
    Program_Data.PeriodWidth = Program_Data.Distance_Goal/32;
    Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;
    
    
}


void FIT_USMC_Init(){
    
    UserData_Init(); 
    
    if(System_Unit == Metric){  
        Program_Data.Distance_Goal   = 4800;  //4.8 km
    }else if(System_Unit == Imperial){        
        Program_Data.Distance_Goal   = 3000;  //3.0 mile
    }

    Program_Data.Goal_Time    = 360000; 
    Program_Data.Goal_Counter = 360000;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    //---------Start Profile 樣本  ---------------
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 4;
    
    Template_To_Table_96(Manual_Profile_Template);
     
    Table_96_To_BarArray_Mapping();
    
    
    //---PeriodNumber固定32格 ---------PeriodWidth = 目標距離 /32等分-------------------------------
    Program_Data.PeriodNumber = 32;
    Program_Data.PeriodWidth = Program_Data.Distance_Goal/32;
    Program_Data.NextPeriodValue = Program_Data.Distance_Goal - Program_Data.PeriodWidth;
    
    
}


/*unsigned char WFI_Profile_Inc_Template[34] ={
  0,0,0,0,0,0,   //Warm up
  0,0,
  2,2,2,2,
  4,4,4,4,
  6,6,6,6,
  8,8,8,8,
  10,10,10,10,
  0,0,0,0,0,0   //Cool
};*/

unsigned char WFI_Profile_Inc_Template[34] ={
  0,0,0,0,0,0,   //Warm up
  0,0,
  4,4,4,4,
  8,8,8,8,
  12,12,12,12,
  16,16,16,16,
  20,20,20,20,
  0,0,0,0,0,0   //Cool
};



unsigned char WFI_Profile_Spd_Template[34] ={
  48,48,48,48,48,48,   //Warm up
  72,72,
  72,72,80,80,
  80,80,88,88,
  88,88,96,96,
  96,96,104,104,
  104,104,112,112,
  33,33,33,33,33,33               //Cool
};


unsigned char WFI_Profile_Spd_Template_Imperial[34] ={
    
  30,30,30,30,30,30,   //Warm up
  45,45,
  45,45,50,50,
  50,50,55,55,
  55,55,60,60,
  60,60,65,65,
  65,65,70,70,
  21,21,21,21,21,21               //Cool
};

void FIT_WFI_Init(){
    
    
    UserData_Init(); 
       
    Program_Data.Goal_Time    = 1020;    //熱身3分 + 目標11分鐘  +cool 3分 =17分
    Program_Data.Goal_Counter = 1020;
    
    Program_Data.NowPeriodIndex          = 0;
    Program_Data.PeriodIndex_After_Shift = 0;
    
    //---------Start Profile 樣本  ---------------
    Program_Data.Template_Loop_Start_Index = 0;
    Program_Data.Template_Table_Num        = 34;
    
    Template_To_Table_96(WFI_Profile_Inc_Template);
    Table_96_To_BarArray_Mapping();
    
    Program_Data.PeriodNumber = Program_Data.Goal_Time / 30;   //1020/30 = 34 格  因為30s變化一次profile    
    Program_Data.PeriodWidth  = 30;
    Program_Data.NextPeriodValue =  Program_Data.Goal_Time - Program_Data.PeriodWidth;   
    
    
    if(System_Unit == Metric ){
        for(unsigned int i = 0 ; i < Program_Data.PeriodNumber; i++){
            if(i < 96){ 
                Program_Data.SPEED_Table_96[i]   = WFI_Profile_Spd_Template[i];
            }
        }
    }else if(System_Unit == Imperial){
        for(unsigned int i = 0 ; i < Program_Data.PeriodNumber; i++){
            if(i < 96){ 
                Program_Data.SPEED_Table_96[i]   = WFI_Profile_Spd_Template_Imperial[i];
            }
        }
    }  
    

    
}
