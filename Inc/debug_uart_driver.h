/*
 * debug_uart_driver.h
 *
 *  Created on: Feb 16, 2023
 *      Author: T. Erckert
 */

#ifndef DEBUG_UART_DRIVER_H_
#define DEBUG_UART_DRIVER_H_

#include "stm32f4xx.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


void debug_uart_init(void);
void debug_uart_transmit(uint8_t ch);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEBUG_UART_DRIVER_H_ */
