#ifndef TC_STREAM_H
#define TC_STREAM_H

#include <stdint.h>

void tcSeek(uint8_t** str, uint8_t* pos);
uint8_t tcRead8(const uint8_t** str);
uint16_t tcRead16(const uint8_t** str);
void tcWrite8(uint8_t** str, uint8_t val);

#endif /* TC_STREAM_H */
