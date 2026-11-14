#ifndef TC_BIT_H
#define TC_BIT_H

#include <stdbit.h>
#include <stdint.h>

#define TC_SWAP16(val) ((uint16_t) ( \
    (uint16_t) ((uint16_t) (val) >> 8) | \
    (uint16_t) ((uint16_t) (val) << 8)))

#define TC_SWAP32(val) ((uint32_t) ( \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) << 8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >> 8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

static inline uint16_t tcFrom16Be(uint16_t val)
{
    return __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_BIG__ ? val : TC_SWAP16 (val);
}

static inline uint16_t tcFrom16Le(uint16_t val)
{
    return __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_LITTLE__ ? val : TC_SWAP16 (val);
}

static inline uint32_t tcFrom32Be(uint32_t val)
{
    return __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_BIG__ ? val : TC_SWAP32 (val);
}

static inline uint32_t tcFrom32Le(uint32_t val)
{
    return __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_LITTLE__ ? val : TC_SWAP32 (val);
}

#define tcTo16Be tcFrom16Be
#define tcTo16Le tcFrom16Le
#define tcTo32Be tcFrom32Be
#define tcTo32Le tcFrom32Le

#endif /* TC_BIT_H */
