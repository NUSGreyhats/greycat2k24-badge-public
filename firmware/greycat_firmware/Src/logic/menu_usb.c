#include "hw/usb_cdc_comms.h"
#include "hw/flash.h"
#include "hw/led.h"
#include "usart.h"
#include "tim.h"

void menu_usb_read_uart(){
    usbPrintf(MAX_BUF_LEN, "Enter baudrate: ");
    uint32_t baudrate = readInteger();
    usbPrintf(MAX_BUF_LEN, "%d\n", baudrate);
    if (baudrate == 0){
        baudrate = 9600;
        usbPrintf(MAX_BUF_LEN, "Defaulting to 9600\n");
    }

    // Reinit UART
    HAL_UART_DeInit(&huart1);
    huart1.Instance = USART1;
    huart1.Init.BaudRate = baudrate;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

  usbPrintf(MAX_BUF_LEN, "UART: will print out UART Data received from USART pins\n");
  while (1)
  {
     uint8_t ch;
     HAL_UART_Receive(&huart1, &ch, 1, HAL_MAX_DELAY);
     //usbPrintf(MAX_BUF_LEN, "%c", ch);
     while (CDC_Transmit_FS(&ch, 1) == USBD_BUSY){};
     //HAL_UART_Transmit(&huart1, &ch, 1, HAL_MAX_DELAY);
  }
}

void menu_usb_write_image(){
    usbPrintf(MAX_BUF_LEN, "Image Data Entry:\n");
    flash_write_image_usb();
}

void menu_usb_display_image(){
    badge_oled_display_image();
}

void menu_usb_custom_lights(){
    usbPrintf(MAX_BUF_LEN, "Custom Lights\n");

  usbPrintf(MAX_BUF_LEN, "Hat Red (0-255): ");
  uint64_t red = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", red);

  usbPrintf(MAX_BUF_LEN, "Hat Green (0-255): ");
  uint64_t green = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", green);

  usbPrintf(MAX_BUF_LEN, "Hat Blue (0-255): ");
  uint64_t blue = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", blue);

  usbPrintf(MAX_BUF_LEN, "Blush Left (0-65535): ");
  uint64_t blush_left = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", blush_left);

  usbPrintf(MAX_BUF_LEN, "Blush Right (0-65535): ");
  uint64_t blush_right = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", blush_right);

  led_hat(red, green, blue);
  led_cheek_l(blush_left);
  led_cheek_r(blush_right);
}
void menu_usb_timer(){
    /* USER CODE BEGIN TIM1_Init 0 */
  
  usbPrintf(MAX_BUF_LEN, "Timer 1 Configurator\n");

  usbPrintf(MAX_BUF_LEN, "Prescalar: ");
  uint64_t prescalar = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", prescalar);

  usbPrintf(MAX_BUF_LEN, "Period: ");
  uint64_t period = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", period);

  usbPrintf(MAX_BUF_LEN, "CCR1: ");
  uint64_t ccr1 = readInteger();
  usbPrintf(MAX_BUF_LEN, "%ld\n", ccr1);

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = prescalar; //0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = period; //65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

  
  TIM1->CCR1 = ccr1;
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

/// Menu //////////////////////////////////////////////////////////////////////////////////////
void menu_usb_header(){
    usbPrintf(MAX_BUF_LEN, "Menu:\n");

    usbPrintf(MAX_BUF_LEN, "\nCTF Challenges\n");
    usbPrintf(MAX_BUF_LEN, "1. Q&A\n");
    usbPrintf(MAX_BUF_LEN, "2. Short\n");
    usbPrintf(MAX_BUF_LEN, "3. Name\n");
    usbPrintf(MAX_BUF_LEN, "4. Send Serialous Message\n");
    usbPrintf(MAX_BUF_LEN, "5. Bootloader\n");
    
    usbPrintf(MAX_BUF_LEN, "\nTools\n");
    // Tools
    usbPrintf(MAX_BUF_LEN, "g. Print GPIO\n");
    usbPrintf(MAX_BUF_LEN, "u. Read USART\n");
    usbPrintf(MAX_BUF_LEN, "t. Timer Configurator\n");
    usbPrintf(MAX_BUF_LEN, "l. Custom Lighting\n");
    // State Variables
    usbPrintf(MAX_BUF_LEN, "a. Display State\n");
    usbPrintf(MAX_BUF_LEN, "s. Reset State\n");
    // Image
    usbPrintf(MAX_BUF_LEN, "w. Write Image\n");
    usbPrintf(MAX_BUF_LEN, "y. Display image\n");
    usbPrintf(MAX_BUF_LEN, "x. Exit\n");
    usbPrintf(MAX_BUF_LEN, "\n");
}

void menu_usb(){
    char run_menu = 1;
    while (run_menu) {
        menu_usb_header();
        usbPrintf(MAX_BUF_LEN, "Select Option > ");

        char input_ans[MAX_BUF_LEN+1];
        memset(input_ans, 0, MAX_BUF_LEN);
        int ans_len = readString(input_ans, MAX_BUF_LEN);
        usbPrintf(MAX_BUF_LEN, "%s\n", input_ans);
        usbPrintf(MAX_BUF_LEN, "------------------------------------------------------------------");
        usbPrintf(MAX_BUF_LEN, "\n");

        switch (input_ans[0]){
            case '1':
                chall_qna();
                break;
            case '2':
                chall_hyper_glitch();
                break;
            case '3':
                chall_name();
                break;
            case '4':
                chall_send_uart();
                break;
            case '5':
                chall_bootloader();
                break;
            // Tools ///////////////////////////////////////////////////////////////////////////////
            case 'g':
                chall_logic_read();
                break;
            case 'u':
                menu_usb_read_uart();
                break;
            case 't':
                menu_usb_timer();
                break;
            case 'l':
                menu_usb_custom_lights();
                break;
            // State ///////////////////////////////////////////////////////////////////////////
            case 'a':
                badge_state_usb_display(flash_read_state());
                break;
            case 's':
                badge_state_reset();
                break;
            // Image ///////////////////////////////////////////////////////////////////////////
            case 'w':
                menu_usb_write_image();
                break;
            case 'y':
                menu_usb_display_image();
                break;
            case 'x':
                run_menu = 0;;
                break;
        }
        usbPrintf(MAX_BUF_LEN, "\n\n\n\n");
    }
}