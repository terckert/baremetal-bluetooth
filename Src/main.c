/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "debug_uart_driver.h"
#include "bluetooth_slave_driver.h"
#include "systick_delays.h"
#include "circular_buffer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GPIOA_ENABLE 		(1U << 0)
#define LED2_PIN 			(1U << 5)

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#ifdef DEBUG
#define DEBUG_PRINT 		printf
#else
#define DEBUG_PRINT(...)
#endif

#define MAXSTRLEN 256

void forward_user_input(char ch);

int main(){
	debug_uart_init();
	bt_uart_init(_normal);



	// Clock to GPIO A, B, C
	RCC->AHB1ENR |=  (1U << 0 | 1U << 1 | 1U << 2);

	// Pin PA4 to output
	GPIOA->MODER |=  (1U << 8);
	GPIOA->MODER &= ~(1U << 9);

	// Pin PB0 to output
	GPIOB->MODER |=  (1U << 0);
	GPIOB->MODER &= ~(1U << 1);

	// Pins PC0, PC1 to output
	GPIOC->MODER |=  (1U << 0 | 1U << 2);
	GPIOC->MODER &= ~(1U << 1 | 1U << 3);

	char console_in_buffer[CIRCULAR_BUFFER_SIZE];
	char cmd[CIRCULAR_BUFFER_SIZE];
	// Main loop.
	while (1){
		// BOARD LIGHT ORDER STARTING ON LEFT
		// GPIOC->ODR ^=  (1U << 0); // PC0
		// GPIOC->ODR ^=  (1U << 1); // PC1
		// GPIOB->ODR ^=  (1U << 0); // PB0
		// GPIOA->ODR ^=  (1U << 4); // PA4

		if (bt_transfer_ready) {
			bt_transmit_data();
		}
		if (bt_data_received){
			strcpy(cmd, c_buff_get_contents(bt_rx_buff));
			c_buff_flush(bt_rx_buff);
			bt_data_received = 0;
			DEBUG_PRINT("%s\r\n", cmd);
			if (strcmp(cmd, "LIGHT1") == 0) {
				GPIOC->ODR ^=  (1U << 0); // PC0
				bt_transmit_string("OK");
			} else if (strcmp(cmd, "LIGHT2") == 0) {
				GPIOC->ODR ^=  (1U << 1); // PC1
				bt_transmit_string("OK");
			} else if (strcmp(cmd, "LIGHT3") == 0) {
				GPIOB->ODR ^=  (1U << 0); // PB0
				bt_transmit_string("OK");
			} else if (strcmp(cmd, "LIGHT4") == 0) {
				GPIOA->ODR ^=  (1U << 4); // PA4
				bt_transmit_string("OK");
			} else {
				bt_transmit_string("Bad command.");
			}

		}
	}
}


//// For function testing
//int main(void)
//{
//	debug_uart_init();
//	bt_uart_init(_normal);
//
//	c_buffer test = c_buff_init();
//
//	// Test that c_buff_is_empty works when first initialized.
//	DEBUG_PRINT("Before insertion c_buff_is_empty returns: %d\r\n", c_buff_is_empty(test));
//
//	for (int i = 0; i < CIRCULAR_BUFFER_SIZE; i ++) {
//		int res = c_buff_push(test, 'a');
//		if (res == CBUF_PUSH_OVERFLOW) {
//			DEBUG_PRINT("Received overflow message on insertion: %d\r\n", i);
//		}
//
//	}
//
//
//
//	DEBUG_PRINT("After insertion c_buff_is_empty returns: %d\r\n", c_buff_is_empty(test));
//
//	// Clock to GPIO A, B, C
//	RCC->AHB1ENR |=  (1U << 0 | 1U << 1 | 1U << 2);
//
//	// Pin PA4 to output
//	GPIOA->MODER |=  (1U << 8);
//	GPIOA->MODER &= ~(1U << 9);
//
//	// Pin PB0 to output
//	GPIOB->MODER |=  (1U << 0);
//	GPIOB->MODER &= ~(1U << 1);
//
//	// Pins PC0, PC1 to output
//	GPIOC->MODER |=  (1U << 0 | 1U << 2);
//	GPIOC->MODER &= ~(1U << 1 | 1U << 3);
//
//	int delay = 500;
//	while(1){
//		// Order of lights to initialize, each should stay on for two cycles.
//
//		GPIOC->ODR |=  (1U << 0); // PC0 on
//		GPIOA->ODR &= ~(1U << 4); // PA4 off
//		systick_delay_ms(delay);
//
//		GPIOC->ODR |=  (1U << 1); // PC1 on
//		GPIOC->ODR &= ~(1U << 0); // PC0 off
//		systick_delay_ms(delay);
//
//		GPIOB->ODR |=  (1U << 0); // PB0 on
//		GPIOC->ODR &= ~(1U << 1); // PC1 off
//		systick_delay_ms(delay);
//
//		GPIOA->ODR |=  (1U << 4); // PA4 on
//		GPIOB->ODR &= ~(1U << 0); // PB0 off
//		systick_delay_ms(delay);
//
//	}
//}


void forward_user_input(char ch) {
	if (ch != '\r')
		c_buff_push(bt_tx_buff, ch);
	else
		bt_transfer_ready=1;
}
