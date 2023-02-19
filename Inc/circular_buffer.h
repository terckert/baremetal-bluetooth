/*
 * circular_buffer.h
 *
 *  Created on: Feb 18, 2023
 *      Author: T. Erckert
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#define CIRCULAR_BUFFER_SIZE	256
#define c_buffer 				struct Circular_Buffer*

// Push return values
#define CBUF_PUSH_OK			0
#define CBUF_PUSH_OVERFLOW		1

// Pop return code
#define CBU_POP_FAILED			-1

// Empty return values
#define CBUF_EMPTY				0
#define CBUF_NOT_EMPTY			1



struct Circular_Buffer;

void* c_buff_init();
uint8_t c_buff_push(c_buffer b, uint8_t ch);
char c_buff_pop(c_buffer b);
int c_buff_is_empty(c_buffer b);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CIRCULAR_BUFFER_H_ */
