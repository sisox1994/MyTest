/*----------------------------------------------------------------------------
* Name: AlaHR.c
* Purpose: 偵測心跳 (有線/無線)
*��
* API:  	void F_HRProcess( void )
*
* Note(s):
*-----------------------------------------------------------------------------
* version : 20150706 V0.01
* 
* Copyright (c) 2015 Stone - ALATECH Company. All rights reserved.
*----------------------------------------------------------------------------*/

#include "system.h"


#if (configUSE_WHR | configUSE_HHR)

#include "stm32f4xx_hal.h"

#if configUSE_WHR

unsigned char HR5KPairOkFlag;
#define C_ON	1
#define C_OFF	0
unsigned char ucWhr;												//無線心跳
static unsigned char ucWhrTemp;	

unsigned char WhrFlag;
unsigned char WPulseFlag;

#define Pulse_5K_GPIO  GPIOD
#define Pulse_5K_PIN     GPIO_PIN_3

#define HHR_GPIO       GPIOE
#define HHR_PIN          GPIO_PIN_1


void HR_5K_Init(){
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*PD3  -->    5K_HR_OUT    IN       Pulse1  */ 
    GPIO_InitStruct.Pin = Pulse_5K_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     
    GPIO_InitStruct.Pull =GPIO_NOPULL ;         
    HAL_GPIO_Init(Pulse_5K_GPIO, &GPIO_InitStruct);
       
}

void Hand_HR__Init(){

    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*PE1   -->    Hand_HR_IN    IN  */ 
    GPIO_InitStruct.Pin = HHR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     
    GPIO_InitStruct.Pull =  GPIO_PULLUP;  //GPIO_NOPULL       
    HAL_GPIO_Init(HHR_GPIO, &GPIO_InitStruct);
    
}

#endif

#if configUSE_HHR
unsigned char ucHhr;

volatile unsigned char HhrFlag;

unsigned char HPulseFlag;
//有線心跳
#define BUF_SIZE 10
unsigned char ucHhrTemp;
unsigned char ucHhr_Buf[BUF_SIZE];
unsigned char ucWhr_Buf[BUF_SIZE];
#endif

/*============================================================================
* Function	: F_HRCalculate
* Description	: 心跳計算
=============================================================================*/
static unsigned char F_HRCalculate(unsigned char HRValue ,unsigned char HRValueTemp , unsigned char *HRbuffer)
{
  if(HRValue == 0)
  {
    /* 心跳初值設定 */
      
    if(HRValueTemp > 90){
	 HRValue = 90;
    }else{
      if(HRValueTemp < 70){
	 HRValue = 70;			
      }else{
	 HRValue = HRValueTemp;			
      }
    }
    for(unsigned char i =0;i<BUF_SIZE;i++){
        HRbuffer[i] = HRValue;
    }
    /*HRbuffer[0] = HRValue;
    HRbuffer[1] = HRValue;
    HRbuffer[2] = HRValue;
    HRbuffer[3] = HRValue;*/	
  }
  else
  {
    /* 不讓差距超過六 */
    if(HRValueTemp >= HRValue)
    {
	 if((HRValueTemp - HRValue) > 4 )
	 {
	   HRValueTemp = HRValue + 4;
	 }
    }
    else
    {
	 if((HRValue - HRValueTemp) > 4 )
	 {
	   HRValueTemp = HRValue -4;
	 }
    }
    
    for(unsigned char i = 0;i<(BUF_SIZE-1);i++){
        HRbuffer[i] = HRbuffer[i+1];
    }
    HRbuffer[BUF_SIZE-1] = HRValueTemp;
    
    unsigned int temp = 0;
    for(unsigned char i = 0;i<BUF_SIZE;i++){
      temp+= HRbuffer[i];
    }
    HRValue = temp / BUF_SIZE;
    
    /*HRbuffer[0] = HRbuffer[1];
    HRbuffer[1] = HRbuffer[2];
    HRbuffer[2] = HRbuffer[3];
    HRbuffer[3] = HRValueTemp;
    HRValue = (HRbuffer[0] + HRbuffer[1] + HRbuffer[2] + HRbuffer[3]) / 4;
    */
    
  }
  return HRValue;
}
/*============================================================================
* Function	: F_WHRTask
* Description	: 無線心跳計算
=============================================================================*/
#if configUSE_WHR
unsigned short usWhrCnt;
unsigned short usWhrWidth;
 unsigned char ucWhrOld;
 unsigned char ucTemp;	
 
static void F_WHRTask( void )
{  
  /* 無線心跳時間計數 unit:1ms */
  usWhrCnt++; 
    
    /* 讀取腳位變化 */
    if(HAL_GPIO_ReadPin(Pulse_5K_GPIO,Pulse_5K_PIN) )
    {
      
	 /* low too high */
	 if(WhrFlag == 0)
	 {
	   usWhrWidth++;
	   /* 寬度需大於10mS才承認心跳訊號 */
	   if(usWhrWidth > 10)    
	   {	
		usWhrWidth = 0;
		WhrFlag = 1;
                
            
		/* 心跳範圍: 40-240 其餘排除 (60000/40)<usWhrCnt<(60000/240)*/
		if((usWhrCnt <= 1500) && (usWhrCnt >= 250))     
		{   
		  ucWhrTemp = 60000 / usWhrCnt;
		  usWhrCnt = 0;
		  /* 一開始先取得兩次 誤差在+-6 的心跳訊號 */
		  if(ucWhr == 0)
		  {
		    __asm("NOP");
		    if(ucWhrOld == 0)
		    {
			 ucWhrOld = ucWhrTemp;
		    }
		    else
		    {
			 if(ucWhrTemp > ucWhrOld)
			   ucTemp = ucWhrTemp - ucWhrOld;
			 else
			   ucTemp = ucWhrOld - ucWhrTemp;
			 
			 /* 誤差小於六 計算心跳 */
			 if(ucTemp < 6 )
			 {
			   WPulseFlag = 1;
                             HR5KPairOkFlag=C_ON;
                             
			 }
			 ucWhrOld = ucWhrTemp;
		    }
		  }
		  else
		  {
		    WPulseFlag = 1;
                     HR5KPairOkFlag=C_ON;
		  }
		}else{	
		  usWhrCnt = 0;
		  if(ucWhr == 0)
		  {
		    ucWhrOld = 0;
		  }
		}
	   }
	 }
    }else{
	 WhrFlag = 0;
	 usWhrWidth = 0;
    }
    
#if configUSE_Buzz
  }
#endif
  
  /* 兩秒沒心跳 清除所有紀錄 */
  if(usWhrCnt > 2000)
  {
      
    usWhrCnt=0;
    ucWhr = 0;
    ucWhrOld = 0;			
    ucWhrTemp = 0;
    
    for(unsigned char i = 0;i<BUF_SIZE;i++){
        ucWhr_Buf[i]=0;   
    }
    /*
    ucWhr_Buf[0]=0;
    ucWhr_Buf[1]=0;
    ucWhr_Buf[2]=0;
    ucWhr_Buf[3]=0;
    */
    
    HR5KPairOkFlag=C_OFF;
  }
}
#endif
/*============================================================================
* Function	: F_HHRTask
* Description	: 有線心跳計算
=============================================================================*/
#if configUSE_HHR

/*
static unsigned short usHhrCnt;										//有線心跳時間計數
static unsigned short usHhrWidth;									//有線心跳脈波時間寬度
static unsigned char ucHhrOld;										//有線心跳
static unsigned char ucTemp;
  */
unsigned short usHhrCnt;		
unsigned short usHhrWidth;
unsigned char ucHhrOld;		
unsigned char ucTemp;

unsigned short usHhrCnt_Record[10];

unsigned short usHhrVal_Record[10];

unsigned short tmp_cnt;


static void F_HHRTask( void )
{
	
  
  /* 有線心跳時間計數 unit:1ms */
  usHhrCnt++;
  /* 讀取腳位變化 */
  if(HAL_GPIO_ReadPin(HHR_GPIO ,HHR_PIN))
  {
    /* low too high */
    if(HhrFlag == 0)
    {
	 usHhrWidth++;
	 /* 寬度需大於10mS才承認心跳訊號 */
	 if(usHhrWidth > 10)    
	 {	
	   usHhrWidth = 0;
	   HhrFlag = 1;
	   /* 心跳範圍: 40-240 其餘排除 (60000/40)<usWhrCnt<(60000/240)*/
	   if((usHhrCnt <= 1500) && (usHhrCnt >= 250))     
	   {   
               
  
		ucHhrTemp = 60000 / usHhrCnt;
                
                
                usHhrCnt_Record[tmp_cnt] = usHhrCnt;
                usHhrVal_Record[tmp_cnt] = ucHhrTemp;
                
                tmp_cnt++;
                tmp_cnt = tmp_cnt%10;
                
                usHhrCnt = 0;
		/* 一開始先取得兩次 誤差在+-6 的心跳訊號 */
		if(ucHhr == 0)
		{
		  if(ucHhrOld == 0)
		  {
		    ucHhrOld = ucHhrTemp;
		  }
		  else
		  {
		    if(ucHhrTemp > ucHhrOld)
			 ucTemp = ucHhrTemp - ucHhrOld;
		    else
			 ucTemp = ucHhrOld - ucHhrTemp;
		    
		    /* 誤差小於六 計算心跳 */
		    if(ucTemp < 6 )
		    {
			 HPulseFlag = 1;
		    }
		    ucHhrOld = ucHhrTemp;
		  }
		}
		else
		{
		  HPulseFlag = 1;
		}
	   }else{	
		usHhrCnt = 0;
	   }
	 }
    }
  }else{
    HhrFlag = 0;
    usHhrWidth = 0;
  }
  /* 兩秒沒心跳 清除所有紀錄 */
  if(usHhrCnt > 3000)
  {
    usHhrCnt=0;
    ucHhr = 0;
    ucHhrOld = 0;			
    ucHhrTemp = 0;
    
    for(unsigned char i = 0;i<BUF_SIZE;i++){
        ucHhr_Buf[i]=0;   
    }
    /*ucHhr_Buf[0]=0;
    ucHhr_Buf[1]=0;
    ucHhr_Buf[2]=0;
    ucHhr_Buf[3]=0;*/
  } 
}
#endif
/*============================================================================
* Function	: F_HR
* Description	: 心跳偵測
=============================================================================*/
void F_HR( void )
{
#if configUSE_WHR
  F_WHRTask();
#endif
  
#if configUSE_HHR
  F_HHRTask();
#endif
}
/*============================================================================
* Function	: F_HRProcess
* Description	: 心跳計算
=============================================================================*/
void F_HRProcess( void )
{
#if configUSE_WHR
  if(WPulseFlag == 1)
  {
    WPulseFlag = 0;
    ucWhr = F_HRCalculate(ucWhr,ucWhrTemp,ucWhr_Buf);
  }
#endif
  
#if configUSE_HHR
  if(HPulseFlag == 1)
  {
    HPulseFlag = 0;
    ucHhr = F_HRCalculate(ucHhr,ucHhrTemp,ucHhr_Buf);
  }
#endif
}

#endif