#include "main.h"
#include "ws2812b.h"
#include "tim.h"

// LED Cheek /////////////////////////////////////////////////////////////////////////////
void led_cheek_r(uint16_t brightness);
void led_cheek_l(uint16_t brightness);

// LED Hat //////////////////////////////////////////////////////////////////////////////
void led_hat_init();
void led_hat(uint32_t r, uint32_t g, uint32_t b);