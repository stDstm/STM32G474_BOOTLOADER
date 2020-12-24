/**
  ******************************************************************************
  * @file           : bootloader.c
  * @brief          : Functions related to bootloader
  * ----------------------------------------------------------------------------
  * @author         : Sebastian Popa
  * @date           : Dec, 2020
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bootloader.h"

/* Private variables ---------------------------------------------------------*/


/* Functions declaration- ----------------------------------------------------*/
/**
  * @brief  Jumps to the user application located at a specified memory location.
  *         Gets the user app Main Stack Pointer (MSP) and the address of the
  *         user app reset_handler() function.
  *         Calls user app reset_handler() to start execution of user app.
  * @param  None
  *         TODO: add input paramter of type uint32_t for memory address.
  * @retval None
  */
void bootloader_JumpToUserApp(void)
{
	// start of user flash
	uint32_t USER_FLASH_ADDRESS = 0x08008000U;

	// define a function pointer to user reset handler
	void (*user_reset_handler)(void);

	// set the MSP
	// MSP located at start of user flash (@0x0800 8000)
	uint32_t user_msp_value = *((volatile uint32_t *) USER_FLASH_ADDRESS);
	__set_MSP(user_msp_value);

	// reset handler address is the next location (@ 0x0800 8004)
	uint32_t user_reset_handler_address = *((volatile uint32_t*) (USER_FLASH_ADDRESS + 4U));
	user_reset_handler = (void*) user_reset_handler_address; // cast to function pointer

	// call of user_reset handler starts execution of user app
	user_reset_handler();
}

/**
  * @brief	This function erases Bank2 of the memory. (page 128 - page 255)
  * @param 	None
  * @retval	None
  */
void bootloader_FlashEraseBank2(void)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t BankNumber = 2;
	// uint32_t PageNumber;
	uint32_t PageError;
	//uint32_t src_addr = (uint32_t)DATA_64;

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_MASSERASE;
	EraseInitStruct.Banks = BankNumber;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
}

/**
  * @brief	This function writes 2KB of data starting from a given address
  * @param	StartAddress is the starting address from where to start the flash write
  * @param	DATA_64 is a pointer to an array of data to be written of size (32 x 64bit)
  * @retval	None
  */
void bootloader_FlashWrite(uint32_t StartAddress, uint64_t *DATA_64)
{
	HAL_FLASH_Unlock();

	// Delete memory bank 2 in order to be able to write (pg 128 - pg 255)
	bootloader_FlashEraseBank2();

	/* HAL_FLASH_Program() requires the address of the data to be flashed in u_integer
	 * format (e.g. 0x2000008) and not pointer format. So we need to do the following
	 * type casting.
	 */
	uint32_t data_address_uint = (uint32_t)DATA_64; // stores the address in integer format
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST, StartAddress, (uint64_t)data_address_uint);

	HAL_FLASH_Lock();
}