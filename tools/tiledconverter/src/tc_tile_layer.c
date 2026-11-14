#include <assert.h>
#include <stdlib.h>
#include "gba/display.h"
#include "tc_compression.h"
#include "tc_stream.h"
#include "tc_tile_layer.h"

struct TcTileLayer* tcTileLayerNew(int* data, uint8_t width, uint8_t height)
{
    struct TcTileLayer* tileLayer = malloc(sizeof(struct TcTileLayer));
    tileLayer->data = data;
    tileLayer->width = width;
    tileLayer->height = height;
    return tileLayer;
}

struct TcTileLayer* tcTileLayerNewFromLz77(const uint8_t* lz77)
{
    uint16_t* decomp;
    int* converted;
    uint8_t width, height;
    int length, size;

    switch (tcPeek32Le(&lz77[0])) {
    case BGCNT_SIZE_256x256:
        width = 16;
        height = 16;
        break;
    case BGCNT_SIZE_512x256:
        width = 32;
        height = 16;
        break;
    case BGCNT_SIZE_256x512:
        width = 16;
        height = 32;
        break;
    case BGCNT_SIZE_512x512:
        width = 32;
        height = 32;
        break;
    default:
        assert(false);
        break;
    }

    length = width * height;
    size = tcPeek16Le(&lz77[4 + 1]);
    /* FIXME Why 8? */
    assert(size == length * 8);
    decomp = malloc(size);
    /* FIXME Check if the returned size is what we expect. */
    tcDecompLZ77(&lz77[4], (uint8_t*)decomp);

    converted = calloc(length, sizeof(int));
    for (int i = 0; i < length; i++)
        converted[i] = decomp[i];

    return tcTileLayerNew(converted, width, height);
}

struct TcTileLayer* tcTileLayerNewFromRle(const uint8_t* rle)
{
    uint16_t* decomp;
    int* converted;
    uint8_t width, height;
    int length;

    width = rle[0];
    height = rle[1];
    length = width * height;
    decomp = calloc(length, sizeof(*decomp));
    /* FIXME Check if the returned size is what we expect. */
    tcDecompRLE(&rle[2], (uint8_t*)decomp);

    converted = calloc(length, sizeof(int));
    for (int i = 0; i < length; i++)
        converted[i] = decomp[i];

    return tcTileLayerNew(converted, width, height);
}

void tcTileLayerAdd(struct TcTileLayer* tileLayer, int value)
{
    int length = tileLayer->width * tileLayer->height;
    for (int i = 0; i < length; i++)
        tileLayer->data[i] += value;
}

void tcTileLayerClear(struct TcTileLayer* tileLayer)
{
    free(tileLayer->data);
}

void tcTileLayerFree(struct TcTileLayer* tileLayer)
{
    tcTileLayerClear(tileLayer);
    free(tileLayer);
}
