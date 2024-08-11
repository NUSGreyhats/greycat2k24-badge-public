#include "main.h"
#include "hw/oled.h"

#define SPAMGAME_NUM_LEN 10
void spamegame_game_display(char *buf, int score, int timing){
    int written_len;
    ssd1306_SetCursor(0, 12*2);
    ssd1306_WriteString("Score:           ", Font_7x10, White);
    ssd1306_SetCursor(10*5, 12*2);
    
    written_len = snprintf(buf, SPAMGAME_NUM_LEN, "%d", score);
    buf[written_len] = '\0';
    ssd1306_WriteString(buf, Font_7x10, White);

    ssd1306_SetCursor(0, 12*3);
    ssd1306_WriteString("Time (s):", Font_7x10, White);
    
    ssd1306_SetCursor(10*7, 12*3);
    written_len = snprintf(buf, SPAMGAME_NUM_LEN, "%d", timing);
    buf[written_len] = '\0';
    ssd1306_WriteString(buf, Font_7x10, White);
    ssd1306_UpdateScreen(&hi2c2);
}
void spamgame(){ // I'm too lazy to make this more efficient
    // Start
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("SpamGame", Font_7x10, White);
    ssd1306_SetCursor(0, 12*2);
    ssd1306_WriteString("Spam for 60s", Font_7x10, White);
    ssd1306_SetCursor(0, 12*3);
    ssd1306_WriteString("Not well coded", Font_7x10, White);
    ssd1306_UpdateScreen(&hi2c2);
    HAL_Delay(2000);

    
    ssd1306_SetCursor(0, 12*2);
    ssd1306_WriteString("Go!            ", Font_7x10, White);
    ssd1306_SetCursor(0, 12*3);
    ssd1306_WriteString("               ", Font_7x10, White);
    ssd1306_UpdateScreen(&hi2c2);

    // Main Loop

    char buf[SPAMGAME_NUM_LEN+1];

    int score = 0;
    int prevstate = 0;
    int state = 0;
    long timing = 60000; //in ms
    long startmillis = HAL_GetTick(); //start time
    long prevtime = 0; //start time
    while (1){ 
        state = HAL_GPIO_ReadPin(GPIOB, BTN3_Pin);
        if (state != prevstate){
            prevstate = state;
            if (state == 0){
                score+=1;
                spamegame_game_display(buf, score, prevtime);
            }
        }
        if ((HAL_GetTick() - startmillis)/1000 != prevtime) {
            prevtime = (HAL_GetTick() - startmillis)/1000;
            spamegame_game_display(buf, score, prevtime);
        }
        if ((HAL_GetTick() - startmillis) > timing){break;}//Timer
    }

    ssd1306_SetCursor(0, 12*3);
    ssd1306_WriteString("End                   ", Font_7x10, White);
    ssd1306_SetCursor(10, 12*4);
    ssd1306_WriteString("                   ", Font_7x10, White);
    ssd1306_UpdateScreen(&hi2c2);
    HAL_Delay(5000);
}
