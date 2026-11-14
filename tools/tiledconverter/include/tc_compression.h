#ifndef TC_COMPRESSION_H
#define TC_COMPRESSION_H

#include <stdint.h>

int tcDecompRLE(const uint8_t* src, uint8_t* dst);
// int tcCompRLE(const uint8_t* src, int length, uint8_t* dst);
// int tcDecompLZ77(const uint8_t* src, uint8_t* dst);
// int tcCompLZ77(const uint8_t* src, int length, uint8_t* dst);

#endif /* TC_COMPRESSION_H */
