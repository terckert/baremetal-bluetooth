/*
 * systick_delays.c
 *
 *  Created on: Feb 18, 2023
 *      Author: T. Erckert
 */

#include "systick_delays.h"


#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

// SysTick information
#define ST_SYSTEM_FREQ		16000000U		// External clock value
#define ST_CTRL_REG			SysTick->CTRL 	// Control register
#define ST_LOAD_REG			SysTick->LOAD	// Register holds value that we are counting to
#define ST_VALUE_REG		SysTick->VAL	// Timer current count register
#define ST_ENABLE			(1U << 0)		// Enable SysTick bit
#define ST_INTERRUPT_EN		(1U << 1)		// Enable SysTick interrupt bit
#define ST_CLOCK_SRC		(1U << 2)		// Clock source bit (1 for processor clock, 0 for external)
#define ST_COUNT_FLAG		(1U << 16)		// CTRL register flag. Is 1 if timer counted to 0 since last read

/*
 * systick_delay_ms
 *
 * Delays for 'n' milliseconds
 *
 * parameters:
 * 		delay: number of milliseconds to delay by
 */
void systick_delay_ms(uint32_t delay){
	// Reload clock with number of clock cycles per millisecond
	ST_LOAD_REG = ST_SYSTEM_FREQ/1000;		// Formula for milliseconds

	// Clear SysTick current value register so it starts over from 0
	ST_VALUE_REG = 0;

	// Enable SysTick and select to use internal clock source
	ST_CTRL_REG = (ST_CLOCK_SRC | ST_ENABLE);

	// Outer loop counts down the number of milliseconds to delay by.
	for (int i = 0; i < delay; i++) {
		// Polls the control registers count flag to find out if timer has reached maximum count
		while ((ST_CTRL_REG & ST_COUNT_FLAG) == 0) { ; }
	}

	ST_CTRL_REG = 0;						// Reset the clock
}

/*
 * systick_delay_us
 *
 * Delays for 'n' microseconds
 *
 * parameters:
 * 		delay: number of microseconds to delay by
 */
void systick_delay_us(uint32_t delay){
	// Reload clock with number of clock cycles per millisecond
	ST_LOAD_REG = ST_SYSTEM_FREQ/1000000;	// Formula for microseconds

	// Clear SysTick current value register so it starts over from 0
	ST_VALUE_REG = 0;

	// Enable SysTick and select to use internal clock source
	ST_CTRL_REG = (ST_CLOCK_SRC | ST_ENABLE);

	// Outer loop counts down the number of milliseconds to delay by.
	for (int i = 0; i < delay; i++) {
		// Polls the control registers count flag to find out if timer has reached maximum count
		while ((ST_CTRL_REG & ST_COUNT_FLAG) == 0) { ; }
	}

	ST_CTRL_REG = 0;						// Reset the clock
}

/*
 * systick_delay_s
 *
 * Delays for 'n' seconds
 *
 * parameters:
 * 		delay: number of seconds to delay by
 */
void systick_delay_s(uint32_t delay){
	// Reload clock with number of clock cycles per millisecond
	ST_LOAD_REG = ST_SYSTEM_FREQ;			// Full frequency is 1s

	// Clear SysTick current value register so it starts over from 0
	ST_VALUE_REG = 0;

	// Enable SysTick and select to use internal clock source
	ST_CTRL_REG = (ST_CLOCK_SRC | ST_ENABLE);

	// Outer loop counts down the number of milliseconds to delay by.
	for (int i = 0; i < delay; i++) {
		// Polls the control registers count flag to find out if timer has reached maximum count
		while ((ST_CTRL_REG & ST_COUNT_FLAG) == 0) { ; }
	}

	ST_CTRL_REG = 0;						// Reset the clock
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
