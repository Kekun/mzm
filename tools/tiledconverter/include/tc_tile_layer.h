#ifndef TC_TILE_LAYER_H
#define TC_TILE_LAYER_H

#include <stdint.h>

struct TcTileLayer {
    int* data;
    uint8_t width;
    uint8_t height;
};

struct TcTileLayer* tcTileLayerNew(int* data, uint8_t width, uint8_t height);
struct TcTileLayer* tcTileLayerNewFromLz77(const uint8_t* lz77);
struct TcTileLayer* tcTileLayerNewFromRle(const uint8_t* rle);
void tcTileLayerAdd(struct TcTileLayer* tileLayer, int value);
void tcTileLayerClear(struct TcTileLayer* tileLayer);
void tcTileLayerFree(struct TcTileLayer* tileLayer);

#endif /* TC_TILE_LAYER_H */
