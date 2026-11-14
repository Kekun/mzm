#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tc_template.h"

static const char* tmjTemplate1 =
    "{ \"compressionlevel\":-1,\n"
    " \"height\":%u,\n"
    " \"infinite\":false,\n"
    " \"layers\":[";

static const char* tileLayerTemplate1 =
    "\n"
    "        {\n"
    "         \"data\":[";

static const char* tileLayerTemplate2 =
    "],\n"
    "         \"height\":%u,\n"
    "         \"id\":%d,\n"
    "         \"name\":\"%s\",\n"
    "         \"opacity\":1,\n"
    "         \"parallaxx\":%lf,\n"
    "         \"parallaxy\":%lf,\n"
    "         \"type\":\"tilelayer\",\n"
    "         \"visible\":true,\n"
    "         \"width\":%u,\n"
    "         \"x\":0,\n"
    "         \"y\":0\n"
    "        }";

static const char* tmjTemplate2 =
    "],\n"
    " \"nextlayerid\":%d,\n"
    " \"nextobjectid\":1,\n"
    " \"orientation\":\"orthogonal\",\n"
    " \"properties\":[";

static const char* tmjPropertyTemplate =
    "\n"
    "        {\n"
    "         \"name\":\"%s\",\n"
    "         \"type\":\"%s\",\n"
    "         \"value\":%s\n"
    "        }";

static const char* tmjTemplate3 =
    "],\n"
    " \"renderorder\":\"right-down\",\n"
    " \"tiledversion\":\"1.12.2\",\n"
    " \"tileheight\":%u,\n"
    " \"tilesets\":[";

static const char* tmjTileset =
    "\n"
    "        {\n"
    "         \"firstgid\":%d,\n"
    "         \"source\":\"%s\"\n"
    "        }";

static const char* tmjTemplate4 =
    "],\n"
    " \"tilewidth\":%u,\n"
    " \"type\":\"map\",\n"
    " \"version\":\"1.10\",\n"
    " \"width\":%u\n"
    "}";

static const char* tsjTemplate =
    "{ \"columns\":%u,\n"
    " \"image\":\"%s\",\n"
    " \"imageheight\":%d,\n"
    " \"imagewidth\":%d,\n"
    " \"margin\":0,\n"
    " \"name\":\"%s\",\n"
    " \"spacing\":0,\n"
    " \"tilecount\":%d,\n"
    " \"tiledversion\":\"1.12.2\",\n"
    " \"tileheight\":%u,\n"
    " \"tilewidth\":%u,\n"
    " \"type\":\"tileset\",\n"
    " \"version\":\"1.10\"\n"
    "}";

static void printComma(FILE* file, bool* needsComma)
{
    if (*needsComma)
        fprintf(file, ", ");
    else
        *needsComma = true;
}

static void fprintTmjTileLayerData(FILE* file, unsigned int width, unsigned int height, const int* data)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x == width - 1 && y == height - 1)
            {
                fprintf(file, "%u", data[x + y * width]);
            }
            else if (x == width - 1)
            {
                fprintf(file, "%u,\n            ", data[x + y * width]);
            }
            else
            {
                fprintf(file, "%u, ", data[x + y * width]);
            }
        }
    }
}

static void fprintTmjTileLayer(FILE* file, const struct TcTileLayer* tileLayer, int id, const char* name, double paralaxX, double paralaxY, bool* prependComma)
{
    printComma(file, prependComma);
    fprintf(file, tileLayerTemplate1);
    fprintTmjTileLayerData(file, tileLayer->width, tileLayer->height, tileLayer->data);
    fprintf(file, tileLayerTemplate2, tileLayer->height, id, name, paralaxX, paralaxY, tileLayer->width);
}

static void fprintTmjTileset(FILE* file, const struct TcTileset* tileset, bool* prependComma)
{
    printComma(file, prependComma);
    fprintf(file, tmjTileset, tileset->firstgid, tileset->source);
}

static void fprintIntProperty(FILE* file, const char* name, int value, bool* prependComma)
{
    char* valueStr = NULL;

    printComma(file, prependComma);
    asprintf(&valueStr, "%d", value);
    fprintf(file, tmjPropertyTemplate, name, "int", valueStr);

    free(valueStr);
}

static void fprintStringProperty(FILE* file, const char* name, const char* value, bool* prependComma)
{
    char* valueStr = NULL;

    assert(value != NULL);
    printComma(file, prependComma);
    asprintf(&valueStr, "\"%s\"", value);
    fprintf(file, tmjPropertyTemplate, name, "string", valueStr);

    free(valueStr);
}

static void fprintTmj(FILE* file,
                      const struct TcTileLayer* clipDataLayer,
                      const struct TcTileLayer* bg0Layer,
                      const struct TcTileLayer* bg1Layer,
                      const struct TcTileLayer* bg2Layer,
                      const struct TcTileLayer* bg3Layer,
                      const struct TcRoomProps* roomProps,
                      const struct TcTileset* tilesets,
                      int tilesetsCount)
{
    const int tileHeight = 16;
    const int tileWidth = 16;
    int layerId = 1;

    fprintf(file, tmjTemplate1, clipDataLayer->height);

    {
        bool prependComma = false;

        /* FIXME bg3 isn't optional but it's not handled yet as it's LZ77. */
        if (bg3Layer) fprintTmjTileLayer(file, bg3Layer, layerId++, "bg3", roomProps->bg3ScrollingX, roomProps->bg3ScrollingY, &prependComma);

        if (bg0Layer && roomProps->bg0Priority == 3) fprintTmjTileLayer(file, bg0Layer, layerId++, "bg0", 1.0, 1.0, &prependComma);

        if (bg2Layer) fprintTmjTileLayer(file, bg2Layer, layerId++, "bg2", 1.0, 1.0, &prependComma);

        if (bg0Layer && roomProps->bg0Priority == 2) fprintTmjTileLayer(file, bg0Layer, layerId++, "bg0", 1.0, 1.0, &prependComma);

        /* Samus is drawn here. */

        if (bg0Layer && roomProps->bg0Priority == 1) fprintTmjTileLayer(file, bg0Layer, layerId++, "bg0", 1.0, 1.0, &prependComma);

        fprintTmjTileLayer(file, bg1Layer, layerId++, "bg1", 1.0, 1.0, &prependComma);

        if (bg0Layer && roomProps->bg0Priority == 0) fprintTmjTileLayer(file, bg0Layer, layerId++, "bg0", 1.0, 1.0, &prependComma);

        fprintTmjTileLayer(file, clipDataLayer, layerId++, "clipdata", 1.0, 1.0, &prependComma);
    }

    fprintf(file, tmjTemplate2, layerId);

    {
        bool prependComma = false;

        /* u8 tileset; */
        /* BgProperty bg0Prop; */
        /* BgProperty bg1Prop; */
        /* BgProperty bg2Prop; */
        /* BgProperty bg3Prop; */
        /* const u8* pBg0Data; */
        /* const u8* pBg1Data; */
        /* const u8* pBg2Data; */
        /* const u8* pClipData; */
        /* const u8* pBg3Data; */

        /* u8 bg3Scrolling; */
        /* u8 transparency; */

        /* const struct SpriteSpawn* pDefaultSpriteLayer; */
        /* u8 defaultSpriteset; */

        /* u8 firstSpriteLayerEvent; */
        /* const struct SpriteSpawn* pFirstSpriteLayer; */
        /* u8 firstSpriteset; */

        /* u8 secondSpriteLayerEvent; */
        /* const struct SpriteSpawn* pSecondSpriteLayer; */
        /* u8 secondSpriteset; */

        if (roomProps->transparency) fprintStringProperty (file, "transparency", roomProps->transparency, &prependComma);
        fprintIntProperty (file, "mapX", roomProps->mapX, &prependComma);
        fprintIntProperty (file, "mapY", roomProps->mapY, &prependComma);
        if (roomProps->effect) fprintStringProperty (file, "effect", roomProps->effect, &prependComma);
        if (roomProps->effect) fprintIntProperty (file, "effectY", roomProps->effectY, &prependComma);
        fprintStringProperty (file, "music", roomProps->music, &prependComma);
    }

    fprintf(file, tmjTemplate3, tileHeight);

    {
        bool prependComma = false;

        for (int i = 0; i < tilesetsCount; i++)
            fprintTmjTileset(file, &tilesets[i], &prependComma);
    }

    fprintf(file, tmjTemplate4, tileWidth, clipDataLayer->width);
}

void tcWriteRoomTmj(const char* tmjPath,
                    const struct TcTileLayer* clipDataLayer,
                    const struct TcTileLayer* bg0Layer,
                    const struct TcTileLayer* bg1Layer,
                    const struct TcTileLayer* bg2Layer,
                    const struct TcTileLayer* bg3Layer,
                    const struct TcRoomProps* roomProps,
                    const struct TcTileset* tilesets,
                    int tilesetsCount)
{
    FILE* file;

    file = fopen(tmjPath, "w");
    if (file == NULL) {
        perror("open failed");
        return;
    }

    fprintTmj(file, clipDataLayer, bg0Layer, bg1Layer, bg2Layer, bg3Layer, roomProps, tilesets, tilesetsCount);

    fclose(file);
}

static void fprintTilemapTmj(FILE* file,
                             const struct TcTileLayer* tilemapLayer,
                             const struct TcTileset* tilesets,
                             int tilesetsCount)
{
    const int tileHeight = 8;
    const int tileWidth = 8;
    int layerId = 1;

    fprintf(file, tmjTemplate1, tilemapLayer->height);

    {
        bool prependComma = false;

        fprintTmjTileLayer(file, tilemapLayer, layerId++, "tilemap", 1.0, 1.0, &prependComma);
    }

    fprintf(file, tmjTemplate2, layerId);

    fprintf(file, tmjTemplate3, tileHeight);

    {
        bool prependComma = false;

        for (int i = 0; i < tilesetsCount; i++)
            fprintTmjTileset(file, &tilesets[i], &prependComma);
    }

    fprintf(file, tmjTemplate4, tileWidth, tilemapLayer->width);
}

void tcWriteTilemapTmj(const char* tmjPath,
                       const struct TcTileLayer* tilemapLayer,
                       const struct TcTileset* tilesets,
                       int tilesetsCount)
{
    FILE* file;

    file = fopen(tmjPath, "w");
    if (file == NULL) {
        perror("open failed");
        return;
    }

    fprintTilemapTmj(file, tilemapLayer, tilesets, tilesetsCount);

    fclose(file);
}

static void fprintTilemapTsj(FILE* file,
                             const char* imageFilename,
                             const char* tilesetName)
{
    const int tileSize = 16;
    const int columns = 16;
    const int rows = 33;
    const int tilesCount = columns * rows;
    int imageWidth = columns * tileSize;
    int imageHeight = rows * tileSize;
    fprintf(file, tsjTemplate, columns, imageFilename, imageHeight, imageWidth, tilesetName, columns * rows, tileSize, tileSize);
}

void tcWriteTilemapTsj(const char* tsjPath,
                       const char* imageFilename,
                       const char* tilesetName)
{
    FILE* file;

    file = fopen(tsjPath, "w");
    if (file == NULL) {
        perror("open failed");
        return;
    }

    fprintTilemapTsj(file, imageFilename, tilesetName);

    fclose(file);
}

static void fprintTilesetTsj(FILE* file,
                             const char* imageFilename,
                             const char* tilesetName,
                             int tilesCount)
{
    const int tileSize = 8;
    const int columns = 1;
    int rows = tilesCount;
    int imageWidth = columns * tileSize;
    int imageHeight = rows * tileSize;
    fprintf(file, tsjTemplate, columns, imageFilename, imageHeight, imageWidth, tilesetName, tilesCount, tileSize, tileSize);
}

void tcWriteTilesetTsj(const char* tsjPath,
                       const char* imageFilename,
                       const char* tilesetName,
                       int tilesCount)
{
    FILE* file;

    file = fopen(tsjPath, "w");
    if (file == NULL) {
        perror("open failed");
        return;
    }

    fprintTilesetTsj(file, imageFilename, tilesetName, tilesCount);

    fclose(file);
}
