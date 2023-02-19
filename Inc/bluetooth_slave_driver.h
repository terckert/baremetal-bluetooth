/*
 * bluetooth_uart_driver.h
 *
 *  Created on: Feb 16, 2023
 *      Author: T. Erckert
 */

#ifndef BLUETOOTH_SLAVE_DRIVER_H_
#define BLUETOOTH_SLAVE_DRIVER_H_

#include "stm32f4xx.h"
#include "circular_buffer.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

typedef enum bt_setup{
	_normal = 0,
	_first_time
} BT_Setup;


void bt_uart_init(BT_Setup setup_mode);
void bt_transmit_single_character(uint32_t ch);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BLUETOOTH_SLAVE_DRIVER_H_ */
