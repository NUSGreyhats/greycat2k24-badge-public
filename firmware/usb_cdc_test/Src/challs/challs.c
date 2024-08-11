#include "hw/usb_cdc_comms.h"
#include "hw/flash.h"
#include "gpio.h"
#include "rtc.h"

/// Defunct ///////////////////////////////////////////////////////////////////////////////////////
void chall_logic_read_once(){
    int tick = HAL_GetTick();
    int value = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
    usbPrintf(MAX_BUF_LEN, "%d,%d,%d,%d,%d\n", 
        tick, 
        HAL_GPIO_ReadPin(GPIOB, BTN1_Pin),
        HAL_GPIO_ReadPin(GPIOB, BTN2_Pin),
        HAL_GPIO_ReadPin(GPIOB, BTN3_Pin),
        HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)
    );
}

void chall_logic_read(){
    while (1) {
        chall_logic_read_once();
    }
}



/// OSINT ////////////////////////////////////////////////////////////////////////////////////////

int qna(char *display, char *ans){
  fastPrintString(display);
  fastPrintString(" ");

  char input_ans[MAX_BUF_LEN+1];
  int ans_len = readString(input_ans, MAX_BUF_LEN);
  input_ans[ans_len] = '\0';
  fastPrintString(input_ans);

  if (strcmp(input_ans, ans) == 0){
      fastPrintString(" - Correct");
      return 1;
  }
  fastPrintString(" - Wrong");
  return 0;
}


int chall_qna(){
  HAL_Delay(1000);
  fastPrintString("Mini Trivia Quiz > \nWe'll be asking you a few simple questions relating to this board.\nPlease answer all in lowercase:\n—-----------------------------------------------------------------");
  if (
    qna("\nWhat is the microcontroller (MCU) chip used?", "stm32f103c8t6") &&
    qna("\nWhat is a popular development board using this MCU?", "bluepill") &&
    qna("\nWhat is the smallest SMD size of a resistor on the board?", "0603") &&
    qna("\nWhat company provides manufacturing of PCBs from $2 (please sponsor us)?", "jlcpcb") &&
    qna("\nWhat does HAL stand for?", "hardware abstraction layer") &&
    qna("\nWhat is the logic level (voltage) used in this board?", "3.3v") &&
    qna("\nWhat usb class are you using to communicate to this board?", "communications device class")
  ) {
    fastPrintString("\nCorrect! Here you get a flag -> grey{STM32_ExP3R1}\n");
    badge_state_set_unlock_status_bit(UNLOCK_BIT_YELLOWHAT);
  } else {
    fastPrintString("\nTry again\n");
  }
}

/// Glitch ///////////////////////////////////////////////////////////////////////////////////////
void chall_hyper_glitch(){
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == 0){
        usbPrintf(MAX_BUF_LEN, "grey{d3Bu9_P4D5}");
        badge_state_set_unlock_status_bit(UNLOCK_BIT_REDHAT); 
    }else{
        usbPrintf(MAX_BUF_LEN, "Connect PB15 to GND");
    }
}


/// Name ///////////////////////////////////////////////////////////////////////////////////////
#define NAME_FLAG_LEN 30
int name_flag_arr1[NAME_FLAG_LEN] = {16, 76, 2, 53, 91, 32, 49, 28, 4, 78, 5, 11, 52, 67, 13, 20, 8, 8, 17, 81, 58, 74, 65, 50, 38, 100, 99, 87, 6, 61};
int name_flag_arr2[NAME_FLAG_LEN] = {119, 62, 103, 76, 32, 89, 94, 105, 91, 45, 100, 101, 107, 51, 127, 125, 102, 124, 119, 14, 85, 36, 30, 65, 82, 9, 80, 101, 57, 64};
void chall_name(){
    // Generate Flag
    volatile char flag[NAME_FLAG_LEN+1]; // = "grey{you_can_printf_on_stm32?}";
    memset(flag, 0, NAME_FLAG_LEN+1);
    for (int i=0; i<NAME_FLAG_LEN; i++){
        flag[i] = name_flag_arr1[i] ^ name_flag_arr2[i];
    }

    usbPrintf(MAX_BUF_LEN, "\nEnter your name> ");
    
    char input_ans[STATE_NAME_SIZE];
    memset(input_ans, 0, STATE_NAME_SIZE);
    int ans_len = readString(input_ans, STATE_NAME_SIZE-1);
    input_ans[ans_len] = '\0';
    
    usbPrintf(MAX_BUF_LEN, input_ans, flag);
    usbPrintf(MAX_BUF_LEN, "\n");
    
    if (strstr(input_ans, "%s") != NULL) {
        badge_state_set_unlock_status_bit(UNLOCK_BIT_BLUEHAT);
    }

    oled_update_name(input_ans);
    badge_state_update_name(input_ans);

    usbPrintf(MAX_BUF_LEN, "Updated!\n");
}

/// UART ///////////////////////////////////////////////////////////////////////////////////////
#define UART_FLAG_LEN 15
int uart_flag_arr1[UART_FLAG_LEN] = {88, 39, 50, 39, 81, 65, 25, 66, 3, 35, 22, 40, 36, 38, 39};
int uart_flag_arr2[UART_FLAG_LEN] = {63, 85, 87, 94, 42, 52, 120, 48, 119, 124, 100, 77, 69, 66, 90};

UART_HandleTypeDef huart1;
void chall_send_uart(){
    //uint8_t *data = "grey{uart_read}\0";
    uint8_t data[UART_FLAG_LEN+1];
    for (int i=0; i<UART_FLAG_LEN; i++){
        data[i] = uart_flag_arr1[i] ^ uart_flag_arr2[i];
    }
    data[UART_FLAG_LEN] = '\0';
    HAL_UART_Transmit(&huart1, data, UART_FLAG_LEN, 200);
    badge_state_set_unlock_status_bit(UNLOCK_BIT_ORANGEHAT);
}

/// Bootloader /////////////////////////////////////////////////////////////////////////////////
#define CMD_BOOT 0x4F42UL

#define BOOT_FLAG_LEN 30
// grey{what_flag_just_boot1_bro}
uint8_t boot_flag_array1[BOOT_FLAG_LEN] = {20, 99, 10, 94, 20, 9, 46, 98, 9, 7, 57, 56, 30, 100, 86, 88, 48, 41, 56, 72, 31, 78, 26, 9, 4, 86, 14, 54, 100, 34};
uint8_t boot_flag_array2[BOOT_FLAG_LEN] = {115, 17, 111, 39, 111, 126, 70, 3, 125, 88, 95, 84, 127, 3, 9, 50, 69, 90, 76, 23, 125, 33, 117, 125, 53, 9, 108, 68, 11, 95};
void chall_bootloader(){
    usbPrintf(MAX_BUF_LEN, "\nEnter flag to reboot into bootloader> ");

    uint8_t input_ans[BOOT_FLAG_LEN+1];
    memset(input_ans, 0, BOOT_FLAG_LEN+1);
    uint8_t ans_len = readString(input_ans, BOOT_FLAG_LEN);
    usbPrintf(MAX_BUF_LEN, "%s\n", input_ans);

    if (ans_len != BOOT_FLAG_LEN){
        usbPrintf(MAX_BUF_LEN, "WRONG!!!");
        return;
    }

    for (int i=0; i<BOOT_FLAG_LEN; i++){
        if ((input_ans[i] ^ boot_flag_array1[i]) != boot_flag_array2[i]){
            usbPrintf(MAX_BUF_LEN, "WRONG!!!");
            return;
        }
    }

    // Reboot into bootloader
    badge_state_set_unlock_status_bit(UNLOCK_BIT_PURPLEHAT);
    
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, CMD_BOOT);
    usbPrintf(MAX_BUF_LEN, "Success - Press reset to reboot into bootloader!!!");
    //usbPrintf(MAX_BUF_LEN, " - bootloader not implemented yet\n");
    ans_len = readString(input_ans, BOOT_FLAG_LEN);
}

// https://github.com/yohanes-erwin/stm32f103-sw4stm32/blob/master/stm32f103-hal-rtc-backup-register/src/main.c