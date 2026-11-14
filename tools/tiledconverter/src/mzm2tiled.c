#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "tc_compression.h"
#include "tc_enum_nick.h"
#include "tc_graphics.h"
#include "tc_macros.h"
#include "tc_room_props.h"
#include "tc_stream.h"
#include "tc_template.h"
#include "tc_tile_layer.h"

#include "data/default_background_data.h"
#include "data/rooms_data.h"

#define TILED_DIR "tiled"
#define ROOMS_DIR TILED_DIR "/rooms"
#define TILESETS_DIR TILED_DIR "/tilesets"

/* MAKE_ENUM(u8, BgLayer) { */
/*     BG_LAYER_EFFECT, */
/*     BG_LAYER_FOREGROUND, */
/*     BG_LAYER_BACKGROUND, */
/*     BG_LAYER_SCENERY, */
/* }; */

/* EFFECT_LAYER_POSITION_ABOVE_SCENERY */
/* EFFECT_LAYER_POSITION_ABOVE_BACKGROUND */
/* EFFECT_LAYER_POSITION_ABOVE_SAMUS */
/* EFFECT_LAYER_POSITION_ABOVE_FOREGROUND */

/* Background layer 3 is the background at the very back.
 * Background layer 2 is the middleground behind Samus, it is optional.
 * Background layer 1 is the middleground before Samus.
 * Background layer 0 is for effects, its is optional and its position depends
 * on the need.
 */

/* Background layers using LZ77-compression have one of the 4 native GBA
 * background sizes: 256×256, 512×256, 256×512, or 512×512. See the matching
 * BGCNT_SIZE_*` values. It is used for the scenery layer and for the effect
 * layer when it doesn't depend on the ground layers, e.g. for fog, lava or
 * darkness effects.
 * Background layers using RLE-compression can be freely sized and always match
 * the size of the room. It is used for the foreground and background layers,
 * and for the effect layer when it depends on the ground layers, e.g. to hide
 * or reveal part of the ground.
 */

/* RLE-compressed backgrounds can have any size, they are typically for . */
/* LZ77-compressed backgrounds must have one of the valid GBA background sizes, they are used for effect layers . */

/* Derived from RoomLoadBackgrounds() from src/room.c. */
static void tcConvertRoom(const struct RoomEntryRom* room, const char* tmjPath)
{
    const int clipdataTilemapStartId = 1;
    const int clipdataTilemapSize = 175;
    const int regularTilemapStartId = clipdataTilemapStartId + clipdataTilemapSize;
    const int regularTilemapSize = 528;
    const int backgroundTilemapStartId = regularTilemapStartId + regularTilemapSize;
    struct TcTileLayer* clipDataLayer = NULL;
    struct TcTileLayer* bg0Layer = NULL;
    struct TcTileLayer* bg1Layer = NULL;
    struct TcTileLayer* bg2Layer = NULL;
    struct TcTileLayer* bg3Layer = NULL;
    struct TcRoomProps roomProps = TC_ROOM_PROPS_INIT;
    struct TcTileset tilesets[2] = { 0 };
    char* tilesetRelPath = NULL;

    // Load clipdata, mandatory, RLE
    assert(room->pClipData != sBackground_Empty);
    clipDataLayer = tcTileLayerNewFromRle(room->pClipData);

    // Load BG0, optional, either RLE or LZ77
    if (room->bg0Prop & BG_PROP_RLE_COMPRESSED)
    {
        assert(room->pBg0Data != sBackground_Empty);
        bg0Layer = tcTileLayerNewFromRle(room->pBg0Data);
        tcTileLayerAdd(bg0Layer, regularTilemapStartId);
    }
    else if (room->bg0Prop & BG_PROP_LZ77_COMPRESSED)
    {
        assert(room->pBg0Data != sBackground_Empty);
        bg0Layer = tcTileLayerNewFromLz77(room->pBg0Data);
        tcTileLayerAdd(bg0Layer, regularTilemapStartId);
    }
    else
    {
        assert(room->pBg0Data == sBackground_Empty);
    }

    // Load BG1, mandatory, RLE
    assert(room->pBg1Data != sBackground_Empty);
    bg1Layer = tcTileLayerNewFromRle(room->pBg1Data);
    tcTileLayerAdd(bg1Layer, regularTilemapStartId);

    // Load BG2, optional, RLE
    if (room->bg2Prop & BG_PROP_RLE_COMPRESSED)
    {
        assert(room->pBg2Data != sBackground_Empty);
        bg2Layer = tcTileLayerNewFromRle(room->pBg2Data);
        tcTileLayerAdd(bg2Layer, regularTilemapStartId);
    }
    else if (room->bg2Prop & BG_PROP_LZ77_COMPRESSED)
    {
        assert(room->pBg2Data == sBackground_Empty);
    }

    // Load BG3, mandatory, LZ77
    assert(room->pBg3Data != sBackground_Empty);
    bg3Layer = tcTileLayerNewFromLz77(room->pBg3Data);
    tcTileLayerAdd(bg3Layer, backgroundTilemapStartId);

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

    switch (room->bg3Scrolling) {
    case BG3_SCROLLING_NONE:
        roomProps.bg3ScrollingX = 0.0;
        roomProps.bg3ScrollingY = 0.0;
        break;
    case BG3_SCROLLING_X_ONLY_HALVED:
        roomProps.bg3ScrollingX = 0.5;
        roomProps.bg3ScrollingY = 0.0;
        break;
    case BG3_SCROLLING_Y_ONLY_HALVED:
        roomProps.bg3ScrollingX = 0.0;
        roomProps.bg3ScrollingY = 0.5;
        break;
    case BG3_SCROLLING_HALVED:
        roomProps.bg3ScrollingX = 0.5;
        roomProps.bg3ScrollingY = 0.5;
        break;
    case BG3_SCROLLING_Y_HALVED:
        roomProps.bg3ScrollingX = 1.0;
        roomProps.bg3ScrollingY = 0.5;
        break;
    case BG3_SCROLLING_X_HALVED:
        roomProps.bg3ScrollingX = 0.5;
        roomProps.bg3ScrollingY = 1.0;
        break;
    case BG3_SCROLLING_NORMAL:
        roomProps.bg3ScrollingX = 1.0;
        roomProps.bg3ScrollingY = 1.0;
        break;
    default:
        assert(false);
        break;
    };

    roomProps.transparency = getTransparencyEvaFlagsNick(room->transparency);
    roomProps.bg0Priority = room->transparency & (~TRANSPARENCY_USE_BG0_PRIORITY_MASK);
    roomProps.mapX = room->mapX;
    roomProps.mapY = room->mapY;
    roomProps.effect = getRoomEffectNick(room->effect);
    roomProps.effectY = room->effectY;
    roomProps.music = getSoundNick(room->musicTrack);

    asprintf(&tilesetRelPath, "..\\/..\\/tilesets/tileset_%u.tm.tsj", room->tileset);

    tilesets[0] = (struct TcTileset){ 1, "..\\/..\\/clipdata.tsj" };
    /* FIXME Set the right tileset */
    tilesets[1] = (struct TcTileset){ 176, tilesetRelPath };

    tcWriteRoomTmj(tmjPath, clipDataLayer, bg0Layer, bg1Layer, bg2Layer, bg3Layer, &roomProps, tilesets, TC_LENGTHOF(tilesets));

    tcRoomPropsClear(&roomProps);
    if (clipDataLayer) tcTileLayerFree(clipDataLayer);
    if (bg0Layer) tcTileLayerFree(bg0Layer);
    if (bg1Layer) tcTileLayerFree(bg1Layer);
    if (bg2Layer) tcTileLayerFree(bg2Layer);
    if (bg3Layer) tcTileLayerFree(bg3Layer);
    free(tilesetRelPath);
}

static void tcConvertArea(const struct RoomEntryRom* area, int roomsCount, const char* areaName)
{
    char* areaDirname = NULL;

    asprintf(&areaDirname, ROOMS_DIR "/%s", areaName);

    /* FIXME Check the returned error. */
    mkdir(areaDirname, 0755);

    for (int i = 0; i < roomsCount; i++)
    {
        char* tmjPath = NULL;

        asprintf(&tmjPath, ROOMS_DIR "/%s/%s_%d.tmj", areaName, areaName, i);

        tcConvertRoom(&area[i], tmjPath);

        free(tmjPath);
    }

    free(areaDirname);
}

static void tcConvertTilesetPalette(const uint16_t* palettes, const char* palBmpFilename)
{
    const int colorsPerPalette = 16;
    const int palettesCount = 14;
    const int tileWidth = 8;
    const int tileHeight = 8;
    const int tilesCount = palettesCount;
    const int width = tileWidth * 1;
    const int height = tileHeight * tilesCount;
    const int rgbSizePerTile = tileWidth * tileHeight* 3;
    const int rgbSize = rgbSizePerTile * tilesCount;
    const TcTile4Bpp indexed[1] = {
        {
            { { 0x0, 0x0 }, { 0x1, 0x1 }, { 0x2, 0x2 }, { 0x3, 0x3 } },
            { { 0x0, 0x0 }, { 0x1, 0x1 }, { 0x2, 0x2 }, { 0x3, 0x3 } },
            { { 0x4, 0x4 }, { 0x5, 0x5 }, { 0x6, 0x6 }, { 0x7, 0x7 } },
            { { 0x4, 0x4 }, { 0x5, 0x5 }, { 0x6, 0x6 }, { 0x7, 0x7 } },
            { { 0x8, 0x8 }, { 0x9, 0x9 }, { 0xa, 0xa }, { 0xb, 0xb } },
            { { 0x8, 0x8 }, { 0x9, 0x9 }, { 0xa, 0xa }, { 0xb, 0xb } },
            { { 0xc, 0xc }, { 0xd, 0xd }, { 0xe, 0xe }, { 0xf, 0xf } },
            { { 0xc, 0xc }, { 0xd, 0xd }, { 0xe, 0xe }, { 0xf, 0xf } },
        }
    };
    uint8_t* rgb;

    rgb = malloc(rgbSize);

    for (int i = 0; i < palettesCount; i++) {
        tcRgbFromTile4Bpp(&rgb[rgbSizePerTile * i], indexed, tileHeight, &palettes[colorsPerPalette * i]);
    }

    tcWriteRgbBmp(rgb, width, height, palBmpFilename);

    free(rgb);
}

static void tcConvertTilesetGraphics(const uint32_t* tilesetBackgroundGraphics, const uint16_t* palettes, const char* tilesetName)
{
    const int width = 8;
    const uint8_t* lz77 = (const uint8_t*)tilesetBackgroundGraphics;
    int height;
    int tilesCount, size;
    TcTile4Bpp* indexed;
    char* bmpFilename = NULL;
    char* bmpPath = NULL;
    char* tsjFilename = NULL;
    char* tsjPath = NULL;

    size = tcPeek16Le(&lz77[1]);

    tilesCount = size / sizeof(TcTile4Bpp);
    assert((size % sizeof(TcTile4Bpp)) == 0);

    height = tilesCount * 8;

    indexed = malloc(size);
    tcDecompLZ77(&lz77[0], (uint8_t*)indexed);

    /* Convert GBA 4bpp to BMP 4bpp */
    tcPixelPair4BppFlip((TcPixelPair4Bpp*)indexed, size);

    asprintf(&bmpFilename, "%s.bmp", tilesetName);
    asprintf(&bmpPath, TILESETS_DIR "/%s", bmpFilename);

    /* Use the second palette */
    tcWriteI4Bmp((TcPixelPair4Bpp*)indexed, &palettes[TC_PALETTE_LEN * 1], width, height, bmpPath);

    asprintf(&tsjFilename, "%s.tsj", tilesetName);
    asprintf(&tsjPath, TILESETS_DIR "/%s", tsjFilename);

    tcWriteTilesetTsj(tsjPath, bmpFilename, tilesetName, tilesCount);

    free(indexed);
    free(bmpFilename);
    free(bmpPath);
    free(tsjFilename);
    free(tsjPath);
}

static void tcConvertTilesetBackgroundGraphics(const uint32_t* tilesetBackgroundGraphics, const char* tilesetName)
{
    const uint8_t* lz77 = (const uint8_t*)tilesetBackgroundGraphics;
    int size;
    uint16_t* decomp;
    FILE* file;

    size = tcPeek16Le(&lz77[1]);
    decomp = malloc(size);
    tcDecompLZ77(&lz77[0], (uint8_t*)decomp);

    /* file = fopen(filename, "w"); */

    /* fwrite(decomp, 1, size, file); */

    free(decomp);
    /* fclose(file); */
}

static int tcConvertTilesetTilemapTile(uint16_t tile)
{
    int converted = TC_TILE_ID(tcFrom16Le(tile)) + 1;
    if (TC_TILE_H_FLIP(tile))
        converted += 0x80000000;
    if (TC_TILE_V_FLIP(tile))
        converted += 0x40000000;
    return converted;
}

static void tcConvertTilesetTilemap(const uint8_t* tilesetTilemap, const char* tilesetName)
{
    struct TcTileLayer* tilemapLayer = NULL;
    const int widthInBlocks = 16;
    const int widthInTiles = widthInBlocks * 2;
    const int heightInBlocks = 33;
    const int heightInTiles = heightInBlocks * 2;
    const int blocksCount = widthInBlocks * heightInBlocks;
    /* Skip some 16 bits value at the start. */
    /* FIXME What is that value we skip? */
    const uint16_t* tilemap = (const uint16_t*)(tilesetTilemap + 2);
    int* converted = NULL;
    char* tilemapTmjFilename = NULL;
    char* tilemapTmjPath = NULL;
    char* tilemapTsjFilename = NULL;
    char* tilemapTsjPath = NULL;
    char* tilesetTsjFilename = NULL;
    struct TcTileset tilesets[1] = { 0 };

    converted = calloc(blocksCount * 4, sizeof(*converted));

    for (int blockY = 0; blockY < heightInBlocks; blockY++) {
        for (int blockX = 0; blockX < widthInBlocks; blockX++) {
            /* Each block has 4 tiles. */
            const uint16_t* blockTilemap = &tilemap[(blockX + blockY * widthInBlocks) * 4];
            int tileX = blockX * 2;
            int tileY = blockY * 2;
            /* Add 1 as 0 is the lack of tile. */
            /* FIXME Add the flip and palette information too */
            converted[(tileX + 0) + (tileY + 0) * widthInTiles] = tcConvertTilesetTilemapTile(blockTilemap[0]);
            converted[(tileX + 1) + (tileY + 0) * widthInTiles] = tcConvertTilesetTilemapTile(blockTilemap[1]);
            converted[(tileX + 0) + (tileY + 1) * widthInTiles] = tcConvertTilesetTilemapTile(blockTilemap[2]);
            converted[(tileX + 1) + (tileY + 1) * widthInTiles] = tcConvertTilesetTilemapTile(blockTilemap[3]);
        }
    }

    tilemapLayer = tcTileLayerNew(converted, widthInTiles, heightInTiles);
    /* tilemapLayer took ownership of the layer data. */
    converted = NULL;

    asprintf(&tilemapTmjFilename, "%s.tm.tmj", tilesetName);
    asprintf(&tilemapTmjPath, TILESETS_DIR "/%s", tilemapTmjFilename);
    asprintf(&tilemapTsjFilename, "%s.tm.tsj", tilesetName);
    asprintf(&tilemapTsjPath, TILESETS_DIR "/%s", tilemapTsjFilename);
    asprintf(&tilesetTsjFilename, "%s.tsj", tilesetName);

    tilesets[0] = (struct TcTileset){ 193, tilesetTsjFilename };

    tcWriteTilemapTmj(tilemapTmjPath, tilemapLayer, tilesets, TC_LENGTHOF(tilesets));
    tcWriteTilemapTsj(tilemapTsjPath, tilemapTmjFilename, tilesetName);

    if (tilemapLayer) tcTileLayerFree(tilemapLayer);
    free(tilemapTmjFilename);
    free(tilemapTmjPath);
    free(tilemapTsjFilename);
    free(tilemapTsjPath);
    free(tilesetTsjFilename);
}

static void tcConvertTilesets(const struct TilesetEntry* tileset, int tilesetsCount)
{
    for (int i = 0; i < tilesetsCount; i++)
    {
        char* tilesetName = NULL;
        char* palBmpFilename = NULL;

        asprintf(&tilesetName, "tileset_%d", i);
        asprintf(&palBmpFilename, TILESETS_DIR "/tileset_%d_pal.bmp", i);

        /* The actual 8x8 graphics tiles */
        /* FIXME Not decompressed correctly */
        tcConvertTilesetPalette(tileset[i].pPalette, palBmpFilename);
        tcConvertTilesetGraphics(tileset[i].pTileGraphics, tileset[i].pPalette, tilesetName);
        tcConvertTilesetBackgroundGraphics(tileset[i].pBackgroundGraphics, tilesetName);
        tcConvertTilesetTilemap(tileset[i].pTilemap, tilesetName);

        /* struct TilesetEntry { */
        /*     const u32* pTileGraphics; // Seemingly always use LZ77 */
        /*     const u16* pPalette; */
        /*     const u32* pBackgroundGraphics; // Seemingly always use LZ77 */
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

        /* CallLZ77UncompVram(entry.pTileGraphics, VRAM_BASE + 0x5800); */

        /* backgroundGfxSize = ((u8*)entry.pBackgroundGraphics)[2] << 8 | ((u8*)entry.pBackgroundGraphics)[1]; */
        /* CallLZ77UncompVram(entry.pBackgroundGraphics, VRAM_BASE + 0xfde0 - backgroundGfxSize); */

        free(tilesetName);
        free(palBmpFilename);
    }
}

int main()
{
    /* FIXME Check the returned error. */
    mkdir(TILED_DIR, 0755);

    /* FIXME Check the returned error. */
    mkdir(TILESETS_DIR, 0755);

    tcConvertTilesets(sTilesetEntries, TC_LENGTHOF(sTilesetEntries));

    /* FIXME Check the returned error. */
    mkdir(ROOMS_DIR, 0755);

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
