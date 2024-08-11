#include "hw/flash.h"
#include "hw/usb_cdc_comms.h"

uint32_t flash_page_erase(uint64_t page_address, uint64_t no_pages){
  
   //Instantiate the FLASH_EraseInitTypeDef struct needed for the HAL_FLASHEx_Erase() function
  FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {0};

  FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;    //Erase type set to erase pages( Available other type is mass erase)
  FLASH_EraseInitStruct.PageAddress = page_address;            //Starting address of flash page (0x0800 0000 - 0x0801 FC00)
  FLASH_EraseInitStruct.NbPages = no_pages;                    //The number of pages to be erased                  
  uint32_t  errorStatus = 0;
  if (HAL_FLASHEx_Erase(&FLASH_EraseInitStruct,&errorStatus) != HAL_OK)
  {
    /*Error occurred while page erase.*/
    return HAL_FLASH_GetError();
  }
  return 0;
}

// Flash Operations ///////////////////////////////////////////////////////////////////////////////////
badge_state flash_read_state(){
  badge_state *badge_state_addr = (badge_state *) STORAGE_ADDR;
  return *badge_state_addr;
}
void flash_write_state(badge_state st){
  flash_page_1 page;
  page.state = st;
  
  HAL_FLASH_Unlock();
  flash_page_erase(STORAGE_ADDR, 1);

  for (int offset = 0; offset < STATE_STRUCT_SIZE; offset++) {
    HAL_FLASH_Program(
      FLASH_TYPEPROGRAM_DOUBLEWORD, 
      STORAGE_ADDR + (offset*8), // 8 for double word 
      page.memory[offset]
    );
  }

  HAL_FLASH_Lock();
}

void flash_write_image_usb(){  
  HAL_FLASH_Unlock();
  flash_page_erase(STORAGE_ADDR_IMAGE, 1);

  for (int offset = 0; offset < 128; offset++) {
    uint64_t column_state = readInteger();
    usbPrintf(MAX_BUF_LEN, "%d %ld\n", offset,column_state);
    while (HAL_OK != HAL_FLASH_Program(
      FLASH_TYPEPROGRAM_DOUBLEWORD, 
      STORAGE_ADDR_IMAGE + (offset*8), // 8 for double word 
      column_state
    )){
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    };
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  }
  HAL_FLASH_Lock();
}

// Bit Shift /////////////////////////////////////////////////////////////////////////////////////////
uint8_t bitread(uint64_t unlock_status, uint16_t bit){
  return (unlock_status & (1 << bit)) != 0;
}

uint8_t bitset(uint64_t unlock_status, uint16_t bit){
  return (unlock_status | (1 << bit));
}

uint8_t bitunset(uint64_t unlock_status, uint16_t bit){
  return (unlock_status & ~(1 << bit));
}

// Badge State ///////////////////////////////////////////////////////////////////////////////////////
void badge_state_usb_display(badge_state st){
  usbPrintf(MAX_BUF_LEN, "BadgeState:\n");
  usbPrintf(MAX_BUF_LEN, "name: %s\n", st.name);
  usbPrintf(MAX_BUF_LEN, "unlock_status: %d\n", st.unlock_status);
}

void badge_state_reset(){
  badge_state st = {
    "GreyCat\0", //"QWERTYUIOPASDFGHJKLZXCA\0", 
    0
  }; 
  flash_write_state(st);
}

void badge_state_update_name(uint8_t new_name[STATE_NAME_SIZE]){
  badge_state st = flash_read_state();
  strcpy(st.name, new_name);
  flash_write_state(st);
}

void badge_state_update_unlock_status(uint64_t new_status){
  badge_state st = flash_read_state();
  st.unlock_status = new_status;
  flash_write_state(st);
}

void badge_state_set_unlock_status_bit(uint64_t bit){
  badge_state st = flash_read_state();
  st.unlock_status = bitset(st.unlock_status, bit);
  flash_write_state(st);
}

void badge_state_unset_unlock_status_bit(uint64_t bit){
  badge_state st = flash_read_state();
  st.unlock_status = bitunset(st.unlock_status, bit);
  flash_write_state(st);
}

uint8_t badge_state_read_unlock_status_bit(uint64_t bit){
  badge_state st = flash_read_state();
  return bitread(st.unlock_status, bit);
}



