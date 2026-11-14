#ifndef TC_TEMPLATE_H
#define TC_TEMPLATE_H

#include "tc_room_props.h"
#include "tc_tile_layer.h"

void tcWriteTmj(const char* filename,
                const struct TcTileLayer* clipDataLayer,
                const struct TcTileLayer* bg0Layer,
                const struct TcTileLayer* bg1Layer,
                const struct TcTileLayer* bg2Layer,
                const struct TcTileLayer* bg3Layer,
                const struct TcRoomProps* roomProps);

#endif /* TC_TEMPLATE_H */
