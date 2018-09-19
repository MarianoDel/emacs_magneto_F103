#ifndef FLASH_PROGRAM_H_
#define FLASH_PROGRAM_H_

#include "GTK_Estructura.h"
#include "stm32f10x.h"

#define SIZE_SLOT	82
#define SLOT_BASE_ADDR (uint32_t)0x08060000
#define SLOT1_START_ADDR 	(SLOT_BASE_ADDR) //84 bytes.
#define SLOT2_START_ADDR 	(SLOT_BASE_ADDR + 1 * SIZE_SLOT) //84 bytes.
#define SLOT3_START_ADDR 	(SLOT_BASE_ADDR + 2 * SIZE_SLOT) //84 bytes.
#define SLOT4_START_ADDR 	(SLOT_BASE_ADDR + 3 * SIZE_SLOT) //84 bytes.
#define SLOT5_START_ADDR 	(SLOT_BASE_ADDR + 4 * SIZE_SLOT) //84 bytes.
#define SLOT6_START_ADDR 	(SLOT_BASE_ADDR + 5 * SIZE_SLOT) //84 bytes.
#define SLOT7_START_ADDR 	(SLOT_BASE_ADDR + 6 * SIZE_SLOT) //84 bytes.
#define SLOT8_START_ADDR 	(SLOT_BASE_ADDR + 7 * SIZE_SLOT) //84 bytes.
#define SLOT9_START_ADDR 	(SLOT_BASE_ADDR + 8 * SIZE_SLOT) //84 bytes.
#define SLOT10_START_ADDR 	(SLOT_BASE_ADDR + 9 * SIZE_SLOT) //84 bytes.
#define SLOT11_START_ADDR 	(SLOT_BASE_ADDR + 10 * SIZE_SLOT) //84 bytes.
#define SLOT12_START_ADDR 	(SLOT_BASE_ADDR + 11 * SIZE_SLOT) //84 bytes.
#define SLOT13_START_ADDR 	(SLOT_BASE_ADDR + 12 * SIZE_SLOT) //84 bytes.
#define SLOT14_START_ADDR 	(SLOT_BASE_ADDR + 13 * SIZE_SLOT) //84 bytes.
#define SLOT15_START_ADDR 	(SLOT_BASE_ADDR + 14 * SIZE_SLOT) //84 bytes.
#define SLOT16_START_ADDR 	(SLOT_BASE_ADDR + 15 * SIZE_SLOT) //84 bytes.


//desde que saque la libST
typedef enum
{ 
  FLASH_BUSY = 1,
  FLASH_ERROR_PG,
  FLASH_ERROR_WRP,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;

/** @defgroup FLASH_Flags 
  * @{
  */
#ifdef STM32F10X_XL
#define FLASH_FLAG_BANK2_BSY                 ((uint32_t)0x80000001)  /*!< FLASH BANK2 Busy flag */
#define FLASH_FLAG_BANK2_EOP                 ((uint32_t)0x80000020)  /*!< FLASH BANK2 End of Operation flag */
#define FLASH_FLAG_BANK2_PGERR               ((uint32_t)0x80000004)  /*!< FLASH BANK2 Program error flag */
#define FLASH_FLAG_BANK2_WRPRTERR            ((uint32_t)0x80000010)  /*!< FLASH BANK2 Write protected error flag */

#define FLASH_FLAG_BANK1_BSY                 FLASH_FLAG_BSY       /*!< FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP                 FLASH_FLAG_EOP       /*!< FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_PGERR               FLASH_FLAG_PGERR     /*!< FLASH BANK1 Program error flag */
#define FLASH_FLAG_BANK1_WRPRTERR            FLASH_FLAG_WRPRTERR  /*!< FLASH BANK1 Write protected error flag */

#define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /*!< FLASH Busy flag */
#define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /*!< FLASH End of Operation flag */
#define FLASH_FLAG_PGERR               ((uint32_t)0x00000004)  /*!< FLASH Program error flag */
#define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /*!< FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR              ((uint32_t)0x00000001)  /*!< FLASH Option Byte error flag */
 
#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0x7FFFFFCA) == 0x00000000) && ((FLAG) != 0x00000000))
#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_EOP) || \
                                  ((FLAG) == FLASH_FLAG_PGERR) || ((FLAG) == FLASH_FLAG_WRPRTERR) || \
                                  ((FLAG) == FLASH_FLAG_OPTERR)|| \
                                  ((FLAG) == FLASH_FLAG_BANK1_BSY) || ((FLAG) == FLASH_FLAG_BANK1_EOP) || \
                                  ((FLAG) == FLASH_FLAG_BANK1_PGERR) || ((FLAG) == FLASH_FLAG_BANK1_WRPRTERR) || \
                                  ((FLAG) == FLASH_FLAG_BANK2_BSY) || ((FLAG) == FLASH_FLAG_BANK2_EOP) || \
                                  ((FLAG) == FLASH_FLAG_BANK2_PGERR) || ((FLAG) == FLASH_FLAG_BANK2_WRPRTERR))
#else
#define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /*!< FLASH Busy flag */
#define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /*!< FLASH End of Operation flag */
#define FLASH_FLAG_PGERR               ((uint32_t)0x00000004)  /*!< FLASH Program error flag */
#define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /*!< FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR              ((uint32_t)0x00000001)  /*!< FLASH Option Byte error flag */

#define FLASH_FLAG_BANK1_BSY                 FLASH_FLAG_BSY       /*!< FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP                 FLASH_FLAG_EOP       /*!< FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_PGERR               FLASH_FLAG_PGERR     /*!< FLASH BANK1 Program error flag */
#define FLASH_FLAG_BANK1_WRPRTERR            FLASH_FLAG_WRPRTERR  /*!< FLASH BANK1 Write protected error flag */
 
#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCA) == 0x00000000) && ((FLAG) != 0x00000000))
#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_EOP) || \
                                  ((FLAG) == FLASH_FLAG_PGERR) || ((FLAG) == FLASH_FLAG_WRPRTERR) || \
								  ((FLAG) == FLASH_FLAG_BANK1_BSY) || ((FLAG) == FLASH_FLAG_BANK1_EOP) || \
                                  ((FLAG) == FLASH_FLAG_BANK1_PGERR) || ((FLAG) == FLASH_FLAG_BANK1_WRPRTERR) || \
                                  ((FLAG) == FLASH_FLAG_OPTERR))
#endif

/*#define SLOT1_START_ADDR 	0x8060000
#define SLOT2_START_ADDR 	0x8060060
#define SLOT3_START_ADDR 	0x80600C0
#define SLOT4_START_ADDR 	0x8060120
#define SLOT5_START_ADDR 	0x8060180
#define SLOT6_START_ADDR 	0x80601E0
#define SLOT7_START_ADDR 	0x8060240
#define SLOT8_START_ADDR 	0x80602A0
#define SLOT9_START_ADDR 	0x8060300
#define SLOT10_START_ADDR 	0x8060360
#define SLOT11_START_ADDR 	0x80603C0
#define SLOT12_START_ADDR 	0x8060420
#define SLOT13_START_ADDR 	0x8060480
#define SLOT14_START_ADDR 	0x80604E0
#define SLOT15_START_ADDR 	0x8060540
#define SLOT16_START_ADDR 	0x80605A0
*/

char FLASH_Program(session_typedef * pslot, unsigned char slot);
void saveData(char * pData, char fin, uint32_t addInicial);
void saveData2(unsigned int * pMemory, session_typedef * pslot, unsigned int start_addr, unsigned char slot);
void checkData(char * pData, char fin, uint32_t addInicial);
void checkData2(unsigned int * pData, uint32_t addInicial);
unsigned char readDataFromFlash(session_typedef * pslot, unsigned char slot);

//desde que saque libST
void FLASH_UnlockBank1(void);
void FLASH_ClearFlag(uint32_t);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
void FLASH_LockBank1(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FLASH_Status FLASH_GetBank1Status(void);

#endif
