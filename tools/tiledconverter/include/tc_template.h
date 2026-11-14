#ifndef TC_TEMPLATE_H
#define TC_TEMPLATE_H

#include "tc_room_props.h"
#include "tc_tile_layer.h"

struct TcTileset {
    int firstgid;
    const char* source;
};

void tcWriteRoomTmj(const char* tmjPath,
                    const struct TcTileLayer* clipDataLayer,
                    const struct TcTileLayer* bg0Layer,
                    const struct TcTileLayer* bg1Layer,
                    const struct TcTileLayer* bg2Layer,
                    const struct TcTileLayer* bg3Layer,
                    const struct TcRoomProps* roomProps,
                    const struct TcTileset* tilesets,
                    int tilesetsCount);

void tcWriteTilemapTmj(const char* tmjPath,
                       const struct TcTileLayer* tilemapLayer,
                       const struct TcTileset* tilesets,
                       int tilesetsCount);

void tcWriteTilemapTsj(const char* tsjPath,
                       const char* imageFilename,
                       const char* tilesetName);

void tcWriteTilesetTsj(const char* tsjPath,
                       const char* imageFilename,
                       const char* tilesetName,
                       int tilesCount);

#endif /* TC_TEMPLATE_H */
