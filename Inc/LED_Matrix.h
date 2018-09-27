
#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_

extern unsigned char MatrixBlink_Flag;
extern unsigned char LedMatrixBuffer[32];  

void writeLEDMatrix();
void Clear_DataCheckBuffer();
void ClearLEDBuffer();

void Draw(short posX ,short posY ,const unsigned char *Data ,unsigned short Speed);    //  跟原本的buffer內容做OR
unsigned char  Draw_Auto( Direction_Def Dir, const unsigned char *Data ,unsigned short Speed ,unsigned char Blink);    

void Turn_ON_All_LEDMatrix();
void Turn_OFF_All_LEDMatrix();

void F_String_buffer(short posX , short posY , unsigned char *StrArray ,unsigned short Length, unsigned short Speed);
unsigned char F_String_buffer_Auto( Direction_Def Dir, unsigned char *StrArray ,unsigned short Speed ,unsigned char Blink);
unsigned char F_String_buffer_Auto_Middle( Direction_Def Dir, unsigned char *StrArray ,unsigned short Speed ,unsigned char Blink);


void F_Num_buffer(short posX ,short posY,unsigned int Value , unsigned short Speed ,Format_Def format);
unsigned char F_Number_buffer_Auto( Direction_Def Dir, unsigned int Value , unsigned short Speed ,Format_Def format ,unsigned char Blink );

void DrawBarArray(unsigned char *BarArray_Temp);
void DrawBarArray_Workout( unsigned char *BarArray_Temp , unsigned char Blink_Index , unsigned char Blink_Flag );
void Basic_HRC_Display();

#endif