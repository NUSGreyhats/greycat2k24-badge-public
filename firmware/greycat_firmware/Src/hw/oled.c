#include "hw/oled.h"

void oled_init(){
    ssd1306_Init(&hi2c2);
}
void oled_update_name(char *string){
    char *string_line_1 = string;
    char *string_line_2 = string+11;
    // Write data to local screenbuffer
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Hi I am", Font_7x10, White);
    ssd1306_SetCursor(0, 12);
    ssd1306_WriteString(string_line_1, Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString(string_line_2, Font_11x18, White);
    // Copy all data from local screenbuffer to the screen
    ssd1306_UpdateScreen(&hi2c2);
}