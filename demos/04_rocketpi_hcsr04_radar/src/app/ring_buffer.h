/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-19 10:05:09
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-19 10:05:16
 * @FilePath: /04_rocketpi_hcsr04_radar/src/app/ring_buffer.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint8_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
} RingBuffer;

void RingBuffer_Init(RingBuffer *rb, uint8_t *storage, size_t capacity);
size_t RingBuffer_Space(const RingBuffer *rb);
size_t RingBuffer_Size(const RingBuffer *rb);
size_t RingBuffer_Write(RingBuffer *rb, const uint8_t *data, size_t len);
size_t RingBuffer_ReadLinear(RingBuffer *rb, uint8_t **ptr);
void RingBuffer_Advance(RingBuffer *rb, size_t len);

#endif /* RING_BUFFER_H */