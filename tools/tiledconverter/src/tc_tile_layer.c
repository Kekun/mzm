#include <stdlib.h>
#include "tc_compression.h"
#include "tc_tile_layer.h"

struct TcTileLayer* tcTileLayerNew(uint16_t* data, uint8_t width, uint8_t height)
{
    struct TcTileLayer* tileLayer = malloc(sizeof(struct TcTileLayer));
    tileLayer->data = data;
    tileLayer->width = width;
    tileLayer->height = height;
    return tileLayer;
}

struct TcTileLayer* tcTileLayerNewFromRle(const uint8_t* rle)
{
    uint16_t* decomp;
    uint8_t width, height;
    int length, size;

    width = rle[0];
    height = rle[1];
    length = width * height;
    size = sizeof(*decomp) * length;
    decomp = malloc(size);
    /* FIXME Check if the returned size is what we expect. */
    tcDecompRLE(&rle[2], (uint8_t*)decomp);

    return tcTileLayerNew(decomp, width, height);
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
