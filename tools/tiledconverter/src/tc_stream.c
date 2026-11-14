#include "tc_stream.h"

void tcSeek(uint8_t** str, uint8_t* pos)
{
    *str = pos;
}

uint8_t tcRead8(const uint8_t** str)
{
    uint8_t val = (*str)[0];
    *str += 1;
    return val;
}

uint16_t tcRead16Be(const uint8_t** str)
{
    uint16_t val = tcPeek16Be(*str);
    *str += 2;
    return val;
}

uint16_t tcRead16Le(const uint8_t** str)
{
    uint16_t val = tcPeek16Le(*str);
    *str += 2;
    return val;
}

uint32_t tcRead32Be(const uint8_t** str)
{
    uint32_t val = tcPeek32Be(*str);
    *str += 4;
    return val;
}

uint32_t tcRead32Le(const uint8_t** str)
{
    uint32_t val = tcPeek32Le(*str);
    *str += 4;
    return val;
}

void tcWrite8(uint8_t** str, uint8_t val)
{
    *str[0] = val;
    *str += 1;
}

void tcWrite16Be(uint8_t** str, uint16_t val)
{
    *((uint16_t**)str)[0] = tcTo16Be(val);
    *str += 2;
}

void tcWrite16Le(uint8_t** str, uint16_t val)
{
    *((uint16_t**)str)[0] = tcTo16Le(val);
    *str += 2;
}

void tcWrite32Be(uint8_t** str, uint32_t val)
{
    *((uint32_t**)str)[0] = tcTo32Be(val);
    *str += 4;
}

void tcWrite32Le(uint8_t** str, uint32_t val)
{
    *((uint32_t**)str)[0] = tcTo32Le(val);
    *str += 4;
}

void tcOverlappingCopy(uint8_t** str, int amount, int window)
{
    for (int i = 0; i < amount; i++)
    {
        (*str)[i] = (*str)[i - window];
    }
    *str += amount;
}
