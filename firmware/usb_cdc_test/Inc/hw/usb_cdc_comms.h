#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO

void fastPrintString(char *string_given);
void usbPrintf(int len, char *string_given, ...) ;
int readString(char *outputBuf, int len);
uint64_t readInteger();
#define MAX_BUF_LEN 128