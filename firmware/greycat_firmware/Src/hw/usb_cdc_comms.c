#include "hw/usb_cdc_comms.h"
#include "SEGGER_RTT.h"

int _write(int file, char *data, int len)
{
  if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
  {
    errno = EBADF;
    return -1;
  }
	SEGGER_RTT_Write(0, data, len);
  //while (CDC_Transmit_FS((uint8_t *)data, len) == USBD_BUSY){};
  return len;
  //return (CDC_Transmit_FS(data, len) == USBD_BUSY ? 0 : len);
}

char __platform_getchar(void)
{
  uint8_t ch;
  while (1)
  {
    uint16_t bytesAvailable = CDC_GetRxBufferBytesAvailable_FS();
    if (bytesAvailable > 0 && CDC_ReadRxBuffer_FS(&ch, 1) == USB_CDC_RX_BUFFER_OK) //HAL_OK == HAL_UART_Receive(&huart1, &ch, 1, 0))
    {
      return ch;
    }
  }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

// Another fast function is putchar
void fastPrintString(char *string_given){
  while (CDC_Transmit_FS((uint8_t *)string_given, strlen(string_given)) == USBD_BUSY){};
}

void usbPrintf(int len, char *fmt, ...)  
{                                                                               
   va_list arg_ptr;                                                            
   char buf[len+1];
    
   va_start(arg_ptr, fmt);                                                      
   int written_len = vsnprintf(buf, len, fmt, arg_ptr);
   buf[written_len] = '\0';
   fastPrintString(buf);
   va_end(arg_ptr);                                                             
}

int readString(char *outputBuf, int len){
  // Read buffer
  uint8_t rxData[1];
  memset(rxData, 0, 1);
  uint8_t buf_pos = 0;

  while (1) {
    // Echo data
    rxData[0] = __platform_getchar();
    if (rxData[0] == '\n' || buf_pos >= len){ break; }
    outputBuf[buf_pos] = rxData[0];
    buf_pos++;
  }
  return buf_pos;
}

uint64_t readInteger(){
  char buf[MAX_BUF_LEN];
  int end_pos = readString(buf, MAX_BUF_LEN);
  buf[end_pos] = '\0';
  // return atoi(buf);

  //usbPrintf(MAX_BUF_LEN, "\"%s\" ", buf);
  char *endptr;
  uint64_t num;

  // Convert the string to a long integer
  num = strtoull(buf, &endptr, 10);
  if (endptr == buf) {
    usbPrintf(MAX_BUF_LEN, "No digits were found.\n");
    return 0;
  } else if (*endptr != '\0') {
    //usbPrintf(MAX_BUF_LEN, "Invalid character: %c\n", *endptr);
  } else {
    //usbPrintf(MAX_BUF_LEN, "The number is: %ld\n", num);
  }
  return num;
}

void printString(char *string_given){
	printf("%s", string_given); 
}