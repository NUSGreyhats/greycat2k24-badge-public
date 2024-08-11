#include "main.h"
#include "hw/led.h"
#include "hw/oled.h"
#include "hw/flash.h"
#include "hw/usb_cdc_comms.h"

// Hat Setup ////////////////////////////////////
uint8_t badge_hat_mode;
void badge_hat_init(){
    led_hat_init();
    badge_hat_mode = 0;
    badge_hat_mode_to_colour();
}

#define NO_HAT_COLOURS 7
// 1st 3 ints is RGB, next int is related state bit position (1-indexed)
uint8_t hat_colours[NO_HAT_COLOURS][4] = {
    {  0,  0,  0, 0},
    {  0, 64,  0, 0},
    { 64, 64,  0, 2}, // Yellow
    { 64,  0,  0, 1},
    {  0,  0, 64, 3},
    { 64, 32,  0, 5}, // Orange
    {  64, 0, 64, 4}  // Purple
    //{  0, 64, 64, 0},
    //{ 64, 64, 64, 0}
};

void badge_hat_mode_to_colour(){
    // Iterate through available options
    for (int i=0; i < NO_HAT_COLOURS; i++){
        uint8_t bitmode = hat_colours[badge_hat_mode][3];
        if (bitmode == 0){break;} // WILL BE HIT - because hardocded
        if (badge_state_read_unlock_status_bit(bitmode-1)){ break; } // unlocked
        badge_hat_mode = (badge_hat_mode+1) % NO_HAT_COLOURS; // else iterate
    }

    // Display on Hat
    led_hat(
        hat_colours[badge_hat_mode][0],
        hat_colours[badge_hat_mode][1],
        hat_colours[badge_hat_mode][2]
    );
}

void badge_hat_next_mode(){
    badge_hat_mode = (badge_hat_mode+1) % NO_HAT_COLOURS;
    badge_hat_mode_to_colour();
}

// Blush ///////////////////////////////////////////////////////////////////////////
uint8_t badge_blush_mode = 0;

void badge_blush_mode_to_led(){
    if (badge_blush_mode % 2 == 0){
        led_cheek_l(0);
        led_cheek_r(0);
    }else {
        led_cheek_l(65535);
        led_cheek_r(65535);
    }
    
    if (badge_blush_mode / 2 == 0){
        oled_update_name((char *)flash_read_state().name);
    }else {
        badge_oled_display_image();
    }
}

void badge_blush_init_mode(){
    badge_blush_mode = 0;
    badge_blush_mode_to_led();
}

void badge_blush_next_mode(){
    badge_blush_mode = (badge_blush_mode + 1) % 4;
    badge_blush_mode_to_led();
}

void badge_blush_only_next_mode(){
    badge_blush_mode = (badge_blush_mode + 1) % 4;
    if (badge_blush_mode % 2 == 0){
        led_cheek_l(0);
        led_cheek_r(0);
    }else {
        led_cheek_l(65535);
        led_cheek_r(65535);
    }
}
// OLED display from memory ///////////////////////////////////////////////////////
void badge_oled_display_image(){
    for (uint8_t col=0; col<128; col++){
        uint64_t *col_addr = (uint64_t *) (STORAGE_ADDR_IMAGE + col*8);
        uint64_t value = *col_addr;
        //usbPrintf(MAX_BUF_LEN, "value = %d\n", value);
        for (uint8_t row=0; row<64; row++){
            ssd1306_DrawPixel(col, row, value % 2 == 1 ? White : Black);
            value /= 2;
        }
    }
    ssd1306_UpdateScreen(&hi2c2);
}

// Badge Setup /////////////////////////////////////////////////////////////////////
void badge_init(){
  badge_hat_init();
  badge_blush_init_mode();
  oled_init();
  oled_update_name((char *)flash_read_state().name);
}

// Badge Control /////////////////////////////////////////////////////////////////////
uint8_t button_state[3] = {1, 1, 1};
uint8_t button_pressed(uint64_t port, uint16_t pin, uint8_t index){
    uint8_t prev_state = button_state[index];
    button_state[index] = HAL_GPIO_ReadPin(port, pin); 
    return (prev_state == 1 && button_state[index] == 0); // Trigger on falling edge
}

// Badge OLED Menu /////////////////////////////////////////////////////////////////////
#define MENU_OPTIONS 7
void menu_badge_oled_display(uint8_t selection){
    char *menuItems[MENU_OPTIONS] = {
        "1. USB (Challs)",
        "2. Cycle Hat LEDs", 
        "3. Toggle Blush",
        "4. Display Name",
        "5. Display Image",
        "6. SpamGame", 
        "7. Pong"
    };
    // Write data to local screenbuffer
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Menu:", Font_7x10, White);
    uint8_t selection_page = selection / 4;
    for (uint8_t i=selection_page*4; i<MIN(selection_page*4+4, MENU_OPTIONS); i++){
        ssd1306_SetCursor(10, 12 *(i - (selection_page*4) + 1));
        ssd1306_WriteString(menuItems[i], Font_7x10, White);
    }
    ssd1306_SetCursor(0, 12*(selection%4+1));
    ssd1306_WriteString(">", Font_7x10, White);
    // Copy all data from local screenbuffer to the screen
    ssd1306_UpdateScreen(&hi2c2);
}

void menu_badge_oled_usb_screen(){
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Running USB Menu/", Font_7x10, White);
    ssd1306_SetCursor(0, 12);
    ssd1306_WriteString("Challs", Font_7x10, White);
    ssd1306_SetCursor(0, 12*3);
    ssd1306_WriteString("Check your Comp.", Font_7x10, White);
    ssd1306_SetCursor(0, 12*4);
    ssd1306_WriteString("RST to return", Font_7x10, White);
    ssd1306_UpdateScreen(&hi2c2);
}
#define MENU_OLED_DEBOUNCE 100
void menu_badge_oled(){
  int selection = 0;
  menu_badge_oled_display(selection);
  HAL_Delay(MENU_OLED_DEBOUNCE);
  while (1){
    // Select Mode 
    if (button_pressed(GPIOB, BTN1_Pin, 0)) { 
        if (selection == 0) {selection = MENU_OPTIONS - 1;}
        else {selection--;}
        HAL_Delay(MENU_OLED_DEBOUNCE);
        menu_badge_oled_display(selection);
    }
    if (button_pressed(GPIOB, BTN2_Pin, 1)) { 
        selection = (selection + 1) % MENU_OPTIONS;
        HAL_Delay(MENU_OLED_DEBOUNCE);
        menu_badge_oled_display(selection);
    }
    // Modes
    if (button_pressed(GPIOB, BTN3_Pin, 2)) {
        HAL_Delay(MENU_OLED_DEBOUNCE);
        if (selection == 0){
            menu_badge_oled_usb_screen();
            menu_usb();
            menu_badge_oled_display(selection);
        } 
        else if (selection == 1){ badge_hat_next_mode(); }
        else if (selection == 2){ badge_blush_only_next_mode();}
        else if (selection == 3){ 
            oled_update_name((char *)flash_read_state().name);
            while (!button_pressed(GPIOB, BTN3_Pin, 2)){}
            menu_badge_oled_display(selection);
        } else if (selection == 4){ 
            badge_oled_display_image();
            while (!button_pressed(GPIOB, BTN3_Pin, 2)){}
            menu_badge_oled_display(selection);
        } else if (selection == 5) {
            spamgame();
            menu_badge_oled_display(selection);
        } else if (selection == 6) {
            pong();
        }
    }
  }
}

void menu_badge(){
  while (1){
    if (button_pressed(GPIOB, BTN3_Pin, 2)) { break;}
    if (button_pressed(GPIOB, BTN1_Pin, 0)) { 
        //usbPrintf(MAX_BUF_LEN, "button pressed %d\n", 0);
        badge_hat_next_mode();
    }
    if (button_pressed(GPIOB, BTN2_Pin, 1)) { 
        //usbPrintf(MAX_BUF_LEN, "button pressed %d\n", 1);
        badge_blush_next_mode();
    }
  }
  menu_badge_oled();
}

// Menu UI

