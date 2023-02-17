/*
 * debug_uart_driver.c
 *
 *  Created on: Feb 16, 2023
 *      Author: T. Erckert
 */

#include <debug_uart_driver.h>


//#ifdef __cplusplus
// extern "C" {
//#endif /* __cplusplus */

#define GPIOx_TX_REG		GPIOA		// TX pin register
#define TX_CLOCK_EN			(1U << 0)	// Clock enable bit for TX pin
#define TX_MODER_1_BITS		(1U << 5)	// 1 bits in the MODER code for alternate function
#define TX_MODER_0_BITS		(1U << 4)	// 0 bits in the MODER code for alternate function
#define AFR_TX_INDEX		0			// AFR has two pages. Use 0 for pins 0-7, 1 for pins 8-15
#define AFR_TX_1_BITS		(7U << 8)	// The one bits in the alternate function code
#define AFR_TX_0_BITS		(8U << 8)	// The zero bits in the alternate function code

#define GPIOx_RX_REG		GPIOA		// RX pin register
#define RX_CLOCK_EN			(1U << 0)	// Clock enable bit for RX pin
#define RX_MODER_1_BITS		(1U << 7)	// 1 bits in the MODER code for alternate function
#define RX_MODER_0_BITS		(1U << 6)	// 0 bits in the MODER code for alternate function
#define AFR_RX_INDEX		0			// AFR has two pages. Use 0 for pins 0-7, 1 for pins 8-15
#define AFR_RX_1_BITS		(7U << 12)	// The one bits in the alternate function code
#define AFR_RX_0_BITS		(8U << 12)	// The zero bits in the alternate function code

#define GPIO_CLOCK_REG		AHB1ENR		// GPIO clock register: RCC->'x'
#define UART_CLOCK_REG		APB1ENR		// UART clock register: RCC->'x'
#define UART_CLOCK_EN		(1U << 17)	// UART clock register bit to set


void debug_uart_init(void) {

	RCC->GPIO_CLOCK_REG |= TX_CLOCK_EN;		// Enable clock to the TX pin
	RCC->GPIO_CLOCK_REG |= RX_CLOCK_EN;		// Enable clock to the RX pin
	RCC->UART_CLOCK_REG |= UART_CLOCK_EN; 	// Enable clock to the UART

	// Set up the TX pin for transferring data to the console
	GPIOx_TX_REG->MODER |= TX_MODER_1_BITS;
	GPIOx_TX_REG->MODER &= ~TX_MODER_0_BITS;
	GPIOx_TX_REG->AFR[AFR_TX_INDEX] |= AFR_TX_1_BITS;
	GPIOx_TX_REG->AFR[AFR_TX_INDEX] &= ~AFR_TX_0_BITS;

	// Set up the RX pin to receive data from the console
	GPIOx_RX_REG->MODER |= RX_MODER_1_BITS;
	GPIOx_RX_REG->MODER &= ~RX_MODER_0_BITS;
	GPIOx_RX_REG->AFR[AFR_RX_INDEX] |= AFR_RX_1_BITS;
	GPIOx_RX_REG->AFR[AFR_RX_INDEX] &= ~AFR_RX_0_BITS;


}

//#ifdef __cplusplus
//}
//#endif /* __cplusplus */
