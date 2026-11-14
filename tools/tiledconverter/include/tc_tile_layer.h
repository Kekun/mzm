#ifndef TC_TILE_LAYER_H
#define TC_TILE_LAYER_H

#include <stdint.h>

struct TcTileLayer {
    uint16_t* data;
    uint8_t width;
    uint8_t height;
};

struct TcTileLayer* tcTileLayerNew(uint16_t* data, uint8_t width, uint8_t height);
struct TcTileLayer* tcTileLayerNewFromRle(const uint8_t* rle);
void tcTileLayerClear(struct TcTileLayer* tileLayer);
void tcTileLayerFree(struct TcTileLayer* tileLayer);

#endif /* TC_TILE_LAYER_H */
