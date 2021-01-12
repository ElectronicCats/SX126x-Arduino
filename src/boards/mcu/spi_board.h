#ifndef _SPI_BOARD_H
#define _SPI_BOARD_H
#if defined ESP8266 || defined ESP32
#include "boards/mcu/espressif/spi_board.h"
#endif
#if defined(ARDUINO_ARCH_MBED)
#include "boards/mcu/nrf52840/spi_board.h"
#endif
#endif