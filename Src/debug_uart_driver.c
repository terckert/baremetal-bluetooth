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

// GPIO clock information
#define GPIO_CLOCK_REG		RCC->AHB1ENR 	// GPIO clock register: RCC->'x'

// TX pin information (PA2)
#define TX_MODER_REG		GPIOA->MODER  	// MODER register for TX pin
#define TX_ALT_FUNC_REG		GPIOA->AFR[0] 	// Alternate function register for TX pin
#define TX_CLOCK_EN			(1U << 0)		// Clock enable bit for TX pin
#define TX_MODER_1_BITS		(1U << 5)		// 1 bits in the MODER code for alternate function
#define TX_MODER_0_BITS		(1U << 4)		// 0 bits in the MODER code for alternate function
#define AFR_TX_1_BITS		(7U << 8)		// 1 bits in the alternate function code
#define AFR_TX_0_BITS		(8U << 8)		// 0 bits in the alternate function code

// RX pin information (PA3)
#define RX_MODER_REG		GPIOA->MODER	// RX pin register
#define RX_ALT_FUNC_REG		GPIOA->AFR[0]	// Alternate function register for RX pin
#define RX_CLOCK_EN			(1U << 0)		// Clock enable bit for RX pin
#define RX_MODER_1_BITS		(1U << 7)		// 1 bits in the MODER code for alternate function
#define RX_MODER_0_BITS		(1U << 6)		// 0 bits in the MODER code for alternate function
#define AFR_RX_1_BITS		(7U << 12)		// 1 bits in the alternate function code
#define AFR_RX_0_BITS		(8U << 12)		// 0 bits in the alternate function code

// UART clock information
#define UART_CLOCK_REG		RCC->APB1ENR	// UART clock register: RCC->'x'
#define UART_CLOCK_EN		(1U << 17)		// UART clock register bit to set

// UART information
#define UART_DATA_REG		USART2->DR		// USART register
#define UART_ENABLE_REG		USART2->CR1 	// USART register containing enable/disable flags
#define UART_BRR_REG		USART2->BRR		// USART BRR register
#define UART_STATUS_REG		USART2->SR		// USART status flags register
#define UART_ENABLE_BIT		(1U << 13)  	// USART enable/disable bit
#define UART_REC_EN			(1U << 2)		// USART receive enable bit
#define UART_RX_INT_FLAG	(1U << 5)		// USART receive interrupt flag
#define UART_TRANS_EN		(1U << 3)		// USART transmit enable bit
#define UART_TRANS_RDY		(1U << 7)		// USART transmit interrupt flag
#define UART_TX_INT_FLAG	(1U << 7)		// USART transmit interrupt flag
#define UART_INT_FUN		USART2_IRQHandler // USART interrupt name in .s file

#define SYSTEM_FREQ			16000000U		// System frequency is 16Mhz
#define UART_BAUDRATE		115200U			// Baud rate we want to set

#define USART_INTERRUPT		USART2_IRQn		// Found in the header file of your board


c_buffer debug_rx_buffer;

static uint16_t compute_uart_baudrate(uint32_t periph_clk, uint32_t baudrate);

void debug_uart_init(void) {

	GPIO_CLOCK_REG |= TX_CLOCK_EN;			// Enable clock to the TX pin
	GPIO_CLOCK_REG |= RX_CLOCK_EN;			// Enable clock to the RX pin

	// Set up the TX pin for transferring data to the console
	TX_MODER_REG |=  TX_MODER_1_BITS;		// Set the MODER 1 bits for the TX pin
	TX_MODER_REG &= ~TX_MODER_0_BITS;		// Set the MODER 0 bits for the TX pin
	TX_ALT_FUNC_REG |=  AFR_TX_1_BITS;		// Set the alternate function registers 1 bits for the TX pin
	TX_ALT_FUNC_REG &= ~AFR_TX_0_BITS;  	// Set the alternate function registers 0 bits for the TX pin

	// Set up the RX pin to receive data from the console
	RX_MODER_REG |=  RX_MODER_1_BITS;		// Set the MODER 1 bits for the RX pin
	RX_MODER_REG &= ~RX_MODER_0_BITS;		// Set the MODER 0 bits for the RX pin
	RX_ALT_FUNC_REG |=  AFR_RX_1_BITS;		// Set the alternate function registers 1 bits for the RX pin
	RX_ALT_FUNC_REG &= ~AFR_RX_0_BITS;  	// Set the alternate function registers 0 bits for the RX pin

	UART_CLOCK_REG |= UART_CLOCK_EN; 		// Enable clock to the UART
	UART_ENABLE_REG &= ~UART_ENABLE_BIT;	// Disable the UART for configuration
	UART_ENABLE_REG |= (UART_REC_EN | UART_TRANS_EN); // Enable transmission and/or receiving for USART
	UART_ENABLE_REG |= UART_RX_INT_FLAG;	// Enable the receive interrupt flag

	// Set the baud rate of the board.
	UART_BRR_REG = compute_uart_baudrate(SYSTEM_FREQ, UART_BAUDRATE);

	NVIC_EnableIRQ(USART_INTERRUPT);		// Enable the interrupt handler

	UART_ENABLE_REG |= UART_ENABLE_BIT;		// Re-enable the interrupt handler

	debug_rx_buffer = c_buff_init();
}



/*
 * debug_uart_transmit
 *
 * Transmits a character through the UART
 *
 * parameters:
 * 		ch: character to be transmitted
 */
void debug_uart_transmit(uint8_t ch) {
	// Block until the transmission ready flag is set
	while (!(UART_STATUS_REG & UART_TRANS_RDY)) { ; }
	UART_DATA_REG = ch;
}

/*
 * __io_putchar
 *
 * Overwrites the putchar function allowing the custom transmit function to work with
 * printf
 *
 * parameters:
 * 		char: character to be trasnmitted
 */
int __io_putchar(int ch){
	debug_uart_transmit((ch & 0xff)); // Guarantees the int passed is only 8 bits
	return ch;
}

void UART_INT_FUN(void) {
	char read = UART_DATA_REG;
	c_buff_push(debug_rx_buffer, read);
	forward_user_input(read);
	if (read != '\r') {
		debug_uart_transmit(read);
	} else {
		debug_uart_transmit('\n');
		debug_uart_transmit('\r');
	}
}

//__attribute__((weak)) void forward_user_input(char ch){
//	return;
//}


/*
 * compute_uart_baudrate
 *
 * Calculates the value to be placed in the BRR register to achieve the desired baudrate
 *
 * parameters:
 * 		periph_clk: default clock rate of the peripheral
 * 		baudrate: desired baud rate
 */
static uint16_t compute_uart_baudrate(uint32_t periph_clk, uint32_t baudrate){
	return ((periph_clk + (baudrate/2U))/baudrate);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
