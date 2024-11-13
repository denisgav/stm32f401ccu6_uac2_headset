#ifndef RING_BUF__H
#define RING_BUF__H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct _ring_buf_t {
    uint32_t *buffer;
    uint16_t head;
    uint16_t tail;
    uint16_t size;
} ring_buf_t;

static inline uint32_t half_word_swap(const uint32_t data){
	uint32_t res = 0;
	uint16_t* half_word_data_ptr = (uint16_t*)(&data);
	uint16_t* half_word_res_ptr = (uint16_t*)(&res);
	half_word_res_ptr[1] = half_word_data_ptr[0];
	half_word_res_ptr[0] = half_word_data_ptr[1];
	return res;
}

void ringbuf_init(ring_buf_t *rbuf, uint32_t *buffer, uint16_t size);

bool ringbuf_push(ring_buf_t *rbuf, uint32_t data);
bool ringbuf_push_half_word_swap(ring_buf_t *rbuf, uint32_t data);

bool ringbuf_pop(ring_buf_t *rbuf, uint32_t *data);
bool ringbuf_pop_half_word_swap(ring_buf_t *rbuf, uint32_t *data);

bool ringbuf_is_empty(ring_buf_t *rbuf);

bool ringbuf_is_full(ring_buf_t *rbuf);

uint16_t ringbuf_available_data(ring_buf_t *rbuf);

uint16_t ringbuf_available_space(ring_buf_t *rbuf);

#ifdef __cplusplus
}
#endif

#endif //RING_BUF__H
