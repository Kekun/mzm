#ifndef TC_GRAPHICS_H
#define TC_GRAPHICS_H

#include <stdint.h>

#define TC_RED(c) (c & 0x1f)
#define TC_GREEN(c) ((c >> 5) & 0x1f)
#define TC_BLUE(c) ((c >> 10) & 0x1f)

#define TC_PALETTE_LEN 16

#define TC_TILE_ID(t) (t & 0x03ff)
#define TC_TILE_H_FLIP(t) ((t >> 10) & 0x0001)
#define TC_TILE_V_FLIP(t) ((t >> 11) & 0x0001)
#define TC_TILE_PALETTE(t) ((t >> 12) & 0x000f)

typedef struct {
    uint8_t a: 4;
    uint8_t b: 4;
} TcPixelPair4Bpp;

typedef TcPixelPair4Bpp TcRow4Bpp[4];

typedef TcRow4Bpp TcTile4Bpp[8];

void tcPixelPair4BppFlip(TcPixelPair4Bpp* indexed, int len);
void tcRgbFromTile4Bpp(uint8_t* rgb, const TcTile4Bpp* indexed, int height, const uint16_t* palette);
void tcWriteI4Bmp(const TcPixelPair4Bpp* indexed, const uint16_t* palettes, int width, int height, const char* bmpFilename);
void tcWriteRgbBmp(const uint8_t* rgb, int width, int height, const char* bmpFilename);

#endif /* TC_GRAPHICS_H */
