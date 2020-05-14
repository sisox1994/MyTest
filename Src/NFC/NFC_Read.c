//==============================================================================
// 2020/5/8 Joe NFC Read (PN532)
// Test for AT-200 uart com port
//==============================================================================
//#include "SystemSwitch.h"
//#include "AlaSystem.h"
#include "NFC_Read.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "system.h"

//------------------------------------------------------------------------------
unsigned char b_NFCTXFlag;

UART_HandleTypeDef huart6;

//------------------------------------------------------------------------------
unsigned char ucNFCCmdCnt;
unsigned char ucNFC_Type=0;     // 0:sleep /1:Idel(scan?) /2:Password /3.Read
static unsigned char ucNFCTimmoutCnt;
unsigned char ucNFCRxAdr;
unsigned char ucNFCtxCmd;
unsigned char ucMaxNFCRxData;
unsigned char ucNFCRxDataBuf[4];
unsigned char ucNFC_TxBuf[30],ucNFC_RxBuf[30];
unsigned char ucNFC_UIDBuf[11];
unsigned char ucNFC_DataAddr;
unsigned char ucNFC_DataAddr_temp;
unsigned short usDataAddrNoChangeCnt;

unsigned char ucNFC_DataBuf[240];       // 16*15

unsigned char ucNFC_BleID[7];

//------------------------------------------------------------------------------
const unsigned char ucNFC_wake[] = {0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x03,0xFD,0xD4,0x14,0x01,0x17,0x00};
const unsigned char ucNFC_Ver[] = {0x00,0x00,0xFF,0x02,0xFE,0xD4,0x02,0x2A,0x00};
const unsigned char ucNFC_Search[] = {0x00,0x00,0xFF,0x04,0xFC,0xD4,0x4A,0x01,0x00,0xE1,0x00};
const unsigned char ucNFC_Certification[] = {0x00,0x00,0xFF,0x0F,0xF1,0xD4,0x40,0x01,0x60,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x37,0xB6,0x1D,0x3B,0x4A,0x00};
const unsigned char ucNFC_Read[] = {0x00,0x00,0xFF,0x05,0xFB,0xD4,0x40,0x01,0x30,0x02,0xB9,0x00};
const unsigned char ucNFC_Write[] = {0x00,0x00,0xFF,0x15,0xEB,0xD4,0x40,0x01,0xA0,0x02,0x67,0x50,0x57,0x87,0x66,0x48,0x48,0x50,0x48,0x49,0x48,0x49,0x48,0x57,0x00,0x00,0xAD,0x00};

const unsigned char ucChk_le[] = {0x74,0x68,0x2E,0x6C,0x65,0x2E,0x6F};
//==============================================================================
// USART1 init function
//==============================================================================



void NFC_UART6_Transmit_Test(){

    unsigned char ucTx_data[10];
    
    ucTx_data[0] = 0x78;
    ucTx_data[1] = 0x87;
    ucTx_data[2] = '\n';
      
    __HAL_UART_DISABLE_IT(&huart6, UART_IT_RXNE);    //禁止Uart 中斷收資料
    HAL_UART_Transmit(&huart6,ucTx_data, 3,5);     // Tx 資料發送
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);      //啟動Uart 中斷收資料
    
}
void F_NFC_Init(void);

void MX_NFC_UART_Init(void)
{
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  
  if(HAL_UART_Init(&huart6) != HAL_OK){
      Error_Handler();
  }
  
  HAL_UART_MspDeInit(&huart6);
  HAL_UART_MspInit(&huart6);
  
  __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
  
  NFC_UART6_Transmit_Test();
  
  F_NFC_Init();
  
  
}
// END MX_NFC_UART_Init --------------------------------------------------------
void F_NFC_Init(void)
{
  ucNFCCmdCnt = 0;
  ucNFCTimmoutCnt = 0;
  
}
// END MX_NFC_UART_Init --------------------------------------------------------

//==============================================================================
// NFC Read
//==============================================================================
//========================================================================
// NFC Send Data  (下控:F_UartSendCmd())
//========================================================================
unsigned char F_NFC_TxCheckSum(unsigned char addr1)
{
  unsigned char chk_data=0;
  unsigned char i;
  for(i=0;i<addr1;i++){
    chk_data += ucNFC_TxBuf[i+5];
  }
  return ((0xFF-chk_data)+1);
}
// END F_NFC_TxCheckSum --------------------------------------------------------
void F_NFCSendCmd(void)
{
  unsigned char uci,uclen;
  //if( R_xKEY_StopKey == 1){
    if(b_NFCTXFlag == 1){
      b_NFCTXFlag = 0;
#if 0// 先關掉無動作計數
      ucNFCTimmoutCnt++;
      if(ucNFCTimmoutCnt > 100){
        ucNFCTimmoutCnt = 0;
        HAL_UART_DeInit(&huart6);
        MX_NFC_UART_Init();
      } 
#endif 
      uclen = 0;
      for(uci = 0;uci < 30;uci++){
        ucNFC_TxBuf[uci] = 0;
      }
      switch(ucNFCCmdCnt)
      {
      case C_Wake:
           ucNFCtxCmd = 0;
           uclen = sizeof(ucNFC_wake);
           for(uci=0;uci<uclen;uci++){
             ucNFC_TxBuf[uci] = ucNFC_wake[uci];
           }
           break;
      case C_ReadVer:
           ucNFCtxCmd = 0x02;
           uclen = sizeof(ucNFC_Ver);
           for(uci=0;uci<uclen;uci++){
             ucNFC_TxBuf[uci] = ucNFC_Ver[uci];
           }
           break;
      case C_CardSearch:
           ucNFCtxCmd = 0x4A;
           uclen = sizeof(ucNFC_Search);
           for(uci=0;uci<uclen;uci++){
             ucNFC_TxBuf[uci] = ucNFC_Search[uci];
           }
           break;
      case C_Certification:
           ucNFCtxCmd = 0x40;
           uclen = sizeof(ucNFC_Certification);
           for(uci=0;uci<uclen;uci++){
             ucNFC_TxBuf[uci] = ucNFC_Certification[uci];
           }
           break;
      case C_ReadCard:
           ucNFCtxCmd = 0x40;
           uclen = sizeof(ucNFC_Read);
           for(uci=0;uci<uclen;uci++){
             ucNFC_TxBuf[uci] = ucNFC_Read[uci];
           }
           ucNFC_TxBuf[9] = (ucNFC_DataAddr/4);
           ucNFC_TxBuf[10] = F_NFC_TxCheckSum(ucNFC_TxBuf[3]);
           
           if(usDataAddrNoChangeCnt >=20){
               ucNFCCmdCnt = 0;
               usDataAddrNoChangeCnt = 0;
               ucNFC_DataAddr = 0;
                Buzzer_Set(500);
               memset(ucNFC_BleID,0x00,7);
           }
           
           if(ucNFC_DataAddr_temp != ucNFC_DataAddr){
                ucNFC_DataAddr_temp = ucNFC_DataAddr;
                usDataAddrNoChangeCnt = 0;
           }else{
                usDataAddrNoChangeCnt++;
           }
           
           break;
      case C_WritwCard:
           ucNFCtxCmd = 0x40;
           uclen = sizeof(ucNFC_Write);
           for(uci=0;uci<uclen;uci++){
             ucNFC_TxBuf[uci] = ucNFC_Write[uci];
           }
           break;
      default:
           break;
      }
      __HAL_UART_ENABLE(&huart6);
      HAL_UART_Transmit(&huart6,ucNFC_TxBuf, uclen,22);         // STM32F072內部晶振
    }
  //}else{
   // __HAL_UART_DISABLE_IT(&huart6, UART_IT_TXE);
    //__HAL_UART_DISABLE(&huart6);
  // }
}
// END F_NFCSendCmd ------------------------------------------------------------
//========================================================================
// NFC Read Data  (下控:F_UartGet())
// 3(START)+2(Num)+3(Type)+16(Data)+1(ChkSum)+1(STOP)
//========================================================================
unsigned char F_NFC_CheckSum(unsigned char addr1)
{
  unsigned char chk_data=0;
  unsigned char i;
  for(i=0;i<addr1;i++){
    chk_data += ucNFC_RxBuf[i+5];
  }
  return ((0xFF-chk_data)+1);
}
// END F_NFC_CheckSum ----------------------------------------------------------
 unsigned char idx,Adr1;	
void F_NFC_DataSearch(void)
{
  //unsigned char i,Adr1;	
  for(idx=0;idx<=224;idx++){
    if(ucNFC_DataBuf[idx]==ucChk_le[0]){
      idx++;
      if(ucNFC_DataBuf[idx]==ucChk_le[1]){
        idx++;
        if(ucNFC_DataBuf[idx]==ucChk_le[2]){
          idx++;
          if(ucNFC_DataBuf[idx]==ucChk_le[3]){
            idx++;
            if(ucNFC_DataBuf[idx]==ucChk_le[4]){
              idx++;
              if(ucNFC_DataBuf[idx]==ucChk_le[5]){
                idx++;
                if(ucNFC_DataBuf[idx]==ucChk_le[6]){
                  Adr1 = (idx+4);
                  for(idx=0;idx<10;idx++){
                    if((ucNFC_DataBuf[Adr1]==0x08)&&(ucNFC_DataBuf[Adr1+1]==0x1B)){
                      ucNFC_BleID[0] = ucNFC_DataBuf[Adr1+2];
                      ucNFC_BleID[1] = ucNFC_DataBuf[Adr1+3];
                      ucNFC_BleID[2] = ucNFC_DataBuf[Adr1+4];
                      ucNFC_BleID[3] = ucNFC_DataBuf[Adr1+5];
                      ucNFC_BleID[4] = ucNFC_DataBuf[Adr1+6];
                      ucNFC_BleID[5] = ucNFC_DataBuf[Adr1+7];
                      ucNFC_BleID[6] = ucNFC_DataBuf[Adr1+8];
                      idx = 250;
                      ucNFC_UIDBuf[10] = 0xAA;
                      ucNFC_DataAddr = 0x00;
                      
                     
                      
                      Btm_Task_Adder(Connect_Paired_NFC_BLE_HR_E2);
                      Buzzer_Set(60);
                      
                    }else{
                        if(ucNFC_DataBuf[Adr1]>0x20){
                            ucNFCCmdCnt = 0;
                            idx = 0;
                            Adr1 = 0;                            
                            memset(ucNFC_BleID,0x00,7);
                            break;
                        }else{
                            Adr1 = Adr1+(ucNFC_DataBuf[Adr1]+1);
                        }
                      
                    }
                    if(Adr1>=200) idx = 10;
                  }
                  if(idx!=250) idx = (Adr1-4);
                }
              }
            }
          }
        }
      }
    }
  }
}
// END F_NFC_ReadData ----------------------------------------------------------
void F_NFCGetCode(void)
{
  unsigned char i,chk1;	
  
  ucNFCTimmoutCnt = 0;
  if(ucNFCCmdCnt == C_Wake){
    ucNFCCmdCnt = C_CardSearch;
  }else{
    if(ucNFCCmdCnt == C_CardSearch){
      if((ucNFC_RxBuf[9] == 0x00)&&(ucNFC_RxBuf[10] == 0x44)){
        chk1 = 0;
        for(i=0;i<ucNFC_RxBuf[12];i++){
          if(ucNFC_UIDBuf[0+i] != ucNFC_RxBuf[12+i]) chk1 = 1;
        }
        if(chk1 == 1){
          for(i=0;i<ucNFC_RxBuf[12];i++){
            ucNFC_UIDBuf[0+i] = ucNFC_RxBuf[12+i];
          }
          ucNFCCmdCnt = C_ReadCard;
          ucNFC_DataAddr = 0;
          ucNFC_UIDBuf[10] = 0x00;
        }else{
            if(ucNFC_UIDBuf[10] != 0xAA){
                ucNFCCmdCnt = C_ReadCard;
                ucNFC_DataAddr = 0;
            }
        }
      }else{
        ucNFCCmdCnt = C_CardSearch;
      }
    }else{
      if(ucNFCCmdCnt == C_ReadCard){
        for(i=0;i<16;i++,ucNFC_DataAddr++){
          ucNFC_DataBuf[ucNFC_DataAddr] = ucNFC_RxBuf[i+8];
        }
        //ucNFC_DataAddr++;
        if(ucNFC_DataAddr>=224){
          F_NFC_DataSearch();
          ucNFCCmdCnt = C_CardSearch;
        }
      }else{
        if(ucNFCCmdCnt == C_WritwCard){
          //
        }else{
          if(ucNFCCmdCnt == C_Certification){
            //
          }
        }
      }
    }
  }
}
// END F_NFCGetCode ------------------------------------------------------------
void F_NFCGetCmd(void)
{
  if( HAL_UART_Receive_IT(&huart6,ucNFCRxDataBuf,1) == HAL_OK)
  {
    unsigned char chk1,chk2;
    __asm("NOP");
    ucNFC_RxBuf[ucNFCRxAdr] = ucNFCRxDataBuf[0];
    chk1 = 0;
    switch(ucNFCRxAdr)
    {
    case 0:
        if(ucNFC_RxBuf[0] != 0x00) chk1 = 0x10;
        break;
    case 1:
        if(ucNFC_RxBuf[1] != 0x00) chk1 = 0x10;
        break;
    case 2:
        if(ucNFC_RxBuf[2] != 0xFF){
            if(ucNFC_RxBuf[2] == 0x00){
                ucNFCRxAdr--;
            }else{
                chk1 = 0x10;
            }            
        }
        break;
    case 3:
        if(ucNFC_RxBuf[3] == 0){
          __asm("NOP");
        }else{
          if(ucNFC_RxBuf[3] == 1)  chk1 = 0x10;
        }
        break;
    case 4:
        if(ucNFC_RxBuf[3] == 0){
          __asm("NOP");
        }else{
          chk2 = (0xFF-ucNFC_RxBuf[3])+1;
          if(ucNFC_RxBuf[4] == chk2){
            ucMaxNFCRxData = (5+ucNFC_RxBuf[3]+1);
          }else
            chk1 = 0x10;
        }
        break;
    case 5:
        if(ucNFC_RxBuf[3] == 0){
          chk1 = 0x10;
        }else{
          if(ucNFC_RxBuf[5] != 0xD5) chk1 = 0x10;
        }
        break;
    case 6:
        if(ucNFC_RxBuf[3] > 2){
          if(ucNFC_RxBuf[6] != (ucNFCtxCmd+1)) chk1 = 0x10;
        }else{
          if(ucNFC_RxBuf[6] != 0x15) chk1 = 0x10;
        }
        break;
    case 7:
        if(ucNFC_RxBuf[6] == 0x15){
          if(ucNFC_RxBuf[7] == F_NFC_CheckSum(ucNFC_RxBuf[3]))
            chk1 = 0x01;
          else
            chk1 = 0x10;
        }
        break;
    case 8:
    default:
        if(ucNFC_RxBuf[5] == 0x15){
          if(ucNFC_RxBuf[8] == 0){
            chk1 = 0xAA;        // End
          }
        }else{
          if(ucNFCRxAdr == ucMaxNFCRxData){
            if(ucNFC_RxBuf[ucNFCRxAdr] == 0)
              chk1 = 0xAA;      // End
          }else{
            if(ucNFCRxAdr == (ucMaxNFCRxData-1)){
              if(ucNFC_RxBuf[ucNFCRxAdr] == F_NFC_CheckSum(ucNFC_RxBuf[3]))
                chk1 = 0x01;
              else
                chk1 = 0x10;    // Err
            }else{
              if(ucNFCRxAdr > ucMaxNFCRxData)
                chk1 = 0x10;    // Err
            }
          }
        }
        break;
    }
    //------------------------------
    if(chk1 <= 1){
      ucNFCRxAdr++;     // Read
    }else{
      if(chk1==0xAA){
        F_NFCGetCode();
      }
      ucNFCRxAdr = 0;   // End or Error
    }
    //------------------------------
  }
}
// END F_NFCGetCmd -------------------------------------------------------------

