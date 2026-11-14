#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "tc_enum_nick.h"
#include "tc_macros.h"
#include "tc_room_props.h"
#include "tc_template.h"
#include "tc_tile_layer.h"

#include "data/rooms_data.h"

#define TILED_DIR "tiled"

/* Derived from RoomLoadBackgrounds() from src/room.c. */
void tcConvertRoom(const struct RoomEntryRom* room, const char* areaName, const char* roomName)
{
    struct TcTileLayer* clipDataLayer = NULL;
    struct TcTileLayer* bg0Layer = NULL;
    struct TcTileLayer* bg1Layer = NULL;
    struct TcTileLayer* bg2Layer = NULL;
    struct TcTileLayer* bg3Layer = NULL;
    struct TcRoomProps roomProps = TC_ROOM_PROPS_INIT;
    char* areaDirname = NULL;
    char* tmjFilename = NULL;

    // Load clipdata, assume RLE
    clipDataLayer = tcTileLayerNewFromRle(room->pClipData);

    // Load BG0, either RLE or LZ77
    if (room->bg0Prop & BG_PROP_RLE_COMPRESSED)
    {
        bg0Layer = tcTileLayerNewFromRle(room->pBg0Data);
    }
    else if (room->bg0Prop & BG_PROP_LZ77_COMPRESSED)
    {
        /* src = entry.pBg0Data; */
        /* room->bg0Size = *src; */

        /* src += 4; */
        /* CallLZ77UncompWram(src, gDecompBg0Map); */
    }

    // Load BG1, assume RLE
    bg1Layer = tcTileLayerNewFromRle(room->pBg1Data);

    // Load BG2, force RLE
    if (room->bg2Prop & BG_PROP_RLE_COMPRESSED)
    {
        bg2Layer = tcTileLayerNewFromRle(room->pBg2Data);
    }

    // Load BG3, always LZ77
    /* room->bg3Size = *entry.pBg3Data; */
    /* src = entry.pBg3Data + 4; */
    /* CallLZ77UncompWram(src, gDecompBg3Map); */

    /* FIXME Extract u8 tileset, entry in sTilesetEntries */
    /* FIXME Extract BgProperty bg0Prop; */
    /* FIXME Extract BgProperty bg1Prop; */
    /* FIXME Extract BgProperty bg2Prop; */
    /* FIXME Extract BgProperty bg3Prop; */

    /* FIXME Extract const struct SpriteSpawn* pDefaultSpriteLayer; */
    /* FIXME Extract u8 defaultSpriteset; */

    /* FIXME Extract u8 firstSpriteLayerEvent; */
    /* FIXME Extract const struct SpriteSpawn* pFirstSpriteLayer; */
    /* FIXME Extract u8 firstSpriteset; */

    /* FIXME Extract u8 secondSpriteLayerEvent; */
    /* FIXME Extract const struct SpriteSpawn* pSecondSpriteLayer; */
    /* FIXME Extract u8 secondSpriteset; */

    roomProps.bg3Scrolling = room->bg3Scrolling;
    roomProps.transparency = getTransparencyEvaFlagsNick(room->transparency);
    roomProps.bg0Priority = room->transparency & (~TRANSPARENCY_USE_BG0_PRIORITY_MASK);
    roomProps.mapX = room->mapX;
    roomProps.mapY = room->mapY;
    roomProps.effect = getRoomEffectNick(room->effect);
    roomProps.effectY = room->effectY;
    roomProps.music = getSoundNick(room->musicTrack);

    asprintf(&areaDirname, TILED_DIR "/%s", areaName);
    asprintf(&tmjFilename, TILED_DIR "/%s/%s.tmj", areaName, roomName);

    /* FIXME Check the returned error. */
    mkdir(TILED_DIR, 0755);
    /* FIXME Check the returned error. */
    mkdir(areaDirname, 0755);
    tcWriteTmj(tmjFilename, clipDataLayer, bg0Layer, bg1Layer, bg2Layer, bg3Layer, &roomProps);

    tcRoomPropsClear(&roomProps);
    if (clipDataLayer) tcTileLayerFree(clipDataLayer);
    if (bg0Layer) tcTileLayerFree(bg0Layer);
    if (bg1Layer) tcTileLayerFree(bg1Layer);
    if (bg2Layer) tcTileLayerFree(bg2Layer);
    if (bg3Layer) tcTileLayerFree(bg3Layer);
    free(areaDirname);
    free(tmjFilename);
}

void tcConvertArea(const struct RoomEntryRom* area, int roomsCount, const char* areaName)
{
    for (int i = 0; i < roomsCount; i++)
    {
        char *roomName;

        asprintf(&roomName, "%s_%d", areaName, i);

        tcConvertRoom(&area[i], areaName, roomName);

        free(roomName);
    }
}

void tcConvertTilesets(const struct TilesetEntry* tileset, int tilesetsCount)
{
    /* FIXME Extract tilesets. */

    /* struct TilesetEntry { */
    /*     const u32* pTileGraphics; */
    /*     const u16* pPalette; */
    /*     const u32* pBackgroundGraphics; */
    /*     const u8* pTilemap; */
    /*     u8 animatedTileset; */
    /*     u8 animatedPalette; */
    /* }; */

    /* .pTileGraphics = sTileset_8_Gfx, */
    /* .pPalette = sTileset_8_Pal, */
    /* .pBackgroundGraphics = sTileset_8_Bg_Gfx, */
    /* .pTilemap = sTileset_8_Tilemap, */
    /* .animatedTileset = 3, */
    /* .animatedPalette = 0 */
}

int main()
{
    tcConvertTilesets(sTilesetEntries, TC_LENGTHOF(sTilesetEntries));

    tcConvertArea(sBrinstarRoomEntries, TC_LENGTHOF(sBrinstarRoomEntries), "brinstar");
    tcConvertArea(sKraidRoomEntries, TC_LENGTHOF(sKraidRoomEntries), "kraid");
    tcConvertArea(sNorfairRoomEntries, TC_LENGTHOF(sNorfairRoomEntries), "norfair");
    tcConvertArea(sRidleyRoomEntries, TC_LENGTHOF(sRidleyRoomEntries), "ridley");
    tcConvertArea(sTourianRoomEntries, TC_LENGTHOF(sTourianRoomEntries), "tourian");
    tcConvertArea(sCrateriaRoomEntries, TC_LENGTHOF(sCrateriaRoomEntries), "crateria");
    tcConvertArea(sChozodiaRoomEntries, TC_LENGTHOF(sChozodiaRoomEntries), "chozodia");
#ifdef DEBUG
    tcConvertArea(sTestRoomEntries, TC_LENGTHOF(sTestRoomEntries), "test");
    tcConvertArea(sTest1RoomEntries, TC_LENGTHOF(sTest1RoomEntries), "test1");
    tcConvertArea(sTest2RoomEntries, TC_LENGTHOF(sTest2RoomEntries), "test2");
    tcConvertArea(sTest3RoomEntries, TC_LENGTHOF(sTest3RoomEntries), "test3");
#endif // DEBUG
    return 0;
}
