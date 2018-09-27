/*
 * COPYRIGHT (c) 2010-2014 MACRONIX INTERNATIONAL CO., LTD
 * SPI Flash Low Level Driver (LLD) Sample Code
 *
 * Flash device information define
 *
 * $Id: MX25_DEF.h,v 1.354 2014/10/21 03:17:29 mxclldb1 Exp $
 */

#ifndef    __MX25_DEF_H__
#define    __MX25_DEF_H__
/*
  Compiler Option
*/

/* Select your flash device type */
#define MX25L3233F


/* Note:
   Synchronous IO     : MCU will polling WIP bit after
                        sending prgram/erase command
   Non-synchronous IO : MCU can do other operation after
                        sending prgram/erase command
   Default is synchronous IO
*/
//#define    NON_SYNCHRONOUS_IO

/*
  Type and Constant Define
*/

// define type
typedef    unsigned long     uint32;
typedef    unsigned int      uint16;
typedef    unsigned char     uint8;
typedef    unsigned char     BOOL;

// variable
#define    TRUE     1
#define    FALSE    0
#define    BYTE_LEN          8
#define    IO_MASK           0x80
#define    HALF_WORD_MASK    0x0000ffff

/*
  Flash Related Parameter Define
*/

#define    Block_Offset       0x10000     // 64K Block size
#define    Block32K_Offset    0x8000      // 32K Block size
#define    Sector_Offset      0x1000      // 4K Sector size
#define    Page_Offset        0x0100      // 256 Byte Page size
#define    Page32_Offset      0x0020      // 32 Byte Page size (some products have smaller page size)
#define    Block_Num          (FlashSize / Block_Offset)

// Flash control register mask define
// status register
#define    FLASH_WIP_MASK         0x01
#define    FLASH_LDSO_MASK        0x02
#define    FLASH_QE_MASK          0x40
// security register
#define    FLASH_OTPLOCK_MASK     0x03
#define    FLASH_4BYTE_MASK       0x04
#define    FLASH_WPSEL_MASK       0x80
// configuration reigster
#define    FLASH_DC_MASK          0x80
#define    FLASH_DC_2BIT_MASK     0xC0
// other
#define    BLOCK_PROTECT_MASK     0xff
#define    BLOCK_LOCK_MASK        0x01

/*
  System Information Define
*/

//--- insert your MCU information ---//
#define    CLK_PERIOD             	20    	// unit: ns
#define    Min_Cycle_Per_Inst   	12  		// cycle count of one instruction
#define    One_Loop_Inst       	8		// instruction count of one loop (estimate)


/*
  Flash ID, Timing Information Define
  (The following information could get from device specification)
*/

#ifdef MX25L3233F
#define    FlashID          0xc22016
#define    ElectronicID     0x15
#define    RESID0           0xc215
#define    RESID1           0x15c2
#define    FlashSize        0x400000       // 4 MB
#define    CE_period        15625000       // tCE /  ( CLK_PERIOD * Min_Cycle_Per_Inst *One_Loop_Inst)
#define    tW               40000000       // 40ms
#define    tDP              10000          // 10us
#define    tBP              50000          // 50us
#define    tPP              1200000        // 1.2ms
#define    tSE              200000000      // 200ms
#define    tBE              1000000000     // 1s
#define    tBE32            600000000      // 0.6s
#define    tPUW             10000000       // 10ms
#define    tWSR             1000000        // 1ms
// Support I/O mode
#define    SIO              0
#define    DIO              1
#define    QIO              2

#define    SUPPORT_RDCR     1
#define    SUPPORT_WRSR_CR  1
#define    SUPPORT_CR_DC    1

#endif

// dummy cycle configration
#ifdef DUMMY_CONF_2

#define    DUMMY_CONF_2READ       0x08040804
#define    DUMMY_CONF_4READ       0x0A080406
#define    DUMMY_CONF_FASTREAD    0x08080808
#define    DUMMY_CONF_DREAD       0x08080808
#define    DUMMY_CONF_QREAD       0x08080808

#else

#define    DUMMY_CONF_2READ       0x0A080604
#define    DUMMY_CONF_4READ       0x0A080406
#define    DUMMY_CONF_FASTREAD    0x0A080608
#define    DUMMY_CONF_DREAD       0x0A080608
#define    DUMMY_CONF_QREAD       0x0A080608

#endif

// Flash information define
#define    WriteStatusRegCycleTime     tW / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    PageProgramCycleTime        tPP / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    SectorEraseCycleTime        tSE / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    BlockEraseCycleTime         tBE / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    ChipEraseCycleTime          CE_period
#define    FlashFullAccessTime         tPUW / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)

#ifdef tBP
#define    ByteProgramCycleTime        tBP / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#ifdef tWSR
#define    WriteSecuRegCycleTime       tWSR / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#ifdef tBE32
#define    BlockErase32KCycleTime      tBE32 / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#ifdef tWREAW
#define    WriteExtRegCycleTime        tWREAW / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#endif    /* end of __MX25_DEF_H__  */

