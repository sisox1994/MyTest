#ifndef SEVENSEGDISPLAY_H_
#define SEVENSEGDISPLAY_H_

typedef enum{
  
  DEC = 0x00,  
  HEX = 0x01

}Format_Def;

typedef enum{
  
  ND = 0x00,  
  D1 = 0x01,
  D2 = 0x02,
  D3 = 0x04,
  D4 = 0x08,

}DOT_Def;


typedef enum{
  
  INCLINE  =   96,
  DISTANCE =   84,
  CALORIES  =  64,
  TIME      =  44,
  PACE      =  24,
  HEARTRATE =  12,
  SPEED     =  0,
  PACE_SPD  =  3

}Seg_Display_ITEM_Def;


void SET_INCLINE_Blink( unsigned short Value ,DOT_Def dot ,Format_Def format ,unsigned char Blink_Swich);
void SET_SPEED_Blink( unsigned short Value ,DOT_Def dot ,Format_Def format ,unsigned char Blink_Swich);

void SET_Seg_Display(Seg_Display_ITEM_Def Item , unsigned short Value ,DOT_Def dot ,Format_Def format );
void SET_Seg_Display_Blink(Seg_Display_ITEM_Def Item , unsigned short Value ,DOT_Def dot ,Format_Def format ,unsigned char Blink_Flag );
void SET_Seg_TIME_Display(Seg_Display_ITEM_Def Item   , unsigned int Value);

void TIME_SET_Display( Seg_Display_ITEM_Def Item  , unsigned int Value , unsigned char BlinkMask , unsigned char Force);

void writeSegmentBuffer();
void Turn_ON_All_Segment();
void Turn_OFF_All_Segment();
void Test_Segment(unsigned char Item ,unsigned char data);
void Test7_Segment(unsigned char data);
#endif