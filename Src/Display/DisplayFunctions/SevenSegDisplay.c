#include "system.h"
#include "SevenSegDisplay.h"
#include "HT1632_C52.h"

const unsigned char SegmentOderTable[27] = {  24, 25, 26, 
                                              23, 22, 21, 
                                              19, 18, 17, 16, 20,
                                              14, 13, 12, 11, 15,
                                              10,  9,  8,  7,  6,
                                               5,  4,  3,
                                               0,  1,  2 };

unsigned char SevenSegmentBuffer[27];
unsigned char numbers[]={0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE4,0xFE,0xE6,0xEE,0x3E,0x9C,0x7A,0x9E,0x8E };

void SET_Seg_Display(Seg_Display_ITEM_Def Item , unsigned short Value ,DOT_Def dot ,Format_Def format ){
    
    unsigned char Units;
    unsigned char Tens;
    unsigned char Hundreds;
    unsigned char Thousands;
    
    
    unsigned char dot_1 = 0;
    unsigned char dot_2 = 0;
    unsigned char dot_3 = 0;
    unsigned char dot_4 = 0;
  
    unsigned short B1_Weight;
    unsigned short B2_Weight;
    unsigned short B3_Weight;
    
    
    if(format == HEX){
        
        Thousands =    Value / 4096;
        Hundreds  =   (Value % 4096) / 256;
        Tens      =  ((Value % 4096) % 256) / 16;
        Units     =  ((Value % 4096) % 256) % 16;
        
        B1_Weight = 16;
        B2_Weight = 256;
        B3_Weight = 4096;
        
    }else if(format == DEC){
        
        Thousands =   Value / 1000;
        Hundreds  =  (Value % 1000) / 100;
        Tens      = ((Value % 1000) % 100) / 10;
        Units     = ((Value % 1000) % 100) % 10;       
    
        B1_Weight = 10;
        B2_Weight = 100;
        B3_Weight = 1000;
        
    }

    if(dot & 0x01){
        dot_1 = 1;
    }
    if(dot & 0x02){
        dot_2 = 1;
    }
    if(dot & 0x04){
        dot_3 = 1;
    }
    if(dot & 0x08){
        dot_4 = 1;
    }
    
    if( (Item == SPEED) || (Item == INCLINE) || (Item == DISTANCE) || (Item == HEARTRATE)){
        
        unsigned char LSB_BYTE;
        unsigned char MID_BYTE;
        unsigned char MSB_BYTE;
        
        
        switch(Item){
          case SPEED:
          case INCLINE:
            LSB_BYTE = Item + 8;
            MID_BYTE = Item + 4;
            MSB_BYTE = Item;
            break;
          case DISTANCE:
          case HEARTRATE:
            LSB_BYTE = Item;
            MID_BYTE = Item + 4;
            MSB_BYTE = Item + 8;
            break;
        }
           
        if(Hundreds){
           
            SevenSegmentBuffer[MSB_BYTE/4] = numbers[Hundreds] + dot_3;
            
        }else{
            
            SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
        }  
        
        if(Tens | Hundreds){
            
            SevenSegmentBuffer[MID_BYTE/4] = numbers[Tens] + dot_2;
        }else{
           
            SevenSegmentBuffer[MID_BYTE/4] = 0x00;
        }   
        
        if(dot_2){
            if( (Value < B1_Weight) ){
             
                SevenSegmentBuffer[MID_BYTE/4] = numbers[0] + dot_2;
            }
            
        }else if(dot_3){
            if(Value < B2_Weight ){
             
                SevenSegmentBuffer[MSB_BYTE/4] = numbers[0] + dot_3;
                if(Value < B1_Weight){
                   
                    SevenSegmentBuffer[MID_BYTE/4] =  numbers[0];
                }
            } 
        }
        
        SevenSegmentBuffer[LSB_BYTE/4] =  numbers[Units] + dot_1;
    }
    
    if( (Item == PACE) || (Item == CALORIES) || (Item == TIME)){
        
        unsigned char BIT_0;
        unsigned char BIT_1;
        unsigned char BIT_2;
        unsigned char BIT_3;
        unsigned char COLON;
        
        
        switch(Item){
          case PACE:
            BIT_0 = Item;
            BIT_1 = Item + 4;
            BIT_2 = Item + 12;
            BIT_3 = Item + 16;
            COLON = Item + 8;
            break;
          case CALORIES:
          case TIME:
            BIT_0 = Item + 16;
            BIT_1 = Item;
            BIT_2 = Item + 8;
            BIT_3 = Item + 12;
            COLON = Item + 4;
            break;
            
        }
        
        
        
        if(Thousands){
            SevenSegmentBuffer[BIT_3/4] =  numbers[Thousands] + dot_4;
        }else{
            SevenSegmentBuffer[BIT_3/4] =  0x00;
        } 
        
        if(Thousands | Hundreds){
            SevenSegmentBuffer[BIT_2/4] =  numbers[Hundreds] + dot_3;
        }else{
            SevenSegmentBuffer[BIT_2/4] =  0x00;
        }  
        
        if(Thousands | Tens | Hundreds){
            SevenSegmentBuffer[BIT_1/4] =  numbers[Tens] + dot_2;
        }else{
            SevenSegmentBuffer[BIT_1/4] =  0x00;
            
        }   
        
        if(dot_2){
            if( (Value < B1_Weight) ){
                SevenSegmentBuffer[BIT_1/4] =  numbers[0] + dot_2;
            }
            
        }else if(dot_3){
            if(Value < B2_Weight ){
                SevenSegmentBuffer[BIT_2/4] = numbers[0] + dot_3;
                if(Value < B1_Weight){
                    SevenSegmentBuffer[BIT_1/4] = numbers[0];
                }
            } 
        }else if(dot_4){
            if(Value < B3_Weight ){
                SevenSegmentBuffer[BIT_3/4] = numbers[0] + dot_4;
                
                if(Value < B2_Weight){
                    SevenSegmentBuffer[BIT_2/4] = numbers[0];
                    if(Value < B1_Weight){
                        SevenSegmentBuffer[BIT_1/4] = numbers[0];
                    }
                }
            } 
        }
        
        SevenSegmentBuffer[BIT_0/4] =numbers[Units] + dot_1;
        
        if(Item == TIME || Item == PACE){
            SevenSegmentBuffer[COLON/4] = 0x00;
        }
        
    }
    
}

unsigned char blink_flag;

unsigned char blink_flag_incline;
unsigned char blink_flag_speed;
void SET_INCLINE_Blink( unsigned short Value ,DOT_Def dot ,Format_Def format ,unsigned char Blink_Swich){
    
    unsigned char Units;
    unsigned char Tens;
    unsigned char Hundreds;
    //unsigned char Thousands;
    
    unsigned char dot_1 = 0;
    unsigned char dot_2 = 0;
    unsigned char dot_3 = 0;
    //unsigned char dot_4 = 0;
    
    unsigned short B1_Weight;
    unsigned short B2_Weight;
    //unsigned short B3_Weight;
    
    
    if(format == HEX){
        
        //Thousands =    Value / 4096;
        Hundreds  =   (Value % 4096) / 256;
        Tens      =  ((Value % 4096) % 256) / 16;
        Units     =  ((Value % 4096) % 256) % 16;
        
        B1_Weight = 16;
        B2_Weight = 256;
        //B3_Weight = 4096;
        
    }else if(format == DEC){
        
        //Thousands =   Value / 1000;
        Hundreds  =  (Value % 1000) / 100;
        Tens      = ((Value % 1000) % 100) / 10;
        Units     = ((Value % 1000) % 100) % 10;       
        
        B1_Weight = 10;
        B2_Weight = 100;
        //B3_Weight = 1000;
        
    }
    
    if(dot & 0x01){
        dot_1 = 1;
    }
    if(dot & 0x02){
        dot_2 = 1;
    }
    if(dot & 0x04){
        dot_3 = 1;
    }
    //if(dot & 0x08){
    //    dot_4 = 1;
    //}
    
    
    unsigned char LSB_BYTE;
    unsigned char MID_BYTE;
    unsigned char MSB_BYTE;
    
    LSB_BYTE = INCLINE + 8;
    MID_BYTE = INCLINE + 4;
    MSB_BYTE = INCLINE;
    
  
    if(Blink_Swich == 0){
    
        //-------------------正常顯示------------------------------
        if(Hundreds){
            
            SevenSegmentBuffer[MSB_BYTE/4] = numbers[Hundreds] + dot_3;
            
        }else{
            
            SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
        }  
        
        if(Tens | Hundreds){
            
            SevenSegmentBuffer[MID_BYTE/4] = numbers[Tens] + dot_2;
        }else{
            
            SevenSegmentBuffer[MID_BYTE/4] = 0x00;
        }
        //-------------------------------------------------------------------------------
        
        //----------------------------------------------------------------------------
        if(dot_2){
            if( (Value < B1_Weight) ){
                
                SevenSegmentBuffer[MID_BYTE/4] = numbers[0] + dot_2;
            }
            
        }else if(dot_3){
            if(Value < B2_Weight ){
                
                SevenSegmentBuffer[MSB_BYTE/4] = numbers[0] + dot_3;
                if(Value < B1_Weight){
                    
                    SevenSegmentBuffer[MID_BYTE/4] =  numbers[0];
                }
            } 
        }
        
        SevenSegmentBuffer[LSB_BYTE/4] =  numbers[Units] + dot_1;
        //-----------------------------------------------------------------------------
        
        
    }else if(Blink_Swich ==1){
    
        if(T500ms_Blink_INCL_Flag){
            T500ms_Blink_INCL_Flag = 0;
            
            if(blink_flag_incline){
                blink_flag_incline = 0;
            }else if(blink_flag_incline == 0){
                blink_flag_incline = 1;
                
            }
        }
        
        
        if(blink_flag_incline == 1){
            
            //-------------------正常顯示------------------------------
            if(Hundreds){
                
                SevenSegmentBuffer[MSB_BYTE/4] = numbers[Hundreds] + dot_3;
                
            }else{
                
                SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
            }  
            
            if(Tens | Hundreds){
                
                SevenSegmentBuffer[MID_BYTE/4] = numbers[Tens] + dot_2;
            }else{
                
                SevenSegmentBuffer[MID_BYTE/4] = 0x00;
            }
            //-------------------------------------------------------------------------------
            
            //----------------------------------------------------------------------------
            if(dot_2){
                if( (Value < B1_Weight) ){
                    
                    SevenSegmentBuffer[MID_BYTE/4] = numbers[0] + dot_2;
                }
                
            }else if(dot_3){
                if(Value < B2_Weight ){
                    
                    SevenSegmentBuffer[MSB_BYTE/4] = numbers[0] + dot_3;
                    if(Value < B1_Weight){
                        
                        SevenSegmentBuffer[MID_BYTE/4] =  numbers[0];
                    }
                } 
            }
            
            SevenSegmentBuffer[LSB_BYTE/4] =  numbers[Units] + dot_1;
            //-----------------------------------------------------------------------------
            
            
        }else if(blink_flag_incline == 0){
            
            SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
            SevenSegmentBuffer[MID_BYTE/4] = 0x00;
            SevenSegmentBuffer[LSB_BYTE/4] = 0x00;
        
        }     
    }
    
}


void SET_SPEED_Blink( unsigned short Value ,DOT_Def dot ,Format_Def format ,unsigned char Blink_Swich){
    
    unsigned char Units;
    unsigned char Tens;
    unsigned char Hundreds;
    //unsigned char Thousands;
    
    unsigned char dot_1 = 0;
    unsigned char dot_2 = 0;
    unsigned char dot_3 = 0;
    //unsigned char dot_4 = 0;
    
    unsigned short B1_Weight;
    unsigned short B2_Weight;
    //unsigned short B3_Weight;
    
    
    if(format == HEX){
        
        //Thousands =    Value / 4096;
        Hundreds  =   (Value % 4096) / 256;
        Tens      =  ((Value % 4096) % 256) / 16;
        Units     =  ((Value % 4096) % 256) % 16;
        
        B1_Weight = 16;
        B2_Weight = 256;
        //B3_Weight = 4096;
        
    }else if(format == DEC){
        
        //Thousands =   Value / 1000;
        Hundreds  =  (Value % 1000) / 100;
        Tens      = ((Value % 1000) % 100) / 10;
        Units     = ((Value % 1000) % 100) % 10;       
        
        B1_Weight = 10;
        B2_Weight = 100;
       //B3_Weight = 1000;
        
    }
    
    if(dot & 0x01){
        dot_1 = 1;
    }
    if(dot & 0x02){
        dot_2 = 1;
    }
    if(dot & 0x04){
        dot_3 = 1;
    }
    //if(dot & 0x08){
    //    dot_4 = 1;
    //}
    
    
    unsigned char LSB_BYTE;
    unsigned char MID_BYTE;
    unsigned char MSB_BYTE;
    
    LSB_BYTE = SPEED + 8;
    MID_BYTE = SPEED + 4;
    MSB_BYTE = SPEED;
    
  
    if(Blink_Swich == 0){
    
        //-------------------正常顯示------------------------------
        if(Hundreds){
            
            SevenSegmentBuffer[MSB_BYTE/4] = numbers[Hundreds] + dot_3;
            
        }else{
            
            SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
        }  
        
        if(Tens | Hundreds){
            
            SevenSegmentBuffer[MID_BYTE/4] = numbers[Tens] + dot_2;
        }else{
            
            SevenSegmentBuffer[MID_BYTE/4] = 0x00;
        }
        //-------------------------------------------------------------------------------
        
        //----------------------------------------------------------------------------
        if(dot_2){
            if( (Value < B1_Weight) ){
                
                SevenSegmentBuffer[MID_BYTE/4] = numbers[0] + dot_2;
            }
            
        }else if(dot_3){
            if(Value < B2_Weight ){
                
                SevenSegmentBuffer[MSB_BYTE/4] = numbers[0] + dot_3;
                if(Value < B1_Weight){
                    
                    SevenSegmentBuffer[MID_BYTE/4] =  numbers[0];
                }
            } 
        }
        
        SevenSegmentBuffer[LSB_BYTE/4] =  numbers[Units] + dot_1;
        //-----------------------------------------------------------------------------
        
        
    }else if(Blink_Swich ==1){
    
        if(T500ms_Blink_SPEED_Flag){
            T500ms_Blink_SPEED_Flag = 0;
            
            if(blink_flag_speed){
                blink_flag_speed = 0;
            }else if(blink_flag_speed == 0){
                blink_flag_speed = 1;
                
            }
        }
        
        
        if(blink_flag_speed == 1){
            
            //-------------------正常顯示------------------------------
            if(Hundreds){
                
                SevenSegmentBuffer[MSB_BYTE/4] = numbers[Hundreds] + dot_3;
                
            }else{
                
                SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
            }  
            
            if(Tens | Hundreds){
                
                SevenSegmentBuffer[MID_BYTE/4] = numbers[Tens] + dot_2;
            }else{
                
                SevenSegmentBuffer[MID_BYTE/4] = 0x00;
            }
            //-------------------------------------------------------------------------------
            
            //----------------------------------------------------------------------------
            if(dot_2){
                if( (Value < B1_Weight) ){
                    
                    SevenSegmentBuffer[MID_BYTE/4] = numbers[0] + dot_2;
                }
                
            }else if(dot_3){
                if(Value < B2_Weight ){
                    
                    SevenSegmentBuffer[MSB_BYTE/4] = numbers[0] + dot_3;
                    if(Value < B1_Weight){
                        
                        SevenSegmentBuffer[MID_BYTE/4] =  numbers[0];
                    }
                } 
            }
            
            SevenSegmentBuffer[LSB_BYTE/4] =  numbers[Units] + dot_1;
            //-----------------------------------------------------------------------------
            
            
        }else if(blink_flag_speed == 0){
            
            SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
            SevenSegmentBuffer[MID_BYTE/4] = 0x00;
            SevenSegmentBuffer[LSB_BYTE/4] = 0x00;
        }     
    }
}

void SET_Seg_Display_Blink(Seg_Display_ITEM_Def Item , unsigned short Value ,DOT_Def dot ,Format_Def format ,unsigned char Blink_Swich ){
    
    unsigned char Units;
    unsigned char Tens;
    unsigned char Hundreds;
    unsigned char Thousands;
    
    unsigned char dot_1 = 0;
    unsigned char dot_2 = 0;
    unsigned char dot_3 = 0;
    unsigned char dot_4 = 0;
  
    unsigned short B1_Weight;
    unsigned short B2_Weight;
    unsigned short B3_Weight;
    
    
    if(format == HEX){
        
        Thousands =    Value / 4096;
        Hundreds  =   (Value % 4096) / 256;
        Tens      =  ((Value % 4096) % 256) / 16;
        Units     =  ((Value % 4096) % 256) % 16;
        
        B1_Weight = 16;
        B2_Weight = 256;
        B3_Weight = 4096;
        
    }else if(format == DEC){
        
        Thousands =   Value / 1000;
        Hundreds  =  (Value % 1000) / 100;
        Tens      = ((Value % 1000) % 100) / 10;
        Units     = ((Value % 1000) % 100) % 10;       
    
        B1_Weight = 10;
        B2_Weight = 100;
        B3_Weight = 1000;
        
    }

    if(dot & 0x01){
        dot_1 = 1;
    }
    if(dot & 0x02){
        dot_2 = 1;
    }
    if(dot & 0x04){
        dot_3 = 1;
    }
    if(dot & 0x08){
        dot_4 = 1;
    }
    
    
    if(Blink_Swich == 0){
        
        
        if( (Item == SPEED) || (Item == INCLINE) || (Item == DISTANCE) || (Item == HEARTRATE)){
            
            unsigned char LSB_BYTE;
            unsigned char MID_BYTE;
            unsigned char MSB_BYTE;
            
            switch(Item){
              case SPEED:
              case INCLINE:
                LSB_BYTE = Item + 8;
                MID_BYTE = Item + 4;
                MSB_BYTE = Item;
                break;
              case DISTANCE:
              case HEARTRATE:
                LSB_BYTE = Item;
                MID_BYTE = Item + 4;
                MSB_BYTE = Item + 8;
                break;
            }
            //-----------------------------------------------------------------------------
            if(Hundreds){
                
                SevenSegmentBuffer[MSB_BYTE/4] = numbers[Hundreds] + dot_3;
                
            }else{
                
                SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
            }  
            
            if(Tens | Hundreds){
                
                SevenSegmentBuffer[MID_BYTE/4] = numbers[Tens] + dot_2;
                
            }else{
                
                SevenSegmentBuffer[MID_BYTE/4] = 0x00;
            }
            //-------------------------------------------------------------------------------
            
            //----------------------------------------------------------------------------
            if(dot_2){
                if( (Value < B1_Weight) ){
                    
                    SevenSegmentBuffer[MID_BYTE/4] = numbers[0] + dot_2;
                }
                
            }else if(dot_3){
                if(Value < B2_Weight ){
                    
                    SevenSegmentBuffer[MSB_BYTE/4] = numbers[0] + dot_3;
                    if(Value < B1_Weight){
                        
                        SevenSegmentBuffer[MID_BYTE/4] =  numbers[0];
                    }
                } 
            }
            
            SevenSegmentBuffer[LSB_BYTE/4] =  numbers[Units] + dot_1;
            //-----------------------------------------------------------------------------
        }
         
        
        
        if( (Item == PACE) || (Item == CALORIES) || (Item == TIME)){
            
            unsigned char BIT_0;
            unsigned char BIT_1;
            unsigned char BIT_2;
            unsigned char BIT_3;
            
            
            switch(Item){
              case PACE:
                BIT_0 = Item;
                BIT_1 = Item + 4;
                BIT_2 = Item + 12;
                BIT_3 = Item + 16;
                break;
              case CALORIES:
              case TIME:
                BIT_0 = Item + 16;
                BIT_1 = Item;
                BIT_2 = Item + 8;
                BIT_3 = Item + 12;
                break;
                
            }
            
            //----------------------------------------------------------------------
            if(Thousands){
                SevenSegmentBuffer[BIT_3/4] =  numbers[Thousands] + dot_4;
            }else{
                SevenSegmentBuffer[BIT_3/4] =  0x00;
            } 
            
            if(Thousands | Hundreds){
                SevenSegmentBuffer[BIT_2/4] =  numbers[Hundreds] + dot_3;
            }else{
                SevenSegmentBuffer[BIT_2/4] =  0x00;
            }  
            
            if(Thousands | Tens | Hundreds){
                SevenSegmentBuffer[BIT_1/4] =  numbers[Tens] + dot_2;
            }else{
                SevenSegmentBuffer[BIT_1/4] =  0x00;
                
            }   
            //-------------------------------------------------------------------------------
            //-------------------------------------------------------------------------------
            if(dot_2){
                if( (Value < B1_Weight) ){
                    SevenSegmentBuffer[BIT_1/4] =  numbers[0] + dot_2;
                }
                
            }else if(dot_3){
                if(Value < B2_Weight ){
                    SevenSegmentBuffer[BIT_2/4] = numbers[0] + dot_3;
                    if(Value < B1_Weight){
                        SevenSegmentBuffer[BIT_1/4] = numbers[0];
                    }
                } 
            }else if(dot_4){
                if(Value < B3_Weight ){
                    SevenSegmentBuffer[BIT_3/4] = numbers[0] + dot_4;
                    
                    if(Value < B2_Weight){
                        SevenSegmentBuffer[BIT_2/4] = numbers[0];
                        if(Value < B1_Weight){
                            SevenSegmentBuffer[BIT_1/4] = numbers[0];
                        }
                    }
                } 
            }
            
            SevenSegmentBuffer[BIT_0/4] =numbers[Units] + dot_1;
            //-------------------------------------------------------------------------------
        }
        
    }else if(Blink_Swich == 1){
        
        
        if(Item == INCLINE){
            
            if(T500ms_Blink_INCL_Flag){
                T500ms_Blink_INCL_Flag = 0;
                
                if(blink_flag){
                    blink_flag = 0;
                }else if(blink_flag == 0){
                    blink_flag = 1;
                    
                }
            }
            
        }else if(Item == SPEED){
            
            if(T500ms_Blink_SPEED_Flag){
                T500ms_Blink_SPEED_Flag = 0;
                
                if(blink_flag){
                    blink_flag = 0;
                }else if(blink_flag == 0){
                    blink_flag = 1; 
                }
            }
            
        }
        
        
        if(blink_flag == 1){

            if( (Item == SPEED) || (Item == INCLINE) || (Item == DISTANCE) || (Item == HEARTRATE)){ 
                
                unsigned char LSB_BYTE;
                unsigned char MID_BYTE;
                unsigned char MSB_BYTE;
                
                switch(Item){
                  case SPEED:
                  case INCLINE:
                    LSB_BYTE = Item + 8;
                    MID_BYTE = Item + 4;
                    MSB_BYTE = Item;
                    break;
                  case DISTANCE:
                  case HEARTRATE:
                    LSB_BYTE = Item;
                    MID_BYTE = Item + 4;
                    MSB_BYTE = Item + 8;
                    break;
                }
                
                if(Hundreds){
                    
                    SevenSegmentBuffer[MSB_BYTE/4] = numbers[Hundreds] + dot_3;
                    
                }else{
                    
                    SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
                }  
                
                if(Tens | Hundreds){
                    
                    SevenSegmentBuffer[MID_BYTE/4] = numbers[Tens] + dot_2;
                }else{
                    
                    SevenSegmentBuffer[MID_BYTE/4] = 0x00;
                }   
                
                if(dot_2){
                    if( (Value < B1_Weight) ){
                        
                        SevenSegmentBuffer[MID_BYTE/4] = numbers[0] + dot_2;
                    }
                    
                }else if(dot_3){
                    if(Value < B2_Weight ){
                        
                        SevenSegmentBuffer[MSB_BYTE/4] = numbers[0] + dot_3;
                        if(Value < B1_Weight){
                            
                            SevenSegmentBuffer[MID_BYTE/4] =  numbers[0];
                        }
                    } 
                }
                
                SevenSegmentBuffer[LSB_BYTE/4] =  numbers[Units] + dot_1;
            }
            
            if( (Item == PACE) || (Item == CALORIES) || (Item == TIME)){
                
                unsigned char BIT_0;
                unsigned char BIT_1;
                unsigned char BIT_2;
                unsigned char BIT_3;
                
                
                switch(Item){
                  case PACE:
                    BIT_0 = Item;
                    BIT_1 = Item + 4;
                    BIT_2 = Item + 12;
                    BIT_3 = Item + 16;
                    break;
                  case CALORIES:
                  case TIME:
                    BIT_0 = Item + 16;
                    BIT_1 = Item;
                    BIT_2 = Item + 8;
                    BIT_3 = Item + 12;
                    break;
                    
                }
                
                if(Thousands){
                    SevenSegmentBuffer[BIT_3/4] =  numbers[Thousands] + dot_4;
                }else{
                    SevenSegmentBuffer[BIT_3/4] =  0x00;
                } 
                
                if(Thousands | Hundreds){
                    SevenSegmentBuffer[BIT_2/4] =  numbers[Hundreds] + dot_3;
                }else{
                    SevenSegmentBuffer[BIT_2/4] =  0x00;
                }  
                
                if(Thousands | Tens | Hundreds){
                    SevenSegmentBuffer[BIT_1/4] =  numbers[Tens] + dot_2;
                }else{
                    SevenSegmentBuffer[BIT_1/4] =  0x00;
                    
                }   
                
                if(dot_2){
                    if( (Value < B1_Weight) ){
                        SevenSegmentBuffer[BIT_1/4] =  numbers[0] + dot_2;
                    }
                    
                }else if(dot_3){
                    if(Value < B2_Weight ){
                        SevenSegmentBuffer[BIT_2/4] = numbers[0] + dot_3;
                        if(Value < B1_Weight){
                            SevenSegmentBuffer[BIT_1/4] = numbers[0];
                        }
                    } 
                }else if(dot_4){
                    if(Value < B3_Weight ){
                        SevenSegmentBuffer[BIT_3/4] = numbers[0] + dot_4;
                        
                        if(Value < B2_Weight){
                            SevenSegmentBuffer[BIT_2/4] = numbers[0];
                            if(Value < B1_Weight){
                                SevenSegmentBuffer[BIT_1/4] = numbers[0];
                            }
                        }
                    } 
                }
                
                SevenSegmentBuffer[BIT_0/4] =numbers[Units] + dot_1;
            }
        }else if(blink_flag == 0){
            //------------------------------------------全滅-----------------------------------------//
            
            if( (Item == SPEED) || (Item == INCLINE) || (Item == DISTANCE) || (Item == HEARTRATE)){
                
                unsigned char LSB_BYTE;
                unsigned char MID_BYTE;
                unsigned char MSB_BYTE;
                
                switch(Item){
                  case SPEED:
                  case INCLINE:
                    LSB_BYTE = Item + 8;
                    MID_BYTE = Item + 4;
                    MSB_BYTE = Item;
                    break;
                  case DISTANCE:
                  case HEARTRATE:
                    LSB_BYTE = Item;
                    MID_BYTE = Item + 4;
                    MSB_BYTE = Item + 8;
                    break;
                }
                
                SevenSegmentBuffer[MSB_BYTE/4] = 0x00;
                SevenSegmentBuffer[MID_BYTE/4] = 0x00;
                SevenSegmentBuffer[LSB_BYTE/4] = 0x00;
                
            }
            
            if( (Item == PACE) || (Item == CALORIES) || (Item == TIME)){
                
                unsigned char BIT_0;
                unsigned char BIT_1;
                unsigned char BIT_2;
                unsigned char BIT_3;
                
                switch(Item){
                  case PACE:
                    BIT_0 = Item;
                    BIT_1 = Item + 4;
                    BIT_2 = Item + 12;
                    BIT_3 = Item + 16;
                    break;
                  case CALORIES:
                  case TIME:
                    BIT_0 = Item + 16;
                    BIT_1 = Item;
                    BIT_2 = Item + 8;
                    BIT_3 = Item + 12;
                    break;
                    
                }
                
                SevenSegmentBuffer[BIT_3/4] =  0x00;
                SevenSegmentBuffer[BIT_2/4] =  0x00;
                SevenSegmentBuffer[BIT_1/4] =  0x00;
                SevenSegmentBuffer[BIT_0/4] =  0x00;
                
            }
            //---------------------------------------------------------------------------
        }
        

    }
    
}


unsigned colon_blink_flag;
void SET_Seg_TIME_Display( Seg_Display_ITEM_Def Item  , unsigned int Value){
    

    unsigned char BYTE_1;
    unsigned char BYTE_2;
    
    unsigned char BYTE_3;
    unsigned char BYTE_4;
    
    unsigned char COLON;
    
    
    if(Value < 3600){
        
        BYTE_4 = (Value / 60) / 10;
        BYTE_3 = (Value / 60) % 10;
        
        BYTE_2 = (Value % 60) / 10;
        BYTE_1 = (Value % 60) % 10;
        
    }else if(Value >= 3600){
         
        BYTE_4 = (Value / 3600) / 10;
        BYTE_3 = (Value / 3600) % 10;
        
        BYTE_2 = ((Value % 3600)/60) / 10;
        BYTE_1 = ((Value % 3600)/60) % 10;
        
    }
    
    if( (Item == PACE) || (Item == CALORIES) || (Item == TIME) ){
        
        unsigned char BIT_0;
        unsigned char BIT_1;
        unsigned char BIT_2;
        unsigned char BIT_3;
        
        switch(Item){
          case PACE:
            BIT_0 = Item;
            BIT_1 = Item + 4;
            BIT_2 = Item + 12;
            BIT_3 = Item + 16;
            break;
          case CALORIES:
          case TIME:
            BIT_0 = Item + 16;
            BIT_1 = Item;
            BIT_2 = Item + 8;
            BIT_3 = Item + 12;
            COLON = Item + 4;
            break;

        }
        
        SevenSegmentBuffer[BIT_3/4] = numbers[BYTE_4];
        SevenSegmentBuffer[BIT_2/4] = numbers[BYTE_3];
        SevenSegmentBuffer[BIT_1/4] = numbers[BYTE_2];
        
        if(Value >= 3600){
            
            if(System_Mode == Prog_Set){   //設定模式裡  大於1小時直接顯示尾巴小數點
                 SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1] + 1;
            }else{
                if(T500ms_Time_Colon_Blink_Flag){
                    T500ms_Time_Colon_Blink_Flag = 0;    //大於一小時  尾巴的小數點0.5閃
                    
                    if(colon_blink_flag){
                        colon_blink_flag = 0;
                    }else{
                        colon_blink_flag = 1;
                    }
                }
                
                if(colon_blink_flag == 1){
                    SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1] + 1;
                }else{
                    SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1];
                }
            }
            
        }else{
            SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1];
        }
        
            
        if(Item == TIME){
            SevenSegmentBuffer[COLON/4] = 0xFF;
        }
        
    }
    
}
unsigned char Blink_Flag;

void TIME_SET_Display( Seg_Display_ITEM_Def Item  , unsigned int Value , unsigned char BlinkMask , unsigned char Force){
    
    unsigned char BYTE_1;
    unsigned char BYTE_2;
    unsigned char BYTE_3;
    unsigned char BYTE_4;
    unsigned char COLON;
    
    if(Item == PACE_SPD){
        
        if(Value < 3600){       //      步速> (一小時 ) ==> 一樣顯示 XX分鐘: XX秒
            BYTE_4 = (Value / 60) / 10;
            BYTE_3 = (Value / 60) % 10;
            BYTE_2 = (Value % 60) / 10;
            BYTE_1 = (Value % 60) % 10;
        }else{
            BYTE_4 = (Value / 3600) / 10;
            BYTE_3 = (Value / 3600) % 10;
            BYTE_2 = ((Value % 3600) / 60) / 10;
            BYTE_1 = ((Value % 3600) / 60) % 10;
        }
        
    }else{
        
        if(Value < 3600){
            
            BYTE_4 = (Value / 60) / 10;
            BYTE_3 = (Value / 60) % 10;
            BYTE_2 = (Value % 60) / 10;
            BYTE_1 = (Value % 60) % 10;
            
        }else if(Value >= 3600){
            
            BYTE_4 = (Value / 3600) / 10;
            BYTE_3 = (Value / 3600) % 10;
            BYTE_2 = ((Value % 3600)/60) / 10;
            BYTE_1 = ((Value % 3600)/60) % 10;
            
        }
    }
    
    if( (Item == PACE) || (Item == CALORIES) || (Item == TIME) || (Item == PACE_SPD)){
        
        unsigned char BIT_0;
        unsigned char BIT_1;
        unsigned char BIT_2;
        unsigned char BIT_3;
        
        switch(Item){
          case PACE:
            BIT_0 = Item;
            BIT_1 = Item + 4;
            BIT_2 = Item + 12;
            BIT_3 = Item + 16;
            COLON = Item + 8;
            break;
          case CALORIES:
          case TIME:
            BIT_0 = Item + 16;
            BIT_1 = Item;
            BIT_2 = Item + 8;
            BIT_3 = Item + 12;
            COLON = Item + 4;
            break;
        }
        
        if(Item == PACE_SPD){
            BIT_0 = PACE;
            BIT_1 = PACE + 4;
            BIT_2 = PACE + 12;
            BIT_3 = PACE + 16;
            COLON = PACE + 8;
        }
        
        //強迫全部顯示
        if(Force == 1){
            
            SevenSegmentBuffer[BIT_3/4] = numbers[BYTE_4];
            SevenSegmentBuffer[BIT_2/4] = numbers[BYTE_3];
            SevenSegmentBuffer[BIT_1/4] = numbers[BYTE_2];
            
            if(Value >= 3600){
                
                if(Item == PACE_SPD){
                    SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1] + 1;
                }else{
                    SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1] + 1;
                }

            }else{
                SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1];
            }
            
            
            if(Item == TIME || Item == PACE || Item == PACE_SPD ){
                SevenSegmentBuffer[COLON/4] = 0xFF;
            }
            
            Blink_Flag = 0;
            ClearBlinkCnt2();
            
        }else if(Force == 0){
            
            if(T500ms_Blink_Flag){
                T500ms_Blink_Flag = 0;
                
                if(Blink_Flag){
                    Blink_Flag = 0;
                }else if(Blink_Flag == 0){
                    Blink_Flag = 1;
                }
                
            }
            
            if( (BlinkMask & (0x01<<3)) == (0x01<<3)){
                if(Blink_Flag == 1){
     
                    SevenSegmentBuffer[BIT_3/4] = numbers[BYTE_4];
                    
                    
                }else if(Blink_Flag == 0){
                    SevenSegmentBuffer[BIT_3/4] =0x00;
                }   
            }else{
                SevenSegmentBuffer[BIT_3/4] = numbers[BYTE_4];
            }
        
            if( (BlinkMask& (0x01<<2)) == (0x01<<2)){
                if(Blink_Flag == 1){
                    SevenSegmentBuffer[BIT_2/4] = numbers[BYTE_3]; 
                }else if(Blink_Flag == 0){
                    SevenSegmentBuffer[BIT_2/4] = 0x00;
                }   
            }else{
                SevenSegmentBuffer[BIT_2/4] = numbers[BYTE_3]; 
            }
            
            
            if( (BlinkMask& (0x01<<1)) == (0x01<<1)){
                if(Blink_Flag == 1){
                    SevenSegmentBuffer[BIT_1/4] = numbers[BYTE_2];   
                }else if(Blink_Flag == 0){
                    SevenSegmentBuffer[BIT_1/4] = 0x00; 
                }  
            }else{
                SevenSegmentBuffer[BIT_1/4] = numbers[BYTE_2];
            }
            
            //------------------------------------------------------------------------
            if( (BlinkMask& (0x01)) == (0x01)){
                if(Blink_Flag == 1){
                    if(Value >= 3600){
                        
                        if(Item == PACE_SPD){
                            SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1];
                        }else{
                            SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1] + 1;
                        }
                        
                    }else{
                        SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1];
                    }
                    
                }else if(Blink_Flag == 0){
                    if(Value >= 3600){
                        
                        if(Item != PACE_SPD){
                            SevenSegmentBuffer[BIT_0/4] = 1;
                        }
                        
                    }else{
                        
                        SevenSegmentBuffer[BIT_0/4] = 0x00;
                        
                    }
                   
                }    
            }else{
                SevenSegmentBuffer[BIT_0/4] = numbers[BYTE_1];
            }
            //-----------------------------------------------------------------------------------
            
            if(Item == TIME || Item == PACE || Item == PACE_SPD){
                SevenSegmentBuffer[COLON/4] = 0xFF;
            }

        }
    }
    
}

unsigned int Pace_Freq;  //步頻
unsigned int Pace_Dist;  //步幅
unsigned int Pace_Spd;   //步速

unsigned char Pace_Display_Switch;

unsigned char L_Mode = 'N';
unsigned char R_Mode = 'N';


extern unsigned int Holding_Cnt;    //偵測按住多久用的Counter  >8 判定為按住
extern unsigned char HoldingCnt;
extern unsigned char Hold_Key_Flag;
extern KeyName_Def  Holding_Key_Name;

void Workout_Value_DisplayProcess(){

    if(System_Mode != StartUp){
        if(Linked_HR_info.usHR_bpm || (Linked_HR_info.Link_state == Linked)){
            //------藍芽 或 ANT 心跳  最優先
            //SET_Seg_Display(HEARTRATE  , Linked_HR_info.usHR_bpm , ND , DEC );
            usNowHeartRate = Linked_HR_info.usHR_bpm;
        }else{
            
            if(HR5KPairOkFlag){
                //5K 無限心跳  第2優先
                //SET_Seg_Display(HEARTRATE  , ucWhr , ND , DEC );
                usNowHeartRate = ucWhr;
            }else{
                
                if(ucHhr){
                    //手握心跳
                    usNowHeartRate = ucHhr;
                }else{
                    //SET_Seg_Display(HEARTRATE  , 0 , ND , DEC );
                    usNowHeartRate = 0;
                }
            }    
        }
        
        if(System_Mode != Summary){  //Summary Mode 顯示平均心跳
            
            if(HeartRate_Display_Type == HR){
                
                if(HR_BlinkOneTime_Cnt > 0){
                    SET_Seg_Display_Blink(HEARTRATE, usNowHeartRate , ND , DEC , 1 );
                }else{
                    SET_Seg_Display(HEARTRATE  , usNowHeartRate , ND , DEC );
                }
   
                
            }else if(HeartRate_Display_Type == METS){
                SET_Seg_Display(HEARTRATE  , usMET , ND , DEC );
            }
        }
        
        //--------------外部按鍵----------------------------
        if(Holding_Cnt >0){ //按鈕按住
            if( (Holding_Key_Name == Inc_Up) || (Holding_Key_Name == Inc_Down) ){
                INCL_Moveing_Flag = 0;
            }
            if( (Holding_Key_Name == Spd_Up) || (Holding_Key_Name == Spd_Down) ){
                 SPEED_Changing_Flag = 0;
            }
        }
 
        //--------------薄膜按鍵-------------------------------------------
        if(HoldingCnt > 0){
            if(  (Press_Key == Key_IncUp) || (Press_Key == Key_IncDwn)){
                INCL_Moveing_Flag = 0;
            }
            if(  (Press_Key == Key_SpdUp) || (Press_Key == Key_SpdDwn) ){
                 SPEED_Changing_Flag = 0;
            }
        }
        //-----------------------------------------------------------------
        

        
        if(System_Mode != Summary){
            
#if SpdIncSegBlink
            SET_INCLINE_Blink( System_INCLINE ,D2 ,DEC  , INCL_Moveing_Flag);
            SET_SPEED_Blink(    System_SPEED  ,D2 ,DEC  , SPEED_Changing_Flag);
            
            if(INCL_Moveing_Flag == 1){
                L_Mode = 'B';
            }else{
                L_Mode = 'N';
            }
            
            if(SPEED_Changing_Flag == 1){
                R_Mode = 'B';
            }else{
                R_Mode = 'N';
            }
                 
#else
            SET_INCLINE_Blink( System_INCLINE ,D2 ,DEC  , 0);
            SET_SPEED_Blink(    System_SPEED  ,D2 ,DEC  , 0);            
           
            L_Mode = 'N';
            R_Mode = 'N';
#endif

            

            SET_DisplayBoard_Data(L_Mode,1,System_INCLINE ,R_Mode,1,System_SPEED);    
        }else if(System_Mode == Summary){
        
        }
       
        //------------------------------Display Board --------------------------
        
        //---------------------------------------------------------------------------------
        
        
        if(Calories_Display_Type == Cal_){
            if(Program_Data.Calories/100>=1000){ //消耗超過1000kcal時 不顯示小數點
                SET_Seg_Display(CALORIES  , Program_Data.Calories/1000 , ND , DEC );  // 1 kcal
            }else{
                SET_Seg_Display(CALORIES  , Program_Data.Calories/100 , D2 , DEC );  // 0.1 kcal
            }
                   
        }else if(Calories_Display_Type == Cal_HR){
            SET_Seg_Display(CALORIES  , Program_Data.Calories_HR , ND , DEC );  //每小時消耗cal
        }
        
        
        if(Dist_Display_Type == DIST){
            if(Program_Data.Distance >= 10000000){ //  >=100 km 顯示整數就好
                SET_Seg_Display(DISTANCE  , Program_Data.Distance/100000 , ND , DEC );
            }else if((Program_Data.Distance >= 1000000) && (Program_Data.Distance < 10000000) ){ // >= 10 km 顯示 小數點後一位
                SET_Seg_Display(DISTANCE  , Program_Data.Distance/10000 , D2 , DEC );
            }else if(Program_Data.Distance < 1000000){ // <10 km 顯示後兩位
                SET_Seg_Display(DISTANCE  , Program_Data.Distance/1000 , D3 , DEC );
            }
        }else if(Dist_Display_Type == ALTI){
            
            unsigned long ulAltitude_Display;
            ulAltitude_Display = ulAltitude/10;   //0.1公尺
            
            
            if(ulAltitude_Display>=10 && ulAltitude_Display<10000){
                SET_Seg_Display(DISTANCE  , ulAltitude_Display, D3 , DEC );
            }else if(ulAltitude_Display>=10000 && ulAltitude_Display<100000){
                SET_Seg_Display(DISTANCE  , ulAltitude_Display/10 , D2 , DEC );
            }else if(ulAltitude_Display>=100000 && ulAltitude_Display<1000000){
                SET_Seg_Display(DISTANCE  , ulAltitude_Display/100 , ND , DEC );
            }   
        }

        
        //--------------Pace 相關計算 --------------------------
        Pace_Freq = (RM6T6_state.Foot_cnt_10s * 6);
        
        if(Pace_Freq){
            Pace_Dist = (System_SPEED * 10000) / (Pace_Freq * 60);   //   單位      (System_SPEED*100 = 公尺)    (System_SPEED*10000 = 公分)
        }else{
            Pace_Dist = 0;
        }
        
        if(System_SPEED){
            Pace_Spd  = 36000 / System_SPEED;      // 單位: 秒
        }else{
            Pace_Spd = 0;
        }
        
        //-----------------------------------------------------
        if(Pace_Display_Switch == 0){
            SET_Seg_Display(PACE   , Program_Data.Pace   , ND , DEC );  //累積步數
        }else if(Pace_Display_Switch == 1){            
            // ----------- 步頻 -----------------------    
            SET_Seg_Display(PACE   ,  Pace_Freq   , ND , DEC );  //  步頻= 每分鐘走幾步   =>   (10s 走幾步) * 6
        }else if(Pace_Display_Switch == 2){  //   
            
            // ----------- 步幅 ---------------------- 
            //System_SPEED(單位0.1km)     步幅   =  每小時距離 / 每小時步數  =>   (( 一步的距離  ))  單位公尺 or 公分?
            SET_Seg_Display(PACE   ,  Pace_Dist   , ND , DEC );   //   步幅   =   一步的距離  
            
        }else if(Pace_Display_Switch == 3){
            //-------------步速-------------------------
            //跑一公里所需的時間   基礎單位 秒(s)   顯示(MM:SS)
            if(System_Mode == Summary){
                
                TIME_SET_Display( PACE_SPD  , 36000/uiAvgSpeed ,0x0F , 1);
            }else{
                TIME_SET_Display( PACE_SPD  , Pace_Spd ,0x0F , 1);
            }
            //SET_Seg_TIME_Display(PACE , Pace_Spd);                    // 步速 =  跑一公里所需的時間
        } 
    }
}

void writeSegmentBuffer(){
    

    if(System_Mode != Engineer){
        Workout_Value_DisplayProcess();
    }
    

    for(unsigned char i = 0; i<27;i++){   
        HT_Write( i* 4  ,SevenSegmentBuffer[i] , 1);
        __asm("NOP"); 
    }
  
}

void Turn_ON_All_Segment(){
    
    for(unsigned char i = 0; i<27;i++){   
        HT_Write( i* 4  ,0xFF , 1);
        __asm("NOP"); 
    }
  
}
void Turn_OFF_All_Segment(){
    
    memset(SevenSegmentBuffer,0x00,27);
    
    for(unsigned char i = 0; i<27;i++){   
        HT_Write( i* 4  ,0x00 , 1);
        __asm("NOP"); 
    }
  
}

void Test_Segment(unsigned char Item ,unsigned char data){
    
    for(unsigned char i = 0; i<27;i++){
        if(i == Item){
            HT_Write( Item * 4  ,data , 1);
        }else{
            HT_Write( i * 4     ,0x00 , 1);
        }
        __asm("NOP"); 
    }
  
}


void Test7_Segment(unsigned char data){
    
    for(unsigned char i = 0; i<27;i++){
        HT_Write( i * 4  ,data , 1);
        __asm("NOP"); 
    }
  
}