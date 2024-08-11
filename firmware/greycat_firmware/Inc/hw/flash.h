#include "main.h"

// https://medium.com/teamarimac/stm32-flash-programming-3418bf09231c
// For this device a page of flash is 1kB = 1024 bytes

// Flash Generic /////////////////////////////////////////////////////////////////////////////
uint32_t flash_page_erase(uint64_t page_address, uint64_t no_pages);

// Flash Organisation ////////////////////////////////////////////////////////////////////////
#define STATE_NAME_SIZE 24
#define STATE_STRUCT_SIZE 4
typedef struct BadgeState{ // 256 bits = 64 bit * 2
  uint8_t name[STATE_NAME_SIZE];              // 22 * 8 = 192 bits
  uint64_t unlock_status; // 64 bits
} badge_state;

#define UNLOCK_BIT_REDHAT 0
#define UNLOCK_BIT_YELLOWHAT 1
#define UNLOCK_BIT_BLUEHAT 2
#define UNLOCK_BIT_PURPLEHAT 3
#define UNLOCK_BIT_ORANGEHAT 4


// Flash State Operations ///////////////////////////////////////////////////////////////////
#define STORAGE_ADDR (0x0801FC00-1024*(64+8))// last page of STM32F103C8T6
#define STORAGE_ADDR_IMAGE (0x0801FC00-1024*(64+8+1)) // last page + leave space for bootloader (8 + 1)

typedef union FlashPage1{
  uint64_t memory[STATE_STRUCT_SIZE];   // [128]; // 64 bits = 8 bytes
  badge_state state;
} flash_page_1;


badge_state flash_read_state();
void flash_write_state(badge_state st);

// Bit Shift ////////////////////////////////////////////////////////////////////////////////
uint8_t bitread(uint64_t unlock_status, uint16_t bit);
uint8_t bitset(uint64_t unlock_status, uint16_t bit);
uint8_t bitunset(uint64_t unlock_status, uint16_t bit);

// Badge State //////////////////////////////////////////////////////////////////////////////
void badge_state_usb_display();
void badge_state_reset();
void badge_state_update_name(uint8_t *new_name);
void badge_state_update_unlock_status(uint64_t new_status);

void badge_state_set_unlock_status_bit(uint64_t bit);
void badge_state_unset_unlock_status_bit(uint64_t bit);

uint8_t badge_state_read_unlock_status_bit(uint64_t bit);