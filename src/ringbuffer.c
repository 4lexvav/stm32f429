#include "ringbuffer.h"

uint8_t RingBuffer_GetByte(RingBuffer * buffer)
{
    uint8_t ret = 0;

    if (RingBuffer_IsEmpty(buffer)) return 0;

    ret = buffer->buf[buffer->iStart];
    buffer->iStart = ((buffer->iStart+1)%(RINGBUFFER_SIZE));

    return ret;
}

uint8_t RingBuffer_IsEmpty(RingBuffer * buffer)
{
    return (buffer->iStart == buffer->iEnd);
}

void RingBuffer_PutByte(RingBuffer * buffer, uint8_t b)
{
    buffer->buf[buffer->iEnd] = b;
    buffer->iEnd = ((buffer->iEnd+1)%(RINGBUFFER_SIZE));

    if (RingBuffer_IsEmpty(buffer))
        buffer->iStart = ((buffer->iStart+1)%(RINGBUFFER_SIZE));
}
