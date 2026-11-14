#include <stdio.h>
#include "tc_graphics.h"
#include "tc_stream.h"

void tcPixelPair4BppFlip(TcPixelPair4Bpp* indexed, int len)
{
    for (int i = 0; i < len; i++) {
        uint8_t tmp = indexed[i].a;
        indexed[i].a = indexed[i].b;
        indexed[i].b = tmp;
    }
}

void tcRgbFromTile4Bpp(uint8_t* rgb, const TcTile4Bpp* indexed, int height, const uint16_t* palette)
{
    const int width = 8;

    for (int y = 0; y < height; y++)
    {
        int tile = y / 8;
        int row = y % 8;

        for (int x = 0; x < width; x++)
        {
            TcPixelPair4Bpp pixelPair = indexed[tile][row][(x % 8) / 2];
            uint8_t pixel = x % 2 == 0 ? pixelPair.a : pixelPair.b;
            uint16_t color = tcFrom16Le(palette[pixel]);
            tcWrite8(&rgb, TC_BLUE(color) << 3);
            tcWrite8(&rgb, TC_GREEN(color) << 3);
            tcWrite8(&rgb, TC_RED(color) << 3);
        }
    }
}

const int BMP_FILE_HEADER_SIZE = 14;
const int BMP_INFO_HEADER_SIZE = 40;
const int BMP_COLORS_COUNT_I4 = 16;
const int BMP_COLORS_COUNT_RGB = 0;
const int BMP_COLOR_TABLE_SIZE_I4 = 4 * BMP_COLORS_COUNT_I4;
const uint16_t BMP_BYTES_PER_PIXEL_RGB = 3;
const uint16_t BMP_BITS_PER_PIXEL_RGB = BMP_BYTES_PER_PIXEL_RGB * 8;
const uint16_t BMP_BITS_PER_PIXEL_I4 = 4;
const uint32_t BMP_RES_UNSPECIFIED = 0;
const uint32_t BMP_ALL_COLORS_IMPORTANT = 0;

static void tcWriteI4BmpFileHeader(uint8_t* bmp, int stride, int height)
{
    uint16_t reserved1 = 0;
    uint16_t reserved2 = 0;

    tcWrite8(&bmp, 'B');
    tcWrite8(&bmp, 'M');
    tcWrite32Le(&bmp, BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE + (stride * height));
    tcWrite16Le(&bmp, reserved1);
    tcWrite16Le(&bmp, reserved2);
    tcWrite32Le(&bmp, BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE);
}

static void tcWriteI4BmpInfoHeader(uint8_t* bmp, int width, int height)
{
    uint16_t colorPlanesCount = 1;
    uint16_t bpp = BMP_BITS_PER_PIXEL_I4;
    uint32_t compression = 0;
    uint32_t imageSize = width * height;
    uint32_t hRes = BMP_RES_UNSPECIFIED;
    uint32_t vRes = BMP_RES_UNSPECIFIED;
    uint32_t colorsCount = BMP_COLORS_COUNT_I4;
    uint32_t importantColorsCount = colorsCount;

    tcWrite32Le(&bmp, BMP_INFO_HEADER_SIZE);
    tcWrite32Le(&bmp, width);
    tcWrite32Le(&bmp, height);
    tcWrite16Le(&bmp, colorPlanesCount);
    tcWrite16Le(&bmp, bpp);
    tcWrite32Le(&bmp, compression);
    tcWrite32Le(&bmp, imageSize);
    tcWrite32Le(&bmp, hRes);
    tcWrite32Le(&bmp, vRes);
    tcWrite32Le(&bmp, colorsCount);
    tcWrite32Le(&bmp, importantColorsCount);
}

static void tcWrite4BppPalette(uint8_t* colorTable, const uint16_t* palette, int colorsCount)
{
    for (int i = 0; i < colorsCount; i++) {
        uint16_t color = tcFrom16Le(palette[i]);
        tcWrite8(&colorTable, TC_BLUE(color) << 3);
        tcWrite8(&colorTable, TC_GREEN(color) << 3);
        tcWrite8(&colorTable, TC_RED(color) << 3);
        tcWrite8(&colorTable, 0);
    }
}

void tcWriteI4Bmp(const TcPixelPair4Bpp* indexed, const uint16_t* palette, int width, int height, const char* bmpFilename)
{
    int widthInBytes = width / 2;

    uint8_t padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes % 4)) % 4;
    int stride = (widthInBytes) + paddingSize;
    FILE* file = NULL;
    uint8_t fileHeader[BMP_FILE_HEADER_SIZE];
    uint8_t infoHeader[BMP_INFO_HEADER_SIZE];
    uint8_t colorTable[BMP_COLOR_TABLE_SIZE_I4];

    tcWriteI4BmpFileHeader(fileHeader, stride, height);
    tcWriteI4BmpInfoHeader(infoHeader, width, height);
    tcWrite4BppPalette(colorTable, palette, BMP_COLORS_COUNT_I4);

    file = fopen(bmpFilename, "wb");

    fwrite(fileHeader, 1, BMP_FILE_HEADER_SIZE, file);
    fwrite(infoHeader, 1, BMP_INFO_HEADER_SIZE, file);
    fwrite(colorTable, 1, BMP_COLOR_TABLE_SIZE_I4, file);

    for (int y = 0; y < height; y++)
    {
        /* BMP stores pixels bottom-up. */
        int flippedY = height - 1 - y;
        fwrite(indexed + (flippedY * widthInBytes), 1, widthInBytes, file);
        fwrite(padding, 1, paddingSize, file);
    }

    fclose(file);
}

static void tcWriteRgbBmpFileHeader(uint8_t* bmp, int stride, int height)
{
    uint16_t reserved1 = 0;
    uint16_t reserved2 = 0;

    tcWrite8(&bmp, 'B');
    tcWrite8(&bmp, 'M');
    tcWrite32Le(&bmp, BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE + (stride * height));
    tcWrite16Le(&bmp, reserved1);
    tcWrite16Le(&bmp, reserved2);
    tcWrite32Le(&bmp, BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE);
}

static void tcWriteRgbBmpInfoHeader(uint8_t* bmp, int width, int height)
{
    uint16_t colorPlanesCount = 1;
    uint16_t bpp = BMP_BITS_PER_PIXEL_RGB;
    uint32_t compression = 0;
    uint32_t imageSize = 0;
    uint32_t hRes = BMP_RES_UNSPECIFIED;
    uint32_t vRes = BMP_RES_UNSPECIFIED;
    uint32_t colorsCount = BMP_COLORS_COUNT_RGB;
    uint32_t importantColorsCount = BMP_ALL_COLORS_IMPORTANT;

    tcWrite32Le(&bmp, BMP_INFO_HEADER_SIZE);
    tcWrite32Le(&bmp, width);
    tcWrite32Le(&bmp, height);
    tcWrite16Le(&bmp, colorPlanesCount);
    tcWrite16Le(&bmp, bpp);
    tcWrite32Le(&bmp, compression);
    tcWrite32Le(&bmp, imageSize);
    tcWrite32Le(&bmp, hRes);
    tcWrite32Le(&bmp, vRes);
    tcWrite32Le(&bmp, colorsCount);
    tcWrite32Le(&bmp, importantColorsCount);
}

void tcWriteRgbBmp(const uint8_t* rgb, int width, int height, const char* bmpFilename)
{
    int widthInBytes = width * BMP_BYTES_PER_PIXEL_RGB;

    uint8_t padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes % 4)) % 4;
    int stride = (widthInBytes) + paddingSize;
    FILE* file = NULL;
    uint8_t fileHeader[BMP_FILE_HEADER_SIZE];
    uint8_t infoHeader[BMP_INFO_HEADER_SIZE];

    tcWriteRgbBmpFileHeader(fileHeader, stride, height);
    tcWriteRgbBmpInfoHeader(infoHeader, width, height);

    file = fopen(bmpFilename, "wb");

    fwrite(fileHeader, 1, BMP_FILE_HEADER_SIZE, file);
    fwrite(infoHeader, 1, BMP_INFO_HEADER_SIZE, file);

    for (int y = 0; y < height; y++)
    {
        /* BMP stores pixels bottom-up. */
        int flippedY = height - 1 - y;
        fwrite(rgb + (flippedY * widthInBytes), BMP_BYTES_PER_PIXEL_RGB, width, file);
        fwrite(padding, 1, paddingSize, file);
    }

    fclose(file);
}
