#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED

#include <stdint.h>

#define RINGBUFFER_SIZE 512
typedef struct
{
    volatile uint8_t buf[RINGBUFFER_SIZE];
    uint16_t iEnd;
    uint16_t iStart;
} RingBuffer;

uint8_t RingBuffer_GetByte(RingBuffer *buffer);
void    RingBuffer_PutByte(RingBuffer *buffer, uint8_t b);
uint8_t RingBuffer_IsEmpty(RingBuffer *buffer);

#endif /* RINGBUFFER_H_INCLUDED */
