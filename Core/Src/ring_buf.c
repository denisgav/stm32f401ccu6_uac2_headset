#include "ring_buf.h"

// Ring Buffer
// Thread safe when used with these constraints:
// - Single Producer, Single Consumer
// - Sequential atomic operations
// One byte of capacity is used to detect buffer empty/full


void ringbuf_init(ring_buf_t *rbuf, uint32_t *buffer, uint16_t size) {
    rbuf->buffer = buffer;
    rbuf->size = size;
    rbuf->head = 0;
    rbuf->tail = 0;
}

bool ringbuf_push(ring_buf_t *rbuf, uint32_t data) {
	uint16_t next_tail = (rbuf->tail + 1);
	if(next_tail >= rbuf->size)
		next_tail -= rbuf->size;

    if (next_tail != rbuf->head) {
        rbuf->buffer[rbuf->tail] = data;
        rbuf->tail = next_tail;
        return true;
    }

    // full
    return false;
}

bool ringbuf_push_half_word_swap(ring_buf_t *rbuf, uint32_t data){
	uint16_t next_tail = (rbuf->tail + 1);
	if(next_tail >= rbuf->size)
		next_tail -= rbuf->size;

	if (next_tail != rbuf->head) {
		rbuf->buffer[rbuf->tail] = half_word_swap(data);
		rbuf->tail = next_tail;
		return true;
	}

	// full
	return false;
}

bool ringbuf_pop(ring_buf_t *rbuf, uint32_t *data) {
    if (rbuf->head == rbuf->tail) {
        // empty
        return false;
    }

    uint16_t next_head = (rbuf->head + 1);
	if(next_head >= rbuf->size)
		next_head -= rbuf->size;

    *data = rbuf->buffer[rbuf->head];
    rbuf->head = next_head;
    return true;
}

bool ringbuf_pop_half_word_swap(ring_buf_t *rbuf, uint32_t *data){
	if (rbuf->head == rbuf->tail) {
		// empty
		return false;
	}

	uint16_t next_head = (rbuf->head + 1);
	if(next_head >= rbuf->size)
		next_head -= rbuf->size;

	*data = half_word_swap(rbuf->buffer[rbuf->head]);
	rbuf->head = next_head;
	return true;
}

bool ringbuf_is_empty(ring_buf_t *rbuf) {
    return rbuf->head == rbuf->tail;
}

bool ringbuf_is_full(ring_buf_t *rbuf) {
    return ((rbuf->tail + 1) % rbuf->size) == rbuf->head;
}

uint16_t ringbuf_available_data(ring_buf_t *rbuf) {
	uint16_t avail_data_tmp = rbuf->size + rbuf->tail - rbuf->head;
	if(avail_data_tmp >= rbuf->size)
		avail_data_tmp -= rbuf->size;
    return avail_data_tmp;
}

uint16_t ringbuf_available_space(ring_buf_t *rbuf) {
	uint16_t avail_data_tmp = rbuf->size + rbuf->tail - rbuf->head;
	if(avail_data_tmp >= rbuf->size)
		avail_data_tmp -= rbuf->size;

    return rbuf->size - avail_data_tmp;
}
