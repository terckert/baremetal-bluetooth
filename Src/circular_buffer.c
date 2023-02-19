/*
 * circular_buffer.c
 *
 *  Created on: Feb 18, 2023
 *      Author: T. Erckert
 */

#include "circular_buffer.h"

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

struct Circular_Buffer {
	char _c_buffer[CIRCULAR_BUFFER_SIZE];
	uint32_t head;
	uint32_t tail;
};


void* c_buff_init() {
	return calloc(sizeof(struct Circular_Buffer), 1);
}

uint8_t c_buff_push(c_buffer b, uint8_t ch) {
	b->_c_buffer[b->head] = ch;
	b->head = (b->head + 1) % CIRCULAR_BUFFER_SIZE;
	return (b->head != b->tail) ? CBUF_PUSH_OK : CBUF_PUSH_OVERFLOW;
}


char c_buff_pop(c_buffer b) {
	if (b->head == b->tail) return CBU_POP_FAILED;
	char ret = b->_c_buffer[b->tail];
	b->tail = (b->tail + 1) % CIRCULAR_BUFFER_SIZE;
	return ret;
}

int c_buff_is_empty(c_buffer b) {
	return (b->head == b->tail) ? CBUF_EMPTY : CBUF_NOT_EMPTY;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
