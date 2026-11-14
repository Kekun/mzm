#ifndef TC_STREAM_H
#define TC_STREAM_H

#include "tc_bit.h"

static inline uint8_t tcPeek8(const uint8_t* str)
{
    return str[0];
}

static inline uint16_t tcPeek16Be(const uint8_t* str)
{
    return tcFrom16Be(((const uint16_t*)str)[0]);
}

static inline uint16_t tcPeek16Le(const uint8_t* str)
{
    return tcFrom16Le(((const uint16_t*)str)[0]);
}

static inline uint32_t tcPeek32Be(const uint8_t* str)
{
    return tcFrom32Be(((const uint32_t*)str)[0]);
}

static inline uint32_t tcPeek32Le(const uint8_t* str)
{
    return tcFrom32Le(((const uint32_t*)str)[0]);
}

void tcSeek(uint8_t** str, uint8_t* pos);
uint8_t tcRead8(const uint8_t** str);
uint16_t tcRead16Be(const uint8_t** str);
uint16_t tcRead16Le(const uint8_t** str);
uint32_t tcRead32Be(const uint8_t** str);
uint32_t tcRead32Le(const uint8_t** str);
void tcWrite8(uint8_t** str, uint8_t val);
void tcWrite16Be(uint8_t** str, uint16_t val);
void tcWrite16Le(uint8_t** str, uint16_t val);
void tcWrite32Be(uint8_t** str, uint32_t val);
void tcWrite32Le(uint8_t** str, uint32_t val);
void tcOverlappingCopy(uint8_t** str, int amount, int window);

#endif /* TC_STREAM_H */
