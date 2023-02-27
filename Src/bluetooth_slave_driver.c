/*
 * debug_uart_driver.c
 *
 *  Created on: Feb 16, 2023
 *      Author: T. Erckert
 */

#include <bluetooth_slave_driver.h>


#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

// GPIO clock information
#define GPIO_CLOCK_REG		RCC->AHB1ENR 	// GPIO clock register

// TX pin information (PA9)
#define TX_MODER_REG		GPIOA->MODER  	// MODER register for TX pin
#define TX_ALT_FUNC_REG		GPIOA->AFR[1] 	// Alternate function register for TX pin
#define TX_CLOCK_EN			(1U << 0)		// Clock enable bit for TX pin
#define TX_MODER_1_BITS		(1U << 19)		// 1 bits in the MODER code for alternate function
#define TX_MODER_0_BITS		(1U << 18)		// 0 bits in the MODER code for alternate function
#define AFR_TX_1_BITS		(7U << 4)		// 1 bits in the alternate function code
#define AFR_TX_0_BITS		(8U << 4)		// 0 bits in the alternate function code

// RX pin information (PA10)
#define RX_MODER_REG		GPIOA->MODER	// RX pin register
#define RX_ALT_FUNC_REG		GPIOA->AFR[1]	// Alternate function register for RX pin
#define RX_CLOCK_EN			(1U << 0)		// Clock enable bit for RX pin
#define RX_MODER_1_BITS		(1U << 21)		// 1 bits in the MODER code for alternate function
#define RX_MODER_0_BITS		(1U << 20)		// 0 bits in the MODER code for alternate function
#define AFR_RX_1_BITS		(7U << 8)		// 1 bits in the alternate function code
#define AFR_RX_0_BITS		(8U << 8)		// 0 bits in the alternate function code

// UART clock information
#define UART_CLOCK_REG		RCC->APB2ENR	// UART clock register: RCC->'x'
#define UART_CLOCK_EN		(1U << 4)		// UART clock register bit to set

// UART information
#define UART_DATA_REG		USART1->DR		// USART register
#define UART_ENABLE_REG		USART1->CR1 	// USART register containing enable/disable flags
#define UART_BRR_REG		USART1->BRR		// USART BRR register
#define UART_STATUS_REG		USART1->SR		// USART status flags register
#define UART_ENABLE_BIT		(1U << 13)  	// USART enable/disable bit
#define UART_REC_EN			(1U << 2)		// USART receive enable bit
#define UART_REC_FLAG		(1U << 5)		// USART receive ready bit (RXNE)
#define UART_RX_INT_FLAG	(1U << 5)		// USART receive interrupt flag
#define UART_TRANS_EN		(1U << 3)		// USART transmit enable bit
#define UART_TRANS_RDY		(1U << 7)		// USART transmit ready flag
#define UART_TX_INT_FLAG	(1U << 7)		// USART transmit interrupt flag
#define UART_INT_FUN		USART1_IRQHandler // USART interrupt name in .s file

#define SYSTEM_FREQ			16000000U		// System frequency is 16Mhz
#define UART_BAUDRATE		9600U			// Baud rate we want to set

#define USART_INTERRUPT		USART1_IRQn		// Found in the header file of your board
#define UART_RECEIVE_DELAY  0			// Microsecond delay between reading characters when receive interrupt flag is
 	 	 	 	 	 	 	 	 	 	 	// thrown. This is needed because processor is faster than flag. Used
 	 	 	 	 	 	 	 	 	 	    // with console debugging. Based on 1/BAUDRATE
c_buffer bt_tx_buff;			// tx pin circular buffer
c_buffer bt_rx_buff;			// rx pin circular buffer
uint8_t bt_transfer_ready;		// bool, 1 - data ready, 0 - no data
uint8_t bt_data_received;		// bool, 1 - data ready, 0 - no data

static uint16_t compute_uart_baudrate(uint32_t periph_clk, uint32_t baudrate);
static void store_received_character(void);


/*
 * bt_uart_init
 *
 * Initializes the UART and it's internal buffers.
 *
 * parameters:
 * 		setup: unused
 */
void bt_uart_init(BT_Setup setup) {

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
	UART_ENABLE_REG |= UART_RX_INT_FLAG;	// Enable interrupt on data reception

	// Set the baud rate of the board.
	UART_BRR_REG = compute_uart_baudrate(SYSTEM_FREQ, UART_BAUDRATE);

	NVIC_EnableIRQ(USART_INTERRUPT);		// Enable the interrupt handler

	UART_ENABLE_REG |= UART_ENABLE_BIT;		// Re-enable the interrupt handler

	bt_tx_buff = c_buff_init();
	bt_rx_buff = c_buff_init();

	bt_transfer_ready = 0;
	bt_data_received = 0;
}



/*
 * debug_uart_transmit
 *
 * Transmits a character through the UART
 *
 * parameters:
 * 		ch: character to be transmitted
 */
void bt_transmit_single_character(uint32_t ch) {
	// Block until the transmission ready flag is set
	while (!(UART_STATUS_REG & UART_TRANS_RDY)) { ; }
	UART_DATA_REG = (ch & 0xff);  // Guarantee that transmitted character is only 8 bits in length
}


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

static void store_received_characters() {
	char c;
	while (UART_STATUS_REG & UART_REC_FLAG) {
		c = (UART_DATA_REG & 0xff);
		c_buff_push(bt_rx_buff, c);
		if (c == 0)
			bt_data_received = 1;
	}
}

void UART_INT_FUN(void) {
	store_received_characters();
}

void bt_transmit_string(char * str) {
	for (int i = 0; str[i] != 0; i++) {
		bt_transmit_single_character(str[i]);
	}
}

void bt_transmit_data() {
	while (c_buff_is_empty(bt_tx_buff) == CBUF_NOT_EMPTY) {
		bt_transmit_single_character(c_buff_pop(bt_tx_buff));
	}
	c_buff_flush(bt_tx_buff);
	bt_transfer_ready = 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
