#include "system.h"
#include "LED_Matrix.h"
#include "HT1632_C52.h"
#include "string.h"

unsigned char LedMatrixBuffer[32];                                                                          
const unsigned char MatrixOderTable[32] = { 14, 0, 1, 16, 2, 15, 13, 12,
                                              19, 3, 4, 21, 5, 20, 18, 17,
                                              24, 6, 7, 26, 8, 25, 23, 22,
                                              29, 9,10, 31,11, 30, 28, 27 };


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

void Draw_Ver2(short posX ,short posY ,const unsigned char *Data ,unsigned short Speed){     //  跟原本的buffer內容做OR
    
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
        }
        
    }
    
    TotalLength = move; 
    
    
    LedMatrixMappingProcess(LedMatrixBuffer_Temp ,posX,posY);
    
 
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
    
    
    for(int i = (Digit-1); i >= 0 ; i--){
     
            
            unsigned char C_index = Digit_Value[i];
            unsigned char C_width = NumberArray[C_index][0];   //去陣列抓字元寬度出來
            
            for(int j = 0; j < C_width ; j++){    //j = 0~5
                
                LedMatrixBuffer_Temp[move + j] = NumberArray[C_index][j +2];
                
            }
            
            move = move + C_width;
        
    }
    
    
    TotalLength = move; 
    
    LedMatrixMappingProcess(LedMatrixBuffer_Temp ,posX,posY);
    
    
}




void writeLEDMatrix(){
    
    for(unsigned char i = 0; i < 32;i++){
        HT_Write( MatrixOderTable[i] * 4  ,LedMatrixBuffer[i] , 0);
        __asm("NOP");
    }
    
    for(unsigned char i = 0; i < 32;i++){
        LedMatrixBuffer[i] = 0x00;
    }
  
    
}


//unsigned char LedMatrixBuffer_Temp_Ex[120][80];

/*
unsigned char Page1_index;
unsigned char Page2_index;




void Draw(short posX ,short posY ,const unsigned char *Data ,unsigned short Speed){     //  跟原本的buffer內容做OR
    
    MarqueeCnt = Speed;
    
    //unsigned char move = 0;
    //unsigned char LedMatrixBuffer_Temp[320];
    
    unsigned char height;
    unsigned char width;
    
    width  = Data[0];
    height = Data[1];
    
    //-----------------------------------------------
    
    
    for(unsigned char i = 0; i < 120 ; i++){
        memset(LedMatrixBuffer_Temp_Ex[i],0x00,80);
    }
    
    for(unsigned char i = 0; i < height ; i++){
        for(unsigned char j = 0; j < width; j++){
            
            LedMatrixBuffer_Temp_Ex[i][j] = Data[ (width * i) + j + 2 ];  
        
        }    
    }

    unsigned char LedMatrixBuffer_Process[32];
    memset(LedMatrixBuffer_Process,0x00,32);
  
    
    if( (posY+8) % 8 == 0){
        
        if(posY < 0){  
            __asm("NOP");
        }else if(posY >=0){
            
            
            Page1_index = (posY+8)/8 -1;
             
            //----------------------------------------------------------------------------------------------
            if(posX < 0){
                
                for(unsigned char i = 0; i < 32 ; i++){
                    
                    if(  (i + (0-posX)) < 80){
                        LedMatrixBuffer_Process[i] = LedMatrixBuffer_Temp_Ex[Page1_index][i + (0-posX)]; 
                        __asm("NOP");
                    }   
                } 
                
            }else if(posX >= 0){
                for(unsigned char i = 0; ( i + posX ) < 32 ; i++){
                    if(Page1_index < ((height )/8 ) ){
                        LedMatrixBuffer_Process[i + posX] = LedMatrixBuffer_Temp_Ex[Page1_index][i] ;
                    }
                      __asm("NOP");
                }
                
                 __asm("NOP");
            }   
            //----------------------------------------------------------------------------------------------
            
        }
        
      
        
    }else if(  ((posY+8) % 8 ) !=  0 ){
        
        if(posY < 0){
            if( (0 - posY) <= 8 ){
                
                Page_offset = (0 - posY) % 8;
                
                for(unsigned char i = 0; ( i + posX ) < 32 ; i++){
                    
                    LedMatrixBuffer_Process[i] = LedMatrixBuffer_Temp_Ex[0][i + (0-posX)]>>(Page_offset);
                    __asm("NOP");
                }
                
            }            
            
            __asm("NOP");
            
        }else if(posY >= 0){
            //--------------------------------------------------------------------------------------------------
            Page_offset = (posY+8) % 8;
            Page1_index = (posY+8)/8 -1;
            Page2_index = (posY+8)/8;
            
            for(unsigned char i = 0; ( i + posX ) < 32 ; i++){
                
                if( posY < height){
                    LedMatrixBuffer_Process[i] = LedMatrixBuffer_Temp_Ex[Page1_index][i + (0-posX)]<<(Page_offset);
                    __asm("NOP");
                    if(  Page1_index <(height )/8 -1){
                        LedMatrixBuffer_Process[i] |= LedMatrixBuffer_Temp_Ex[Page2_index][i + (0-posX)]>>(8-Page_offset);
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
*/