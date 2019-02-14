/*----------------------------------------------------------------------------
* Name: AlaHR.c
* Purpose: �����߸� (���u/�L�u)
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
unsigned char ucWhr;												//�L�u�߸�
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
//���u�߸�
#define BUF_SIZE 10
unsigned char ucHhrTemp;
unsigned char ucHhr_Buf[BUF_SIZE];
unsigned char ucWhr_Buf[BUF_SIZE];
#endif

/*============================================================================
* Function	: F_HRCalculate
* Description	: �߸��p��
=============================================================================*/
static unsigned char F_HRCalculate(unsigned char HRValue ,unsigned char HRValueTemp , unsigned char *HRbuffer)
{
  if(HRValue == 0)
  {
    /* �߸���ȳ]�w */
      
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
    /* �����t�Z�W�L�� */
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
* Description	: �L�u�߸��p��
=============================================================================*/
#if configUSE_WHR
unsigned short usWhrCnt;
unsigned short usWhrWidth;
 unsigned char ucWhrOld;
 unsigned char ucTemp;	
 
static void F_WHRTask( void )
{  
  /* �L�u�߸��ɶ��p�� unit:1ms */
  usWhrCnt++; 
    
    /* Ū���}���ܤ� */
    if(HAL_GPIO_ReadPin(Pulse_5K_GPIO,Pulse_5K_PIN) )
    {
      
	 /* low too high */
	 if(WhrFlag == 0)
	 {
	   usWhrWidth++;
	   /* �e�׻ݤj��10mS�~�ӻ{�߸��T�� */
	   if(usWhrWidth > 10)    
	   {	
		usWhrWidth = 0;
		WhrFlag = 1;
                
            
		/* �߸��d��: 40-240 ��l�ư� (60000/40)<usWhrCnt<(60000/240)*/
		if((usWhrCnt <= 1500) && (usWhrCnt >= 250))     
		{   
		  ucWhrTemp = 60000 / usWhrCnt;
		  usWhrCnt = 0;
		  /* �@�}�l�����o�⦸ �~�t�b+-6 ���߸��T�� */
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
			 
			 /* �~�t�p�� �p��߸� */
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
  
  /* ���S�߸� �M���Ҧ����� */
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
* Description	: ���u�߸��p��
=============================================================================*/
#if configUSE_HHR

/*
static unsigned short usHhrCnt;										//���u�߸��ɶ��p��
static unsigned short usHhrWidth;									//���u�߸��ߪi�ɶ��e��
static unsigned char ucHhrOld;										//���u�߸�
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
	
  
  /* ���u�߸��ɶ��p�� unit:1ms */
  usHhrCnt++;
  /* Ū���}���ܤ� */
  if(HAL_GPIO_ReadPin(HHR_GPIO ,HHR_PIN))
  {
    /* low too high */
    if(HhrFlag == 0)
    {
	 usHhrWidth++;
	 /* �e�׻ݤj��10mS�~�ӻ{�߸��T�� */
	 if(usHhrWidth > 10)    
	 {	
	   usHhrWidth = 0;
	   HhrFlag = 1;
	   /* �߸��d��: 40-240 ��l�ư� (60000/40)<usWhrCnt<(60000/240)*/
	   if((usHhrCnt <= 1500) && (usHhrCnt >= 250))     
	   {   
               
  
		ucHhrTemp = 60000 / usHhrCnt;
                
                
                usHhrCnt_Record[tmp_cnt] = usHhrCnt;
                usHhrVal_Record[tmp_cnt] = ucHhrTemp;
                
                tmp_cnt++;
                tmp_cnt = tmp_cnt%10;
                
                usHhrCnt = 0;
		/* �@�}�l�����o�⦸ �~�t�b+-6 ���߸��T�� */
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
		    
		    /* �~�t�p�� �p��߸� */
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
  /* ���S�߸� �M���Ҧ����� */
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
* Description	: �߸�����
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
* Description	: �߸��p��
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