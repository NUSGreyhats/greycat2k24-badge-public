# GreyCat2K24 Firmware

## dapboot-master

Bootloader for GreyCat2K24. Same as https://github.com/devanlai/dapboot/tree/master, except that we modified `\src\stm32f103\bluepill\config.h` to support our board.

The compilation steps are (on WSL)

```
cd src/
make clean
make TARGET=BLUEPILL_HIGH
```

## usb_cdc_test

Firmware for finals ~~yes some things are very hacky~~

## greycat_firmware

Updated firmware with 
1. Menu system
2. Fixed image upload system
3. SpamGame & Pong