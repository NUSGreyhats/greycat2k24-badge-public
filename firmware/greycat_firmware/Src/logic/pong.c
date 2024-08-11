#include "main.h"
#include "hw/led.h"
#include "hw/oled.h"
#include "hw/flash.h"
#include "hw/usb_cdc_comms.h"
#include "adc.h"

//Variables declared here
int gamestate = 0;
int ballx = 62;
int bally = 0;
#define BALL_SIZE 4
int ballright = 1;
int balldown = 1;
#define PADDLE_WIDTH 4
#define PADDLE_HEIGHT 9
int paddleheight = PADDLE_HEIGHT;
int playerx = 0;
int playery = 22;
int computery = 22;
int computerx = 127 - PADDLE_WIDTH;
int playerscore = 0;
int computerscore = 0;

void resetGame() {
  ballx = 63;
  playerscore = 0;
  computerscore = 0;
}

// Label Screens /////////////////////////////////////////////////

void pong_title_screen(){
    //Title screen
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Pong", Font_7x10, White);
    ssd1306_SetCursor(0, 12*2);
    ssd1306_WriteString("BTN3 to start", Font_7x10, White);
    ssd1306_UpdateScreen(&hi2c2);

    // ADC Logic //////////////////////////////////////////
    // start ADC convertion
    HAL_ADC_Start(&hadc1);
    // ADC poll for conversion
    HAL_ADC_PollForConversion(&hadc1, 10);
    // get the ADC conversion value
    int adc_value = HAL_ADC_GetValue(&hadc1);
    srand(adc_value);

    HAL_Delay(500);
    while (HAL_GPIO_ReadPin(GPIOB, BTN3_Pin)!=0){}
}

// State Machine /////////////////////////////////////////////////
void oled_draw_rect(int x_given, int y_given, int width, int height, int colour){
    for (int i=x_given; i<x_given+width; i++){
        for (int j=y_given; j<y_given+height; j++){
            ssd1306_DrawPixel(i, j, colour);
        }
    }
}

void pong_game_drawing(char *buf){
    // Gameplay screen
    // Display the player's score
    ssd1306_SetCursor(20, 0);

    int written_len = snprintf(buf, 100, "%d          %d", playerscore, computerscore);
    buf[written_len] = '\0';
    ssd1306_WriteString(buf, Font_7x10, White);

    // arduboy.print(playerscore);
    // //Display the computer's score
    // arduboy.setCursor(101, 0);
    // arduboy.print(computerscore);
    
    
    oled_draw_rect(ballx, bally, BALL_SIZE, BALL_SIZE, White); //Draw the ball
    oled_draw_rect(playerx, playery, PADDLE_WIDTH, PADDLE_HEIGHT, White); // Draw the player's paddle
    oled_draw_rect(computerx, computery, PADDLE_WIDTH, PADDLE_HEIGHT, White); // Draw the computer's paddle

    // arduboy.fillRect(computerx, computery, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    ssd1306_UpdateScreen(&hi2c2);
    oled_draw_rect(ballx, bally, BALL_SIZE, BALL_SIZE, Black); //Draw the ball
    oled_draw_rect(playerx, playery, PADDLE_WIDTH, PADDLE_HEIGHT, Black); // Draw the player's paddle
    oled_draw_rect(computerx, computery, PADDLE_WIDTH, PADDLE_HEIGHT, Black); // Draw the computer's paddle
}
void pong_game_logic(){
    int hash = rand() % (20 + 1); 
    // end ADC convertion
    HAL_ADC_Stop(&hadc1);

    //Move the ball right
    if(ballright == 1) {
        ballx = ballx + 1;
    }
    //Move the ball left
    if(ballright == -1) {
        ballx = ballx - 1;
    }

    //Move the ball down
    if(balldown == 1) {
        bally = bally + 1;
    }
    //Move the ball up
    if(balldown == -1) {
        bally = bally - 1;
    }
    //Reflect the ball off of the top of the screen
    if(bally == 0) {
        balldown = 1;
    }
    //Reflect the ball off of the bottom of the screen
    if(bally + BALL_SIZE == 63) {
        balldown = -1;
    }
    
    //If the player presses Up and the paddle is not touching the top of the screen, move the paddle up
    if(HAL_GPIO_ReadPin(GPIOB, BTN1_Pin)==0 && playery > 0) {
        playery = playery - 1;
    }
    //If the player presses down and the paddle is not touching the bottom of the screen, move the paddle down
    if(HAL_GPIO_ReadPin(GPIOB, BTN2_Pin)==0 && playery + paddleheight < 63) {
        playery = playery + 1;
    }
    
    // If the ball is higher than the computer's paddle, move the computer's paddle up
    if (ballx > 115-15 || hash > 17) { // || random(0, 20) == 1) {
        if (bally < computery) {
            computery = computery - 1;
        }
        // If the bottom of the ball is lower than the bottom of the computer's paddle, move the comptuer's paddle down
        if (bally + BALL_SIZE > computery + paddleheight) {
            computery = computery + 1;
        }
    }
    //If the ball moves off of the screen to the left...
    if(ballx < -10) {
        //Move the ball back to the middle of the screen
        ballx = 63;
        //Give the computer a point
        computerscore = computerscore + 1;
    }
    //If the ball moves off of the screen to the right....
    if(ballx > 130) {
        //Move the ball back to the middle of the screen
        ballx = 63;
        //Give the player a point
        playerscore = playerscore + 1;
    }

    //Check if the player wins
    if(playerscore == 5) {
        gamestate = 2;
    }
    //Check if the computer wins
    if(computerscore == 5) {
        gamestate = 3;
    }

    //If the ball makes contact with the player's paddle, bounce it back to the right
    if (ballx == playerx + PADDLE_WIDTH && playery < bally + BALL_SIZE && playery + PADDLE_HEIGHT > bally) {
        ballright = 1;
    }
    //If the ball makes contact with the computer's paddle, bounce it back to the left
    if (ballx + BALL_SIZE == computerx && computery < bally + BALL_SIZE && computery + PADDLE_HEIGHT > bally) {
        ballright = -1;
    }
}

// State Machine /////////////////////////////////////////////////
void pong_state_machine() {
    // Title Screen
    pong_title_screen();
    ssd1306_Fill(Black);

    char buf[101];
    
    while (1){
        pong_game_drawing(buf);
        pong_game_logic();
    }
}

void pong(){
    pong_state_machine();
}