/*
 * debug_uart_driver.h
 *
 *  Created on: Feb 16, 2023
 *      Author: T. Erckert
 */

#ifndef DEBUG_UART_DRIVER_H_
#define DEBUG_UART_DRIVER_H_

#include "stm32f4xx.h"
#include "circular_buffer.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */



extern c_buffer debug_rx_buffer; 		// incoming data stream

// Allows forwarding of characters to other TX pins, should be redefined in main if used.
extern void forward_user_input(char ch) __attribute__((weak));
void debug_uart_init(void);
void debug_uart_transmit(uint8_t ch);
int debug_uart_has_data();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEBUG_UART_DRIVER_H_ */
