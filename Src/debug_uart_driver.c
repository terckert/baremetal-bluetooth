/*
 * debug_uart_driver.c
 *
 *  Created on: Feb 16, 2023
 *      Author: T. Erckert
 */

#include <debug_uart_driver.h>


#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#define GPIOx_REGISTER		GPIOA
#define GPIO_CLOCK_EN		(1U << 0)
#define UART_CLOCK_EN		(1U << 17)
#define MODER_FIRST_BITS	((1U << 2) | (1U << 4))
#define AFRL_LOWER_BITS		0x770
#define AFRL_MSB			0x880

void debug_uart_init(void) {

	// Enable the clocks for GPIOA and USART2
	RCC->AHB1ENR |= GPIO_CLOCK_EN;
	RCC->APB1ENR |= UART_CLOCK_EN;

	// Set to alternate function mode for PA2 and PA3
	GPIOx_REGISTER->MODER &= ~(MODER_FIRST_BITS);
	GPIOx_REGISTER->MODER |=  (MODER_FIRST_BITS << 1);

}

#ifdef __cplusplus
}
#endif /* __cplusplus */
