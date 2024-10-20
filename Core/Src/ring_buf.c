#include "ring_buf.h"

// Ring Buffer
// Thread safe when used with these constraints:
// - Single Producer, Single Consumer
// - Sequential atomic operations
// One byte of capacity is used to detect buffer empty/full

void ringbuf_init(ring_buf_t *rbuf, uint32_t *buffer, size_t size) {
    rbuf->buffer = buffer;
    rbuf->size = size;
    rbuf->head = 0;
    rbuf->tail = 0;
}

bool ringbuf_push(ring_buf_t *rbuf, uint32_t data) {
    size_t next_tail = (rbuf->tail + 1) % rbuf->size;

    if (next_tail != rbuf->head) {
        rbuf->buffer[rbuf->tail] = data;
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

    *data = rbuf->buffer[rbuf->head];
    rbuf->head = (rbuf->head + 1) % rbuf->size;
    return true;
}

bool ringbuf_pop_half_word_swap(ring_buf_t *rbuf, uint32_t *data){
	if (rbuf->head == rbuf->tail) {
		// empty
		return false;
	}

	//uint32_t word_data = rbuf->buffer[rbuf->head];
	//uint32_t msb_data = word_data >> 16;
	//uint32_t lsb_data = word_data & 0xFFFF;

	//uint32_t res = lsb_data << 16;
	//res |= msb_data;
	//*data = res;

	uint16_t* half_word_data_ptr = (uint16_t*)(&(rbuf->buffer[rbuf->head]));
	uint16_t* half_word_res_ptr = (uint16_t*)(data);
	half_word_res_ptr[1] = half_word_data_ptr[0];
	half_word_res_ptr[0] = half_word_data_ptr[1];

	rbuf->head = (rbuf->head + 1) % rbuf->size;
	return true;
}

bool ringbuf_is_empty(ring_buf_t *rbuf) {
    return rbuf->head == rbuf->tail;
}

bool ringbuf_is_full(ring_buf_t *rbuf) {
    return ((rbuf->tail + 1) % rbuf->size) == rbuf->head;
}

size_t ringbuf_available_data(ring_buf_t *rbuf) {
    return (rbuf->tail - rbuf->head + rbuf->size) % rbuf->size;
}

size_t ringbuf_available_space(ring_buf_t *rbuf) {
    return rbuf->size - ringbuf_available_data(rbuf) - 1;
}
