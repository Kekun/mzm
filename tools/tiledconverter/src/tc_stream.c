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

uint16_t tcRead16(const uint8_t** str)
{
    uint16_t val = ((*str)[0] << 8) + (*str)[1];
    *str += 2;
    return val;
}

void tcWrite8(uint8_t** str, uint8_t val)
{
    *str[0] = val;
    *str += 1;
}
