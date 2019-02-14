#include "system.h"
#include "LED_Matrix.h"
#include "HT1632_C52.h"
#include "string.h"

#define StayCnt_Def      10
#define Sus_Speed_Value  15

unsigned char Stay_100ms_Cnt = 0;
static unsigned char  StayCnt;
unsigned char MatrixBlink_Flag = 0;

short Shift_X =0;
short Shift_Y = 0;

unsigned char DataCheckBuffer[32];

unsigned char LedMatrixBuffer[32];                                                                          
const unsigned char MatrixOderTable[32] = { 14, 0, 1, 16, 2, 15, 13, 12,
                                            19, 3, 4, 21, 5, 20, 18, 17,
                                            24, 6, 7, 26, 8, 25, 23, 22,
                                            29, 9,10, 31,11, 30, 28, 27 };


void Clear_DataCheckBuffer(){
    memset(DataCheckBuffer,0x00,32);
}

void ClearLEDBuffer(){
    
    for(unsigned char i = 0; i < 32;i++){
        LedMatrixBuffer[i] = 0x00;
    }
}

void LedMatrixMappingProcess(unsigned char *buffer ,short posX ,short posY){

    unsigned char LedMatrixBuffer_Process[32];
    memset(LedMatrixBuffer_Process,0x00,32);
    
    if(posX < 0){
        
        for(unsigned char i = 0; i < 32 ; i++){
            if(  (i + (0-posX)) < 320){
                LedMatrixBuffer_Process[i] = buffer[i + (0-posX)];
            } 
        }   
    }else if(posX >= 0){
        
        for(unsigned char i = 0; (i+posX)< 32 ; i++){
            LedMatrixBuffer_Process[i + posX] = buffer[i];
        }
        
    }
    if(posY < 0){
        
        for(unsigned char i = 0; i < 32 ; i++){
            LedMatrixBuffer_Process[i] = LedMatrixBuffer_Process[i] << (0-posY);
        }   
    }else if(posY >= 0){
        
        for(unsigned char i = 0; i < 32 ; i++){
            LedMatrixBuffer_Process[i] = LedMatrixBuffer_Process[i] >> (posY);
        }  
    }
    for(unsigned char i = 0; i < 32 ; i++){
        LedMatrixBuffer[i] |= LedMatrixBuffer_Process[i];
    }  
 
}


unsigned char LedMatrixBuffer_Temp_For_Show[2][80];

void Draw(short posX ,short posY ,const unsigned char *Data ,unsigned short Speed){     //  跟原本的buffer內容做OR
    
    MarqueeCnt = Speed;
    
    unsigned char height;
    unsigned char width;
    unsigned char Page_1_index;
    unsigned char Page_2_index;
    unsigned char Page_offset;
     
    width  = Data[0];
    height = Data[1];
    //-----------------------------------------------
    
    unsigned char PageNum;
    
    //剛好顯示一個Row  Y 軸 不用做位移處理
    if(posY >=0){
        
        if( (posY+8) % 8 == 0){
            
            PageNum = 1;
            Page_1_index = (posY + 8)/8 -1;
           
        }else if((posY+8) % 8 != 0){
            
            PageNum = 2;                  //需要抓兩個Row的資料OR 合成一個Row
            Page_1_index = (posY + 8)/8 -1;
            Page_2_index = (posY + 8)/8;  
        }
        
    }else if(posY < 0){
        
        if( 0 - posY < 8){
            PageNum = 1;
            Page_1_index = 0;
        }else{
            PageNum = 0;
            Page_1_index = 0;
            Page_2_index = 0;
        }
    }
    //先清掉站存資料的Buffer
    for(unsigned char i = 0; i < 2 ; i++){
        memset(LedMatrixBuffer_Temp_For_Show[i],0x00,80);
    }
    
    //從Data 裡面取要處理的陣列數值
    if(PageNum == 0){
        

    }else if(PageNum == 1){  
        for(unsigned char j = 0; j < width; j++){
            
            LedMatrixBuffer_Temp_For_Show[0][j] = Data[ (width * Page_1_index) + j + 2 ];  
        } 
    }else if(PageNum == 2){
        for(unsigned char j = 0; j < width; j++){
            LedMatrixBuffer_Temp_For_Show[0][j] = Data[ (width * Page_1_index) + j + 2 ];  
            LedMatrixBuffer_Temp_For_Show[1][j] = Data[ (width * Page_2_index) + j + 2 ]; 
        } 
    }

    unsigned char LedMatrixBuffer_Process[32];
    memset(LedMatrixBuffer_Process,0x00,32);
  
    //剛好顯示一整列
    if( PageNum == 1){
        
        if(posY < 0){  
            if( (0 - posY) < 8 ){
                Page_offset = (0 - posY) % 8;
                for(unsigned char i = 0; ( i + posX ) < 32 ; i++){
                    
                    LedMatrixBuffer_Process[i] = LedMatrixBuffer_Temp_For_Show[0][i + (0-posX)]>>(Page_offset);
                    __asm("NOP");
                }
            }
            __asm("NOP");
            
        }else if(posY >=0){
            
            //----------------------------------------------------------------------------------------------
            if(posX < 0){
                
                for(unsigned char i = 0; i < 32 ; i++){
                    
                    if(  (i + (0-posX)) < 80){
                        LedMatrixBuffer_Process[i] = LedMatrixBuffer_Temp_For_Show[0][i + (0-posX)]; 
                        __asm("NOP");
                    }   
                } 
                
            }else if(posX >= 0){
              
                for(unsigned char i = 0; ( i + posX ) < 32 ; i++){
                  
                    if(Page_1_index < ( height / 8 ) ){
                        LedMatrixBuffer_Process[i + posX] = LedMatrixBuffer_Temp_For_Show[0][i] ;
                    }
                      __asm("NOP");
                }
                 __asm("NOP");
            }   
            //----------------------------------------------------------------------------------------------
        }
        
    }else if( PageNum == 2 ){
        
        if(posY < 0){
            
            if( (0 - posY) <= 8 ){
                
                Page_offset = (0 - posY) % 8;
                
                for(unsigned char i = 0; ( i + posX ) < 32 ; i++){
                    
                    LedMatrixBuffer_Process[i] = LedMatrixBuffer_Temp_For_Show[0][i + (0-posX)]>>(Page_offset);
                    __asm("NOP");
                }
            }            
            
            __asm("NOP");
            
        }else if(posY >= 0){
            //--------------------------------------------------------------------------------------------------
            Page_offset = (posY + 8) % 8;
            
            for(unsigned char i = 0; ( i + posX ) < 32 ; i++){
                
                if( posY < height){
                    
                    LedMatrixBuffer_Process[i] = LedMatrixBuffer_Temp_For_Show[0][i + (0-posX)]<<(Page_offset);
                    __asm("NOP");
                    
                    if( Page_1_index < (height )/8 - 1){
                        
                        LedMatrixBuffer_Process[i] |= LedMatrixBuffer_Temp_For_Show[1][i + (0-posX)]>>(8 - Page_offset);
                        
                    }
                }

            }
            //----------------------------------------------------------------------------------------------------
            
        }        
        
        
        __asm("NOP");
    }
    for(unsigned char i = 0; i < 32 ; i++){
        LedMatrixBuffer[i] |= LedMatrixBuffer_Process[i];
    } 
}

unsigned char Draw_Auto( Direction_Def Dir, const unsigned char *Data ,unsigned short Speed ,unsigned char Blink){
        
    unsigned char height;
    unsigned char width;
    
    width  = Data[0];
    height = Data[1];
    
    for(unsigned char i = 0; i<32; i++){
        if(DataCheckBuffer[i] != Data[i]){
            memcpy(DataCheckBuffer,Data,32);
            
            if(Dir == Left ){
                Shift_X = width;
            }else if(Dir == Right){
                Shift_X = -1 * width;
            }else if(Dir == Down ){
                Shift_Y = height;
            }else if(Dir == Up){
                //Shift_Y = -1 * height;
                Shift_Y = -8;
            }else if(Dir == Stay ){
                Stay_100ms_Cnt = 0;
                Shift_X = 0;
                Shift_Y = 0;
            }
        }  
    }
    
    if(Dir == Left){
        
        if(Shift_X >= 0){
            if(Shift_X > width ){
                Shift_X = width;
                return 1;
            }
        }else if(Shift_X < 0){
            if( ( 0 - Shift_X) > width ){
                Shift_X = width;
                return 1;
            }
        } 
        
    }else if(Dir == Right){
    
        if(Shift_X >= 0){
            if(Shift_X > width ){
                Shift_X = -1 * width;
                return 1;
            }
        }else if(Shift_X < 0){
            if( (0 - Shift_X) > width ){
                Shift_X = -1 * width;
                return 1;
            }
        } 
        
    }else if(Dir == Down){
        
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                Shift_Y = height;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = height;
                return 1;
            }
        } 
        
    }else if(Dir == Up ){
    
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                //Shift_Y = -1 * height;
                Shift_Y = -8;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = -1 * height;
                return 1;
            }
        } 
    }

    if(Dir == Left ){
        
        Shift_X --;
        Shift_Y = 0;
  
    }else if(Dir == Right ){
        
        Shift_X ++;
        Shift_Y = 0;

    }else if(Dir == Down ){
        
        Shift_X = 0;
        Shift_Y --;

    }else if(Dir == Up ){
        
        Shift_X = 0;
        Shift_Y ++;

    }
    
    //----------------閃爍 處理  ---------------
    if(Blink == 1){
        
        if(T500ms_Matrix_Blink_Flag == 1){
            T500ms_Matrix_Blink_Flag = 0;
            
            if(MatrixBlink_Flag){
                MatrixBlink_Flag = 0;
            }else if(MatrixBlink_Flag == 0){
                MatrixBlink_Flag = 1;
            }
        }
        
        if(MatrixBlink_Flag == 1){
            Draw(Shift_X ,Shift_Y ,Data , Speed);
        }else if(MatrixBlink_Flag == 0){
            ClearLEDBuffer();
        }

    }else if(Blink == 0){
        ClearBlinkCnt();
        MatrixBlink_Flag = 0;
        Draw(Shift_X ,Shift_Y ,Data , Speed);
    }

    
    
    if(Dir == Stay){
        
        if(T100ms_Flag){
            T100ms_Flag = 0;
            if(Stay_100ms_Cnt >= Speed){
                Stay_100ms_Cnt = 0;
                return 1;
            }else{
                Stay_100ms_Cnt ++;
            }
        }
    }
    

    return 0;
}

short TotalLength;

void F_String_buffer(short posX ,short posY,unsigned char *StrArray ,unsigned short Length , unsigned short Speed){
    
    MarqueeCnt = Speed;

    unsigned char move = 0;
    unsigned char LedMatrixBuffer_Temp[320];
    
    memset(LedMatrixBuffer_Temp,0x00,320);

    for(int i = 0; i< Length ; i++){
        if ( (*(StrArray+i) >= 'A' ) &&  (*(StrArray+i) <= 'Z' )){
            
            unsigned char C_index = *(StrArray+i) - 0x41 ;
            unsigned char C_width = CapitalAlphabetArray[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5   
                LedMatrixBuffer_Temp[move + j] = CapitalAlphabetArray[C_index][j +2];   
            }
            
            move = move + C_width;
            
        }else if  ( *(StrArray+i) ==' '){
            
            move = move + 1;
            
        }else if (*(StrArray+i) >= '0' &&  (*(StrArray+i) <= '9' ) ){
            
            unsigned char C_index = *(StrArray+i) - 0x30 ;
            unsigned char C_width = NumberArray[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                LedMatrixBuffer_Temp[move + j] = NumberArray[C_index][j +2];
            }         
            move = move + C_width;
            
        }else if (*(StrArray+i) >= 0x20 &&  (*(StrArray+i) <= 0x2F ) ){
            
            unsigned char C_index = *(StrArray+i) - 0x20 ;
            unsigned char C_width = Pattern_Array[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                LedMatrixBuffer_Temp[ move+ j ] = Pattern_Array[C_index][j +2];  
            }       
            move = move + C_width;
        }else if (*(StrArray+i) == ':' ){
            
            unsigned char C_index = 16 ;
            unsigned char C_width = Pattern_Array[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                LedMatrixBuffer_Temp[ move+ j ] = Pattern_Array[C_index][j +2];  
            }       
            move = move + C_width;
        }
    }
    
    TotalLength = move; 

    LedMatrixMappingProcess(LedMatrixBuffer_Temp ,posX,posY);

}

unsigned char F_String_buffer_Auto( Direction_Def Dir, unsigned char *StrArray ,unsigned short Speed ,unsigned char Blink){
    
    
    unsigned char  LedMatrixBuffer_Temp[320];
    unsigned char  width  = 0;
    unsigned char  height = 8;
    unsigned short Length = 0;
    unsigned char offSet_Center;
    //------------------------------算字串長度 Length  +  顯示寬度 width
    while(StrArray[Length] != '\0'){
        Length++;
    }
    
    for(int i = 0; i< Length ; i++){
        if ( (*(StrArray+i) >= 'A' ) &&  (*(StrArray+i) <= 'Z' )){
            
            unsigned char C_index = *(StrArray+i) - 0x41 ;
            unsigned char C_width = CapitalAlphabetArray[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                LedMatrixBuffer_Temp[width + j] = CapitalAlphabetArray[C_index][j +2];
            }
            
            width = width + C_width;
            
        }else if  ( *(StrArray+i) ==' '){
            
            width = width + 1;
            
        }else if (*(StrArray+i) >= '0' &&  (*(StrArray+i) <= '9' ) ){
            
            unsigned char C_index = *(StrArray+i) - 0x30 ;
            unsigned char C_width = NumberArray[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[width + j] = NumberArray[C_index][j +2];
                
            }         
            width = width + C_width;
            
        }else if (*(StrArray+i) >= 0x20 &&  (*(StrArray+i) <= 0x2F ) ){
            
            unsigned char C_index = *(StrArray+i) - 0x20 ;
            unsigned char C_width = Pattern_Array[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[ width+ j ] = Pattern_Array[C_index][j +2];
                
            }       
            width = width + C_width;
        }else if (*(StrArray+i) == ':' ){
            
            unsigned char C_index = 16 ;
            unsigned char C_width = Pattern_Array[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                LedMatrixBuffer_Temp[ width+ j ] = Pattern_Array[C_index][j +2];  
            }       
            width = width + C_width;
        }  
    }
    //-------------------------------------------------------------------------------------

    //----數字沒有超出範圍就置中----------
    
    if(width < 32){
        offSet_Center = 0;
       // offSet_Center = (32 - width)/2;
    }else{
        offSet_Center = 0;
    }

    __asm("NOP");
    
    //---------------------------  顯示物件判斷  --------------------------------------------

    for(unsigned char i = 0; i<32; i++){
        if(DataCheckBuffer[i] != LedMatrixBuffer_Temp[i]){
            memcpy(DataCheckBuffer,LedMatrixBuffer_Temp,32);
            
            
            if((Dir == Left) || (Dir == Left_and_Stay) ){
                //Shift_X = width;
                Shift_X = 32;
            }else if((Dir == Right) || (Dir == Right_and_Stay) ){
                Shift_X = -1 * width;
            }else if((Dir == Up) || (Dir == Up_and_Stay) ){
                Shift_Y = height;
            }else if((Dir == Down )|| (Dir == Down_and_Stay) ){
                Shift_Y = -1 * height;
            }else if(Dir == Stay ){
                Stay_100ms_Cnt = 0;
                Shift_X = 0 + offSet_Center;
                Shift_Y = 0;
            }
        }  
    }
    
    //--------------------------------  跑馬燈 跑完一輪   --------------------------------
    
    if((Dir == Left) || (Dir == Left_and_Stay)){
        
        if(Shift_X >= 0){
            if(Shift_X > width ){
               // Shift_X = width;
                Shift_X = 32;
                return 1;
            }
        }else if(Shift_X < 0){
            if( ( 0 - Shift_X) > width ){
                //Shift_X = width;
                Shift_X = 32;
                return 1;
            }
        } 
        
    }else if((Dir == Right) || (Dir == Right_and_Stay)){
    
        if(Shift_X >= 0){
            if(Shift_X > width ){
                Shift_X = -1 * width;
                return 1;
            }
        }else if(Shift_X < 0){
            if( (0 - Shift_X) > width ){
                Shift_X = -1 * width;
                return 1;
            }
        } 
    }else if((Dir == Up) || (Dir == Up_and_Stay)){
        
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                Shift_Y = height;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = height;
                return 1;
            }
        }    
    }else if( (Dir == Down) || (Dir == Down_and_Stay) ){
    
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                Shift_Y = -1 * height;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = -1 * height;
                return 1;
            }
        } 
    }
    //------------------------------------------------------------------------------

    
    //-------------------------   移動參數處理  ------------------------------------
    if(Dir == Left ){
        
        Shift_X --;
        Shift_Y = 0;
        
    }else if(Dir == Left_and_Stay){
        //------停頓點判斷---------------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_X --;
        }
        if( (StayCnt == 0) && (Shift_X == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
        Shift_Y = 0;  
    
    }else if(Dir == Right ){
        
        Shift_X ++;
        Shift_Y = 0;

    }else if(Dir == Right_and_Stay){
    
        //------停頓點判斷-----------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_X ++;
        }
        if( (StayCnt == 0) && (Shift_X == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
        
        Shift_Y = 0;  
        
    }else if(Dir == Up ){
        
        Shift_X = 0 + offSet_Center;
        Shift_Y --;

    }else if(Dir == Up_and_Stay){
        
        Shift_X = 0 + offSet_Center;
        
        //------停頓點判斷---------------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_Y --;
        }
        if( (StayCnt == 0) && (Shift_Y == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y < 0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
    }else if(Dir == Down ){
        
        Shift_X = 0 + offSet_Center;
        Shift_Y ++;

    }else if(Dir == Down_and_Stay){
        
        Shift_X = 0 + offSet_Center;

        //-----------停頓點判斷---------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_Y ++;
        }
        if( (StayCnt == 0) && (Shift_Y == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //--------------------------------------     
    }
   //--------------------------------------------------------------- 
    //----------------閃爍 處理  ---------------
    if(Blink == 1){
        
        if(T500ms_Matrix_Blink_Flag == 1){
            T500ms_Matrix_Blink_Flag = 0;
            
            if(MatrixBlink_Flag){
                MatrixBlink_Flag = 0;
            }else if(MatrixBlink_Flag == 0){
                MatrixBlink_Flag = 1;
            }
        }
        
        if(MatrixBlink_Flag == 1){
            F_String_buffer(Shift_X , Shift_Y+1 , StrArray , Length , Speed);
        }else if(MatrixBlink_Flag == 0){
            ClearLEDBuffer();
        }
    
    }else if(Blink == 0){
        ClearBlinkCnt();
        MatrixBlink_Flag = 0;
        F_String_buffer(Shift_X , Shift_Y+1 , StrArray , Length , Speed);
    }
    if(Dir == Stay){
        
        if(T100ms_Flag){
            T100ms_Flag = 0;
            if(Stay_100ms_Cnt >= Speed){
                Stay_100ms_Cnt = 0;
                return 1;
            }else{
                Stay_100ms_Cnt ++;
            }
            
        }
    }

    return 0;
}

unsigned char F_String_buffer_Auto_Middle( Direction_Def Dir, unsigned char *StrArray ,unsigned short Speed ,unsigned char Blink){

    unsigned char  LedMatrixBuffer_Temp[320];
    unsigned char  width  = 0;
    unsigned char  height = 8;
    unsigned short Length = 0;
    unsigned char offSet_Center;
    //------------------------------算字串長度 Length  +  顯示寬度 width
    while(StrArray[Length] != '\0'){
        Length++;
    }
    
    for(int i = 0; i< Length ; i++){
        if ( (*(StrArray+i) >= 'A' ) &&  (*(StrArray+i) <= 'Z' )){
            
            unsigned char C_index = *(StrArray+i) - 0x41 ;
            unsigned char C_width = CapitalAlphabetArray[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                LedMatrixBuffer_Temp[width + j] = CapitalAlphabetArray[C_index][j +2];
            }
            
            width = width + C_width;
            
        }else if  ( *(StrArray+i) ==' '){
            
            width = width + 1;
            
        }else if (*(StrArray+i) >= '0' &&  (*(StrArray+i) <= '9' ) ){
            
            unsigned char C_index = *(StrArray+i) - 0x30 ;
            unsigned char C_width = NumberArray[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[width + j] = NumberArray[C_index][j +2];
                
            }         
            width = width + C_width;
            
        }else if (*(StrArray+i) >= 0x20 &&  (*(StrArray+i) <= 0x2F ) ){
            
            unsigned char C_index = *(StrArray+i) - 0x20 ;
            unsigned char C_width = Pattern_Array[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[ width+ j ] = Pattern_Array[C_index][j +2];
                
            }       
            width = width + C_width;
        }  
    }
    //-------------------------------------------------------------------------------------

    //----數字沒有超出範圍就置中----------
    
    if(width < 32){
        //offSet_Center = 0;
        offSet_Center = (32 - width)/2;
    }else{
        offSet_Center = 0;
    }
    __asm("NOP");
    
    //---------------------------  顯示物件判斷  --------------------------------------------

    for(unsigned char i = 0; i<32; i++){
        if(DataCheckBuffer[i] != LedMatrixBuffer_Temp[i]){
            memcpy(DataCheckBuffer,LedMatrixBuffer_Temp,32);
            
            
            if((Dir == Left) || (Dir == Left_and_Stay) ){
                Shift_X = width;
            }else if((Dir == Right) || (Dir == Right_and_Stay) ){
                Shift_X = -1 * width;
            }else if((Dir == Up) || (Dir == Up_and_Stay) ){
                Shift_Y = height;
            }else if((Dir == Down )|| (Dir == Down_and_Stay) ){
                Shift_Y = -1 * height;
            }else if(Dir == Stay ){
                Stay_100ms_Cnt = 0;
                Shift_X = 0 + offSet_Center;
                Shift_Y = 0;
            } 
        }  
    }
 
    //--------------------------------  跑馬燈 跑完一輪   --------------------------------
    
    if((Dir == Left) || (Dir == Left_and_Stay)){
        
        if(Shift_X >= 0){
            if(Shift_X > width ){
                Shift_X = width;
                return 1;
            }
        }else if(Shift_X < 0){
            if( ( 0 - Shift_X) > width ){
                Shift_X = width;
                return 1;
            }
        } 
        
    }else if((Dir == Right) || (Dir == Right_and_Stay)){
    
        if(Shift_X >= 0){
            if(Shift_X > width ){
                Shift_X = -1 * width;
                return 1;
            }
        }else if(Shift_X < 0){
            if( (0 - Shift_X) > width ){
                Shift_X = -1 * width;
                return 1;
            }
        } 
        
    }else if((Dir == Up) || (Dir == Up_and_Stay)){
        
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                Shift_Y = height;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = height;
                return 1;
            }
        } 
        
    }else if( (Dir == Down) || (Dir == Down_and_Stay) ){
    
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                Shift_Y = -1 * height;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = -1 * height;
                return 1;
            }
        } 
    }
    //------------------------------------------------------------------------------
    
    //-------------------------   移動參數處理  ------------------------------------
    if(Dir == Left ){
        
        Shift_X --;
        Shift_Y = 0;
        
    }else if(Dir == Left_and_Stay){
        
        //------停頓點判斷---------------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_X --;
        }
        if( (StayCnt == 0) && (Shift_X == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
        Shift_Y = 0;  
    
    }else if(Dir == Right ){
        
        Shift_X ++;
        Shift_Y = 0;

    }else if(Dir == Right_and_Stay){
    
        //------停頓點判斷-----------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_X ++;
        }
        if( (StayCnt == 0) && (Shift_X == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
        
        Shift_Y = 0;  
        
    }else if(Dir == Up ){
        
        Shift_X = 0 + offSet_Center;
        Shift_Y --;

    }else if(Dir == Up_and_Stay){
        
        Shift_X = 0 + offSet_Center;
        
        //------停頓點判斷---------------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_Y --;
        }
        if( (StayCnt == 0) && (Shift_Y == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y < 0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
    }else if(Dir == Down ){
        
        Shift_X = 0 + offSet_Center;
        Shift_Y ++;

    }else if(Dir == Down_and_Stay){
        
        Shift_X = 0 + offSet_Center;

        //-----------停頓點判斷---------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_Y ++;
        }
        if( (StayCnt == 0) && (Shift_Y == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //--------------------------------------
                
    }
   //--------------------------------------------------------------- 
    
    //----------------閃爍 處理  ---------------
    if(Blink == 1){
        
        if(T500ms_Matrix_Blink_Flag == 1){
            T500ms_Matrix_Blink_Flag = 0;
            
            
            if(MatrixBlink_Flag){
                MatrixBlink_Flag = 0;
            }else if(MatrixBlink_Flag == 0){
                MatrixBlink_Flag = 1;
            }
        }
        
        if(MatrixBlink_Flag == 1){
            F_String_buffer(Shift_X , Shift_Y+1 , StrArray , Length , Speed);
        }else if(MatrixBlink_Flag == 0){
            ClearLEDBuffer();
        }

    }else if(Blink == 0){
        ClearBlinkCnt();
        MatrixBlink_Flag = 0;
        F_String_buffer(Shift_X , Shift_Y+1 , StrArray , Length , Speed);
    }
    
    if(Dir == Stay){
        
        if(T100ms_Flag){
            T100ms_Flag = 0;
            if(Stay_100ms_Cnt >= Speed){
                Stay_100ms_Cnt = 0;
                return 1;
            }else{
                Stay_100ms_Cnt ++;
            }
            
        }
    }
    

    return 0;
    
}
    




uint8_t Digit = 1;
uint8_t Digit_Value[10];


//最長可顯示9位數
void F_Num_buffer(short posX ,short posY,unsigned int Value , unsigned short Speed ,Format_Def format){
  
    MarqueeCnt = Speed;

    unsigned char move = 0;
    unsigned char LedMatrixBuffer_Temp[320];
    unsigned int PowerValue;
    
    unsigned int Value_Temp;
    
    Digit = 1;
    
    memset(LedMatrixBuffer_Temp,0x00,320);

    Value_Temp = Value;
    
    if(format == DEC){
        
        PowerValue = 10;
        
        while((Value_Temp/10)!=0){
            Digit++;
            Value_Temp = Value_Temp /10;
            
        }
        

        for(unsigned i = 0; i < Digit; i++){
            
            Value_Temp = Value;
            
            Digit_Value[i] = (Value_Temp%PowerValue)/(PowerValue/10);
            PowerValue = PowerValue * 10;
        }
        
        
       
    }else if(format == HEX){
    
        PowerValue = 16;
        
        while((Value_Temp)!=0){
            
            Digit++;
            Value_Temp = Value_Temp>>8;
            
        }
        
        for(unsigned i = 0; i < Digit; i++){
            
            Value_Temp = Value;
            
            Digit_Value[i] = (Value_Temp%PowerValue)/(PowerValue/16);
            PowerValue = PowerValue * 16;
        }
    }
 
    __asm("NOP");
    
    

    
    
    
    if(Setting_item_Index == SET_MAX_INC){
        
        if(Digit < 2){
            //------------只有一位數 先補0
            unsigned char C_width = NumberArray[0][0];
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[move + j] = NumberArray[0][j +2];
                
            }
            move = move + C_width;
            
        }
        
        for(int i = (Digit-1); i >= 0 ; i--){
            
            unsigned char C_index = Digit_Value[i];
            unsigned char C_width = NumberArray[C_index][0];   //去陣列抓字元寬度出來
                 
            //--------------偷 + 小屬點進去------------
           
            if(i ==0){
                LedMatrixBuffer_Temp[move] = 0x02;
                move +=2;
            }
         
            //--------------------------
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[move + j] = NumberArray[C_index][j +2];
                
            }
            
            move = move + C_width;
            
        }
        
    }else{
        
        for(int i = (Digit-1); i >= 0 ; i--){
            
            unsigned char C_index = Digit_Value[i];
            unsigned char C_width = NumberArray[C_index][0];   //去陣列抓字元寬度出來
            

            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[move + j] = NumberArray[C_index][j +2];
                
            }
            
            move = move + C_width;
            
        }
    
    }
    
    
 
    
    TotalLength = move; 
    
    LedMatrixMappingProcess(LedMatrixBuffer_Temp ,posX,posY);
    
    
}



void F_Time_buffer(short posX ,short posY,unsigned int Value){
  

    unsigned char LedMatrixBuffer_Temp[320];
    
    unsigned char move = 0;
    
    
    Digit = 3;
    
    memset(LedMatrixBuffer_Temp,0x00,320);


    
    Digit_Value[2] =  Value/60;  //分鐘
    Digit_Value[1] = (Value%60)/10;  //秒 (十位) 
    Digit_Value[0] = (Value%60)%10;  //秒 (個位)
    

    __asm("NOP");
    
    
    unsigned char C_index; 
    unsigned char C_width; 
    
    
    //------------------------------分鐘---------------------------
    C_index = Digit_Value[2];
    C_width = NumberArray[C_index][0];  //去陣列抓字元寬度出來
    
    for(int j = 0; j < C_width ; j++){    //j = 0~5
        LedMatrixBuffer_Temp[j + move] = NumberArray[C_index][j+2];
    }
    move = move + C_width; 
    //------------------------------------------------------------------
    
    //-----------------------------冒號---------------------------
   
    LedMatrixBuffer_Temp[move+1] = 0x24;
    move = move + 4; 
    //------------------------------------------------------------------
    
    
    //------------------------------秒(十位)---------------------------
    C_index = Digit_Value[1];
    C_width = NumberArray[C_index][0];  //去陣列抓字元寬度出來
    
    for(int j = 0; j < C_width ; j++){    //j = 0~5
        LedMatrixBuffer_Temp[j + move] = NumberArray[C_index][j+2];
    }
    move = move + C_width; 
    //------------------------------------------------------------------
    //------------------------------秒(個位)---------------------------
    C_index = Digit_Value[0];
    C_width = NumberArray[C_index][0];  //去陣列抓字元寬度出來
    
    for(int j = 0; j < C_width ; j++){    //j = 0~5
        LedMatrixBuffer_Temp[j + move] = NumberArray[C_index][j+2];
    }
    move = move + C_width; 
    //------------------------------------------------------------------
 
    LedMatrixMappingProcess(LedMatrixBuffer_Temp ,posX,posY);
    
    
}
unsigned char F_Number_buffer_Auto( Direction_Def Dir, unsigned int Value , unsigned short Speed ,Format_Def format ,unsigned char Blink ){

    unsigned char  LedMatrixBuffer_Temp[320];
    unsigned char  width  = 0;
    unsigned char  height = 8;

    unsigned int PowerValue;
    unsigned int Value_Temp;
    unsigned char offSet_Center;
    
    //-------------------------算顯示寬度點數  width  ----------------------------------------
    Digit = 1;
    memset(LedMatrixBuffer_Temp,0x00,320);
    Value_Temp = Value;
    
    if(format == DEC){
        
        PowerValue = 10;
        
        while((Value_Temp/10)!=0){
            Digit++;
            Value_Temp = Value_Temp /10;    
        }
        for(unsigned i = 0; i < Digit; i++){
            
            Value_Temp = Value;
            
            Digit_Value[i] = (Value_Temp%PowerValue)/(PowerValue/10);
            PowerValue = PowerValue * 10;
        }
        
    }else if(format == HEX){
    
        PowerValue = 16;
        
        while((Value_Temp)!=0){
            Digit++;
            Value_Temp = Value_Temp>>8;
        }
        
        for(unsigned i = 0; i < Digit; i++){    
            Value_Temp = Value;
            Digit_Value[i] = (Value_Temp%PowerValue)/(PowerValue/16);
            PowerValue = PowerValue * 16;
        }
    }
 
    
    for(int i = (Digit-1); i >= 0 ; i--){
        unsigned char C_index = Digit_Value[i];
        unsigned char C_width = NumberArray[C_index][0];   //去陣列抓字元寬度出來
        
        for(int j = 0; j < C_width ; j++){    //j = 0~5
            LedMatrixBuffer_Temp[width + j] = NumberArray[C_index][j +2];
        }
        
        width = width + C_width;
        
    }
    
    //--設定 揚升最大值  前面會先補0 所以寬度也要先+回來 置中才不會有問題
    if(Setting_item_Index == SET_MAX_INC){
        if(Digit<2){
            width += NumberArray[0][0];
        }
    }
    
    
    //----數字沒有超出範圍就置中----------
 
    if(width < 32){
        offSet_Center = (32 - width)/2;
    }else{
        offSet_Center = 0;
    }
   
    
    //----------------------------------------------------------------------------------
      //---------------------------  顯示物件判斷  --------------------------------------------
    for(unsigned char i = 0; i<32; i++){
        if(DataCheckBuffer[i] != LedMatrixBuffer_Temp[i]){
            memcpy(DataCheckBuffer,LedMatrixBuffer_Temp,32);
            
            if((Dir == Left) || (Dir == Left_and_Stay) ){
                Shift_X = width;
            }else if((Dir == Right) || (Dir == Right_and_Stay) ){
                Shift_X = -1 * width;
            }else if((Dir == Up) || (Dir == Up_and_Stay) ){
                Shift_Y = height;
            }else if((Dir == Down )|| (Dir == Down_and_Stay) ){
                Shift_Y = -1 * height;
            }else if(Dir == Stay){
                Stay_100ms_Cnt = 0;
                Shift_X = 0 + offSet_Center;
                Shift_Y = 0;
            }
        }  
    }
    
    
    //--------------------------------  跑馬燈 跑完一輪   --------------------------------
   
    if((Dir == Left) || (Dir == Left_and_Stay)){
        
        if(Shift_X >= 0){
            if(Shift_X > width ){
                Shift_X = width;
                return 1;
            }
        }else if(Shift_X < 0){
            if( ( 0 - Shift_X) > width ){
                Shift_X = width;
                return 1;
            }
        } 
        
    }else if((Dir == Right) || (Dir == Right_and_Stay)){
    
        if(Shift_X >= 0){
            if(Shift_X > width ){
                Shift_X = -1 * width;
                return 1;
            }
        }else if(Shift_X < 0){
            if( (0 - Shift_X) > width ){
                Shift_X = -1 * width;
                return 1;
            }
        } 
        
    }else if((Dir == Up) || (Dir == Up_and_Stay)){
        
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                Shift_Y = height;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = height;
                return 1;
            }
        } 
        
    }else if( (Dir == Down) || (Dir == Down_and_Stay) ){
    
        if(Shift_Y >= 0){
            if(Shift_Y > height ){
                Shift_Y = -1 * height;
                return 1;
            }
        }else if(Shift_Y < 0){
            if( (0 - Shift_Y) > height ){
                Shift_Y = -1 * height;
                return 1;
            }
        } 
    }
    //------------------------------------------------------------------------------

    
    
    //-------------------------   移動參數處理  ------------------------------------
    if(Dir == Left ){
        
        Shift_X --;
        Shift_Y = 0;
        
    }else if(Dir == Left_and_Stay){
        
        //------停頓點判斷---------------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_X --;
        }
        if( (StayCnt == 0) && (Shift_X == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
        Shift_Y = 0;  
        
    }else if(Dir == Right ){
        
        Shift_X ++;
        Shift_Y = 0;
        
    }else if(Dir == Right_and_Stay){
        
        //------停頓點判斷-----------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_X ++;
        }
        if( (StayCnt == 0) && (Shift_X == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
        
        Shift_Y = 0;  
        
    }else if(Dir == Up ){
        
        Shift_X = 0 + offSet_Center;
        Shift_Y --;
        
    }else if(Dir == Up_and_Stay){
        
        Shift_X = 0 + offSet_Center;
        
        //------停頓點判斷---------------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_Y --;
        }
        if( (StayCnt == 0) && (Shift_Y == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y < 0){
            Speed = Speed + Sus_Speed_Value;
        }
        //-----------------------------------
    }else if(Dir == Down ){
        
        Shift_X = 0 + offSet_Center;
        Shift_Y ++;
        
    }else if(Dir == Down_and_Stay){
        
        Shift_X = 0 + offSet_Center;
        
        //-----------停頓點判斷---------------
        if(StayCnt > 0){
            StayCnt--;
        }
        if(StayCnt == 0){
            Shift_Y ++;
        }
        if( (StayCnt == 0) && (Shift_Y == 0 )){
            StayCnt = StayCnt_Def;
        }
        if(Shift_Y >0){
            Speed = Speed + Sus_Speed_Value;
        }
        //--------------------------------------
        
    }
    //--------------------------------------------------------------- 
    
    //-------------------閃爍處理-------------------------
    if(Blink == 1){
        
        if(T500ms_Matrix_Blink_Flag == 1){
            T500ms_Matrix_Blink_Flag = 0;
            
            if(MatrixBlink_Flag){
                MatrixBlink_Flag = 0;
            }else if(MatrixBlink_Flag == 0){
                MatrixBlink_Flag = 1;
            }
        }
        
        if(MatrixBlink_Flag == 1){
            F_Num_buffer(Shift_X ,Shift_Y + 1 , Value , Speed , format);
        }else if(MatrixBlink_Flag == 0){
            ClearLEDBuffer();
        }
        
        
        
    }else if(Blink == 0){
        ClearBlinkCnt();
        MatrixBlink_Flag = 0;
        F_Num_buffer(Shift_X ,Shift_Y + 1 , Value , Speed , format);
    }
    
    
    
    if(Dir == Stay){
        if(T100ms_Flag){
            T100ms_Flag = 0;
            if(Stay_100ms_Cnt >= Speed){
                Stay_100ms_Cnt = 0;
                return 1;
            }else{
                Stay_100ms_Cnt ++;
            }
            
        }
    }
    
    
    return 0;
        
    
}




void Turn_ON_All_LEDMatrix(){
    
    for(unsigned char i = 0; i < 32;i++){
        LedMatrixBuffer[i] = 0xFF;
    } 
    
    for(unsigned char i = 0; i < 32;i++){
        HT_Write( MatrixOderTable[i] * 4  ,LedMatrixBuffer[i] , 0);
        __asm("NOP");
    }

}

void Turn_OFF_All_LEDMatrix(){
    
    for(unsigned char i = 0; i < 32;i++){
        LedMatrixBuffer[i] = 0x00;
    } 
    
    for(unsigned char i = 0; i < 32;i++){
        HT_Write( MatrixOderTable[i] * 4  ,LedMatrixBuffer[i] , 0);
        __asm("NOP");
    }

}


unsigned char PowerBarArray[] = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};

void DrawBarArray(unsigned char *BarArray_Temp){

   
    for(unsigned char i = 0; i < 32; i++){  
        LedMatrixBuffer[i] = PowerBarArray[BarArray_Temp[i]];
    }
   
}

unsigned char Blink_Flag_Work;

unsigned short Hr_Display_Cnt;
void Basic_HRC_Display(){
    
    //Hr_Display_Cnt++;
    
    //if(Hr_Display_Cnt%100==0){
        switch((str_cNt/100)%2){
            
          case 0:
            Draw(7 ,0 ,Heart_Empty ,500);
            Draw(18 ,0 ,Heart_Empty ,500);
            
            
            //str_cNt++;
            //str_cNt = str_cNt%2;
            break; 
          case 1:
            Draw(7 ,0 ,Heart_Empty ,500);
            
            if(usNowHeartRate != 0){
                Draw(18 ,0 ,Heart_Solid ,500);
            }else{
                Draw(18 ,0 ,Heart_Empty ,500);
            }
            
            
           //str_cNt = str_cNt/;
            break; 
        }
        str_cNt++;
        
    //}
    
}

void DrawBarArray_Workout( unsigned char *BarArray_Temp , unsigned char Blink_Index , unsigned char B_Flag ){

   
    //***************************************************
    for(unsigned char i = 0; i < 32; i++){
       /* if(BarArray_Temp[i] >=30 ){
            BarArray_Temp[i] = 30;
        }else if( BarArray_Temp[i] <= 0){
            BarArray_Temp[i] = 0; 
        }*/
        LedMatrixBuffer[i] = PowerBarArray[BarArray_Temp[i]];
         
    }
    
    if(Blink_Flag_Work == 1){
        if(Blink_Index < 32){
            LedMatrixBuffer[Blink_Index] = 0x00;
        }
    }
    
    if(B_Flag == 1){
        
        if(T500ms_BarArray_Blink_Flag){
            T500ms_BarArray_Blink_Flag = 0;
            
            if(Blink_Flag_Work){
               Blink_Flag_Work = 0;
            }else if(Blink_Flag_Work == 0){
                Blink_Flag_Work = 1;
            }   
        }
        
 
    }else if(B_Flag == 0){
        //ClearBlinkCnt_BarArray();
        Blink_Flag_Work = 1;
    }
    
   
}

void writeLEDMatrix(){
    
    
    if(DIST_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay, BLE_HR_Icon ,50 ,0);
         F_String_buffer_Auto_Middle(Stay,"DIST" ,30 ,0);
    }
    
    if(ALTI_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay,ANT_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"ALTI" ,30 ,0);
    }
    if(CAL_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay, BLE_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"CAL" ,30 ,0);
    }
    if(CALH_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay,ANT_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"CAL  H" ,30 ,0);
    }
    if(ELAPSED_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay, BLE_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"ETIME" ,30 ,0);
  
    }
    if(REMAINNING_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
       //Draw_Auto( Stay,ANT_HR_Icon ,50 ,0);
       F_String_buffer_Auto_Middle(Stay,"RTIME" ,30 ,0);
        
    }
    if(PACE_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay, BLE_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"PACE" ,30 ,0);
    }
    if(STEP_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay,ANT_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"STEP" ,30 ,0);
    }
    if(MET_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay, BLE_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"MET" ,30 ,0);
    }
    if(HR_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        //Draw_Auto( Stay,ANT_HR_Icon ,50 ,0);
        F_String_buffer_Auto_Middle(Stay,"HR" ,30 ,0);
    }
    
    
    
    //-----------------------------------------------
    
    if(Ble_Icon_Display_Cnt){
        //Draw(16 ,0 ,BLE_HR_Icon ,70);.
        memset(LedMatrixBuffer,0x00,32);
        Draw_Auto( Stay, BLE_HR_Icon ,50 ,0);
    }
    
    if(ANT_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        Draw_Auto( Stay,ANT_HR_Icon ,50 ,0);
    }
    
    
    if(BTSPK_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        Draw_Auto( Stay, SPK_ON_Icon ,50 ,0);
    }
    
    if(BTSPK_OFF_Icon_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        Draw_Auto( Stay, SPK_OFF_Icon ,50 ,0);
    }
    
    if(APP_Connected_Display_Cnt){
        memset(LedMatrixBuffer,0x00,32);
        F_String_buffer_Auto_Middle( Stay, "APP" ,50 ,0);
    }
    
    //---------------------------------------------------
    
    
    for(unsigned char i = 0; i < 32;i++){
        HT_Write( MatrixOderTable[i] * 4  ,LedMatrixBuffer[i] , 0);
        __asm("NOP");
    }
    
    for(unsigned char i = 0; i < 32;i++){
        LedMatrixBuffer[i] = 0x00;
    }  
}
