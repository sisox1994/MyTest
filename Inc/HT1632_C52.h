#ifndef HT1632_C52_H_
#define HT1632_C52_H_

void LED_Matrix_Init();
void HT_Write(unsigned char Address ,unsigned char Data,unsigned char sw );
void Blink_On(unsigned char sw);
void Blink_Off(unsigned  char sw);
void LED_On(unsigned char);

#define HT_RD   0xC0    //110 0   0000
#define HT_WR   0xA0    //101 0   0000
#define HT_CMD 0x80     //100 0   0000

#define HT_SYS_DIS 0x00
#define HT_SYS_EN 0x01
#define HT_LED_OFF 0x02
#define HT_LED_ON 0x03
#define HT_BLINK_OFF 0x08
#define HT_BLINK_ON 0x09
#define HT_SLAVE_MODE 0x10
#define HT_MASTER_MODE 0x14
#define HT_INT_RC 0x18
#define HT_EXT_CLK 0x1C

#define HT_PWM_CONTROL_1 0xA0
#define HT_PWM_CONTROL_2 0xA1
#define HT_PWM_CONTROL_3 0xA2
#define HT_PWM_CONTROL_4 0xA3
#define HT_PWM_CONTROL_5 0xA4
#define HT_PWM_CONTROL_6 0xA5
#define HT_PWM_CONTROL_7 0xA6
#define HT_PWM_CONTROL_8 0xA7
#define HT_PWM_CONTROL_9 0xA8
#define HT_PWM_CONTROL_10 0xA9
#define HT_PWM_CONTROL_11 0xAA
#define HT_PWM_CONTROL_12 0xAB
#define HT_PWM_CONTROL_13 0xAC
#define HT_PWM_CONTROL_14 0xAD
#define HT_PWM_CONTROL_15 0xAE
#define HT_PWM_CONTROL_16 0xAF

#define HT_COM_8_NMOS  0x20
#define HT1632_COMMON_16NMOS  0x24
#define HT_COM_8_PMOS  0x28
#define HT1632_COMMON_16PMOS  0x2C

#endif 