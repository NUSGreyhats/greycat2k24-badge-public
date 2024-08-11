#include "hw/led.h"

// LED Cheek /////////////////////////////////////////////////////////////////////////////
void led_cheek_r(uint16_t brightness){
  // LED_CHEEK_R - PA7
  TIM3->CCR2 = 65535 - brightness;
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

void led_cheek_l(uint16_t brightness){
  // LED_CHEEK_L - PB9
  TIM4->CCR4 = 65535 - brightness;
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

// LED Hat //////////////////////////////////////////////////////////////////////////////
void led_hat_init(){
    WS2812B_init();
}
void led_hat(uint32_t r, uint32_t g, uint32_t b){
    WS2812B_clearBuffer();
    WS2812B_setPixel( 0, 0, r, g, b); 
    WS2812B_sendBuffer();
    HAL_Delay(10);
    WS2812B_setPixel( 0, 1, r, g, b); 
    WS2812B_sendBuffer();
    HAL_Delay(10);
}

// LED User /////////////////////////////////////////////////////////////////////////////
/* 
  Sample Code:

  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  HAL_Delay(500);
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

*/