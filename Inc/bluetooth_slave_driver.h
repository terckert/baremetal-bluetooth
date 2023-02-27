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
#include "systick_delays.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

typedef enum bt_setup{
	_normal = 0,
	_first_time
} BT_Setup;


extern c_buffer bt_tx_buff;
extern c_buffer bt_rx_buff;
extern uint8_t bt_transfer_ready;
extern uint8_t bt_data_received;

void bt_uart_init(BT_Setup setup_mode);
void bt_transmit_single_character(uint32_t ch);
void bt_transmit_data();
void bt_transmit_string(char * str);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BLUETOOTH_SLAVE_DRIVER_H_ */
