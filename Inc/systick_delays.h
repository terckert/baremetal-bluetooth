/*
 * systick_delays.h
 *
 *  Created on: Feb 18, 2023
 *      Author: T. Erckert
 */

#ifndef SYSTICK_DELAYS_H_
#define SYSTICK_DELAYS_H_

#include "stm32f4xx.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


void systick_delay_s(uint32_t delay);
void systick_delay_ms(uint32_t delay);
void systick_delay_us(uint32_t delay);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYSTICK_DELAYS_H_ */
